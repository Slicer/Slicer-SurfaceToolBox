project(vtkSlicer${MODULE_NAME}ModuleLogic)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "VTK_SLICER_${MODULE_NAME_UPPER}_MODULE_LOGIC_EXPORT")

add_subdirectory(FastMarching)

#
# vtkAddon
#
if(NOT Slicer_BUILD_vtkAddon)
  find_package(vtkAddon REQUIRED)
endif()

set(${KIT}_INCLUDE_DIRECTORIES
  ${vtkAddon_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/FastMarching
  ${CMAKE_CURRENT_SOURCE_DIR}/FastMarching/gw_geodesic
  ${CMAKE_CURRENT_SOURCE_DIR}/FastMarching/gw_core
  )

set(${KIT}_SRCS
  vtkImplicitPolyDataPointDistance.cxx
  vtkImplicitPolyDataPointDistance.h
  vtkSlicer${MODULE_NAME}AppendTool.cxx
  vtkSlicer${MODULE_NAME}AppendTool.h
  vtkSlicer${MODULE_NAME}BoundaryCutTool.cxx
  vtkSlicer${MODULE_NAME}BoundaryCutTool.h
  vtkSlicer${MODULE_NAME}CurveCutTool.cxx
  vtkSlicer${MODULE_NAME}CurveCutTool.h
  vtkSlicer${MODULE_NAME}ExtrudeTool.cxx
  vtkSlicer${MODULE_NAME}ExtrudeTool.h
  vtkSlicer${MODULE_NAME}RevolveTool.cxx
  vtkSlicer${MODULE_NAME}RevolveTool.h
  vtkSlicer${MODULE_NAME}HollowTool.cxx
  vtkSlicer${MODULE_NAME}HollowTool.h
  vtkSlicer${MODULE_NAME}Logic.cxx
  vtkSlicer${MODULE_NAME}Logic.h
  vtkSlicer${MODULE_NAME}MarginTool.cxx
  vtkSlicer${MODULE_NAME}MarginTool.h
  vtkSlicer${MODULE_NAME}MirrorTool.cxx
  vtkSlicer${MODULE_NAME}MirrorTool.h
  vtkSlicer${MODULE_NAME}PlaneCutTool.cxx
  vtkSlicer${MODULE_NAME}PlaneCutTool.h
  vtkSlicer${MODULE_NAME}ROICutTool.cxx
  vtkSlicer${MODULE_NAME}ROICutTool.h
  vtkSlicer${MODULE_NAME}SelectByPointsTool.cxx
  vtkSlicer${MODULE_NAME}SelectByPointsTool.h
  vtkSlicer${MODULE_NAME}Tool.cxx
  vtkSlicer${MODULE_NAME}Tool.h
  vtkSlicer${MODULE_NAME}ToolFactory.cxx
  vtkSlicer${MODULE_NAME}ToolFactory.h
  ${CMAKE_CURRENT_SOURCE_DIR}/FastMarching/vtkFastMarchingGeodesicDistance.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/FastMarching/vtkFastMarchingGeodesicDistance.h
  ${CMAKE_CURRENT_SOURCE_DIR}/FastMarching/vtkFastMarchingGeodesicPath.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/FastMarching/vtkFastMarchingGeodesicPath.h
  ${CMAKE_CURRENT_SOURCE_DIR}/FastMarching/vtkPolyDataGeodesicDistance.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/FastMarching/vtkPolyDataGeodesicDistance.h
  ${CMAKE_CURRENT_SOURCE_DIR}/FastMarching/vtkPolygonalSurfaceContourLineInterpolator2.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/FastMarching/vtkPolygonalSurfaceContourLineInterpolator2.h
  )

set(${KIT}_TARGET_LIBRARIES
  vtkSlicer${MODULE_NAME}ModuleMRML
  vtkAddon
  MeshGeodesics
  )

#-----------------------------------------------------------------------------
SlicerMacroBuildModuleLogic(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )
