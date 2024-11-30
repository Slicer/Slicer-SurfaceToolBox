#include "KWMToPolyDataCLP.h"

//VTK includes
#include "vtkXMLPolyDataWriter.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkSmartPointer.h"
#include "vtkCleanPolyData.h"
#include "vtkNew.h"
#include "vtkPolyData.h"
#include "vtkFloatArray.h"
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

    char line[70];
    ifstream input;
    int NPoints, NDimension;
    char * aux;
    input.open(inFile.c_str(), ios::in);

    // get values from file format of KWM
    input.getline(line, 500, '\n');
    aux = strtok(line, " = ");
    aux = strtok(NULL, " = ");
    // TODO: Change from atof
    NPoints = atof(aux);

    input.getline(line, 500, '\n');
    aux = strtok(line, " = ");
    aux = strtok(NULL, " = ");
    // TODO: Change from atof
    NDimension = atof(aux);

    input.getline(line, 500, '\n');

    vtkNew<vtkFloatArray> scalars;
    scalars->SetNumberOfComponents(NDimension);
    scalars->SetName(scalarFile.c_str());

    for(int i = 0; i < NPoints; i++)
    {
      input.getline(line, 500, '\n');
      float value = 0;
      std::string proc_string = line;
      std::istringstream iss(proc_string);
      float *tuple = new float[NDimension];
      int count = 0;
      do {
        std::string sub;
        iss >> sub;
        if(sub != "")
        {
          if(count >= NDimension )
          {
            std::cerr << "Error" << std::endl;
            return 1;
          }
          value = atof(sub.c_str());
          tuple[ count ] = value;
          count++;
        }
      } while(iss);
      if ( count != NDimension )
      {
        // improper reading in file
        std::cerr << "Error in Line: "<< line << std::endl;
        return 1;
      }
      scalars->InsertNextTuple(tuple);

    }
    // close file
    input.close();

    // apply transformation
    polyData->GetPointData()->AddArray(scalars);

    vtkNew<vtkXMLPolyDataWriter> SurfaceWriter;
    SurfaceWriter->SetInputData(polyData);
    SurfaceWriter->SetFileName(outputVolume.c_str());
    SurfaceWriter->Update();


  }
catch (int e)
 {
   cout << "An exception occurred. Exception Nr. " << e << '\n';
   return EXIT_FAILURE;
 }

 return EXIT_SUCCESS;

 }
