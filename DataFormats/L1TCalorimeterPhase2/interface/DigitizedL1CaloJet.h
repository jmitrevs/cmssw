#ifndef DataFormats_L1TCalorimeterPhase2_DigitizedL1CaloJet_h
#define DataFormats_L1TCalorimeterPhase2_DigitizedL1CaloJet_h

#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include "DataFormats/L1Trigger/interface/L1Candidate.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

namespace l1tp2 {

  class DigitizedL1CaloJet : public l1t::L1Candidate {
  public:
    DigitizedL1CaloJet()
        : l1t::L1Candidate(),
          jetEt_(0.),
          jetEta_(-99.),
          jetPhi_(-99.) {}

    DigitizedL1CaloJet(const PolarLorentzVector& p4,
                    float jetEt,
                    float jetEta,
                    float jetPhi)
        : l1t::L1Candidate(p4),
          jetEt_(jetEt),
          jetEta_(jetEta),
          jetPhi_(jetPhi) {}

    inline float jetEt() const { return jetEt_; };
    inline float jetEta() const { return jetEta_; };
    inline float jetPhi() const { return jetPhi_; };

    void setJetEt(float jetEtIn) { jetEt_ = jetEtIn; };
    void setJetEta(float jetEtaIn) { jetEta_ = jetEtaIn; };
    void setJetPhi(float jetPhiIn) { jetPhi_ = jetPhiIn; };

  private:
    // ET
    float jetEt_;
    // Tower (real) eta
    float jetEta_;
    // Tower (real) phi
    float jetPhi_;
  };

  // Concrete collection of output objects (with extra tuning information)
  typedef std::vector<l1tp2::DigitizedL1CaloJet> DigitizedL1CaloJetCollection;
}  // namespace l1tp2
#endif
