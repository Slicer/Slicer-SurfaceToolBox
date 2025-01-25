/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

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

// Qt includes
#include <QCheckBox>
#include <QDebug>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>

// ctk includes
#include <ctkDoubleSpinBox.h>

// SlicerQt includes
#include <qMRMLNodeComboBox.h>

// DynamicModeler Module includes
#include "qSlicerDynamicModelerModuleWidget.h"
#include "ui_qSlicerDynamicModelerModuleWidget.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLNode.h>

// VTK includes
#include <vtkStringArray.h>

// DynamicModeler Logic includes
#include <vtkSlicerDynamicModelerAppendTool.h>
#include <vtkSlicerDynamicModelerBoundaryCutTool.h>
#include <vtkSlicerDynamicModelerCurveCutTool.h>
#include <vtkSlicerDynamicModelerExtrudeTool.h>
#include <vtkSlicerDynamicModelerRevolveTool.h>
#include <vtkSlicerDynamicModelerHollowTool.h>
#include <vtkSlicerDynamicModelerMarginTool.h>
#include <vtkSlicerDynamicModelerLogic.h>
#include <vtkSlicerDynamicModelerMirrorTool.h>
#include <vtkSlicerDynamicModelerPlaneCutTool.h>
#include <vtkSlicerDynamicModelerROICutTool.h>
#include <vtkSlicerDynamicModelerSelectByPointsTool.h>
#include <vtkSlicerDynamicModelerToolFactory.h>

// DynamicModeler MRML includes
#include <vtkMRMLDynamicModelerNode.h>

// Subject hierarchy includes
#include <qMRMLSubjectHierarchyModel.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerDynamicModelerModuleWidgetPrivate: public Ui_qSlicerDynamicModelerModuleWidget
{
public:
  qSlicerDynamicModelerModuleWidgetPrivate();
  vtkWeakPointer<vtkMRMLDynamicModelerNode> DynamicModelerNode{ nullptr };

  std::string CurrentToolName;
};

//-----------------------------------------------------------------------------
// qSlicerDynamicModelerModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerDynamicModelerModuleWidgetPrivate::qSlicerDynamicModelerModuleWidgetPrivate()
= default;

//-----------------------------------------------------------------------------
// qSlicerDynamicModelerModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerDynamicModelerModuleWidget::qSlicerDynamicModelerModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerDynamicModelerModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerDynamicModelerModuleWidget::~qSlicerDynamicModelerModuleWidget()
= default;

