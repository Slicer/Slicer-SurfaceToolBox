#include "pointDataOPCLP.h"

//VTK Includes
#include "vtkFillHolesFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkNew.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"



int main (int argc, char * argv[])
 {
 PARSE_ARGS;

 try{

   //TODO: Testing

    vtkSmartPointer<vtkPolyData> polyData;

    // Read the file
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(inputVolume.c_str());
    reader->Update();
    polyData = reader->GetOutput();

    unsigned int    nPoints = polyData->GetNumberOfPoints();

    //unsigned int numberOfArrays = polyData->GetPointData()->GetNumberOfArrays();

    const char * name = names.c_str();
    polyData->GetPointData()->SetActiveScalars(name);
    vtkDoubleArray *scalarData = (vtkDoubleArray*) polyData->GetPointData()->GetArray(name);

    if (scalarData)
      {
      double opValue = opVal;


      if (! operation.compare("threshBelow"))
      {
        for (unsigned int i = 0 ; i < nPoints; i++)
          {
            if (scalarData->GetComponent(i,0) < opValue)  scalarData->SetComponent(i,0, 0);
          }

      } else if (! operation.compare("sub"))
      {
        for (unsigned int i = 0 ; i < nPoints; i++)
          {
            double curValue = scalarData->GetComponent(i,0);
            scalarData->SetComponent(i,0, curValue - opValue);
          }
      } else
      {
        std::cout << "No (known) point operation detected -> exiting" << std::endl;
      }
    }



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
