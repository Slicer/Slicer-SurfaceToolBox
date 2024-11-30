#include "closestPointCLP.h"

// VTK Includes
#include "vtkDecimatePro.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkTriangleFilter.h"
#include "vtkNew.h"



int main (int argc, char * argv[])
 {
   PARSE_ARGS;

   try{

    vtkSmartPointer<vtkPolyData> mesh1;
    vtkSmartPointer<vtkPolyData> mesh2;


    // Read the file
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(inputVolume.c_str());
    reader->Update();
    mesh1 = reader->GetOutput();

    vtkNew<vtkXMLPolyDataReader> reader2;
    reader2->SetFileName(inputVolumeTwo.c_str());
    reader2->Update();
    mesh2 = reader2->GetOutput();

    std::ifstream inFile;
    char Line[40];
    float CurrentAttribute;
    std::vector<float> v_AttributeIn, v_AttributeOut;
    // open closest point file
    inFile.open(valFile.c_str());

    while( std::strncmp(Line, "NUMBER_OF_POINTS =", 18) && strncmp(Line, "NUMBER_OF_POINTS", 17) )
    {
      inFile.getline(Line, 40);
    }

    unsigned int NbVertices = atoi(strrchr(Line, '=') + 1);
    inFile.getline(Line, 40);
    inFile.getline(Line, 40);
    for(unsigned int i = 0; i < NbVertices; i++)
    {
      inFile >> CurrentAttribute;
      v_AttributeIn.push_back(CurrentAttribute);
    }
    inFile.close();

    double x[3];
    vtkIdType closestVertId;
    for(int PointId = 0; PointId < mesh2->GetNumberOfPoints(); PointId++)
    {
      mesh2->GetPoint(PointId, x);
      closestVertId = mesh1->FindPoint(x);
      if(closestVertId >= 0)
      {
        v_AttributeOut.push_back(v_AttributeIn[(int)closestVertId]);
      }
      else
      {
        return EXIT_FAILURE;
      }
    }
    // Write out to file

    std::ofstream outputfile;
    outputfile.open(outFile);
    outputfile << "NUMBER_OF_POINTS=" << v_AttributeOut.size() << std::endl;
    outputfile << "DIMENSION=" << 1 << std::endl;
    outputfile << "TYPE=Scalar" << std::endl;
    for(unsigned int i = 0; i < v_AttributeOut.size(); i++)
    {
      outputfile << v_AttributeOut[i] << std::endl;
    }
    outputfile.close();


  }
  catch (int e)
   {
     cout << "An exception occurred. Exception Nr. " << e << '\n';
     return EXIT_FAILURE;
   }

  return EXIT_SUCCESS;

 }
