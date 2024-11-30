#include "avgMeshCLP.h"

//VTK Includes
#include "vtkXMLPolyDataWriter.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkNew.h"


int main (int argc, char * argv[])
 {
 PARSE_ARGS;

 try{

    vtkSmartPointer<vtkPolyData> polyData;
    vtkSmartPointer<vtkPolyData> polyData2;

    // Read the file
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(inputVolume.c_str());
    reader->Update();
    polyData = reader->GetOutput();

    vtkNew<vtkXMLPolyDataReader> reader2;
    reader2->SetFileName(inputVolumeTwo.c_str());
    reader2->Update();
    reader2->Update();
    polyData2 = reader2->GetOutput();

    vtkSmartPointer<vtkPoints> tmpPoints = vtkSmartPointer<vtkPoints>::New();
    for( unsigned int pointID = 0; pointID < polyData2->GetNumberOfPoints(); pointID++ )
    {
		    double curPoint[3];
		    double avgPoint[3];
		    double tmpPoint[3];
        polyData2->GetPoint(pointID, curPoint);
 		    polyData->GetPoint(pointID, avgPoint);
	      for( unsigned int dim = 0; dim < 3; dim++ )
        {
 		       tmpPoint[dim] = curPoint[dim] + avgPoint[dim];
		    }
		    tmpPoints->InsertPoint(pointID, tmpPoint);
		  }

      polyData->SetPoints(tmpPoints);
      vtkNew<vtkXMLPolyDataWriter> writer;
      writer->SetFileName(outputVolume.c_str());
      writer->SetInputData(polyData);
      writer->Update();

    }
catch (int e)
 {
   cout << "An exception occurred. Exception Nr. " << e << '\n';
   return EXIT_FAILURE;
 }

 return EXIT_SUCCESS;
}
