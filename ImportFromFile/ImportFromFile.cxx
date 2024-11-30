#include "ImportFromFileCLP.h"

// VTK Includes
#include "vtkXMLPolyDataWriter.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkNew.h"
#include "vtkTransformFilter.h"
#include "vtkTransform.h"
#include "vtkPoints.h"



int main (int argc, char * argv[])
 {
   PARSE_ARGS;

   try{

     std::string line;

     // open inputFile
     std::ifstream input, input2;
     input2.open( inputFile );
     input2.seekg(0, std::ios::beg);
     // calculate number of lines of the inputFilename
     int numberOfLines = 0;

     while(std::getline(input, line))
     {
       numberOfLines++;
     }
     input2.close();

     numberOfLines = numberOfLines - 3;

     input.open(inputFile);
     input.seekg(0, std::ios::beg);

     std::ofstream output;
     output.open(outputFile);
     output << "NUMBER_OF_POINTS= " << numberOfLines << std::endl;
     output << "DIMENSION=1" << std::endl;
     output << "TYPE=Scalar" << std::endl;

     // extract column 5 and write the values
     while( std::getline(input, line) )
     {
       float dummy, extractValue, point;
       std::sscanf(line.c_str(), " %f %f %f %f %f ", &point, &dummy, &dummy, &dummy, &extractValue);
       
     }

   // close input&output
   input.close();
   output.close();

  }
  catch (int e)
   {
     cout << "An exception occurred. Exception Nr. " << e << '\n';
     return EXIT_FAILURE;
   }

  return EXIT_SUCCESS;

 }
