#if !defined(__CLING__) || defined(__ROOTCLING__)
#include "TGeoManager.h"
#include "TString.h"
#include "TSystem.h"

#include "DetectorsPassive/Cave.h"
#include "DetectorsPassive/FrameStructure.h"
#include "EMCALSimulation/Detector.h"
#include "FairLogger.h"
#include "FairRunSim.h"
#endif

void PutEmcalInTop()
{
  // minimal macro to test setup of the geometry
  FairLogger::GetLogger()->SetLogScreenLevel("DEBUG3");

  TString dir = getenv("VMCWORKDIR");
  TString geom_dir = dir + "/Detectors/Geometry/";
  gSystem->Setenv("GEOMPATH", geom_dir.Data());

  TString tut_configdir = dir + "/Detectors/gconfig";
  gSystem->Setenv("CONFIG_DIR", tut_configdir.Data());

  // Create simulation run
  FairRunSim* run = new FairRunSim();
  run->SetOutputFile("foo.root"); // Output file
  run->SetName("TGeant3");        // Transport engine
  // Create media
  run->SetMaterials("media.geo"); // Materials

  // Create geometry
  o2::Passive::Cave* cave = new o2::Passive::Cave("CAVE");
  cave->SetGeometryFileName("cave.geo");
  run->AddModule(cave);

  o2::EMCAL::Detector* emcdet = new o2::EMCAL::Detector(kTRUE);
  run->AddModule(emcdet);

  run->Init();

  gGeoManager->Export("geometry.root");
}
