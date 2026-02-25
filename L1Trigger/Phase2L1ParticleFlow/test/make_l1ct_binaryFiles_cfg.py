import argparse
import sys
import math

# example: cmsRun L1Trigger/Phase2L1ParticleFlow/test/make_l1ct_patternFiles_cfg.py --patternFilesOFF
# example: cmsRun L1Trigger/Phase2L1ParticleFlow/test/make_l1ct_patternFiles_cfg.py --dumpFilesOFF --serenity

parser = argparse.ArgumentParser(prog=sys.argv[0], description='Optional parameters')

parser.add_argument("--dumpFilesOFF", help="switch on dump file production", action="store_true", default=False)
parser.add_argument("--patternFilesOFF", help="switch on Layer-1 pattern file production", action="store_true", default=False)
parser.add_argument("--serenity", help="use Serenity settigns as default everwhere, i.e. also for barrel", action="store_true", default=False)
parser.add_argument("--tm18", help="Add TM18 emulators", action="store_true", default=False)
parser.add_argument("--split18", help="Make 3 TM18 layer 1 pattern files", action="store_true", default=False)

args = parser.parse_args()

if args.dumpFilesOFF:
    print(f'Switching off dump file creation: dumpFilesOFF is {args.dumpFilesOFF}')
if args.patternFilesOFF:
    print(f'Switching off pattern file creation: patternFilesOFF is {args.patternFilesOFF}')

import FWCore.ParameterSet.Config as cms
from Configuration.StandardSequences.Eras import eras

process = cms.Process("RESP", eras.Phase2C17I13M9)

process.load('Configuration.StandardSequences.Services_cff')
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True), allowUnscheduled = cms.untracked.bool(False) )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1))
process.MessageLogger.cerr.FwkReport.reportEvery = 1

process.source = cms.Source("PoolSource",
            fileNames = cms.untracked.vstring('file:inputs140X_1.root',
                ),
    inputCommands = cms.untracked.vstring("keep *",
            "drop l1tPFClusters_*_*_*",
            "drop l1tPFTracks_*_*_*",
            "drop l1tPFCandidates_*_*_*",
            "drop l1tTkPrimaryVertexs_*_*_*")
)

process.load('Configuration.Geometry.GeometryExtendedRun4D110Reco_cff')
process.load('Configuration.Geometry.GeometryExtendedRun4D110_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('SimCalorimetry.HcalTrigPrimProducers.hcaltpdigi_cff') # needed to read HCal TPs
process.load('SimCalorimetry.HGCalSimProducers.hgcalDigitizer_cfi') # needed for HGCAL_noise_fC
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('RecoMET.Configuration.GenMETParticles_cff')
process.load('RecoMET.METProducers.genMetTrue_cfi')

from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
from RecoMET.METProducers.pfMet_cfi import pfMet

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '141X_mcRun4_realistic_v3', '')

# NOTE: we need this to avoid saving the stubs
process.l1tTrackSelectionProducer.processSimulatedTracks = False

from L1Trigger.L1CaloTrigger.l1tPhase2L1CaloEGammaEmulator_cfi import l1tPhase2L1CaloEGammaEmulator
process.l1tPhase2L1CaloEGammaEmulator = l1tPhase2L1CaloEGammaEmulator.clone()

from L1Trigger.L1CaloTrigger.l1tPhase2CaloPFClusterEmulator_cfi import l1tPhase2CaloPFClusterEmulator
process.l1tPhase2CaloPFClusterEmulator = l1tPhase2CaloPFClusterEmulator.clone()

from L1Trigger.L1CaloTrigger.l1tPhase2GCTBarrelToCorrelatorLayer1Emulator_cfi import l1tPhase2GCTBarrelToCorrelatorLayer1Emulator
process.l1tPhase2GCTBarrelToCorrelatorLayer1Emulator = l1tPhase2GCTBarrelToCorrelatorLayer1Emulator.clone()

