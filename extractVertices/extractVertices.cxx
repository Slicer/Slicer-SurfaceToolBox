#include "extractVerticesCLP.h"

//VTK includes
#include "vtkXMLPolyDataWriter.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkSmartPointer.h"
#include "vtkCleanPolyData.h"
#include "vtkNew.h"
#include "vtkPolyData.h"


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

    std::ofstream outfile0, outfile1, outfile2;

    outfile0.open(outFile0);
    outfile1.open(outFile1);
    outfile2.open(outFile2);

    int NbPoints = polyData->GetNumberOfPoints();
    outfile0 << "NUMBER_OF_POINTS=" << NbPoints << std::endl << "DIMENSION=1" << std::endl << "TYPE=Scalar"
             << std::endl;
    outfile1 << "NUMBER_OF_POINTS=" << NbPoints << std::endl << "DIMENSION=1" << std::endl << "TYPE=Scalar"
             << std::endl;
    outfile2 << "NUMBER_OF_POINTS=" << NbPoints << std::endl << "DIMENSION=1" << std::endl << "TYPE=Scalar"
             << std::endl;

    double x[3];
    for(int PointId = 0; PointId < NbPoints; PointId++)
    {
      polyData->GetPoint(PointId, x);
      outfile0 << x[0] << std::endl;
      outfile1 << x[1] << std::endl;
      outfile2 << x[2] << std::endl;
    }
    outfile0.close();
    outfile1.close();
    outfile2.close();



  }
catch (int e)
 {
   cout << "An exception occurred. Exception Nr. " << e << '\n';
   return EXIT_FAILURE;
 }

 return EXIT_SUCCESS;

 }
