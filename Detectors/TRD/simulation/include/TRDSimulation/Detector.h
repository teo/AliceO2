// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef ALICEO2_TRD_DETECTOR_H_
#define ALICEO2_TRD_DETECTOR_H_

#include <vector>
#include "DetectorsBase/Detector.h"
#include "SimulationDataFormat/BaseHits.h"
#include "CommonUtils/ShmAllocator.h"

#include "TRDSimulation/TRsim.h"

class FairVolume;

namespace o2
{
namespace trd
{
class HitType : public o2::BasicXYZQHit<float>
{
 public:
  using BasicXYZQHit<float>::BasicXYZQHit;
};
} // namespace trd
} // namespace o2

#ifdef USESHM
namespace std
{
template <>
class allocator<o2::trd::HitType> : public o2::utils::ShmAllocator<o2::trd::HitType>
{
};
} // namespace std
#endif

namespace o2
{
namespace trd
{
class TRDGeometry;

class Detector : public o2::base::DetImpl<Detector>
{
 public:
  Detector(Bool_t active = true);
  ~Detector() override;
  void InitializeO2Detector() override;
  bool ProcessHits(FairVolume* v = nullptr) override;
  void Register() override;
  std::vector<HitType>* getHits(int iColl) const
  {
    if (iColl == 0) {
      return mHits;
    }
    return nullptr;
  }
  void FinishEvent() override;
  void Reset() override;
  void EndOfEvent() override;
  void createMaterials();
  void ConstructGeometry() override;
  /// Add alignable top volumes
  void addAlignableVolumes() const override;

 private:
  /// copy constructor (used in MT)
  Detector(const Detector& rhs);

  // defines/sets-up the sensitive volumes
  void defineSensitiveVolumes();

  // addHit
  template <typename T>
  void addHit(T x, T y, T z, T tof, int charge, int trackId, int detId);

  // Create TR hits
  void createTRhit(int);

  std::vector<HitType>* mHits = nullptr; ///!< Collection of TRD hits

  float mFoilDensity;
  float mGasNobleFraction;
  float mGasDensity;

  bool mTRon; // Switch for TR simulation
  TRsim* mTR; // Access to TR simulation

  float mWion; // Ionization potential

  TRDGeometry* mGeom = nullptr;

  template <typename Det>
  friend class o2::base::DetImpl;
  ClassDefOverride(Detector, 1)
};

template <typename T>
void Detector::addHit(T x, T y, T z, T tof, int charge, int trackId, int detId)
{
  mHits->emplace_back(x, y, z, tof, charge, trackId, detId);
}

} // namespace trd
} // namespace o2

#ifdef USESHM
namespace o2
{
namespace base
{
template <>
struct UseShm<o2::trd::Detector> {
  static constexpr bool value = true;
};
} // namespace base
} // namespace o2
#endif
#endif
