/*==============================================================================

  Copyright (c) Laboratory for Percutaneous Surgery (PerkLab)
  Queen's University, Kingston, ON, Canada. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Kyle Sunderland, PerkLab, Queen's University
  and was supported through CANARIE's Research Software Program, Cancer
  Care Ontario, OpenAnatomy, and Brigham and Women's Hospital through NIH grant R01MH112748.

==============================================================================*/

#include "vtkSlicerDynamicModelerTool.h"

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

/// DynamicModeler MRML includes
#include "vtkMRMLDynamicModelerNode.h"

/// MRML includes
#include <vtkMRMLDisplayableNode.h>
#include <vtkMRMLDisplayNode.h>

//----------------------------------------------------------------------------
vtkSlicerDynamicModelerTool::vtkSlicerDynamicModelerTool()
= default;

//----------------------------------------------------------------------------
vtkSlicerDynamicModelerTool::~vtkSlicerDynamicModelerTool()
= default;

//----------------------------------------------------------------------------
vtkSlicerDynamicModelerTool* vtkSlicerDynamicModelerTool::Clone()
{
  vtkSlicerDynamicModelerTool* clone = this->CreateToolInstance();
  return clone;
}

//----------------------------------------------------------------------------
int vtkSlicerDynamicModelerTool::GetNumberOfInputNodes()
{
  return this->InputNodeInfo.size();
}

//----------------------------------------------------------------------------
int vtkSlicerDynamicModelerTool::GetNumberOfInputParameters()
{
  return this->InputParameterInfo.size();
}

//----------------------------------------------------------------------------
int vtkSlicerDynamicModelerTool::GetNumberOfOutputNodes()
{
  return this->OutputNodeInfo.size();
}

//----------------------------------------------------------------------------
std::string vtkSlicerDynamicModelerTool::GetNthInputNodeName(int n)
{
  if (n >= this->GetNumberOfInputNodes())
    {
    vtkErrorMacro("Input node " << n << " is out of range!");
    return "";
    }
  return this->InputNodeInfo[n].Name;
}

//----------------------------------------------------------------------------
std::string vtkSlicerDynamicModelerTool::GetNthInputNodeDescription(int n)
{
  if (n >= this->GetNumberOfInputNodes())
    {
    vtkErrorMacro("Input node " << n << " is out of range!");
    return "";
    }
  return this->InputNodeInfo[n].Description;
}

//----------------------------------------------------------------------------
vtkStringArray* vtkSlicerDynamicModelerTool::GetNthInputNodeClassNames(int n)
{
  if (n >= this->GetNumberOfInputNodes())
    {
    vtkErrorMacro("Input node " << n << " is out of range!");
    return nullptr;
    }
  return this->InputNodeInfo[n].ClassNames;
}

//----------------------------------------------------------------------------
std::string vtkSlicerDynamicModelerTool::GetNthInputNodeReferenceRole(int n)
{
  if (n >= this->GetNumberOfInputNodes())
    {
    vtkErrorMacro("Input node " << n << " is out of range!");
    return "";
    }
  return this->InputNodeInfo[n].ReferenceRole;
}

//----------------------------------------------------------------------------
bool vtkSlicerDynamicModelerTool::GetNthInputNodeRequired(int n)
{
  if (n >= this->GetNumberOfInputNodes())
    {
    vtkErrorMacro("Input node " << n << " is out of range!");
    return false;
    }
  return this->InputNodeInfo[n].Required;
}

//----------------------------------------------------------------------------
bool vtkSlicerDynamicModelerTool::GetNthInputNodeRepeatable(int n)
{
  if (n >= this->GetNumberOfInputNodes())
    {
    vtkErrorMacro("Input node " << n << " is out of range!");
    return false;
    }
  return this->InputNodeInfo[n].Repeatable;
}

//----------------------------------------------------------------------------
std::string vtkSlicerDynamicModelerTool::GetNthOutputNodeName(int n)
{
  if (n >= this->GetNumberOfOutputNodes())
    {
    vtkErrorMacro("Output node " << n << " is out of range!");
    return "";
    }
  return this->OutputNodeInfo[n].Name;
}

