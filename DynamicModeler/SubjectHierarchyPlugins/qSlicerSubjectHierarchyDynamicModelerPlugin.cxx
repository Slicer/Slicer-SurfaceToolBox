/*==============================================================================

  Program: 3D Slicer

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

// SubjectHierarchy MRML includes
#include "vtkMRMLSubjectHierarchyNode.h"
#include "vtkMRMLSubjectHierarchyConstants.h"

// SubjectHierarchy Plugins includes
#include "qSlicerSubjectHierarchyPluginHandler.h"
#include "qSlicerSubjectHierarchyDynamicModelerPlugin.h"
#include "qSlicerSubjectHierarchyDefaultPlugin.h"

// DynamicModeler logic includes
#include "vtkSlicerDynamicModelerAppendTool.h"
#include "vtkSlicerDynamicModelerBoundaryCutTool.h"
#include "vtkSlicerDynamicModelerCurveCutTool.h"
#include "vtkSlicerDynamicModelerExtrudeTool.h"
#include "vtkSlicerDynamicModelerRevolveTool.h"
#include "vtkSlicerDynamicModelerHollowTool.h"
#include "vtkSlicerDynamicModelerLogic.h"
#include "vtkSlicerDynamicModelerMarginTool.h"
#include "vtkSlicerDynamicModelerMirrorTool.h"
#include "vtkSlicerDynamicModelerPlaneCutTool.h"
#include "vtkSlicerDynamicModelerROICutTool.h"
#include "vtkSlicerDynamicModelerSelectByPointsTool.h"

// DynamicModeler MRML includes
#include <vtkMRMLDynamicModelerNode.h>

// Terminologies includes
#include "qSlicerTerminologyItemDelegate.h"
#include "vtkSlicerTerminologiesModuleLogic.h"

// MRML widgets includes
#include "qMRMLNodeComboBox.h"

// MRML includes
#include <vtkMRMLDisplayableNode.h>
#include <vtkMRMLDisplayNode.h>
#include <vtkMRMLScene.h>

// vtkSegmentationCore includes
#include <vtkSegment.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

// Qt includes
#include <QDebug>
#include <QInputDialog>
#include <QStandardItem>
#include <QAction>

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_SubjectHierarchy_Plugins
class qSlicerSubjectHierarchyDynamicModelerPluginPrivate: public QObject
{
  Q_DECLARE_PUBLIC(qSlicerSubjectHierarchyDynamicModelerPlugin);
protected:
  qSlicerSubjectHierarchyDynamicModelerPlugin* const q_ptr;
public:
  qSlicerSubjectHierarchyDynamicModelerPluginPrivate(qSlicerSubjectHierarchyDynamicModelerPlugin& object);
  ~qSlicerSubjectHierarchyDynamicModelerPluginPrivate() override;
  void init();

public:
  QAction* ContinuousUpdateAction { nullptr };
  QAction* UpdateAction{ nullptr };

  vtkSlicerDynamicModelerLogic* DynamicModelerLogic{ nullptr };
};

//-----------------------------------------------------------------------------
// qSlicerSubjectHierarchyDynamicModelerPluginPrivate methods

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyDynamicModelerPluginPrivate::qSlicerSubjectHierarchyDynamicModelerPluginPrivate(qSlicerSubjectHierarchyDynamicModelerPlugin& object)
: q_ptr(&object)
{
}

//------------------------------------------------------------------------------
void qSlicerSubjectHierarchyDynamicModelerPluginPrivate::init()
{
  Q_Q(qSlicerSubjectHierarchyDynamicModelerPlugin);

  this->ContinuousUpdateAction = new QAction("Continuous update", q);
  this->ContinuousUpdateAction->setCheckable(true);
  QObject::connect(this->ContinuousUpdateAction, SIGNAL(triggered(bool)), q, SLOT(continuousUpdateChanged()));

  this->UpdateAction = new QAction("Update", q);
  QObject::connect(this->UpdateAction, SIGNAL(triggered(bool)), q, SLOT(updateTriggered()));
}

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyDynamicModelerPluginPrivate::~qSlicerSubjectHierarchyDynamicModelerPluginPrivate()
= default;

//-----------------------------------------------------------------------------
// qSlicerSubjectHierarchyDynamicModelerPlugin methods

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyDynamicModelerPlugin::qSlicerSubjectHierarchyDynamicModelerPlugin(QObject* parent)
 : Superclass(parent)
 , d_ptr( new qSlicerSubjectHierarchyDynamicModelerPluginPrivate(*this) )
{
  this->m_Name = QString("DynamicModeler");

  Q_D(qSlicerSubjectHierarchyDynamicModelerPlugin);
  d->init();
}

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyDynamicModelerPlugin::~qSlicerSubjectHierarchyDynamicModelerPlugin()
= default;

//----------------------------------------------------------------------------
double qSlicerSubjectHierarchyDynamicModelerPlugin::canAddNodeToSubjectHierarchy(
  vtkMRMLNode* node, vtkIdType parentItemID/*=vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID*/)const
{
  Q_UNUSED(parentItemID);
  if (!node)
    {
    qCritical() << Q_FUNC_INFO << ": Input node is NULL";
    return 0.0;
    }
  else if (node->IsA("vtkMRMLDynamicModelerNode"))
    {
    return 0.5;
    }
  return 0.0;
}

