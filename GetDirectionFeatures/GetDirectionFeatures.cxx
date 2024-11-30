#include "GetDirectionFeaturesCLP.h"

// VTK Includes
#include "vtkDecimatePro.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkTriangleFilter.h"
#include "vtkNew.h"
#include "vtkPoints.h"
#include "vtkPolyDataNormals.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"



int main (int argc, char * argv[])
 {
   PARSE_ARGS;

   try{
    vtkSmartPointer<vtkPolyData> polyData;


    // Read the file
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(inputVolume.c_str());
    reader->Update();
    polyData = reader->GetOutput();

    vtkIdType numPoints = polyData->GetNumberOfPoints();

    double point[3], vectorx[3], vectory[3], vectorz[3];
    vectorx[0] = 1; vectorx[1] = 0; vectorx[2] = 0;
    vectory[0] = 0; vectory[1] = 1; vectory[2] = 0;
    vectorz[0] = 0; vectorz[1] = 0; vectorz[2] = 1;

    vtkNew<vtkPolyDataNormals> meshNormals;
    meshNormals->SetComputePointNormals(1);
    meshNormals->SetComputeCellNormals(0);
    meshNormals->SetSplitting(0);
    meshNormals->SetInputData(polyData);
    meshNormals->Update();

    vtkSmartPointer<vtkPolyData> vtkMeshNormals = meshNormals->GetOutput();

    vtkSmartPointer<vtkDataArray> Array = vtkDataArray::SafeDownCast(vtkMeshNormals->GetPointData()->GetNormals());

    std::ofstream outfileVecX;
    std::ofstream outfileVecY;
    std::ofstream outfileVecZ;

    outfileVecX.open(vecXFile.c_str());
    outfileVecY.open(vecYFile.c_str());
    outfileVecZ.open(vecZFile.c_str());
    outfileVecX << "NUMBER_OF_POINTS = " << numPoints << std::endl;
    outfileVecX << "DIMENSION = " << 1 << std::endl;
    outfileVecX << "TYPE = Scalar" << std::endl;

    outfileVecY << "NUMBER_OF_POINTS = " << numPoints << std::endl;
    outfileVecY << "DIMENSION = " << 1 << std::endl;
    outfileVecY << "TYPE = Scalar" << std::endl;

    outfileVecZ << "NUMBER_OF_POINTS = " << numPoints << std::endl;
    outfileVecZ << "DIMENSION = " << 1 << std::endl;
    outfileVecZ << "TYPE = Scalar" << std::endl;

    

    for(vtkIdType i = 0; i <numPoints; i++)
    {
      Array->GetTuple(i, point);

      double dotprodX = point[0] * vectorx[0] + point[1] * vectorx[1] + point[2] * vectorx[2];
      double dotprodY = point[0] * vectory[0] + point[1] * vectory[1] + point[2] * vectory[2];
      double dotprodZ = point[0] * vectorz[0] + point[1] * vectorz[1] + point[2] * vectorz[2];

      outfileVecX << dotprodX << std::endl;
      outfileVecY << dotprodY << std::endl;
      outfileVecZ << dotprodZ << std::endl;

    }

    outfileVecX.close();
    outfileVecY.close();
    outfileVecZ.close();


  }
  catch (int e)
   {
     cout << "An exception occurred. Exception Nr. " << e << '\n';
     return EXIT_FAILURE;
   }

  return EXIT_SUCCESS;

 }
