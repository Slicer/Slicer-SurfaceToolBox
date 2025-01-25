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

// DynamicModeler includes
#include "vtkSlicerDynamicModelerAppendTool.h"
#include "vtkSlicerDynamicModelerBoundaryCutTool.h"
#include "vtkSlicerDynamicModelerCurveCutTool.h"
#include "vtkSlicerDynamicModelerExtrudeTool.h"
#include "vtkSlicerDynamicModelerRevolveTool.h"
#include "vtkSlicerDynamicModelerHollowTool.h"
#include "vtkSlicerDynamicModelerMarginTool.h"
#include "vtkSlicerDynamicModelerMirrorTool.h"
#include "vtkSlicerDynamicModelerPlaneCutTool.h"
#include "vtkSlicerDynamicModelerROICutTool.h"
#include "vtkSlicerDynamicModelerSelectByPointsTool.h"
#include "vtkSlicerDynamicModelerToolFactory.h"
#include "vtkSlicerDynamicModelerTool.h"

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkDataObject.h>

// STD includes
#include <algorithm>

//----------------------------------------------------------------------------
// The compression tool manager singleton.
// This MUST be default initialized to zero by the compiler and is
// therefore not initialized here.  The ClassInitialize and ClassFinalize methods handle this instance.
static vtkSlicerDynamicModelerToolFactory* vtkSlicerDynamicModelerToolFactoryInstance;


//----------------------------------------------------------------------------
// Must NOT be initialized.  Default initialization to zero is necessary.
unsigned int vtkSlicerDynamicModelerToolFactoryInitialize::Count;

//----------------------------------------------------------------------------
// Implementation of vtkSlicerDynamicModelerToolFactoryInitialize class.
//----------------------------------------------------------------------------
vtkSlicerDynamicModelerToolFactoryInitialize::vtkSlicerDynamicModelerToolFactoryInitialize()
{
  if (++Self::Count == 1)
    {
    vtkSlicerDynamicModelerToolFactory::classInitialize();
    }
}

