/*==============================================================================

  This dynamic modeler tool was developed by Mauro I. Dominguez, Independent
  as Ad-Honorem work.

  Copyright (c) All Rights Reserved.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

#include "vtkSlicerDynamicModelerRevolveTool.h"

#include "vtkMRMLDynamicModelerNode.h"

// MRML includes
#include <vtkMRMLMarkupsNode.h>
#include <vtkMRMLMarkupsFiducialNode.h>
#include <vtkMRMLMarkupsLineNode.h>
#include <vtkMRMLMarkupsPlaneNode.h>
#include <vtkMRMLMarkupsAngleNode.h>
#include <vtkMRMLMarkupsCurveNode.h>
#include <vtkMRMLMarkupsClosedCurveNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLTransformNode.h>

// VTK includes
#include <vtkCommand.h>
#include <vtkGeneralTransform.h>
#include <vtkIntArray.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkRotationalExtrusionFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkFeatureEdges.h>
#include <vtkAppendPolyData.h>

//----------------------------------------------------------------------------
vtkToolNewMacro(vtkSlicerDynamicModelerRevolveTool);

const char* REVOLVE_INPUT_PROFILE_REFERENCE_ROLE = "Revolve.InputProfile";
const char* REVOLVE_INPUT_MARKUPS_REFERENCE_ROLE = "Revolve.InputMarkups";
const char* REVOLVE_OUTPUT_MODEL_REFERENCE_ROLE = "Revolve.OutputModel";
const char* REVOLVE_ANGLE_DEGREES = "Revolve.AngleDegrees";
const char* REVOLVE_AXIS_IS_AT_ORIGIN = "Revolve.AxisIsAtOrigin";
const char* REVOLVE_TRANSLATE_DISTANCE_ALONG_AXIS = "Revolve.TranslateDistanceAlongAxis";

//----------------------------------------------------------------------------
vtkSlicerDynamicModelerRevolveTool::vtkSlicerDynamicModelerRevolveTool()
{
  /////////
  // Inputs
  vtkNew<vtkIntArray> inputModelEvents;
  inputModelEvents->InsertNextTuple1(vtkCommand::ModifiedEvent);
  inputModelEvents->InsertNextTuple1(vtkMRMLModelNode::MeshModifiedEvent);
  inputModelEvents->InsertNextTuple1(vtkMRMLMarkupsNode::PointModifiedEvent);
  inputModelEvents->InsertNextTuple1(vtkMRMLTransformableNode::TransformModifiedEvent);
  vtkNew<vtkStringArray> inputModelClassNames;
  inputModelClassNames->InsertNextValue("vtkMRMLModelNode");
  inputModelClassNames->InsertNextValue("vtkMRMLMarkupsCurveNode");
  inputModelClassNames->InsertNextValue("vtkMRMLMarkupsClosedCurveNode");
  NodeInfo inputProfile(
    "Model or Curve",
    "Profile to be revolved.",
    inputModelClassNames,
    REVOLVE_INPUT_PROFILE_REFERENCE_ROLE,
    true,
    false,
    inputModelEvents
  );
  this->InputNodeInfo.push_back(inputProfile);

  vtkNew<vtkIntArray> inputMarkupEvents;
  inputMarkupEvents->InsertNextTuple1(vtkCommand::ModifiedEvent);
  inputMarkupEvents->InsertNextTuple1(vtkMRMLMarkupsNode::PointModifiedEvent);
  inputMarkupEvents->InsertNextTuple1(vtkMRMLTransformableNode::TransformModifiedEvent);
  vtkNew<vtkStringArray> inputMarkupClassNames;
  inputMarkupClassNames->InsertNextValue("vtkMRMLMarkupsFiducialNode");
  inputMarkupClassNames->InsertNextValue("vtkMRMLMarkupsLineNode");
  inputMarkupClassNames->InsertNextValue("vtkMRMLMarkupsPlaneNode");
  inputMarkupClassNames->InsertNextValue("vtkMRMLMarkupsAngleNode");
  NodeInfo inputMarkups(
    "Markups",
    "Markups to specify spatial revolution axis. Normal for plane and angle, superior axis for a point.",
    inputMarkupClassNames,
    REVOLVE_INPUT_MARKUPS_REFERENCE_ROLE,
    /*required*/ true,
    /*repeatable*/ false,
    inputMarkupEvents
  );
  this->InputNodeInfo.push_back(inputMarkups);

  /////////
  // Outputs
  NodeInfo outputModel(
    "Revolved model",
    "Result of the revolving operation.",
    inputModelClassNames,
    REVOLVE_OUTPUT_MODEL_REFERENCE_ROLE,
    false,
    false
  );
  this->OutputNodeInfo.push_back(outputModel);

  /////////
  // Parameters

  ParameterInfo parameterRotationAngleDegress(
    "Rotation degrees",
    "Rotation angle in degrees. Ignored for angle markup.",
    REVOLVE_ANGLE_DEGREES,
    PARAMETER_DOUBLE,
    90.0);
  this->InputParameterInfo.push_back(parameterRotationAngleDegress);

  ParameterInfo parameterRotationAxisIsAtOrigin(
    "Rotation axis at origin",
    "If true, the revolution will be done around the origin.",
    REVOLVE_AXIS_IS_AT_ORIGIN,
    PARAMETER_BOOL,
    false);
  this->InputParameterInfo.push_back(parameterRotationAxisIsAtOrigin);

  ParameterInfo parameterTranslationAlongAxisDistance(
    "Translate along axis",
    "Translation distance during the swept.",
    REVOLVE_TRANSLATE_DISTANCE_ALONG_AXIS,
    PARAMETER_DOUBLE,
    0.0);
  this->InputParameterInfo.push_back(parameterTranslationAlongAxisDistance);

  this->InputProfileToWorldTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->InputProfileNodeToWorldTransform = vtkSmartPointer<vtkGeneralTransform>::New();
  this->InputProfileToWorldTransformFilter->SetTransform(this->InputProfileNodeToWorldTransform);

  this->ModelingTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->ModelingTransform = vtkSmartPointer<vtkTransform>::New();
  this->ModelingTransform->PostMultiply();
  this->ModelingTransformFilter->SetTransform(this->ModelingTransform);

  this->CapTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->CapTransform = vtkSmartPointer<vtkTransform>::New();
  this->CapTransform->PostMultiply();
  this->CapTransformFilter->SetTransform(this->CapTransform);

  this->BoundaryEdgesFilter = vtkSmartPointer<vtkFeatureEdges>::New();
  this->BoundaryEdgesFilter->BoundaryEdgesOn();
  this->BoundaryEdgesFilter->FeatureEdgesOff();
  this->BoundaryEdgesFilter->NonManifoldEdgesOff();
  this->BoundaryEdgesFilter->ManifoldEdgesOff();
  this->BoundaryEdgesFilter->PassLinesOn();

  this->RevolveFilter = vtkSmartPointer<vtkRotationalExtrusionFilter>::New();
  this->RevolveFilter->SetInputConnection(this->BoundaryEdgesFilter->GetOutputPort());

  this->AppendFilter = vtkSmartPointer<vtkAppendPolyData>::New();

  this->ResamplingTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->ResamplingTransform = vtkSmartPointer<vtkTransform>::New();
  this->ResamplingTransform->PostMultiply();
  this->ResamplingTransformFilter->SetTransform(this->ResamplingTransform);

  this->OutputModelToWorldTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->OutputWorldToModelTransform = vtkSmartPointer<vtkGeneralTransform>::New();
  this->OutputModelToWorldTransformFilter->SetTransform(this->OutputWorldToModelTransform);
  this->OutputModelToWorldTransformFilter->SetInputConnection(this->RevolveFilter->GetOutputPort());
}

