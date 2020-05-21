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

#ifndef __vtkSlicerDynamicModelerCurveCutRule_h
#define __vtkSlicerDynamicModelerCurveCutRule_h

#include "vtkSlicerDynamicModelerModuleLogicExport.h"

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

// STD includes
#include <map>
#include <string>
#include <vector>

class vtkCleanPolyData;
class vtkClipPolyData;
class vtkConnectivityFilter;
class vtkGeneralTransform;
class vtkMRMLDynamicModelerNode;
class vtkTransformPolyDataFilter;
class vtkSelectPolyData;

#include "vtkSlicerDynamicModelerRule.h"

/// \brief Dynamic modelling rule for cutting a single surface mesh with planes
///
/// Has two node inputs (Plane and Surface), and two outputs (Positive/Negative direction surface segments)
class VTK_SLICER_DYNAMICMODELER_MODULE_LOGIC_EXPORT vtkSlicerDynamicModelerCurveCutRule : public vtkSlicerDynamicModelerRule
{
public:
  static vtkSlicerDynamicModelerCurveCutRule* New();
  vtkSlicerDynamicModelerRule* CreateRuleInstance() override;
  vtkTypeMacro(vtkSlicerDynamicModelerCurveCutRule, vtkSlicerDynamicModelerRule);

  /// Human-readable name of the mesh modification rule
  const char* GetName() override;

  /// Run the plane cut on the input model node
  bool RunInternal(vtkMRMLDynamicModelerNode* surfaceEditorNode) override;

protected:
  vtkSlicerDynamicModelerCurveCutRule();
  ~vtkSlicerDynamicModelerCurveCutRule() override;
  void operator=(const vtkSlicerDynamicModelerCurveCutRule&);

protected:
  vtkSmartPointer<vtkGeneralTransform>        InputModelToWorldTransform;
  vtkSmartPointer<vtkTransformPolyDataFilter> InputModelToWorldTransformFilter;

  vtkSmartPointer<vtkSelectPolyData>          SelectionFilter;
  vtkSmartPointer<vtkClipPolyData>            ClipFilter;
  vtkSmartPointer<vtkConnectivityFilter>      ConnectivityFilter;
  vtkSmartPointer<vtkCleanPolyData>           CleanFilter;

  vtkSmartPointer<vtkGeneralTransform>        OutputWorldToModelTransform;
  vtkSmartPointer<vtkTransformPolyDataFilter> OutputWorldToModelTransformFilter;
};

#endif // __vtkSlicerDynamicModelerCurveCutRule_h
