// Histograms the per-event PF output multiplicities (nOutPF*) put in the event by
// L1TCorrelatorLayer1Producer, for a list of producer instances. One TFileService
// directory per producer label.
#include <array>
#include <string>
#include <vector>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TH1I.h"

class L1TCorrelatorLayer1PFCountHistos : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit L1TCorrelatorLayer1PFCountHistos(const edm::ParameterSet &);
  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

private:
  void analyze(const edm::Event &, const edm::EventSetup &) override;

  static constexpr unsigned int nTypes = 4;
  static constexpr std::array<const char *, nTypes> names_ = {"pfcharged", "pfphoton", "pfneutral", "pfmuon"};
  static constexpr std::array<const char *, nTypes> instances_ = {
      "nOutPFcharged", "nOutPFphoton", "nOutPFneutral", "nOutPFmuon"};

  struct Src {
    std::array<edm::EDGetTokenT<unsigned int>, nTypes> tokens;
    std::array<TH1I *, nTypes> histos;
  };
  std::vector<Src> srcs_;
};

L1TCorrelatorLayer1PFCountHistos::L1TCorrelatorLayer1PFCountHistos(const edm::ParameterSet &iConfig) {
  usesResource(TFileService::kSharedResource);
  edm::Service<TFileService> fs;
  const int nBins = iConfig.getParameter<int>("nBins");
  const double xMax = iConfig.getParameter<double>("xMax");
  for (const auto &label : iConfig.getParameter<std::vector<std::string>>("producers")) {
    TFileDirectory dir = fs->mkdir(label);
    Src src;
    for (unsigned int i = 0; i < nTypes; ++i) {
      src.tokens[i] = consumes<unsigned int>(edm::InputTag(label, instances_[i]));
      src.histos[i] = dir.make<TH1I>(names_[i],
                                     (label + ": number of " + names_[i] + " objects with pt > 0;N;events").c_str(),
                                     nBins,
                                     -0.5,
                                     xMax - 0.5);
    }
    srcs_.push_back(src);
  }
}

void L1TCorrelatorLayer1PFCountHistos::analyze(const edm::Event &iEvent, const edm::EventSetup &) {
  for (auto &src : srcs_) {
    for (unsigned int i = 0; i < nTypes; ++i) {
      src.histos[i]->Fill(iEvent.get(src.tokens[i]));
    }
  }
}

void L1TCorrelatorLayer1PFCountHistos::fillDescriptions(edm::ConfigurationDescriptions &descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::vector<std::string>>("producers", {"l1tLayer1Barrel"});
  desc.add<int>("nBins", 500);
  desc.add<double>("xMax", 500.);
  descriptions.add("l1tCorrelatorLayer1PFCountHistos", desc);
}

DEFINE_FWK_MODULE(L1TCorrelatorLayer1PFCountHistos);