from L1Trigger.Phase2L1ParticleFlow.L1NNTauProducer_cff import l1tNNTauProducerPuppi
process.l1tNNTauProducerPuppi = l1tNNTauProducerPuppi.clone()

from L1Trigger.Phase2L1ParticleFlow.l1tMETPFProducer_cfi import l1tMETPFProducer
process.l1tMETPFProducer = l1tMETPFProducer.clone()

from L1Trigger.L1CaloTrigger.l1tPhase2CaloToCorrelatorTMI18_cfi import l1tPhase2CaloToCorrelatorTMI18
process.l1tPhase2CaloToCorrelatorTMI18 = l1tPhase2CaloToCorrelatorTMI18.clone()

process.L1TInputTask = cms.Task(
        process.l1tPhase2L1CaloEGammaEmulator,
        process.l1tPhase2CaloPFClusterEmulator,
        process.l1tPhase2GCTBarrelToCorrelatorLayer1Emulator,
        process.l1tPhase2CaloToCorrelatorTMI18,
        process.l1tSAMuonsGmt,
        process.l1tGTTInputProducer,
        process.l1tTrackSelectionProducer,
        process.l1tVertexFinderEmulator,
        # process.l1tLayer1Barrel,
        # process.l1tLayer1HGCal,
        # process.l1tLayer1HGCalNoTK,
        process.L1TLayer1TaskInputsTask,
        # process.L1TLayer1Task,
        process.L1TLayer2EGTask,
        process.l1tMETPFProducer)

process.centralGen = cms.EDFilter("CandPtrSelector", src = cms.InputTag("genParticlesForMETAllVisible"), cut = cms.string("abs(eta) < 2.4"))
process.barrelGen = cms.EDFilter("CandPtrSelector", src = cms.InputTag("genParticlesForMETAllVisible"), cut = cms.string("abs(eta) < 1.5"))
process.genMetCentralTrue = process.genMetTrue.clone(src = cms.InputTag("centralGen"))
process.L1TInputTask.add(
    process.genParticlesForMETAllVisible,
    process.centralGen,
    process.barrelGen,
    process.genMetCentralTrue
)

#### ntuple producer -- TO BE REMOVED
process.load('L1Trigger.L1CaloPhase2Analyzer.l1TCaloAnalyzer_cfi')
process.TFileService = cms.Service("TFileService", fileName = cms.string("perfTuple.root"))

## Realistic barrel emulation
process.l1tLayer1BarrelTDR = process.l1tLayer1Barrel.clone()
process.l1tLayer1BarrelTDR.regionizerAlgo = cms.string("TDR")
process.l1tLayer1BarrelTDR.regionizerAlgoParameters = cms.PSet(
        nTrack = cms.uint32(22),
        nCalo = cms.uint32(15),
        nEmCalo = cms.uint32(12),
        nMu = cms.uint32(2),
        debug = cms.untracked.bool(True),
        debug_emcalo = cms.untracked.bool(True)
    )
process.l1tLayer1BarrelTDR.pfAlgoParameters.nTrack = 22
process.l1tLayer1BarrelTDR.pfAlgoParameters.nSelCalo = 15
process.l1tLayer1BarrelTDR.pfAlgoParameters.nCalo = 15
process.l1tLayer1BarrelTDR.pfAlgoParameters.nAllNeutral = 27
process.l1tLayer1BarrelTDR.puAlgoParameters.nTrack = 22
process.l1tLayer1BarrelTDR.puAlgoParameters.nIn = 27
process.l1tLayer1BarrelTDR.puAlgoParameters.nOut = 27
process.l1tLayer1BarrelTDR.puAlgoParameters.finalSortAlgo = "BitonicVHDL"
process.l1tLayer1BarrelTDR.tkEgAlgoParameters.nTRACK_EGIN = 22
process.l1tLayer1BarrelTDR.tkEgAlgoParameters.nEMCALO_EGIN = 12

