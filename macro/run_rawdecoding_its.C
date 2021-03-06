#if !defined(__CLING__) || defined(__ROOTCLING__)

#include <TTree.h>
#include <TFile.h>
#include <TStopwatch.h>
#include <FairLogger.h>
#include <vector>
#include <string>

#include "ITSMFTReconstruction/ChipMappingITS.h"
#include "ITSMFTReconstruction/GBTWord.h"
#include "ITSMFTReconstruction/PayLoadCont.h"
#include "ITSMFTReconstruction/PixelData.h"
#include "DataFormatsITSMFT/ROFRecord.h"
#include "ITSMFTReconstruction/RawPixelReader.h"
#include "CommonDataFormat/InteractionRecord.h"

#endif

// example of ITS raw data decoding
// Data can be prepared from the MC digits using run_digi2raw_its.C
// The padding parameter should be set to "true" for CRU data and to "false" for
// the data obtained by the removing the 128 bit padding from GBT words

void run_rawdecoding_its(std::string inpName = "rawits.bin", // input binary data file name
                         std::string outDigName = "",        // name for optinal digits tree
                         bool outDigPerROF = false,          // in case digits are requested, create separate tree entry for each ROF
                         bool padding = true,                // payload in raw data comes in 128 bit CRU words
                         bool page8kb = true,                // full 8KB CRU pages are provided (no skimming applied)
                         int verbose = 0)
{

  o2::ITSMFT::RawPixelReader<o2::ITSMFT::ChipMappingITS> rawReader;
  rawReader.openInput(inpName);
  rawReader.setPadding128(padding); // payload GBT words are padded to 16B
  rawReader.imposeMaxPage(page8kb); // pages are 8kB in size (no skimming)
  rawReader.setVerbosity(verbose);

  o2::ITSMFT::ChipPixelData chipData;
  TStopwatch sw;
  sw.Start();
  uint32_t roFrame = 0;
  o2::InteractionRecord irHB, irTrig;
  std::vector<o2::ITSMFT::Digit> digits, *digitsPtr = &digits;
  std::vector<o2::ITSMFT::ROFRecord> rofRecVec, *rofRecVecPtr = &rofRecVec;
  std::size_t rofEntry = 0, nrofdig = 0;
  std::unique_ptr<TFile> outFileDig;
  std::unique_ptr<TTree> outTreeDig; // output tree with digits
  std::unique_ptr<TTree> outTreeROF; // output tree with ROF records

  if (!outDigName.empty()) { // output to digit is requested
    outFileDig = std::make_unique<TFile>(outDigName.c_str(), "recreate");
    outTreeDig = std::make_unique<TTree>("o2sim", "Digits tree");
    outTreeDig->Branch("ITSDigit", &digitsPtr);
    outTreeROF = std::make_unique<TTree>("ITSDigitROF", "ROF records tree");
    outTreeROF->Branch("ITSDigitROF", &rofRecVecPtr);
  }

  while (rawReader.getNextChipData(chipData)) {
    if (verbose >= 10) {
      chipData.print();
    }

    if (outTreeDig) { // >> store digits
      if (irHB != rawReader.getInteractionRecordHB() || irTrig != rawReader.getInteractionRecord()) {
        if (!irTrig.isDummy()) {
          o2::dataformats::EvIndex<int, int> evId(outTreeDig->GetEntries(), rofEntry);
          rofRecVec.emplace_back(irHB, roFrame, evId, nrofdig); // registed finished ROF
          if (outDigPerROF) {
            outTreeDig->Fill();
            digits.clear();
          }
          roFrame++;
        }
        irHB = rawReader.getInteractionRecordHB();
        irTrig = rawReader.getInteractionRecord();
        rofEntry = digits.size();
        nrofdig = 0;
      }
      const auto& pixdata = chipData.getData();
      for (const auto& pix : pixdata) {
        digits.emplace_back(chipData.getChipID(), roFrame, pix.getRowDirect(), pix.getCol());
        nrofdig++;
      }

      printf("ROF %7d ch: %5d IR: ", roFrame, chipData.getChipID());
      irHB.print();

    } // << store digits
    //
  }

  if (outTreeDig) {
    // register last ROF
    o2::dataformats::EvIndex<int, int> evId(outTreeDig->GetEntries(), rofEntry);
    rofRecVec.emplace_back(irHB, roFrame, evId, nrofdig); // registed finished ROF

    // fill last (and the only one?) entry
    outTreeDig->Fill();
    outTreeROF->Fill();

    // and store trees
    outTreeDig->Write();
    outTreeROF->Write();
  }

  sw.Stop();

  const auto& MAP = rawReader.getMapping();
  for (int ir = 0; ir < MAP.getNRUs(); ir++) {
    const auto ruStat = rawReader.getRUDecodingStatSW(ir);
    if (ruStat && ruStat->nPackets) {
      printf("\nStatistics for RU%3d (HWID:0x%4x)\n", ir, MAP.RUSW2FEEId(ir, 0));
      ruStat->print();
    }
  }
  rawReader.getDecodingStat().print();

  sw.Print();
}
