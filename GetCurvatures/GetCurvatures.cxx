#include "GetCurvaturesCLP.h"

// VTK Includes
#include "vtkXMLPolyDataWriter.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkNew.h"
#include "vtkTransformFilter.h"
#include "vtkTransform.h"
#include "vtkPoints.h"
#include "vtkCurvatures.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"



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

    vtkNew<vtkCurvatures> curveMax;
    curveMax->SetInputData(polyData);
    curveMax->SetCurvatureTypeToMaximum();
    curveMax->Update();

    vtkSmartPointer<vtkPolyData> polyDataCurveMax = curveMax->GetOutput();

    vtkNew<vtkCurvatures> curveMin;
    curveMin->SetInputData(polyData);
    curveMin->SetCurvatureTypeToMinimum();
    curveMin->Update();
    vtkSmartPointer<vtkPolyData> polyDataCurveMin = curveMin->GetOutput();

    unsigned int nPoints = polyDataCurveMax->GetNumberOfPoints();
    vtkSmartPointer<vtkDoubleArray> ArrayCurveMax = vtkDoubleArray::SafeDownCast(
      polyDataCurveMax->GetPointData()->GetScalars()
    );
    vtkSmartPointer<vtkDoubleArray> ArrayCurveMin = vtkDoubleArray::SafeDownCast(
      polyDataCurveMin->GetPointData()->GetScalars()
    );

    std::ofstream curvedness(outputCurve);
    std::ofstream shapeIndex(outputShape);
    std::ofstream gauss(outputGauss);
    std::ofstream mean(outputMean);

    curvedness << "NUMBER_OF_POINTS=" << nPoints << std::endl;
    curvedness << "DIMENSION=1" << std::endl << "TYPE=Scalar" << std::endl;

    shapeIndex << "NUMBER_OF_POINTS=" << nPoints << std::endl;
    shapeIndex << "DIMENSION=1" << std::endl << "TYPE=Scalar" << std::endl;

    gauss << "NUMBER_OF_POINTS=" << nPoints << std::endl;
    gauss << "DIMENSION=1" << std::endl << "TYPE=Scalar" << std::endl;

    mean << "NUMBER_OF_POINTS=" << nPoints << std::endl;
    mean << "DIMENSION=1" << std::endl << "TYPE=Scalar" << std::endl;

    if( ArrayCurveMax && ArrayCurveMin )
      {

      for( unsigned int i = 0; i < nPoints; i++ )
        {

        float curvmax, curvmin;
        curvmax = ArrayCurveMax->GetValue(i);
        curvmin = ArrayCurveMin->GetValue(i);

        float aux = sqrt( (pow(curvmax, 2) + pow(curvmin, 2) ) / 2);
        curvedness << aux << std::endl;

        aux = (2 / M_PI) * (atan( (curvmax + curvmin) / (curvmax - curvmin) ) );
        if( aux != aux )
          {
          aux = 0;
          }
        shapeIndex << aux << std::endl;

        aux = curvmax * curvmin;
        gauss << aux << std::endl;

        aux = (curvmax + curvmin) / 2;
        mean << aux << std::endl;
        // std::cout << "Curvature: " << curv << std::endl;
        }
      }
    else
      {
      // std::cout << "Got no array?" << std::endl;

      curvedness.close();
      }

    curvedness.close();
    shapeIndex.close();
    gauss.close();
    mean.close();

    //Write to file
    //vtkNew<vtkXMLPolyDataWriter> writer;
    //writer->SetFileName(outputVolume.c_str());
    //writer->SetInputData(polyData);
    //writer->Update();
  }
  catch (int e)
   {
     cout << "An exception occurred. Exception Nr. " << e << '\n';
     return EXIT_FAILURE;
   }

  return EXIT_SUCCESS;

 }
