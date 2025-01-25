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

#ifndef __vtkSlicerDynamicModelerRevolveTool_h
#define __vtkSlicerDynamicModelerRevolveTool_h

#include "vtkSlicerDynamicModelerModuleLogicExport.h"

class vtkMRMLDynamicModelerNode;

// MRML includes
#include <vtkMRMLModelNode.h>
#include <vtkMRMLTransformNode.h>

// VTK includes
#include <vtkCommand.h>
#include <vtkGeneralTransform.h>
#include <vtkIntArray.h>
#include <vtkPolyDataNormals.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkFeatureEdges.h>
#include <vtkRotationalExtrusionFilter.h>
#include <vtkAppendPolyData.h>

#include "vtkSlicerDynamicModelerTool.h"

/// \brief Dynamic modelling tool to revolve an open surface.
///
class VTK_SLICER_DYNAMICMODELER_MODULE_LOGIC_EXPORT vtkSlicerDynamicModelerRevolveTool : public vtkSlicerDynamicModelerTool
{
public:
  static vtkSlicerDynamicModelerRevolveTool* New();
  vtkSlicerDynamicModelerTool* CreateToolInstance() override;
  vtkTypeMacro(vtkSlicerDynamicModelerRevolveTool, vtkSlicerDynamicModelerTool);

  /// Human-readable name of the mesh modification tool
  const char* GetName() override;

  /// Run the faces selection on the input model node
  bool RunInternal(vtkMRMLDynamicModelerNode* surfaceEditorNode) override;

protected:

  vtkSlicerDynamicModelerRevolveTool();
  ~vtkSlicerDynamicModelerRevolveTool() override;
  void operator=(const vtkSlicerDynamicModelerRevolveTool&);

protected:
  vtkSmartPointer<vtkTransformPolyDataFilter> InputProfileToWorldTransformFilter;
  vtkSmartPointer<vtkGeneralTransform> InputProfileNodeToWorldTransform;

  vtkSmartPointer<vtkTransformPolyDataFilter> ModelingTransformFilter;
  vtkSmartPointer<vtkTransform> ModelingTransform;

  vtkSmartPointer<vtkFeatureEdges> BoundaryEdgesFilter;

  vtkSmartPointer<vtkTransformPolyDataFilter> CapTransformFilter;
  vtkSmartPointer<vtkTransform> CapTransform;

  vtkSmartPointer<vtkRotationalExtrusionFilter> RevolveFilter;

  vtkSmartPointer<vtkAppendPolyData> AppendFilter;

  vtkSmartPointer<vtkTransformPolyDataFilter> ResamplingTransformFilter;
  vtkSmartPointer<vtkTransform> ResamplingTransform;

  vtkSmartPointer<vtkTransformPolyDataFilter> OutputModelToWorldTransformFilter;
  vtkSmartPointer<vtkGeneralTransform>        OutputWorldToModelTransform;

private:
  vtkSlicerDynamicModelerRevolveTool(const vtkSlicerDynamicModelerRevolveTool&) = delete;
};

#endif // __vtkSlicerDynamicModelerRevolveTool_h
