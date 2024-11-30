#include "AlignMeshCLP.h"


// VTK Includes
#include "vtkDecimatePro.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkTriangleFilter.h"
#include "vtkNew.h"
#include "vtkCellLocator.h"

//ITK Includes
#include "itkMesh.h"
#include "itkMeshFileReader.h"
#include "itkMeshFileWriter.h"
#include "itkLineCell.h"
#include "itkTriangleCell.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkConstrainedValueAdditionImageFilter.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkPluginUtilities.h"
#include <itkDefaultDynamicMeshTraits.h>
#include <itkMetaMeshConverter.h>


int main (int argc, char * argv[])
 {
   PARSE_ARGS;
   try{
     typedef itk::DefaultDynamicMeshTraits<float, 3, 3, float, float> MeshTraitsType;


     typedef MeshTraitsType::PointType                                PointType;

     constexpr unsigned int Dimension = 3;
     using InputPixelType = float;


     using MeshType = itk::Mesh<InputPixelType, Dimension>;
     using  MeshReaderType = itk::MeshFileReader<MeshType>;

     using MeshWriterType = itk::MeshFileWriter<MeshType>;

     MeshReaderType::Pointer meshReader = MeshReaderType::New();
     meshReader->SetFileName(inputVolume.c_str());
    // meshReader->Update();
     MeshType::Pointer                mesh = meshReader->GetOutput();
     MeshType::PointsContainerPointer Points = mesh->GetPoints();
     int                              numberOfPoints = Points->Size();

     MeshReaderType::Pointer meshReader2 = MeshReaderType::New();
     meshReader2->SetFileName(inputVolumeTwo.c_str());
    // meshReader->Update();
     MeshType::Pointer                mesh2 = meshReader2->GetOutput();





     MeshWriterType::Pointer writer = MeshWriterType::New();
     writer->SetInput(mesh);
     writer->SetFileName(outputVolume.c_str());
     //writer->Update();


   }
   catch (int e)
    {
      cout << "An exception occurred. Exception Nr. " << e << '\n';
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;

}