//-----------------------------------------------------------------------------
void qSlicerDynamicModelerModuleWidget::setup()
{
  Q_D(qSlicerDynamicModelerModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  d->SubjectHierarchyTreeView->setMultiSelection(false);
  d->SubjectHierarchyTreeView->setColumnHidden(d->SubjectHierarchyTreeView->model()->idColumn(), true);
  d->SubjectHierarchyTreeView->setColumnHidden(d->SubjectHierarchyTreeView->model()->colorColumn(), true);
  d->SubjectHierarchyTreeView->setColumnHidden(d->SubjectHierarchyTreeView->model()->transformColumn(), true);
  d->SubjectHierarchyTreeView->setColumnHidden(d->SubjectHierarchyTreeView->model()->descriptionColumn(), true);

  int buttonPosition = 1;
  const int columns = 5;

  vtkNew<vtkSlicerDynamicModelerPlaneCutTool> planeCutTool;
  this->addToolButton(QIcon(":/Icons/PlaneCut.png"), planeCutTool, buttonPosition / columns, buttonPosition % columns);
  buttonPosition++;

  vtkNew<vtkSlicerDynamicModelerCurveCutTool> curveCutTool;
  this->addToolButton(QIcon(":/Icons/CurveCut.png"), curveCutTool, buttonPosition / columns, buttonPosition % columns);
  buttonPosition++;

  vtkNew<vtkSlicerDynamicModelerBoundaryCutTool> boundaryCutTool;
  this->addToolButton(QIcon(":/Icons/BoundaryCut.png"), boundaryCutTool, buttonPosition / columns, buttonPosition % columns);
  buttonPosition++;

  vtkNew<vtkSlicerDynamicModelerExtrudeTool> extrudeTool;
  this->addToolButton(QIcon(":/Icons/Extrude.png"), extrudeTool, buttonPosition / columns, buttonPosition % columns);
  buttonPosition++;

  vtkNew<vtkSlicerDynamicModelerRevolveTool> revolveTool;
  this->addToolButton(QIcon(":/Icons/Revolve.png"), revolveTool, buttonPosition / columns, buttonPosition % columns);
  buttonPosition++;

  vtkNew<vtkSlicerDynamicModelerHollowTool> hollowTool;
  this->addToolButton(QIcon(":/Icons/Hollow.png"), hollowTool, buttonPosition / columns, buttonPosition % columns);
  buttonPosition++;

  vtkNew<vtkSlicerDynamicModelerMarginTool> marginTool;
  this->addToolButton(QIcon(":/Icons/Margin.png"), marginTool, buttonPosition / columns, buttonPosition % columns);
  buttonPosition++;

  vtkNew<vtkSlicerDynamicModelerMirrorTool> mirrorTool;
  this->addToolButton(QIcon(":/Icons/Mirror.png"), mirrorTool, buttonPosition / columns, buttonPosition % columns);
  buttonPosition++;

  vtkNew<vtkSlicerDynamicModelerAppendTool> appendTool;
  this->addToolButton(QIcon(":/Icons/Append.png"), appendTool, buttonPosition / columns, buttonPosition % columns);
  buttonPosition++;

  vtkNew<vtkSlicerDynamicModelerROICutTool> roiTool;
  this->addToolButton(QIcon(":/Icons/ROICut.png"), roiTool, buttonPosition / columns, buttonPosition % columns);
  buttonPosition++;

  vtkNew<vtkSlicerDynamicModelerSelectByPointsTool> selectByPointsTool;
  this->addToolButton(QIcon(":/Icons/SelectByPoints.png"), selectByPointsTool, buttonPosition / columns, buttonPosition % columns);
  buttonPosition++;

  connect(d->SubjectHierarchyTreeView, SIGNAL(currentItemChanged(vtkIdType)),
    this, SLOT(onParameterNodeChanged()));
  connect(d->ApplyButton, SIGNAL(checkStateChanged(Qt::CheckState)),
    this, SLOT(onApplyButtonClicked()));
  connect(d->ApplyButton, SIGNAL(clicked()),
    this, SLOT(onApplyButtonClicked()));
}

//-----------------------------------------------------------------------------
void qSlicerDynamicModelerModuleWidget::addToolButton(QIcon icon, vtkSlicerDynamicModelerTool* tool, int row, int column)
{
  Q_D(qSlicerDynamicModelerModuleWidget);
  if (!tool)
    {
    qCritical() << "Invalid tool object!";
    }

  QPushButton* button = new QPushButton();
  button->setIcon(icon);
  if (tool->GetName())
    {
    button->setToolTip(tool->GetName());
    button->setProperty("ToolName", tool->GetName());
    }
  d->ButtonLayout->addWidget(button, row, column);

  connect(button, SIGNAL(clicked()), this, SLOT(onAddToolClicked()));
}

//-----------------------------------------------------------------------------
void qSlicerDynamicModelerModuleWidget::onAddToolClicked()
{
  Q_D(qSlicerDynamicModelerModuleWidget);

  if (!QObject::sender() || !this->mrmlScene())
    {
    return;
    }

  QString toolName = QObject::sender()->property("ToolName").toString();
  std::string nodeName = this->mrmlScene()->GenerateUniqueName(toolName.toStdString());

  vtkNew<vtkMRMLDynamicModelerNode> dynamicModelerNode;
  dynamicModelerNode->SetName(nodeName.c_str());
  dynamicModelerNode->SetToolName(toolName.toUtf8());
  this->mrmlScene()->AddNode(dynamicModelerNode);
  d->SubjectHierarchyTreeView->setCurrentNode(dynamicModelerNode);
}

//-----------------------------------------------------------------------------
void qSlicerDynamicModelerModuleWidget::onParameterNodeChanged()
{
  Q_D(qSlicerDynamicModelerModuleWidget);
  if (!this->mrmlScene() || this->mrmlScene()->IsBatchProcessing())
    {
    return;
    }

  vtkMRMLDynamicModelerNode* meshModifyNode = vtkMRMLDynamicModelerNode::SafeDownCast(d->SubjectHierarchyTreeView->currentNode());
  qvtkReconnect(d->DynamicModelerNode, meshModifyNode, vtkCommand::ModifiedEvent, this, SLOT(updateWidgetFromMRML()));

  d->DynamicModelerNode = meshModifyNode;
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
bool qSlicerDynamicModelerModuleWidget::isInputWidgetsRebuildRequired()
{
  Q_D(qSlicerDynamicModelerModuleWidget);

  vtkSlicerDynamicModelerLogic* dynamicModelerLogic = vtkSlicerDynamicModelerLogic::SafeDownCast(this->logic());
  vtkSlicerDynamicModelerTool* tool = nullptr;
  if (dynamicModelerLogic && d->DynamicModelerNode)
    {
    tool = dynamicModelerLogic->GetDynamicModelerTool(d->DynamicModelerNode);
    }
  if (!tool)
    {
    return true;
    }

  std::map<std::string, int> numberOfInputWidgetsByReferenceRole;
  QList<qMRMLNodeComboBox*> inputNodeSelectors = d->InputNodesCollapsibleButton->findChildren<qMRMLNodeComboBox*>();
  for (qMRMLNodeComboBox* inputNodeSelector : inputNodeSelectors)
    {
    QString referenceRole = inputNodeSelector->property("ReferenceRole").toString();
    numberOfInputWidgetsByReferenceRole[referenceRole.toStdString()] += 1;
    }

  for (int i = 0; i < tool->GetNumberOfInputNodes(); ++i)
    {
    std::string inputReferenceRole = tool->GetNthInputNodeReferenceRole(i);
    int expectedNumberOfInputWidgets = d->DynamicModelerNode->GetNumberOfNodeReferences(inputReferenceRole.c_str());
    if (tool->GetNthInputNodeRepeatable(i))
      {
      expectedNumberOfInputWidgets += 1;
      }
    int numberOfInputWidgets = numberOfInputWidgetsByReferenceRole[inputReferenceRole];
    if (numberOfInputWidgets != expectedNumberOfInputWidgets)
      {
      // We expected a different number of input widgets.
      // A rebuild is required.
      return true;
      }
    }
  return false;
}

//-----------------------------------------------------------------------------
void qSlicerDynamicModelerModuleWidget::rebuildInputWidgets()
{
  Q_D(qSlicerDynamicModelerModuleWidget);
  vtkSlicerDynamicModelerTool* tool = nullptr;
  vtkSlicerDynamicModelerLogic* meshModifyLogic = vtkSlicerDynamicModelerLogic::SafeDownCast(this->logic());
  if (meshModifyLogic && d->DynamicModelerNode)
    {
    tool = meshModifyLogic->GetDynamicModelerTool(d->DynamicModelerNode);
    }

  QList<QWidget*> widgets = d->InputNodesCollapsibleButton->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
  for (QWidget* widget : widgets)
    {
    widget->deleteLater();
    }

  if (tool == nullptr || tool->GetNumberOfInputNodes() == 0)
    {
    d->InputNodesCollapsibleButton->setEnabled(false);
    return;
    }
  d->InputNodesCollapsibleButton->setEnabled(true);

  QWidget* inputNodesWidget = new QWidget();
  QFormLayout* inputNodesLayout = new QFormLayout();
  inputNodesWidget->setLayout(inputNodesLayout);
  d->InputNodesCollapsibleButton->layout()->addWidget(inputNodesWidget);
  for (int inputIndex = 0; inputIndex < tool->GetNumberOfInputNodes(); ++inputIndex)
    {
    std::string name = tool->GetNthInputNodeName(inputIndex);
    std::string description = tool->GetNthInputNodeDescription(inputIndex);
    std::string referenceRole = tool->GetNthInputNodeReferenceRole(inputIndex);
    vtkStringArray* classNameArray = tool->GetNthInputNodeClassNames(inputIndex);
    QStringList classNames;
    for (int classNameIndex = 0; classNameIndex < classNameArray->GetNumberOfValues(); ++classNameIndex)
      {
      vtkStdString className = classNameArray->GetValue(classNameIndex);
      classNames << className.c_str();
      }

    int numberOfInputs = 1;
    if (tool->GetNthInputNodeRepeatable(inputIndex))
      {
      numberOfInputs = d->DynamicModelerNode->GetNumberOfNodeReferences(referenceRole.c_str()) + 1;
      }

    for (int inputSelectorIndex = 0; inputSelectorIndex < numberOfInputs; ++inputSelectorIndex)
      {
      QLabel* nodeLabel = new QLabel();
      std::stringstream labelTextSS;
      labelTextSS << name;
      if (tool->GetNthInputNodeRepeatable(inputIndex))
        {
        labelTextSS << " [" << inputSelectorIndex + 1 << "]"; // Start index at 1
        }
      labelTextSS << ":";

      std::string labelText = labelTextSS.str();
      nodeLabel->setText(labelText.c_str());
      nodeLabel->setToolTip(description.c_str());

      qMRMLNodeComboBox* nodeSelector = new qMRMLNodeComboBox();
      nodeSelector->setNodeTypes(classNames);
      nodeSelector->setToolTip(description.c_str());
      nodeSelector->setNoneEnabled(true);
      nodeSelector->setMRMLScene(this->mrmlScene());
      nodeSelector->setProperty("ReferenceRole", referenceRole.c_str());
      nodeSelector->setProperty("InputIndex", inputIndex);
      nodeSelector->setProperty("InputSelectorIndex", inputSelectorIndex);
      nodeSelector->setAddEnabled(false);
      nodeSelector->setRemoveEnabled(false);
      nodeSelector->setRenameEnabled(false);

      inputNodesLayout->addRow(nodeLabel, nodeSelector);

      connect(nodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
        this, SLOT(updateMRMLFromWidget()));
      }
    }
}

//-----------------------------------------------------------------------------
void qSlicerDynamicModelerModuleWidget::rebuildParameterWidgets()
{
  Q_D(qSlicerDynamicModelerModuleWidget);
  vtkSlicerDynamicModelerLogic* meshModifyLogic = vtkSlicerDynamicModelerLogic::SafeDownCast(this->logic());
  vtkSlicerDynamicModelerTool* tool = nullptr;
  if (meshModifyLogic && d->DynamicModelerNode)
    {
    tool = meshModifyLogic->GetDynamicModelerTool(d->DynamicModelerNode);
    }

  QList<QWidget*> widgets = d->ParametersCollapsibleButton->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
  for (QWidget* widget : widgets)
    {
    widget->deleteLater();
    }

  if (tool == nullptr || tool->GetNumberOfInputParameters() == 0)
    {
    d->ParametersCollapsibleButton->setEnabled(false);
    d->ParametersCollapsibleButton->setVisible(false);
    return;
    }
  d->ParametersCollapsibleButton->setEnabled(true);
  d->ParametersCollapsibleButton->setVisible(true);

  QWidget* inputParametersWidget = new QWidget();
  QFormLayout* inputParametersLayout = new QFormLayout();
  inputParametersWidget->setLayout(inputParametersLayout);
  d->ParametersCollapsibleButton->layout()->addWidget(inputParametersWidget);
  for (int i = 0; i < tool->GetNumberOfInputParameters(); ++i)
    {
    std::string name = tool->GetNthInputParameterName(i);
    std::string description = tool->GetNthInputParameterDescription(i);
    std::string attributeName = tool->GetNthInputParameterAttributeName(i);
    int type = tool->GetNthInputParameterType(i);

    QLabel* parameterLabel = new QLabel();
    std::stringstream labelTextSS;
    labelTextSS << name << ":";
    std::string labelText = labelTextSS.str();
    parameterLabel->setText(labelText.c_str());
    parameterLabel->setToolTip(description.c_str());

    QWidget* parameterSelector = nullptr;
    if (type == vtkSlicerDynamicModelerTool::PARAMETER_BOOL)
      {
      QCheckBox* checkBox = new QCheckBox();
      parameterSelector = checkBox;
      connect(checkBox, SIGNAL(stateChanged(int)),
        this, SLOT(updateMRMLFromWidget()));
      }
    else if (type == vtkSlicerDynamicModelerTool::PARAMETER_INT)
      {
      QSpinBox* spinBox = new QSpinBox();
      vtkDoubleArray* numbersRange = tool->GetNthInputParameterNumberRange(i);
      spinBox->setMinimum(int(std::round(numbersRange->GetTuple1(0))));
      spinBox->setMaximum(int(std::round(numbersRange->GetTuple1(1))));
      double dNumberSingleStep = tool->GetNthInputParameterNumberSingleStep(i);
      int numberSingleStep = (
        int(std::pow(10,std::floor(std::log10(dNumberSingleStep))))
        );
      spinBox->setSingleStep(numberSingleStep);
      connect(spinBox, SIGNAL(valueChanged(int)),
        this, SLOT(updateMRMLFromWidget()));
      parameterSelector = spinBox;
      }
    else if (type == vtkSlicerDynamicModelerTool::PARAMETER_DOUBLE)
      {
      ctkDoubleSpinBox* doubleSpinBox = new ctkDoubleSpinBox();
      vtkDoubleArray* numbersRange = tool->GetNthInputParameterNumberRange(i);
      doubleSpinBox->setMinimum(numbersRange->GetTuple1(0));
      doubleSpinBox->setMaximum(numbersRange->GetTuple1(1));
      doubleSpinBox->setDecimals(tool->GetNthInputParameterNumberDecimals(i));
      doubleSpinBox->setSingleStep(tool->GetNthInputParameterNumberSingleStep(i));
      connect(doubleSpinBox, SIGNAL(valueChanged(double)),
        this, SLOT(updateMRMLFromWidget()));
      parameterSelector = doubleSpinBox;
      }
    else if (type == vtkSlicerDynamicModelerTool::PARAMETER_STRING_ENUM)
      {
      QComboBox* enumComboBox = new QComboBox();
      vtkStringArray* possibleValues = tool->GetNthInputParameterPossibleValues(i);
      for (int valueIndex = 0; valueIndex < possibleValues->GetNumberOfValues(); ++valueIndex)
        {
        enumComboBox->addItem(QString::fromStdString(possibleValues->GetValue(valueIndex)));
        }
      connect(enumComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(updateMRMLFromWidget()));
      parameterSelector = enumComboBox;
      }
    else
      {
      QLineEdit* lineEdit = new QLineEdit();
      connect(lineEdit, SIGNAL(textChanged(QString)),
          this, SLOT(updateMRMLFromWidget()));
      parameterSelector = lineEdit;
      }

    parameterSelector->setObjectName(attributeName.c_str());
    parameterSelector->setToolTip(description.c_str());
    parameterSelector->setProperty("AttributeName", attributeName.c_str());
    inputParametersLayout->addRow(parameterLabel, parameterSelector);
    }
}

//-----------------------------------------------------------------------------
void qSlicerDynamicModelerModuleWidget::rebuildOutputWidgets()
{
  Q_D(qSlicerDynamicModelerModuleWidget);
  vtkSlicerDynamicModelerLogic* meshModifyLogic = vtkSlicerDynamicModelerLogic::SafeDownCast(this->logic());
  vtkSlicerDynamicModelerTool* tool = nullptr;
  if (meshModifyLogic && d->DynamicModelerNode)
    {
    tool = meshModifyLogic->GetDynamicModelerTool(d->DynamicModelerNode);
    }

  QList<QWidget*> widgets = d->OutputNodesCollapsibleButton->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
  for (QWidget* widget : widgets)
    {
    widget->deleteLater();
    }

  if (tool == nullptr || tool->GetNumberOfOutputNodes() == 0)
    {
    d->OutputNodesCollapsibleButton->setEnabled(false);
    return;
    }
  d->OutputNodesCollapsibleButton->setEnabled(true);

  QWidget* outputNodesWidget = new QWidget();
  QFormLayout* outputNodesLayout = new QFormLayout();
  outputNodesWidget->setLayout(outputNodesLayout);
  d->OutputNodesCollapsibleButton->layout()->addWidget(outputNodesWidget);
  for (int i = 0; i < tool->GetNumberOfOutputNodes(); ++i)
    {
    std::string name = tool->GetNthOutputNodeName(i);
    std::string description = tool->GetNthOutputNodeDescription(i);
    std::string referenceRole = tool->GetNthOutputNodeReferenceRole(i);
    vtkStringArray* classNameArray = tool->GetNthOutputNodeClassNames(i);
    QStringList classNames;
    for (int i = 0; i < classNameArray->GetNumberOfValues(); ++i)
      {
      vtkStdString className = classNameArray->GetValue(i);
      classNames << className.c_str();
      }

    QLabel* nodeLabel = new QLabel();
    std::stringstream labelTextSS;
    labelTextSS << name << ":";
    std::string labelText = labelTextSS.str();
    nodeLabel->setText(labelText.c_str());
    nodeLabel->setToolTip(description.c_str());

    qMRMLNodeComboBox* nodeSelector = new qMRMLNodeComboBox();
    nodeSelector->setNodeTypes(classNames);
    nodeSelector->setToolTip(description.c_str());
    nodeSelector->setNoneEnabled(true);
    nodeSelector->setMRMLScene(this->mrmlScene());
    nodeSelector->setProperty("ReferenceRole", referenceRole.c_str());
    nodeSelector->setAddEnabled(true);
    nodeSelector->setRemoveEnabled(true);
    nodeSelector->setRenameEnabled(true);

    outputNodesLayout->addRow(nodeLabel, nodeSelector);

    connect(nodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
      this, SLOT(updateMRMLFromWidget()));
    }
}

//-----------------------------------------------------------------------------
void qSlicerDynamicModelerModuleWidget::updateInputWidgets()
{
  Q_D(qSlicerDynamicModelerModuleWidget);
  if (!d->DynamicModelerNode)
    {
    return;
    }

  QList<qMRMLNodeComboBox*> inputNodeSelectors = d->InputNodesCollapsibleButton->findChildren<qMRMLNodeComboBox*>();
  for (qMRMLNodeComboBox* inputNodeSelector : inputNodeSelectors)
    {
    QString referenceRole = inputNodeSelector->property("ReferenceRole").toString();
    int inputSelectorIndex = inputNodeSelector->property("InputSelectorIndex").toInt();
    vtkMRMLNode* referenceNode = d->DynamicModelerNode->GetNthNodeReference(referenceRole.toUtf8(), inputSelectorIndex);
    bool wasBlocking = inputNodeSelector->blockSignals(true);
    inputNodeSelector->setCurrentNode(referenceNode);
    inputNodeSelector->blockSignals(wasBlocking);
    }
}

//-----------------------------------------------------------------------------
void qSlicerDynamicModelerModuleWidget::updateParameterWidgets()
{
  Q_D(qSlicerDynamicModelerModuleWidget);
  vtkSlicerDynamicModelerLogic* meshModifyLogic = vtkSlicerDynamicModelerLogic::SafeDownCast(this->logic());
  vtkSlicerDynamicModelerTool* tool = nullptr;
  if (meshModifyLogic && d->DynamicModelerNode)
    {
    tool = meshModifyLogic->GetDynamicModelerTool(d->DynamicModelerNode);
    }
  if (!d->DynamicModelerNode || tool == nullptr || tool->GetNumberOfInputParameters() == 0)
    {
    return;
    }

  for (int i = 0; i < tool->GetNumberOfInputParameters(); ++i)
    {
    std::string name = tool->GetNthInputParameterName(i);
    std::string description = tool->GetNthInputParameterDescription(i);
    std::string attributeName = tool->GetNthInputParameterAttributeName(i);
    vtkVariant value = tool->GetNthInputParameterValue(i, d->DynamicModelerNode);
    int type = tool->GetNthInputParameterType(i);

    QWidget* parameterSelector = d->ParametersCollapsibleButton->findChild<QWidget*>(attributeName.c_str());
    if (type == vtkSlicerDynamicModelerTool::PARAMETER_BOOL)
      {
      QCheckBox* checkBox = qobject_cast<QCheckBox*>(parameterSelector);
      if (!checkBox)
        {
        qCritical() << "Could not find widget for parameter " << name.c_str();
        continue;
        }

      bool wasBlocking = checkBox->blockSignals(true);
      bool checked = value.ToInt() != 0;
      checkBox->setChecked(checked);
      checkBox->blockSignals(wasBlocking);
      }
    else if (type == vtkSlicerDynamicModelerTool::PARAMETER_INT)
      {
      QSpinBox* spinBox = qobject_cast<QSpinBox*>(parameterSelector);
      if (!spinBox)
        {
        qCritical() << "Could not find widget for parameter " << name.c_str();
        continue;
        }
      bool wasBlocking = spinBox->blockSignals(true);
      spinBox->setValue(value.ToInt());
      spinBox->blockSignals(wasBlocking);
      }
    else if (type == vtkSlicerDynamicModelerTool::PARAMETER_DOUBLE)
      {
      ctkDoubleSpinBox* doubleSpinBox = qobject_cast<ctkDoubleSpinBox*>(parameterSelector);
      if (!doubleSpinBox)
        {
        qCritical() << "Could not find widget for parameter " << name.c_str();
        continue;
        }
      bool wasBlocking = doubleSpinBox->blockSignals(true);
      doubleSpinBox->setValue(value.ToDouble());
      doubleSpinBox->blockSignals(wasBlocking);
      }
    else if (type == vtkSlicerDynamicModelerTool::PARAMETER_STRING_ENUM)
      {
      QComboBox* comboBox = qobject_cast<QComboBox*>(parameterSelector);
      if (!comboBox)
        {
        qCritical() << "Could not find widget for parameter " << name.c_str();
        continue;
        }
      bool wasBlocking = comboBox->blockSignals(true);
      int index = comboBox->findText(QString::fromStdString(value.ToString()));
      comboBox->setCurrentIndex(index);
      comboBox->blockSignals(wasBlocking);
      }
    else
      {
      QLineEdit* lineEdit = qobject_cast<QLineEdit*>(parameterSelector);
      if (!lineEdit)
        {
        qCritical() << "Could not find widget for parameter " << name.c_str();
        continue;
        }
      int cursorPosition = lineEdit->cursorPosition();
      bool wasBlocking = lineEdit->blockSignals(true);
      lineEdit->setText(QString::fromStdString(value.ToString()));
      lineEdit->setCursorPosition(cursorPosition);
      lineEdit->blockSignals(wasBlocking);
      }
    }
}

//-----------------------------------------------------------------------------
void qSlicerDynamicModelerModuleWidget::updateOutputWidgets()
{
  Q_D(qSlicerDynamicModelerModuleWidget);
  if (!d->DynamicModelerNode)
    {
    return;
    }

  QList< qMRMLNodeComboBox*> outputNodeSelectors = d->OutputNodesCollapsibleButton->findChildren<qMRMLNodeComboBox*>();
  for (qMRMLNodeComboBox* outputNodeSelector : outputNodeSelectors)
    {
    QString referenceRole = outputNodeSelector->property("ReferenceRole").toString();
    vtkMRMLNode* referenceNode = d->DynamicModelerNode->GetNodeReference(referenceRole.toUtf8());
    bool wasBlocking = outputNodeSelector->blockSignals(true);
    outputNodeSelector->setCurrentNode(referenceNode);
    outputNodeSelector->blockSignals(wasBlocking);
    }
}

//-----------------------------------------------------------------------------
void qSlicerDynamicModelerModuleWidget::updateWidgetFromMRML()
{
  Q_D(qSlicerDynamicModelerModuleWidget);
  vtkSlicerDynamicModelerLogic* meshModifyLogic = vtkSlicerDynamicModelerLogic::SafeDownCast(this->logic());
  vtkSlicerDynamicModelerTool* tool = nullptr;
  if (meshModifyLogic && d->DynamicModelerNode)
    {
    tool = meshModifyLogic->GetDynamicModelerTool(d->DynamicModelerNode);
    }
  d->ApplyButton->setEnabled(tool != nullptr && tool->HasRequiredInputs(d->DynamicModelerNode) &&
    tool->HasOutput(d->DynamicModelerNode));

  std::string toolName = "";
  if (d->DynamicModelerNode && d->DynamicModelerNode->GetToolName())
    {
    toolName = d->DynamicModelerNode->GetToolName();
    }

  if (toolName != d->CurrentToolName)
    {
    this->rebuildInputWidgets();
    this->rebuildParameterWidgets();
    this->rebuildOutputWidgets();
    d->CurrentToolName = toolName;
    }
  else if (this->isInputWidgetsRebuildRequired())
    {
    this->rebuildInputWidgets();
    }

  this->updateInputWidgets();
  this->updateParameterWidgets();
  this->updateOutputWidgets();

  bool wasBlocking = d->ApplyButton->blockSignals(true);
  if (d->DynamicModelerNode && d->DynamicModelerNode->GetContinuousUpdate())
    {
    d->ApplyButton->setCheckState(Qt::Checked);
    }
  else
    {
    d->ApplyButton->setCheckState(Qt::Unchecked);
    }
  d->ApplyButton->blockSignals(wasBlocking);
}

//-----------------------------------------------------------------------------
void qSlicerDynamicModelerModuleWidget::updateMRMLFromWidget()
{
  Q_D(qSlicerDynamicModelerModuleWidget);
  if (!d->DynamicModelerNode)
    {
    return;
    }

  MRMLNodeModifyBlocker blocker(d->DynamicModelerNode);

  // Continuous update
  d->DynamicModelerNode->SetContinuousUpdate(d->ApplyButton->checkState() == Qt::Checked);

  // If no tool is specified, there is nothing else to update
  vtkSlicerDynamicModelerLogic* dynamicModelerLogic = vtkSlicerDynamicModelerLogic::SafeDownCast(this->logic());
  vtkSlicerDynamicModelerTool* tool = nullptr;
  if (dynamicModelerLogic && d->DynamicModelerNode)
    {
    tool = dynamicModelerLogic->GetDynamicModelerTool(d->DynamicModelerNode);
    }
  if (!tool)
    {
    return;
    }

  for (int i = 0; i < tool->GetNumberOfInputNodes(); ++i)
    {
    std::string referenceRole = tool->GetNthInputNodeReferenceRole(i);
    d->DynamicModelerNode->RemoveNodeReferenceIDs(referenceRole.c_str());
    }

  // Update all input reference roles from the parameter node
  QList<qMRMLNodeComboBox*> inputNodeSelectors = d->InputNodesCollapsibleButton->findChildren<qMRMLNodeComboBox*>();
  for (qMRMLNodeComboBox* inputNodeSelector : inputNodeSelectors)
    {
    QString referenceRole = inputNodeSelector->property("ReferenceRole").toString();
    QString currentNodeID = inputNodeSelector->currentNodeID();
    d->DynamicModelerNode->AddNodeReferenceID(referenceRole.toUtf8(), currentNodeID.toUtf8());
    }

  QList<qMRMLNodeComboBox*> outputNodeSelectors = d->OutputNodesCollapsibleButton->findChildren<qMRMLNodeComboBox*>();
  for (qMRMLNodeComboBox* outputNodeSelector : outputNodeSelectors)
    {
    QString referenceRole = outputNodeSelector->property("ReferenceRole").toString();
    QString currentNodeID = outputNodeSelector->currentNodeID();
    d->DynamicModelerNode->SetNodeReferenceID(referenceRole.toUtf8(), currentNodeID.toUtf8());
    }

  d->ApplyButton->setToolTip("");
  d->ApplyButton->setCheckBoxUserCheckable(true);
  // If a node is selected in both the input and outputs, then disable continuous updates
  if (dynamicModelerLogic->HasCircularReference(d->DynamicModelerNode))
    {
    d->DynamicModelerNode->SetContinuousUpdate(false);
    d->ApplyButton->setToolTip("Output node detected in input. Continuous update is not availiable.");
    d->ApplyButton->setCheckBoxUserCheckable(false);
    }

  QList<QWidget*> parameterSelectors = d->ParametersCollapsibleButton->findChildren<QWidget*>();
  for (QWidget* parameterSelector : parameterSelectors)
    {
    vtkVariant value;
    QCheckBox* checkBox = qobject_cast<QCheckBox*>(parameterSelector);
    if (checkBox)
      {
      value = checkBox->isChecked() ? 1 : 0;
      }

    QSpinBox* spinBox = qobject_cast<QSpinBox*>(parameterSelector);
    if (spinBox)
      {
      value = spinBox->value();
      }

    ctkDoubleSpinBox* doubleSpinBox = qobject_cast<ctkDoubleSpinBox*>(parameterSelector);
    if (doubleSpinBox)
      {
      value = doubleSpinBox->value();
      }

    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(parameterSelector);
    if (lineEdit)
      {
      std::string text = lineEdit->text().toStdString();
      value = text.c_str();
      }

    QComboBox* comboBox = qobject_cast<QComboBox*>(parameterSelector);
    if (comboBox)
      {
      std::string text = comboBox->currentText().toStdString();
      value = text.c_str();
      }

    std::string attributeName = parameterSelector->property("AttributeName").toString().toStdString();
    if (attributeName != "")
      {
      d->DynamicModelerNode->SetAttribute(attributeName.c_str(), value.ToString().c_str());
      }
    }
}

//-----------------------------------------------------------------------------
void qSlicerDynamicModelerModuleWidget::onApplyButtonClicked()
{
  Q_D(qSlicerDynamicModelerModuleWidget);
  if (!d->DynamicModelerNode)
    {
    return;
    }
  this->updateMRMLFromWidget();

  /// Checkbox is checked. Should be handled by continuous update in logic
  if (d->ApplyButton->checkState() == Qt::Checked)
    {
    return;
    }

  /// Continuous update is off, trigger manual update.
  vtkSlicerDynamicModelerLogic* meshModifyLogic = vtkSlicerDynamicModelerLogic::SafeDownCast(this->logic());
  meshModifyLogic->RunDynamicModelerTool(d->DynamicModelerNode);
}