//----------------------------------------------------------------------------
std::string vtkSlicerDynamicModelerTool::GetNthOutputNodeDescription(int n)
{
  if (n >= this->GetNumberOfOutputNodes())
    {
    vtkErrorMacro("Output node " << n << " is out of range!");
    return "";
    }
  return this->OutputNodeInfo[n].Description;
}

//----------------------------------------------------------------------------
vtkStringArray* vtkSlicerDynamicModelerTool::GetNthOutputNodeClassNames(int n)
{
  if (n >= this->GetNumberOfOutputNodes())
    {
    vtkErrorMacro("Output node " << n << " is out of range!");
    return nullptr;
    }
  return this->OutputNodeInfo[n].ClassNames;
}

//----------------------------------------------------------------------------
std::string vtkSlicerDynamicModelerTool::GetNthOutputNodeReferenceRole(int n)
{
  if (n >= this->GetNumberOfOutputNodes())
    {
    vtkErrorMacro("Output node " << n << " is out of range!");
    return "";
    }
  return this->OutputNodeInfo[n].ReferenceRole;
}

//----------------------------------------------------------------------------
bool vtkSlicerDynamicModelerTool::GetNthOutputNodeRequired(int n)
{
  if (n >= this->GetNumberOfOutputNodes())
    {
    vtkErrorMacro("Output node " << n << " is out of range!");
    return false;
    }
  return this->OutputNodeInfo[n].Required;
}

//---------------------------------------------------------------------------
void vtkSlicerDynamicModelerTool::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Name:\t" << this->GetName() << std::endl;
}

//---------------------------------------------------------------------------
vtkIntArray* vtkSlicerDynamicModelerTool::GetNthInputNodeEvents(int n)
{
  if (n >= this->GetNumberOfInputNodes())
    {
    vtkErrorMacro("Input node " << n << " is out of range!");
    return nullptr;
    }
  return this->InputNodeInfo[n].Events;
}

//---------------------------------------------------------------------------
vtkMRMLNode* vtkSlicerDynamicModelerTool::GetNthInputNode(int n, vtkMRMLDynamicModelerNode* surfaceEditorNode)
{
  if(!surfaceEditorNode)
    {
    vtkErrorMacro("Invalid parameter node");
    return nullptr;
    }
  if (n >= this->GetNumberOfInputNodes())
    {
    vtkErrorMacro("Input node " << n << " is out of range!");
    return nullptr;
    }
  std::string referenceRole = this->GetNthInputNodeReferenceRole(n);
  return surfaceEditorNode->GetNodeReference(referenceRole.c_str());
}

//---------------------------------------------------------------------------
vtkMRMLNode* vtkSlicerDynamicModelerTool::GetNthOutputNode(int n, vtkMRMLDynamicModelerNode* surfaceEditorNode)
{
  if(!surfaceEditorNode)
    {
    vtkErrorMacro("Invalid parameter node");
    return nullptr;
    }
  if (n >= this->GetNumberOfOutputNodes())
    {
    vtkErrorMacro("Output node " << n << " is out of range!");
    return nullptr;
    }
  std::string referenceRole = this->GetNthOutputNodeReferenceRole(n);
  return surfaceEditorNode->GetNodeReference(referenceRole.c_str());
}

//---------------------------------------------------------------------------
std::string vtkSlicerDynamicModelerTool::GetNthInputParameterName(int n)
{
  if (n >= this->GetNumberOfInputParameters())
    {
    vtkErrorMacro("Parameter " << n << " is out of range!");
    return "";
    }
  return this->InputParameterInfo[n].Name;
}

//---------------------------------------------------------------------------
std::string vtkSlicerDynamicModelerTool::GetNthInputParameterDescription(int n)
{
  if (n >= this->GetNumberOfInputParameters())
    {
    vtkErrorMacro("Parameter " << n << " is out of range!");
    return "";
    }
  return this->InputParameterInfo[n].Description;
}