if args.serenity:
    process.l1tLayer1.pfProducers[0] = "l1tLayer1BarrelSerenity"
    process.l1tLayer2EG.tkElectrons[1].pfProducer = "l1tLayer1BarrelSerenity:L1TkElePerBoard"
    process.l1tLayer2EG.tkEms[2].pfProducer = "l1tLayer1BarrelSerenity:L1TkEmPerBoard"

from L1Trigger.Phase2L1ParticleFlow.l1ctLayer1_patternWriters_cff import *
from L1Trigger.Phase2L1ParticleFlow.l1ctLayer1_patternWriters_cff import _eventsPerFile
if not args.patternFilesOFF:
    process.l1tLayer1Barrel.patternWriters = cms.untracked.VPSet(*barrelWriterConfigs)
    process.l1tLayer1BarrelTDR.patternWriters = cms.untracked.VPSet(barrelInputWriterConfigsAPx,
                                                                    barrelOutputWriterConfigsAPx,
                                                                    barrelWriterDebugPFInConfigsAPx,
                                                                    barrelWriterDebugPFOutConfigsAPx
                                                                    )
    # process.l1tLayer1BarrelSerenity.patternWriters = cms.untracked.VPSet(barrelSerenityVU9PPhi1Config,barrelSerenityVU13PPhi1Config)
    process.l1tLayer1HGCal.patternWriters = cms.untracked.VPSet(*hgcalWriterConfigs)
    process.l1tLayer1HGCalElliptic.patternWriters = cms.untracked.VPSet(*hgcalWriterConfigs)
    process.l1tLayer1HGCalNoTK.patternWriters = cms.untracked.VPSet(*hgcalNoTKWriterConfigs)
    process.l1tLayer1HF.patternWriters = cms.untracked.VPSet(*hfWriterConfigs)

process.l1tSC4NGJetProducer.jets = cms.InputTag("l1tSC4PFL1PuppiEmulator")
process.l1tSC4NGJetProducer.doJEC = cms.bool(True)
process.l1tSC4NGJetProducer.correctorFile = cms.string("L1Trigger/Phase2L1ParticleFlow/data/jecs/jecs_20220308.root")
process.l1tSC4NGJetProducer.correctorDir = cms.string("L1PuppiSC4EmuJets")

from L1Trigger.Phase2L1ParticleFlow.l1tMHTPFProducer_cfi import l1tMHTPFProducer
process.l1tNGMHTPFProducer = l1tMHTPFProducer.clone(jets = cms.InputTag("l1tSC4NGJetProducer","l1tSC4NGJets"))

process.l1tLayer1HGCal.hgcalInputConversionParameters.emulateCorrections = True
process.l1tLayer1HGCalElliptic.hgcalInputConversionParameters.emulateCorrections = True
process.l1tLayer1HGCalNoTK.hgcalInputConversionParameters.emulateCorrections = True

# The main path
process.runPF = cms.Path(
    process.l1NtupleProducer +
    # process.l1tLayer1Barrel +
    process.l1tLayer1BarrelTDR
    # process.l1tLayer1HGCal +
    # process.l1tLayer1HGCalElliptic +
    # process.l1tLayer1HGCalNoTK +
    # process.l1tLayer1HF +
    # process.l1tLayer1 +
    # process.l1tLayer1BarrelExtended +
    # process.l1tLayer1HGCalExtended +
    # process.l1tLayer2Deregionizer +
    # process.l1tSC4PFL1PuppiEmulator +
    # process.l1tSC4PFL1PuppiCorrectedEmulator +
    # process.l1tSC4NGJetProducer +
    # process.l1tMHTPFProducer +
    # process.l1tNGMHTPFProducer +
    # process.l1tSC4PFL1PuppiCorrectedEmulatorMHT +
    # process.l1tSC8PFL1PuppiCorrectedEmulator +
    # # process.l1tLayer2SeedConeJetWriter +
    # process.l1tLayer2EG
)
#### add L1T producers to path
process.runPF.associate(process.L1TInputTask)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(5) )



