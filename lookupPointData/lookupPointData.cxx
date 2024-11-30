#include "lookupPointDataCLP.h"


//VTK Includes
#include "vtkFillHolesFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkDelimitedTextReader.h"
#include "vtkTable.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkNew.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"

#include <iostream>
#include <fstream>


// Needs testing

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

    //std::ifstream csvfile(csvFile);
    vtkNew<vtkDelimitedTextReader> CSVDelimiter;
    CSVDelimiter->SetFieldDelimiterCharacters(",");
    CSVDelimiter->SetFileName(csvFile.c_str());
    CSVDelimiter->SetHaveHeaders(true);
    CSVDelimiter->Update();
    vtkSmartPointer<vtkTable> table = CSVDelimiter->GetOutput();

    vtkDoubleArray* outputScalars = (vtkDoubleArray*) polyData->GetPointData()->GetArray(scalarVal.c_str());

    for (unsigned int idx=0; idx < table->GetNumberOfRows() ; idx++)
    {
      for (unsigned int idxs = 0 ; idxs < polyData->GetNumberOfPoints() ; idxs++)
      {
          if (outputScalars->GetVariantValue(idxs) == table->GetValue(idx,0))
          {
              outputScalars->SetVariantValue(idxs,table->GetValue(idx,1));
          }
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
