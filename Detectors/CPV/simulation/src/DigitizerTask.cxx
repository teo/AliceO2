// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include <iostream>
#include "CPVSimulation/DigitizerTask.h"
#include "CPVBase/Geometry.h"
#include "CPVBase/Hit.h"
#include "CPVSimulation/Digitizer.h"

#include "FairLogger.h"      // for LOG
#include "FairRootManager.h" // for FairRootManager
#include "FairTask.h"        // for FairTask, InitStatus
#include "Rtypes.h"          // for DigitizerTask::Class, ClassDef, etc

ClassImp(o2::cpv::DigitizerTask);

using o2::cpv::Digit;
using o2::cpv::Digitizer;
using o2::cpv::Hit;

using namespace o2::cpv;

DigitizerTask::DigitizerTask() : FairTask("CPVDigitizerTask"), mDigitizer(), mHitsArray(nullptr) {}

DigitizerTask::~DigitizerTask()
{
  if (mDigitsArray) {
    delete mDigitsArray;
  }
}

/// \brief Init function
///
/// Inititializes the digitizer and connects input and output container
InitStatus DigitizerTask::Init()
{
  FairRootManager* mgr = FairRootManager::Instance();
  if (!mgr) {
    LOG(ERROR) << "Could not instantiate FairRootManager. Exiting ..." << FairLogger::endl;
    return kERROR;
  }

  mHitsArray = mgr->InitObjectAs<const std::vector<o2::cpv::Hit>*>("PHSHit");
  if (!mHitsArray) {
    LOG(ERROR) << "CPV hits not registered in the FairRootManager. Exiting ..." << FairLogger::endl;
    return kERROR;
  }

  // Register output container
  mgr->RegisterAny("PHSDigit", mDigitsArray, kTRUE);

  mDigitizer.setCoeffToNanoSecond(mFairTimeUnitInNS);

  mDigitizer.init();

  return kSUCCESS;
}

//________________________________________________________
void DigitizerTask::Exec(Option_t* option)
{
  FairRootManager* mgr = FairRootManager::Instance();

  if (mDigitsArray) {
    mDigitsArray->clear();
  }
  mDigitizer.setEventTime(mgr->GetEventTime());

  LOG(DEBUG) << "Running digitization on new event " << mEventID << " from source " << mSourceID << FairLogger::endl;
  mDigitizer.setCurrSrcID(mSourceID);
  mDigitizer.setCurrEvID(mEventID);

  mDigitizer.process(*mHitsArray, *mDigitsArray);

  mEventID++;
}

//________________________________________________________
void DigitizerTask::FinishTask()
{
  // finalize digitization, if needed, flash remaining digits
  std::cout << "Finish task" << std::endl;
  FairRootManager* mgr = FairRootManager::Instance();
  mgr->SetLastFill(kTRUE); /// necessary, otherwise the data is not written out
  if (mDigitsArray) {
    mDigitsArray->clear();
  }
  // mDigitizer.fillOutputContainer(mDigitsArray);
  mDigitizer.finish();
}
