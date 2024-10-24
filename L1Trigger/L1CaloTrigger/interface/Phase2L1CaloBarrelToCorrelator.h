#ifndef PHASE_2_L1_CALO_BARREL_TO_CORRELATOR
#define PHASE_2_L1_CALO_BARREL_TO_CORRELATOR

#include "DataFormats/L1TCalorimeterPhase2/interface/CaloPFDigiClusterToCorrLayer1.h"
#include "DataFormats/L1TCalorimeterPhase2/interface/GCTBarrelDigiClusterToCorrLayer1.h"

#include "L1Trigger/L1CaloTrigger/interface/Phase2L1CaloEGammaUtils.h"

/* 
 * Comparators for sorting EG clusters
 */
inline bool p2eg::compareBarrelDigiClusterCorrelatorET(const l1tp2::GCTBarrelDigiClusterToCorrLayer1& lhs, const l1tp2::GCTBarrelDigiClusterToCorrLayer1& rhs) {
    return (lhs.ptFloat() > rhs.ptFloat());
}

/*
 * Sort the clusters in each egamma SLR in descending pT, then pad any zero clusters so that the total number of clusters in the SLR is six
 */
void p2eg::sortAndPad_eg_SLR(l1tp2::GCTBarrelDigiClusterToCorrLayer1Collection &thisSLR) {
    // input is a vector and can be sorted
    std::sort(thisSLR.begin(), thisSLR.end(), p2eg::compareBarrelDigiClusterCorrelatorET);
    int nClusters = thisSLR.size();
    std::cout << ">>> p2eg::sortAndPad_eg_SLR: Total entries in SLR: " << nClusters << std::endl;
    // if fewer than designated number of clusters, pad with zeros
    if (nClusters < p2eg::N_EG_CLUSTERS_PER_SLR) {
        // do padding. if size == 2, push back four clusters
        for (int i = 0; i < (p2eg::N_EG_CLUSTERS_PER_SLR - nClusters); i++) {
            l1tp2::GCTBarrelDigiClusterToCorrLayer1 zeroCluster;
            thisSLR.push_back(zeroCluster);
        }
    }
    std::cout << ">>> p2eg::sortAndPad_eg_SLR: After push-back: Total entries in SLR: " << thisSLR.size() << std::endl;

}

/* 
 * Comparators for sorting PF clusters
 */
inline bool p2eg::compareBarrelPFClustersET(const l1tp2::CaloPFDigiClusterToCorrLayer1& lhs, const l1tp2::CaloPFDigiClusterToCorrLayer1& rhs) {
    return (lhs.ptFloat() > rhs.ptFloat());
}

/*
 * Sort the clusters in each egamma SLR in descending pT, then pad any zero clusters so that the total number of clusters in the SLR is six
 */
void p2eg::sortAndPad_PF_SLR(l1tp2::CaloPFDigiClusterToCorrLayer1Collection &thisSLR) {
    // input is a vector and can be sorted
    std::sort(thisSLR.begin(), thisSLR.end(), p2eg::compareBarrelPFClustersET);
    int nClusters = thisSLR.size();
    std::cout << ">>> p2eg::sortAndPad_PF_SLR: Total entries in SLR: " << nClusters << std::endl;
    // if fewer than designated number of clusters, pad with zeros
    if (nClusters < p2eg::N_PF_CLUSTERS_PER_SLR) {
        // do padding. if size == 2, push back four clusters
        for (int i = 0; i < (p2eg::N_PF_CLUSTERS_PER_SLR - nClusters); i++) {
            l1tp2::CaloPFDigiClusterToCorrLayer1 zeroCluster;
            thisSLR.push_back(zeroCluster);
        }
    }
    std::cout << ">>> p2eg::sortAndPad_PF_SLR: After push-back: Total entries in SLR: " << thisSLR.size() << std::endl;

}

#endif