//---------------------------------------------------------------------------
double qSlicerSubjectHierarchyDynamicModelerPlugin::canOwnSubjectHierarchyItem(vtkIdType itemID)const
{
  if (itemID == vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid input item";
    return 0.0;
    }
  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return 0.0;
    }

  vtkMRMLNode* associatedNode = shNode->GetItemDataNode(itemID);
  if (associatedNode && associatedNode->IsA("vtkMRMLDynamicModelerNode"))
    {
    return 0.5;
    }

  return 0.0;
}

//---------------------------------------------------------------------------
const QString qSlicerSubjectHierarchyDynamicModelerPlugin::roleForPlugin()const
{
  return "DynamicModeler";
}

//---------------------------------------------------------------------------
QIcon qSlicerSubjectHierarchyDynamicModelerPlugin::icon(vtkIdType itemID)
{
  if (itemID == vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid input item";
    return QIcon();
    }

  Q_D(qSlicerSubjectHierarchyDynamicModelerPlugin);

  if (!this->canOwnSubjectHierarchyItem(itemID))
    {
    // Item unknown by plugin
    return QIcon();
    }
  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return QIcon();
    }

  vtkMRMLDynamicModelerNode* associatedNode = vtkMRMLDynamicModelerNode::SafeDownCast(shNode->GetItemDataNode(itemID));
  if (!associatedNode || !associatedNode->GetToolName())
    {
    return QIcon();
    }

  vtkNew<vtkSlicerDynamicModelerPlaneCutTool> planeCutTool;
  if (strcmp(associatedNode->GetToolName(), planeCutTool->GetName()) == 0)
    {
    return QIcon(":Icons/PlaneCut.png");
    }

  vtkNew<vtkSlicerDynamicModelerCurveCutTool> curveCutTool;
  if (strcmp(associatedNode->GetToolName(), curveCutTool->GetName()) == 0)
    {
    return QIcon(":Icons/CurveCut.png");
    }

  vtkNew<vtkSlicerDynamicModelerBoundaryCutTool> boundaryCutTool;
  if (strcmp(associatedNode->GetToolName(), boundaryCutTool->GetName()) == 0)
    {
    return QIcon(":Icons/BoundaryCut.png");
    }

  vtkNew<vtkSlicerDynamicModelerMirrorTool> mirrorTool;
  if (strcmp(associatedNode->GetToolName(), mirrorTool->GetName()) == 0)
    {
    return QIcon(":Icons/Mirror.png");
    }

  vtkNew<vtkSlicerDynamicModelerExtrudeTool> extrudeTool;
  if (strcmp(associatedNode->GetToolName(), extrudeTool->GetName()) == 0)
    {
    return QIcon(":Icons/Extrude.png");
    }

  vtkNew<vtkSlicerDynamicModelerRevolveTool> revolveTool;
  if (strcmp(associatedNode->GetToolName(), revolveTool->GetName()) == 0)
    {
    return QIcon(":Icons/Revolve.png");
    }

  vtkNew<vtkSlicerDynamicModelerHollowTool> hollowTool;
  if (strcmp(associatedNode->GetToolName(), hollowTool->GetName()) == 0)
    {
    return QIcon(":Icons/Hollow.png");
    }

  vtkNew<vtkSlicerDynamicModelerMarginTool> marginTool;
  if (strcmp(associatedNode->GetToolName(), marginTool->GetName()) == 0)
    {
    return QIcon(":Icons/Margin.png");
    }

  vtkNew<vtkSlicerDynamicModelerAppendTool> appendTool;
  if (strcmp(associatedNode->GetToolName(), appendTool->GetName()) == 0)
    {
      return QIcon(":Icons/Append.png");
    }

  vtkNew<vtkSlicerDynamicModelerROICutTool> roiCutTool;
  if (strcmp(associatedNode->GetToolName(), roiCutTool->GetName()) == 0)
    {
      return QIcon(":Icons/ROICut.png");
    }

  vtkNew<vtkSlicerDynamicModelerSelectByPointsTool> selectByPointsTool;
  if (strcmp(associatedNode->GetToolName(), selectByPointsTool->GetName()) == 0)
    {
      return QIcon(":Icons/SelectByPoints.png");
    }

  return QIcon();
}

