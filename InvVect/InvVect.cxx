#include "InvVectCLP.h"

// VTK Includes
#include "vtkStdString.h"


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
     //Needs Testing

    std::ifstream vectorFile;
    vectorFile.open(vectFile.c_str());
    std::ofstream outfileVec;
    outfileVec.open(outFile.c_str());
    // Get the number of points in the file
    char output[64];
    vectorFile.getline(output, 32);
    outfileVec << output << std::endl;
    int Val = GetIntFromString(output);
    vectorFile.getline(output, 32);
    outfileVec << output << std::endl;
    vectorFile.getline(output, 32);
    outfileVec << output << std::endl;

    // Write to file in format
    float X, Y, Z;
    for(int Line = 0; Line < Val; Line++)
    {
      vectorFile.getline(output, 64);
      sscanf(output, "%f %f %f", &X, &Y, &Z);
      X = -1 * X;
      Y = -1 * Y;
      Z = -1 * Z;
      outfileVec << X << " " << Y << " " << Z << std::endl;
    }
    outfileVec.close();


  }
  catch (int e)
   {
     cout << "An exception occurred. Exception Nr. " << e << '\n';
     return EXIT_FAILURE;
   }

  return EXIT_SUCCESS;

 }