//---------------------------------------------------------------------------
std::string vtkSlicerDynamicModelerTool::GetNthInputParameterAttributeName(int n)
{
  if (n >= this->GetNumberOfInputParameters())
    {
    vtkErrorMacro("Parameter " << n << " is out of range!");
    return "";
    }
  return this->InputParameterInfo[n].AttributeName;
}

//---------------------------------------------------------------------------
int vtkSlicerDynamicModelerTool::GetNthInputParameterType(int n)
{
  if (n >= this->GetNumberOfInputParameters())
    {
    vtkErrorMacro("Parameter " << n << " is out of range!");
    return PARAMETER_STRING;
    }
  return this->InputParameterInfo[n].Type;
}

//---------------------------------------------------------------------------
vtkVariant vtkSlicerDynamicModelerTool::GetNthInputParameterValue(int n, vtkMRMLDynamicModelerNode* surfaceEditorNode)
{
  if (n >= this->GetNumberOfInputParameters())
    {
    vtkErrorMacro("Parameter " << n << " is out of range!");
    return PARAMETER_STRING;
    }
  std::string attributeName = this->GetNthInputParameterAttributeName(n);
  const char* parameterValue = surfaceEditorNode->GetAttribute(attributeName.c_str());
  if (!parameterValue)
    {
    return this->InputParameterInfo[n].DefaultValue;
    }
  return vtkVariant(parameterValue);
}

//---------------------------------------------------------------------------
vtkStringArray* vtkSlicerDynamicModelerTool::GetNthInputParameterPossibleValues(int n)
{
  if (n >= this->GetNumberOfInputParameters())
    {
    vtkErrorMacro("Parameter " << n << " is out of range!");
    return nullptr;
    }
  return this->InputParameterInfo[n].PossibleValues;
}

//---------------------------------------------------------------------------
vtkDoubleArray* vtkSlicerDynamicModelerTool::GetNthInputParameterNumberRange(int n)
{
  if (n >= this->GetNumberOfInputParameters())
    {
    vtkErrorMacro("Parameter " << n << " is out of range!");
    return nullptr;
    }
  return this->InputParameterInfo[n].NumbersRange;
}

//---------------------------------------------------------------------------
int vtkSlicerDynamicModelerTool::GetNthInputParameterNumberDecimals(int n)
{
  if (n >= this->GetNumberOfInputParameters())
    {
    vtkErrorMacro("Parameter " << n << " is out of range!");
    return 2;
    }
  return this->InputParameterInfo[n].NumberDecimals;
}

//---------------------------------------------------------------------------
double vtkSlicerDynamicModelerTool::GetNthInputParameterNumberSingleStep(int n)
{
  if (n >= this->GetNumberOfInputParameters())
    {
    vtkErrorMacro("Parameter " << n << " is out of range!");
    return 1.0;
    }
  return this->InputParameterInfo[n].NumberSingleStep;
}

//---------------------------------------------------------------------------
bool vtkSlicerDynamicModelerTool::HasRequiredInputs(vtkMRMLDynamicModelerNode* surfaceEditorNode)
{
  for (int i = 0; i < this->GetNumberOfInputNodes(); ++i)
    {
    if (!this->GetNthInputNodeRequired(i))
      {
      continue;
      }

    std::string referenceRole = this->GetNthInputNodeReferenceRole(i);
    if (surfaceEditorNode->GetNodeReference(referenceRole.c_str()) == nullptr)
      {
      return false;
      }
    }
  return true;
}

//---------------------------------------------------------------------------
bool vtkSlicerDynamicModelerTool::HasOutput(vtkMRMLDynamicModelerNode* surfaceEditorNode)
{
  for (int i = 0; i < this->GetNumberOfOutputNodes(); ++i)
    {
    std::string referenceRole = this->GetNthOutputNodeReferenceRole(i);
    if (surfaceEditorNode->GetNodeReference(referenceRole.c_str()) != nullptr)
      {
      return true;
      }
    }
  return false;
}

