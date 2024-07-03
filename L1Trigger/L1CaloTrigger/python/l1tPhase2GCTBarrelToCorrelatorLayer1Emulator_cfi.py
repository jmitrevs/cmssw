import FWCore.ParameterSet.Config as cms

l1tPhase2GCTBarrelToCorrelatorLayer1Emulator = cms.EDProducer("Phase2GCTBarrelToCorrelatorLayer1", 
    
    gctDigiClustersInput = cms.InputTag("l1tPhase2L1CaloEGammaEmulator", "GCTDigitizedClusterToCorrelator", "REPR"),
    
)