import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
                                fileNames = cms.untracked.vstring(
                                  'file:/uscms/home/ali/nobackup/CMSSW_11_1_6/src/23251.0_QCD_Pt15To7000_Flat_14+QCD_Pt15To7000_Flat_14TeV_TuneCP5_2026D49_GenSimHLBeamSpotFull14+DigiFullTrigger_2026D49+RecoFullGlobal_2026D49+HARVESTFullGlobal_2026D49/step3.root'
                                )
                           )

#process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic_T15', '')

process.load("SimTracker.TrackerHitAssociation.tpClusterProducer_cfi")
process.load("SimTracker.TrackAssociatorProducers.quickTrackAssociatorByHits_cfi")
process.load('SimTracker.TrackAssociation.trackingParticleRecoTrackAsssociation_cfi')

process.JetAnalyzer = cms.EDAnalyzer('JetAnalyzer',
                                     tracks = cms.untracked.InputTag("generalTracks"),
                                     rectosim = cms.untracked.InputTag("trackingParticleRecoTrackAsssociation"),
                                    )

process.TFileService = cms.Service("TFileService", fileName = cms.string("JetTree.root") )

process.runseq = cms.Sequence()
#process.runseq += process.tpClusterProducer + process.quickTrackAssociatorByHits + process.trackingParticleRecoTrackAsssociation
process.runseq += process.JetAnalyzer
process.path = cms.Path(process.runseq)
process.schedule = cms.Schedule(process.path)

