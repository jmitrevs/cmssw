#ifndef L1Trigger_Phase2L1ParticleFlow_newfirmware_gcteminput_ref_h
#define L1Trigger_Phase2L1ParticleFlow_newfirmware_gcteminput_ref_h

#include "DataFormats/L1TParticleFlow/interface/layer1_emulator.h"
#include "DataFormats/L1TCalorimeterPhase2/interface/GCTEmDigiCluster.h"

#include "L1Trigger/Phase2L1ParticleFlow/interface/corrector.h"
#include "L1Trigger/Phase2L1ParticleFlow/interface/ParametricResolution.h"

namespace edm {
  class ParameterSet;
  class ParameterSetDescription;
}  // namespace edm

namespace l1ct {
  class GctEmClusterDecoderEmulator {

  public:
    GctEmClusterDecoderEmulator() {};
    GctEmClusterDecoderEmulator(const edm::ParameterSet &pset);

    ~GctEmClusterDecoderEmulator();

    static edm::ParameterSetDescription getParameterSetDescription();

    l1ct::EmCaloObjEmu decode(const l1ct::DetectorSector<l1ct::EmCaloObjEmu> &sec,
                              const l1tp2::GCTEmDigiCluster &digi) const;

  private:
    edm::ParameterSet config_;
    
    // tools for GCT clusters
    l1tpf::corrector corrector_;
    l1tpf::ParametricResolution resol_;
  };

}  // namespace l1ct

#endif
