#include <memory>
#include <vector>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "TTree.h"

struct eventInfo
{
  int evt;
  std::vector <double> jet_pt;
  std::vector <double> jet_eta;
  std::vector <double> jet_phi;
};

class JetAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit JetAnalyzer(const edm::ParameterSet&);
  ~JetAnalyzer();

private:
  void beginJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void initEventStructure();

  TTree *eventTree;
  eventInfo *evInfo;

  edm::EDGetTokenT<edm::View<reco::Track>> TrackToken_;
  edm::EDGetTokenT<reco::RecoToSimCollection> rectosimToken_;
};

JetAnalyzer::JetAnalyzer(const edm::ParameterSet& iConfig)
    : TrackToken_(consumes< edm::View<reco::Track> >(iConfig.getUntrackedParameter<edm::InputTag>("tracks"))),
      rectosimToken_(consumes<reco::RecoToSimCollection>(iConfig.getUntrackedParameter<edm::InputTag>("rectosim")))
{
  evInfo = new eventInfo;
}

JetAnalyzer::~JetAnalyzer() {
}

// ------------ method called for each event  ------------
void JetAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  initEventStructure();
  std::cout << "Processing event: " << iEvent.id().event() << std::endl;
  evInfo->evt = iEvent.id().event();

  edm::Handle<reco::RecoToSimCollection > rectosimCollection;
  iEvent.getByToken(rectosimToken_,rectosimCollection);
  //iEvent.getByLabel("trackingParticleRecoTrackAsssociation", rectosimCollection);
  auto& recSimColl = *(rectosimCollection.product());

  edm::Handle<edm::View<reco::Track>> trackCollectionH;
  iEvent.getByToken(TrackToken_, trackCollectionH);
  const edm::View<reco::Track>& trackCollection = *(trackCollectionH.product());

  //edm::View<reco::Track>::size_type&
  for(edm::View<reco::Track>::size_type i=0; i<trackCollection.size(); ++i){
    edm::RefToBase<reco::Track> track(trackCollectionH, i);
    std::vector<std::pair<TrackingParticleRef, double> > tp;
    if(recSimColl.find(track) != recSimColl.end()){
      tp = recSimColl[track];
      if (tp.size()!=0) {
              TrackingParticleRef tpr = tp.begin()->first;
              double associationQuality = tp.begin()->second;
              std::cout << "asso. particle " << tpr->pdgId() << " with quality " << associationQuality << std::endl;
          }
    }
  }

  eventTree->Fill();

}

void JetAnalyzer::beginJob() {
  edm::Service<TFileService> fileService;
  if(!fileService) throw edm::Exception(edm::errors::Configuration, "TFileService is not registered in cfg file");

  eventTree = fileService->make<TTree>( "jettree", "jettree" );
  eventTree->Branch( "evt",                  &evInfo->evt);
  eventTree->Branch( "jet_pt",               &evInfo->jet_pt);
  eventTree->Branch( "jet_eta",              &evInfo->jet_eta);
  eventTree->Branch( "jet_phi",              &evInfo->jet_phi);
}

void JetAnalyzer::initEventStructure()
{
  evInfo->evt=-1;
  evInfo->jet_pt.clear();
  evInfo->jet_eta.clear();
  evInfo->jet_phi.clear();
}

//define this as a plug-in
DEFINE_FWK_MODULE(JetAnalyzer);
