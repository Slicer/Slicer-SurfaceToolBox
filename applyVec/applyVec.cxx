#include "applyVecCLP.h"

// VTK Includes
#include "vtkDecimatePro.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkTriangleFilter.h"
#include "vtkNew.h"


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

static int GetIntFromString(const char *inputLine)
{
  std::string Line = inputLine;
  int         loc = Line.find("=", 1);
  std::string NbPoint = Line.erase(0, loc + 1);
  int         Val = atoi(NbPoint.c_str() );
  return Val;
}

int main (int argc, char * argv[])
 {
   PARSE_ARGS;

   try{

     typedef itk::DefaultDynamicMeshTraits<float, 3, 3, float, float> MeshTraitsType;

     typedef itk::DefaultDynamicMeshTraits<double, 3, 3, double, double> TriangleMeshTraits;
     //typedef itk::Mesh<double, 3, TriangleMeshTraits>                    TriangleMeshType;
     typedef TriangleMeshTraits::PointType                            PointTriangleType;


     typedef MeshTraitsType::PointType                                PointType;

     constexpr unsigned int Dimension = 3;
     using InputPixelType = float;


     using MeshType = itk::Mesh<InputPixelType, Dimension>;
     using  MeshReaderType = itk::MeshFileReader<MeshType>;

     using MeshWriterType = itk::MeshFileWriter<MeshType>;

     MeshReaderType::Pointer meshReader = MeshReaderType::New();
     meshReader->SetFileName(inputVolume.c_str());
     MeshType::Pointer                mesh = meshReader->GetOutput();
     MeshType::PointsContainerPointer Points = mesh->GetPoints();


     char          output[64];
     std::ifstream vectorFile;
     vectorFile.open(vectFile);

     vectorFile.getline(output, 64);
     unsigned int Val = GetIntFromString(output);

     if( mesh->GetNumberOfPoints() != Val )
      {
      std::cout << "Mesh and vector field must have the same number of points" << std::endl;
      std::cout << "The mesh has " << mesh->GetNumberOfPoints() << " points and the vector field has " << Val
                << " points." << std::endl;
      exit(-1);
      }
    // Skipping the second and third lines of the file
    vectorFile.getline(output, 32);
    vectorFile.getline(output, 32);

    float              X, Y, Z;
    std::vector<float> Xval;
    std::vector<float> Yval;
    std::vector<float> Zval;
    for( unsigned int Line = 0; Line < Val; Line++ )
      {
      vectorFile.getline(output, 64);
      sscanf(output, "%f %f %f", &X, &Y, &Z);
      Xval.push_back(X);
      Yval.push_back(Y);
      Zval.push_back(Z);
      }

    PointType *        point1 = new PointType;
    PointTriangleType *Tripoint = new PointTriangleType;
    for( unsigned int i = 0; i < mesh->GetNumberOfPoints(); i++ )
      {
      if( mesh->GetPoint( i, point1 ) && &Xval[i] != NULL )
        {

        Tripoint->SetElement(0, point1->GetElement(0) + Xval[i]);
        Tripoint->SetElement(1, point1->GetElement(1) + Yval[i]);
        Tripoint->SetElement(2, point1->GetElement(2) + Zval[i]);

        mesh->SetPoint(i, *Tripoint);
        }
      else
        {
        return -1;
        }
      }
    MeshWriterType::Pointer writer = MeshWriterType::New();

    writer->SetInput(mesh);
    writer->SetFileName(outputVolume.c_str() );
    writer->Update();

  }
  catch (int e)
   {
     cout << "An exception occurred. Exception Nr. " << e << '\n';
     return EXIT_FAILURE;
   }

  return EXIT_SUCCESS;

 }