//----------------------------------------------------------------------------
vtkSlicerDynamicModelerToolFactoryInitialize::~vtkSlicerDynamicModelerToolFactoryInitialize()
{
  if (--Self::Count == 0)
    {
    vtkSlicerDynamicModelerToolFactory::classFinalize();
    }
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Up the reference count so it behaves like New
vtkSlicerDynamicModelerToolFactory* vtkSlicerDynamicModelerToolFactory::New()
{
  vtkSlicerDynamicModelerToolFactory* ret = vtkSlicerDynamicModelerToolFactory::GetInstance();
  ret->Register(nullptr);
  return ret;
}

//----------------------------------------------------------------------------
// Return the single instance of the vtkSlicerDynamicModelerToolFactory
vtkSlicerDynamicModelerToolFactory* vtkSlicerDynamicModelerToolFactory::GetInstance()
{
  if (!vtkSlicerDynamicModelerToolFactoryInstance)
    {
    // Try the factory first
    vtkSlicerDynamicModelerToolFactoryInstance = (vtkSlicerDynamicModelerToolFactory*)vtkObjectFactory::CreateInstance("vtkSlicerDynamicModelerToolFactory");
    // if the factory did not provide one, then create it here
    if (!vtkSlicerDynamicModelerToolFactoryInstance)
      {
      vtkSlicerDynamicModelerToolFactoryInstance = new vtkSlicerDynamicModelerToolFactory;
#ifdef VTK_HAS_INITIALIZE_OBJECT_BASE
      vtkSlicerDynamicModelerToolFactoryInstance->InitializeObjectBase();
#endif
      }
    }
  // return the instance
  return vtkSlicerDynamicModelerToolFactoryInstance;
}

//----------------------------------------------------------------------------
vtkSlicerDynamicModelerToolFactory::vtkSlicerDynamicModelerToolFactory()
= default;

//----------------------------------------------------------------------------
vtkSlicerDynamicModelerToolFactory::~vtkSlicerDynamicModelerToolFactory()
= default;

//----------------------------------------------------------------------------
void vtkSlicerDynamicModelerToolFactory::PrintSelf(ostream & os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkSlicerDynamicModelerToolFactory::classInitialize()
{
  // Allocate the singleton
  vtkSlicerDynamicModelerToolFactoryInstance = vtkSlicerDynamicModelerToolFactory::GetInstance();

  vtkSlicerDynamicModelerToolFactoryInstance->RegisterDynamicModelerTool(vtkSmartPointer<vtkSlicerDynamicModelerPlaneCutTool>::New());
  vtkSlicerDynamicModelerToolFactoryInstance->RegisterDynamicModelerTool(vtkSmartPointer<vtkSlicerDynamicModelerExtrudeTool>::New());
  vtkSlicerDynamicModelerToolFactoryInstance->RegisterDynamicModelerTool(vtkSmartPointer<vtkSlicerDynamicModelerRevolveTool>::New());
  vtkSlicerDynamicModelerToolFactoryInstance->RegisterDynamicModelerTool(vtkSmartPointer<vtkSlicerDynamicModelerHollowTool>::New());
  vtkSlicerDynamicModelerToolFactoryInstance->RegisterDynamicModelerTool(vtkSmartPointer<vtkSlicerDynamicModelerMarginTool>::New());
  vtkSlicerDynamicModelerToolFactoryInstance->RegisterDynamicModelerTool(vtkSmartPointer<vtkSlicerDynamicModelerMirrorTool>::New());
  vtkSlicerDynamicModelerToolFactoryInstance->RegisterDynamicModelerTool(vtkSmartPointer<vtkSlicerDynamicModelerCurveCutTool>::New());
  vtkSlicerDynamicModelerToolFactoryInstance->RegisterDynamicModelerTool(vtkSmartPointer<vtkSlicerDynamicModelerBoundaryCutTool>::New());
  vtkSlicerDynamicModelerToolFactoryInstance->RegisterDynamicModelerTool(vtkSmartPointer<vtkSlicerDynamicModelerAppendTool>::New());
  vtkSlicerDynamicModelerToolFactoryInstance->RegisterDynamicModelerTool(vtkSmartPointer<vtkSlicerDynamicModelerROICutTool>::New());
  vtkSlicerDynamicModelerToolFactoryInstance->RegisterDynamicModelerTool(vtkSmartPointer<vtkSlicerDynamicModelerSelectByPointsTool>::New());
}

//----------------------------------------------------------------------------
void vtkSlicerDynamicModelerToolFactory::classFinalize()
{
  vtkSlicerDynamicModelerToolFactoryInstance->Delete();
  vtkSlicerDynamicModelerToolFactoryInstance = nullptr;
}

//----------------------------------------------------------------------------
bool vtkSlicerDynamicModelerToolFactory::RegisterDynamicModelerTool(vtkSmartPointer<vtkSlicerDynamicModelerTool> tool)
{
  for (unsigned int i = 0; i < this->RegisteredTools.size(); ++i)
    {
    if (strcmp(this->RegisteredTools[i]->GetClassName(), tool->GetClassName()) == 0)
      {
      vtkWarningMacro("RegisterStreamingCodec failed: tool is already registered");
      return false;
      }
    }
  this->RegisteredTools.push_back(tool);
  return true;
}

//----------------------------------------------------------------------------
bool vtkSlicerDynamicModelerToolFactory::UnregisterDynamicModelerToolByClassName(const std::string & className)
{
  std::vector<vtkSmartPointer<vtkSlicerDynamicModelerTool> >::iterator toolIt;
  for (toolIt = this->RegisteredTools.begin(); toolIt != this->RegisteredTools.end(); ++toolIt)
    {
    vtkSmartPointer<vtkSlicerDynamicModelerTool> tool = *toolIt;
    if (strcmp(tool->GetClassName(), className.c_str()) == 0)
      {
      this->RegisteredTools.erase(toolIt);
      return true;
      }
    }
  vtkWarningMacro("UnRegisterStreamingCodecByClassName failed: tool not found");
  return false;
}

//----------------------------------------------------------------------------
vtkSlicerDynamicModelerTool* vtkSlicerDynamicModelerToolFactory::CreateToolByClassName(const std::string & className)
{
  std::vector<vtkSmartPointer<vtkSlicerDynamicModelerTool> >::iterator toolIt;
  for (toolIt = this->RegisteredTools.begin(); toolIt != this->RegisteredTools.end(); ++toolIt)
    {
    vtkSmartPointer<vtkSlicerDynamicModelerTool> tool = *toolIt;
    if (strcmp(tool->GetClassName(), className.c_str()) == 0)
      {
      return tool->CreateToolInstance();
      }
    }
  return nullptr;
}

//----------------------------------------------------------------------------
vtkSlicerDynamicModelerTool* vtkSlicerDynamicModelerToolFactory::CreateToolByName(const std::string name)
{
  std::vector<vtkSmartPointer<vtkSlicerDynamicModelerTool> >::iterator toolIt;
  for (toolIt = this->RegisteredTools.begin(); toolIt != this->RegisteredTools.end(); ++toolIt)
    {
    vtkSmartPointer<vtkSlicerDynamicModelerTool> tool = *toolIt;
    if (tool->GetName() == name)
      {
      return tool->CreateToolInstance();
      }
    }
  return nullptr;
}

//----------------------------------------------------------------------------
const std::vector<std::string> vtkSlicerDynamicModelerToolFactory::GetDynamicModelerToolClassNames()
{
  std::vector<std::string> toolClassNames;
  std::vector<vtkSmartPointer<vtkSlicerDynamicModelerTool> >::iterator toolIt;
  for (toolIt = this->RegisteredTools.begin(); toolIt != this->RegisteredTools.end(); ++toolIt)
    {
    vtkSmartPointer<vtkSlicerDynamicModelerTool> tool = *toolIt;
    toolClassNames.emplace_back(tool->GetClassName());
    }
  return toolClassNames;
}

//----------------------------------------------------------------------------
const std::vector<std::string> vtkSlicerDynamicModelerToolFactory::GetDynamicModelerToolNames()
{
  std::vector<std::string> names;
  std::vector<vtkSmartPointer<vtkSlicerDynamicModelerTool> >::iterator toolIt;
  for (toolIt = this->RegisteredTools.begin(); toolIt != this->RegisteredTools.end(); ++toolIt)
    {
    vtkSmartPointer<vtkSlicerDynamicModelerTool> tool = *toolIt;
    names.push_back(tool->GetName());
    }
  return names;
}
