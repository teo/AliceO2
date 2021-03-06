# Copyright CERN and copyright holders of ALICE O2. This software is
# distributed under the terms of the GNU General Public License v3 (GPL
# Version 3), copied verbatim in the file "COPYING".
#
# See https://alice-o2.web.cern.ch/ for full licensing information.
#
# In applying this license CERN does not waive the privileges and immunities
# granted to it by virtue of its status as an Intergovernmental Organization
# or submit itself to any jurisdiction.

set(MODULE_NAME "DigitizerWorkflow")
set(MODULE_BUCKET_NAME digitizer_workflow_bucket)

O2_SETUP(NAME ${MODULE_NAME})

## TODO: feature of macro, it deletes the variables we pass to it, set them again
## this has to be fixed in the macro implementation
set(BUCKET_NAME ${MODULE_BUCKET_NAME})

O2_GENERATE_EXECUTABLE(
  EXE_NAME digitizer-workflow

  SOURCES
  src/SimpleDigitizerWorkflow.cxx
  src/SimReaderSpec.cxx
  src/CollisionTimePrinter.cxx
  src/MIDDigitizerSpec.cxx
  src/TPCDigitizerSpec.cxx
  src/TPCDigitRootWriterSpec.cxx
  src/ITSMFTDigitizerSpec.cxx
  src/ITSMFTDigitWriterSpec.cxx
  src/TOFDigitizerSpec.cxx
  src/TOFDigitWriterSpec.cxx
  src/TOFClusterizerSpec.cxx
  src/TOFClusterWriterSpec.cxx
  src/FITDigitizerSpec.cxx
  src/FITDigitWriterSpec.cxx
  src/EMCALDigitizerSpec.cxx
  src/EMCALDigitWriterSpec.cxx
  src/HMPIDDigitizerSpec.cxx
  src/MCHDigitizerSpec.cxx
  src/TRDDigitizerSpec.cxx
  src/GRPUpdaterSpec.cxx

  BUCKET_NAME ${MODULE_BUCKET_NAME}
)
