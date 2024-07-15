// -*- C++ -*-
//
// Package:    L1Trigger/L1CaloTrigger
// Class:      L1TCaloBarrelToCorrelator
//
/*
 Description: Creates 3x3 PF clusters from GCTintTowers to be sent to correlator. Follows firmware logic, creates 8 clusters per (2+17+2)x(2+4+2).

 Implementation: To be run together with Phase2L1CaloEGammaEmulator.
*/


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/L1TCalorimeterPhase2/interface/DigitizedClusterCorrelator.h"
#include "DataFormats/L1TCalorimeterPhase2/interface/GCTBarrelDigiClusterToCorrLayer1.h"

#include <ap_int.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cstdio>
#include "L1Trigger/L1CaloTrigger/interface/Phase2L1CaloBarrelToCorrelator.h"

//
// class declaration
//

class Phase2GCTBarrelToCorrelatorLayer1 : public edm::stream::EDProducer<> {
public:
  explicit Phase2GCTBarrelToCorrelatorLayer1(const edm::ParameterSet&);
  ~Phase2GCTBarrelToCorrelatorLayer1() override = default;

private:
  void produce(edm::Event&, const edm::EventSetup&) override;

  // ----------member data ---------------------------
  const edm::EDGetTokenT<l1tp2::DigitizedClusterCorrelatorCollection> digiInputClusterToken_;
};

//
// constructors and destructor
//
Phase2GCTBarrelToCorrelatorLayer1::Phase2GCTBarrelToCorrelatorLayer1(const edm::ParameterSet& iConfig) 
  // gctClustersSrc_(consumes<l1tp2::CaloCrystalClusterCollection >(cfg.getParameter<edm::InputTag>("gctClusters"))),
  : digiInputClusterToken_(consumes<l1tp2::DigitizedClusterCorrelatorCollection>(iConfig.getParameter<edm::InputTag>("gctDigiClustersInput")))
{
  produces<l1tp2::GCTBarrelDigiClustersToCorrLayer1Collection>("GCTBarrelDigiClustersToCorrLayer1");
}

void Phase2GCTBarrelToCorrelatorLayer1::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {

  using namespace edm;

  //***************************************************//
  // Get the GCT digitized clusters 
  //***************************************************//
  edm::Handle<l1tp2::DigitizedClusterCorrelatorCollection> inputGCTBarrelClusters;
  iEvent.getByToken(digiInputClusterToken_, inputGCTBarrelClusters);
  
  // All of the clusters (no duplicates)
  auto outputClustersFromBarrel = std::make_unique<l1tp2::GCTBarrelDigiClustersToCorrLayer1Collection>();

  // Clusters output by GCT SLR (duplicates included )
  auto out_GCT1_SLR1 = std::make_unique<l1tp2::GCTBarrelDigiClustersToCorrLayer1Collection>();
  auto out_GCT1_SLR3 = std::make_unique<l1tp2::GCTBarrelDigiClustersToCorrLayer1Collection>();
  auto out_GCT2_SLR1 = std::make_unique<l1tp2::GCTBarrelDigiClustersToCorrLayer1Collection>();
  auto out_GCT2_SLR3 = std::make_unique<l1tp2::GCTBarrelDigiClustersToCorrLayer1Collection>();
  auto out_GCT3_SLR1 = std::make_unique<l1tp2::GCTBarrelDigiClustersToCorrLayer1Collection>();
  auto out_GCT3_SLR3 = std::make_unique<l1tp2::GCTBarrelDigiClustersToCorrLayer1Collection>();

  // format digitized correlators in correct format
  std::cout << "In the Phase2GCTBarrelToCorrelatorLayer1 produce method" << std::endl;

  // Loop over the regions: in order: GCT1 SLR1, GCT1 SLR3, GCT2 SLR1, GCT2 SLR3, GCT3 SLR1, GCT3SLR3
  int nRegions = 6;
  float regionCentersInDegrees[nRegions] = {10.0, 70.0, 130.0, -170.0, -110.0, -50.0};

  for (int i = 0; i < nRegions; i++) {
    for (auto &clusterIn : *inputGCTBarrelClusters.product()) {
     
      l1tp2::GCTBarrelDigiClusterToCorrLayer1 clusterOut;

      // Check if this cluster falls into each card 
      float clusterRealPhiAsDegree = clusterIn.realPhi() * 180/M_PI; 
      float regionLowerPhiBound = regionCentersInDegrees[i] - 60;
      float regionUpperPhiBound = regionCentersInDegrees[i] + 60;
      if ((clusterRealPhiAsDegree > regionLowerPhiBound) && (clusterRealPhiAsDegree < regionUpperPhiBound)) {
        std::cout << "[INFO: In region: " << i << "]: "; 
         std::cout << "Found digitized correlator Cluster with pT " << clusterIn.pt() * clusterIn.ptLSB()
                   << " with iEta and iPhi " << clusterIn.eta() << ", " << clusterIn.phi()
                  << " and real eta and phi " << clusterIn.realEta() << ", " << clusterIn.realPhi() 
                  << " where I have converted the real phi to degrees as " << clusterRealPhiAsDegree << std::endl;
      }
    }
  }


  // TODO: change this dummy example
  for (auto &clusterIn : *inputGCTBarrelClusters.product()) {
    std::cout << "Found digitized correlator Cluster with pT " << clusterIn.pt() * clusterIn.ptLSB()
              << " with iEta and iPhi " << clusterIn.eta() << ", " << clusterIn.phi() << " and real eta and phi " << clusterIn.realEta() << ", " << clusterIn.realPhi() << std::endl;
    l1tp2::GCTBarrelDigiClusterToCorrLayer1 clusterOut;
    outputClustersFromBarrel->push_back(clusterOut);
  }

  iEvent.put(std::move(outputClustersFromBarrel), "GCTBarrelDigiClustersToCorrLayer1");
}

//define this as a plug-in
DEFINE_FWK_MODULE(Phase2GCTBarrelToCorrelatorLayer1);