//----------------------------------------------------------------------------
vtkSlicerDynamicModelerRevolveTool::~vtkSlicerDynamicModelerRevolveTool()
= default;

//----------------------------------------------------------------------------
const char* vtkSlicerDynamicModelerRevolveTool::GetName()
{
  return "Revolve";
}

//----------------------------------------------------------------------------
bool vtkSlicerDynamicModelerRevolveTool::RunInternal(vtkMRMLDynamicModelerNode* surfaceEditorNode)
{
  if (!this->HasRequiredInputs(surfaceEditorNode))
  {
    vtkErrorMacro("Invalid number of inputs");
    return false;
  }

  vtkMRMLModelNode* outputModelNode = vtkMRMLModelNode::SafeDownCast(surfaceEditorNode->GetNodeReference(REVOLVE_OUTPUT_MODEL_REFERENCE_ROLE));
  if (!outputModelNode)
  {
    // Nothing to output
    return true;
  }

  vtkMRMLModelNode* inputProfileModelNode = vtkMRMLModelNode::SafeDownCast(surfaceEditorNode->GetNodeReference(REVOLVE_INPUT_PROFILE_REFERENCE_ROLE));
  vtkMRMLMarkupsCurveNode* inputProfileMarkupsCurveNode = vtkMRMLMarkupsCurveNode::SafeDownCast(surfaceEditorNode->GetNodeReference(REVOLVE_INPUT_PROFILE_REFERENCE_ROLE));
  vtkMRMLMarkupsClosedCurveNode* inputProfileMarkupsClosedCurveNode = vtkMRMLMarkupsClosedCurveNode::SafeDownCast(surfaceEditorNode->GetNodeReference(REVOLVE_INPUT_PROFILE_REFERENCE_ROLE));
  bool profileIsModel = (inputProfileModelNode) && (!inputProfileMarkupsCurveNode) && (!inputProfileMarkupsClosedCurveNode);
  bool profileIsCurve = (!inputProfileModelNode) && (inputProfileMarkupsCurveNode) && (!inputProfileMarkupsClosedCurveNode);
  bool profileIsClosedCurve = (!inputProfileModelNode) && (inputProfileMarkupsCurveNode) && (inputProfileMarkupsClosedCurveNode);
  if ((!profileIsModel) && (!profileIsCurve) && (!profileIsClosedCurve))
  {
    vtkErrorMacro("Invalid input node!");
    return false;
  }

  if (profileIsModel)
  {
    if (!inputProfileModelNode->GetMesh() || inputProfileModelNode->GetMesh()->GetNumberOfPoints() == 0)
    {
      vtkNew<vtkPolyData> outputPolyData;
      outputModelNode->SetAndObservePolyData(outputPolyData);
      return true;
    } 
    else
    {
      if (inputProfileModelNode->GetParentTransformNode())
      {
        inputProfileModelNode->GetParentTransformNode()->GetTransformToWorld(this->InputProfileNodeToWorldTransform);
      }
      this->InputProfileToWorldTransformFilter->SetInputConnection(inputProfileModelNode->GetMeshConnection());
    }
  }
  else 
  {
    this->InputProfileNodeToWorldTransform->Identity(); // this way the transformFilter is a pass-through
    if (profileIsCurve)
    {
      if (!inputProfileMarkupsCurveNode->GetCurveWorld() || inputProfileMarkupsCurveNode->GetCurveWorld()->GetNumberOfPoints() == 0)
      {
        vtkNew<vtkPolyData> outputPolyData;
        outputModelNode->SetAndObservePolyData(outputPolyData);
        return true;
      }
      else
      {
        this->InputProfileToWorldTransformFilter->SetInputConnection(inputProfileMarkupsCurveNode->	GetCurveWorldConnection());
      }
    }
    else if (profileIsClosedCurve)
    {
      if (!inputProfileMarkupsClosedCurveNode->GetCurveWorld() || inputProfileMarkupsClosedCurveNode->GetCurveWorld()->GetNumberOfPoints() == 0)
      {
        vtkNew<vtkPolyData> outputPolyData;
        outputModelNode->SetAndObservePolyData(outputPolyData);
        return true;
      }
      else
      {
        this->InputProfileToWorldTransformFilter->SetInputConnection(inputProfileMarkupsClosedCurveNode->	GetCurveWorldConnection());
      }
    }
  }


  if (outputModelNode && outputModelNode->GetParentTransformNode())
  {
    outputModelNode->GetParentTransformNode()->GetTransformFromWorld(this->OutputWorldToModelTransform);
  }
  else
  {
    this->OutputWorldToModelTransform->Identity();
  }


  vtkMRMLMarkupsNode* markupsNode = vtkMRMLMarkupsNode::SafeDownCast(surfaceEditorNode->GetNodeReference(REVOLVE_INPUT_MARKUPS_REFERENCE_ROLE));

  // check if markups are valid
  if (!markupsNode)
  {
    return true;
  }

  int numberOfControlPoints = markupsNode->GetNumberOfControlPoints();
  if (numberOfControlPoints == 0)
  {
    return true;
  }

  vtkMRMLMarkupsFiducialNode* markupsFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(markupsNode);
  vtkMRMLMarkupsLineNode* markupsLineNode = vtkMRMLMarkupsLineNode::SafeDownCast(markupsNode);
  vtkMRMLMarkupsPlaneNode* markupsPlaneNode = vtkMRMLMarkupsPlaneNode::SafeDownCast(markupsNode);
  vtkMRMLMarkupsAngleNode* markupsAngleNode = vtkMRMLMarkupsAngleNode::SafeDownCast(markupsNode);
  
  if ((markupsLineNode) && (numberOfControlPoints != 2))
  {
    return true;
  }
  if ((markupsAngleNode) && (numberOfControlPoints != 3))
  {
    return true;
  }

  // get parameters
  double rotationAngleDegress = this->GetNthInputParameterValue(0, surfaceEditorNode).ToDouble();
  bool axisIsAtOrigin = vtkVariant(surfaceEditorNode->GetAttribute(REVOLVE_AXIS_IS_AT_ORIGIN)).ToInt() != 0;
  double translationAlongAxisDistance = this->GetNthInputParameterValue(2, surfaceEditorNode).ToDouble();

  this->RevolveFilter->SetResolution(
    std::ceil(std::fabs(rotationAngleDegress))*2);
  this->RevolveFilter->SetAngle(rotationAngleDegress); // redefined below if angle markup
  //this->RevolveFilter->SetDeltaRadius(scale)
  this->RevolveFilter->SetTranslation(translationAlongAxisDistance);

  // calculate the origin, axis
  double origin[3] = {0.,0.,0.};
  double axis[3] = {0.,0.,0.};
  if (markupsFiducialNode)
  {
    markupsFiducialNode->GetNthControlPointPositionWorld(0, origin);
    axis[2]= 1.0; // superior direction
  }
  if (markupsLineNode)
  {
    double endPoint[3] = {0.,0.,0.};
    markupsLineNode->GetNthControlPointPositionWorld(1, endPoint);
    markupsLineNode->GetNthControlPointPositionWorld(0, origin);
    vtkMath::Subtract(endPoint, origin, axis);
    vtkMath::Normalize(axis);
  }
  if (markupsPlaneNode)
  {
    markupsPlaneNode->GetNthControlPointPositionWorld(0, origin);
    markupsPlaneNode->GetNormalWorld(axis);
  }
  if (markupsAngleNode)
  {
    double firstPoint[3] = {0.,0.,0.};
    double thirdPoint[3] = {0.,0.,0.};
    markupsAngleNode->GetNthControlPointPositionWorld(0, firstPoint);
    markupsAngleNode->GetNthControlPointPositionWorld(1, origin);
    markupsAngleNode->GetNthControlPointPositionWorld(2, thirdPoint);
    double vector1[3] = {0.,0.,0.};
    double vector2[3] = {0.,0.,0.};
    vtkMath::Subtract(firstPoint, origin, vector1);
    vtkMath::Subtract(thirdPoint, origin, vector2);
    vtkMath::Normalize(vector1);
    vtkMath::Normalize(vector2);
    vtkMath::Cross(vector1,vector2,axis);
    vtkMath::Normalize(axis);

    double rotationAngleRadians = vtkMath::AngleBetweenVectors(vector1,vector2);
    rotationAngleDegress = vtkMath::DegreesFromRadians(rotationAngleRadians);
    this->RevolveFilter->SetAngle(rotationAngleDegress);
  }

  this->RevolveFilter->SetRotationAxis(axis);

  // final position of the cap
  this->CapTransform->Identity();
  this->CapTransform->RotateWXYZ(rotationAngleDegress,axis[0],axis[1],axis[2]);
  this->CapTransform->Translate(
    translationAlongAxisDistance*axis[0],
    translationAlongAxisDistance*axis[1],
    translationAlongAxisDistance*axis[2]);

  // translate to origin the mesh to revolve
  if (axisIsAtOrigin == false)
  {
    this->ModelingTransform->Identity();
    this->ModelingTransform->Translate(-origin[0],-origin[1],-origin[2]);
    this->ModelingTransformFilter->SetInputConnection(this->InputProfileToWorldTransformFilter->GetOutputPort());
    this->BoundaryEdgesFilter->SetInputConnection(this->ModelingTransformFilter->GetOutputPort());
    this->CapTransformFilter->SetInputConnection(this->ModelingTransformFilter->GetOutputPort());
    this->AppendFilter->RemoveAllInputs();
    this->AppendFilter->AddInputConnection(this->ModelingTransformFilter->GetOutputPort());
    this->AppendFilter->AddInputConnection(this->RevolveFilter->GetOutputPort());
    this->AppendFilter->AddInputConnection(this->CapTransformFilter->GetOutputPort());
    this->ResamplingTransform->Identity();
    this->ResamplingTransform->Translate(origin[0],origin[1],origin[2]);
    this->ResamplingTransformFilter->SetInputConnection(this->AppendFilter->GetOutputPort());
    this->OutputModelToWorldTransformFilter->SetInputConnection(this->ResamplingTransformFilter->GetOutputPort());
  }
  else
  {
    this->BoundaryEdgesFilter->SetInputConnection(this->InputProfileToWorldTransformFilter->GetOutputPort());
    this->CapTransformFilter->SetInputConnection(this->InputProfileToWorldTransformFilter->GetOutputPort());
    this->AppendFilter->RemoveAllInputs();
    this->AppendFilter->AddInputConnection(this->InputProfileToWorldTransformFilter->GetOutputPort());
    this->AppendFilter->AddInputConnection(this->RevolveFilter->GetOutputPort());
    this->AppendFilter->AddInputConnection(this->CapTransformFilter->GetOutputPort());
    this->OutputModelToWorldTransformFilter->SetInputConnection(this->AppendFilter->GetOutputPort());
  }
  
  this->OutputModelToWorldTransformFilter->Update();
  vtkNew<vtkPolyData> outputMesh;
  outputMesh->DeepCopy(this->OutputModelToWorldTransformFilter->GetOutput());

  MRMLNodeModifyBlocker blocker(outputModelNode);
  outputModelNode->SetAndObserveMesh(outputMesh);
  outputModelNode->InvokeCustomModifiedEvent(vtkMRMLModelNode::MeshModifiedEvent);

  return true;
}