//---------------------------------------------------------------------------
void vtkSlicerDynamicModelerTool::GetInputNodes(vtkMRMLDynamicModelerNode* surfaceEditorNode, std::vector<vtkMRMLNode*>& inputNodes)
{
  for (int inputIndex = 0; inputIndex < this->GetNumberOfInputNodes(); ++inputIndex)
    {
    std::string referenceRole = this->GetNthInputNodeReferenceRole(inputIndex);
    int numberOfNodeReferences = surfaceEditorNode->GetNumberOfNodeReferences(referenceRole.c_str());
    for (int referenceIndex = 0; referenceIndex < numberOfNodeReferences; ++referenceIndex)
      {
      vtkMRMLNode* inputNode = surfaceEditorNode->GetNthNodeReference(referenceRole.c_str(), referenceIndex);
      if (inputNode)
        {
        inputNodes.push_back(inputNode);
        }
      }
    }
}

//---------------------------------------------------------------------------
void vtkSlicerDynamicModelerTool::GetOutputNodes(vtkMRMLDynamicModelerNode* surfaceEditorNode, std::vector<vtkMRMLNode*>& outputNodes)
{
  for (int outputIndex = 0; outputIndex < this->GetNumberOfOutputNodes(); ++outputIndex)
    {
    std::string referenceRole = this->GetNthOutputNodeReferenceRole(outputIndex);
    int numberOfNodeReferences = surfaceEditorNode->GetNumberOfNodeReferences(referenceRole.c_str());
    for (int referenceIndex = 0; referenceIndex < numberOfNodeReferences; ++referenceIndex)
      {
      vtkMRMLNode* outputNode = surfaceEditorNode->GetNthNodeReference(referenceRole.c_str(), referenceIndex);
      if (outputNode)
        {
        outputNodes.push_back(outputNode);
        }
      }
    }
}

//---------------------------------------------------------------------------
void vtkSlicerDynamicModelerTool::CreateOutputDisplayNodes(vtkMRMLDynamicModelerNode* surfaceEditorNode)
{
  if (!surfaceEditorNode)
    {
    vtkErrorMacro("Invalid surfaceEditorNode!");
    return;
    }

  std::vector<vtkMRMLNode*> inputNodes;
  this->GetInputNodes(surfaceEditorNode, inputNodes);

  std::vector<vtkMRMLNode*> outputNodes;
  this->GetOutputNodes(surfaceEditorNode, outputNodes);

  for (vtkMRMLNode* outputNode : outputNodes)
    {
    vtkMRMLDisplayableNode* outputDisplayableNode = vtkMRMLDisplayableNode::SafeDownCast(outputNode);
    if (outputDisplayableNode == nullptr || outputDisplayableNode->GetDisplayNode())
      {
      continue;
      }

    outputDisplayableNode->CreateDefaultDisplayNodes();
    vtkMRMLDisplayNode* outputDisplayNode = outputDisplayableNode->GetDisplayNode();
    if (outputDisplayNode == nullptr)
      {
      continue;
      }

    for (vtkMRMLNode* inputNode : inputNodes)
      {
      vtkMRMLDisplayableNode* inputDisplayableNode = vtkMRMLDisplayableNode::SafeDownCast(inputNode);
      if (inputDisplayableNode == nullptr || !inputDisplayableNode->IsA(outputNode->GetClassName()))
        {
        continue;
        }

      vtkMRMLDisplayNode* inputDisplayNode = inputDisplayableNode->GetDisplayNode();
      if (!inputDisplayNode)
        {
        continue;
        }
      outputDisplayNode->CopyContent(inputDisplayNode);
      break;
      }
    }
}

//---------------------------------------------------------------------------
bool vtkSlicerDynamicModelerTool::Run(vtkMRMLDynamicModelerNode* surfaceEditorNode)
{
  if (!this->HasRequiredInputs(surfaceEditorNode))
    {
    vtkErrorMacro("Input node missing!");
    return false;
    }
  if (!this->HasOutput(surfaceEditorNode))
    {
    vtkErrorMacro("Output node missing!");
    return false;
    }

  this->CreateOutputDisplayNodes(surfaceEditorNode);
  return this->RunInternal(surfaceEditorNode);
}
