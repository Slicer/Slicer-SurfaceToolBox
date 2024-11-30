#include "AlignMeshICPCLP.h"

// VTK Includes
#include "vtkXMLPolyDataWriter.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkTriangleFilter.h"
#include "vtkNew.h"
#include "vtkIterativeClosestPointTransform.h"
#include "vtkLandmarkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkAppendPolyData.h"


int main (int argc, char * argv[])
 {
   PARSE_ARGS;

   try{
    vtkSmartPointer<vtkPolyData> source;
    vtkSmartPointer<vtkPolyData> target;

    // Read the file
    vtkNew<vtkXMLPolyDataReader> targetReader;
    targetReader->SetFileName(inputVolume.c_str());
    targetReader->Update();
    target = targetReader->GetOutput();

    vtkNew<vtkXMLPolyDataReader> sourceReader;
    sourceReader->SetFileName(inputVolumeTwo.c_str());
    sourceReader->Update();
    source = sourceReader->GetOutput();


    vtkNew<vtkIterativeClosestPointTransform> icp;
    icp->SetSource(source);
    icp->SetTarget(target);
    vtkSmartPointer<vtkLandmarkTransform> transform =
      icp->GetLandmarkTransform();
    transform->SetModeToRigidBody();
    icp->SetMaximumNumberOfIterations(20);
    icp->StartByMatchingCentroidsOn();
    icp->Modified();
    icp->Update();

    vtkNew<vtkTransformPolyDataFilter> icpTransformFilter;
    icpTransformFilter->SetInputData(source);
    icpTransformFilter->SetTransform(icp);
    icpTransformFilter->Update();

    //Write to file
    vtkNew<vtkXMLPolyDataWriter> writer;
    writer->SetFileName(outputVolume.c_str());
    writer->SetInputData(icpTransformFilter->GetOutput());
    writer->Update();
  }
  catch (int e)
   {
     cout << "An exception occurred. Exception Nr. " << e << '\n';
     return EXIT_FAILURE;
   }

  return EXIT_SUCCESS;

 }