//-----------------------------------------------------------------------------
QList<QAction*> qSlicerSubjectHierarchyDynamicModelerPlugin::itemContextMenuActions()const
{
  Q_D(const qSlicerSubjectHierarchyDynamicModelerPlugin);

  QList<QAction*> actions;
  actions << d->ContinuousUpdateAction << d->UpdateAction;
  return actions;
}

//-----------------------------------------------------------------------------
void qSlicerSubjectHierarchyDynamicModelerPlugin::showContextMenuActionsForItem(vtkIdType itemID)
{
  Q_D(qSlicerSubjectHierarchyDynamicModelerPlugin);

  if (itemID == vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid input item";
    return;
    }
  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return;
    }

  vtkMRMLDynamicModelerNode* associatedNode = vtkMRMLDynamicModelerNode::SafeDownCast(shNode->GetItemDataNode(itemID));
  if (associatedNode)
    {
    d->ContinuousUpdateAction->setVisible(true);
    d->ContinuousUpdateAction->setChecked(associatedNode->GetContinuousUpdate());

    d->UpdateAction->setVisible(true);
    }
}

//-----------------------------------------------------------------------------
void qSlicerSubjectHierarchyDynamicModelerPlugin::continuousUpdateChanged()
{
  Q_D(qSlicerSubjectHierarchyDynamicModelerPlugin);

  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return;
    }
  vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->mrmlScene();
  if (!scene)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid MRML scene";
    return;
    }
  vtkIdType currentItemID = qSlicerSubjectHierarchyPluginHandler::instance()->currentItem();
  vtkMRMLNode* node = shNode->GetItemDataNode(currentItemID);
  vtkMRMLDynamicModelerNode* dynamicModelerNode = vtkMRMLDynamicModelerNode::SafeDownCast(node);
  if (!dynamicModelerNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to get DynamicModeler node by item ID " << currentItemID;
    return;
    }

  dynamicModelerNode->SetContinuousUpdate(!dynamicModelerNode->GetContinuousUpdate());
}

//-----------------------------------------------------------------------------
void qSlicerSubjectHierarchyDynamicModelerPlugin::updateTriggered()
{
  Q_D(qSlicerSubjectHierarchyDynamicModelerPlugin);

  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return;
    }
  vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->mrmlScene();
  if (!scene)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid MRML scene";
    return;
    }
  vtkIdType currentItemID = qSlicerSubjectHierarchyPluginHandler::instance()->currentItem();
  vtkMRMLNode* node = shNode->GetItemDataNode(currentItemID);
  vtkMRMLDynamicModelerNode* dynamicModelerNode = vtkMRMLDynamicModelerNode::SafeDownCast(node);
  if (!dynamicModelerNode)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid dynamic modeler node";
    return;
    }
  if (!d->DynamicModelerLogic)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid dynamic modeler logic";
    return;
    }

  d->DynamicModelerLogic->RunDynamicModelerTool(dynamicModelerNode);
}

//-----------------------------------------------------------------------------
void qSlicerSubjectHierarchyDynamicModelerPlugin::setDynamicModelerLogic(vtkSlicerDynamicModelerLogic* dynamicModelerLogic)
{
  Q_D(qSlicerSubjectHierarchyDynamicModelerPlugin);
  d->DynamicModelerLogic = dynamicModelerLogic;
}
