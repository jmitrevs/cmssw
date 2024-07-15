#ifndef DataFormats_L1TCalorimeterPhase2_GCTBarrelDigiClusterToCorrLayer1_h
#define DataFormats_L1TCalorimeterPhase2_GCTBarrelDigiClusterToCorrLayer1_h

#include <ap_int.h>
#include <vector>

namespace l1tp2 {

  class GCTBarrelDigiClusterToCorrLayer1 {
  private:
    // Data
    unsigned long long int clusterData;
    unsigned int idxGCTCard;  // 0, 1, or 2
    unsigned int idxGCTRegion; // 0, 1, 2, 3, 4, or 5
    int etaSign; // +1 or -1

    // Constants
    static constexpr unsigned int n_towers_eta = 34;  // in GCT card unique region
    static constexpr unsigned int n_towers_phi = 24;  // in GCT card unique region
    static constexpr unsigned int n_crystals_in_tower = 5;
    static constexpr float LSB_PT = 0.5;                 // 0.5 GeV
    static constexpr float ETA_RANGE_ONE_SIDE = 1.4841;  // barrel goes from (-1.4841, +1.4841)
    static constexpr float LSB_ETA = ((2 * ETA_RANGE_ONE_SIDE) / (n_towers_eta * n_crystals_in_tower));  // (2.8 / 170)
    static constexpr float LSB_PHI = ((2 * M_PI) / (3 * n_towers_phi * n_crystals_in_tower));            // (2 pi * 360)

    // "top" of the correlator card #0 in GCT coordinates is iPhi tower index 24
    static constexpr int correlatorCard0_tower_iphi_offset = 24;
    // same but for correlator cards #1 and 2 (cards wrap around phi = 180 degrees):
    static constexpr int correlatorCard1_tower_iphi_offset = 48;
    static constexpr int correlatorCard2_tower_iphi_offset = 0;

    // start of the unused bits 
    static constexpr int n_bits_unused_start = 52; 

  public:
    GCTBarrelDigiClusterToCorrLayer1() { clusterData = 0x0; }

    GCTBarrelDigiClusterToCorrLayer1(ap_uint<64> data) { clusterData = data; }

    GCTBarrelDigiClusterToCorrLayer1(
                               unsigned int iGCTCard,
                               unsigned int iGCTRegion,
                               int etaSign,
                               ap_uint<12> pt,
                               ap_uint<7> etaCr,
                               ap_uint<7> phiCr,
                               ap_uint<4> hoe,
                               ap_uint<2> hoeFlag,
                               ap_uint<3> iso,
                               ap_uint<2> isoFlag,
                               ap_uint<6> fb,
                               ap_uint<5> timing,
                               ap_uint<2> shapeFlag,
                               ap_uint<2> brems) {
      idxGCTCard = iGCTCard;
      idxGCTRegion = iGCTRegion;
      clusterData = ((ap_uint<64>)pt) | (((ap_uint<64>)etaCr) << 12) | (((ap_uint<64>)phiCr) << 19) |
                    (((ap_uint<64>)hoe) << 26) | (((ap_uint<64>)hoeFlag) << 30) | (((ap_uint<64>)iso) << 35) |
                    (((ap_uint<64>)isoFlag) << 37) | (((ap_uint<64>)fb) << 43) | (((ap_uint<64>)timing) << 48) |
                    (((ap_uint<64>)shapeFlag << 50)) | (((ap_uint<64>)brems << 52));
    }

    // Getters
    ap_uint<64> data() const { return clusterData; }
    // which GCT card (0, 1, or 2)
    unsigned int gctCard() const { return idxGCTCard; }
    unsigned int gctRegion() const { return idxGCTRegion; }

    // Other getters
    float ptLSB() const { return LSB_PT; }
    ap_uint<12> pt() const { return (clusterData & 0xFFF); }

    // crystal eta in the correlator region (LSB: 2.8/170)
    ap_uint<7> eta() const { return ((clusterData >> 12) & 0xFF); }  // (eight 1's) 0b11111111 = 0xFF

    // crystal phi in the correlator region (LSB: 2pi/360)
    ap_uint<7> phi() const { return ((clusterData >> 19) & 0x7F); }  // (seven 1's) 0b1111111 = 0x7F

    // HoE value and flag: not defined yet in the emulator
    ap_uint<4> hoe() const { return ((clusterData >> 26) & 0xF); }      // (four 1's) 0b1111 = 0xF
    ap_uint<2> hoeFlag() const { return ((clusterData >> 30) & 0x3); }  // (two 1's) 0b11 = 0x3

    // Raw isolation sum: not saved in the emulator
    ap_uint<3> iso() const { return ((clusterData >> 35) & 0x7); }

    // iso flag: two bits, least significant bit is the standalone WP (true or false), second bit is the looseTk WP (true or false)
    // e.g. 0b01 : standalone iso flag passed, loose Tk iso flag did not pass
    ap_uint<2> isoFlags() const { return ((clusterData >> 37) & 0x3); }  // (two 1's) 0b11 = 0x3
    bool passes_iso() const { return (isoFlags() & 0x1); }               // standalone iso WP
    bool passes_looseTkiso() const { return (isoFlags() & 0x2); }        // loose Tk iso WP

    // fb and timing: not saved in the current emulator
    ap_uint<6> fb() const { return ((clusterData >> 43) & 0x3F); }
    ap_uint<5> timing() const { return ((clusterData >> 48) & 0x1F); }

    // shower shape shape flag: two bits, least significant bit is the standalone WP, second bit is the looseTk WP
    // e.g. 0b01 : standalone shower shape flag passed, loose Tk shower shape flag did not pass
    ap_uint<2> shapeFlags() const { return ((clusterData >> 50) & 0x3); }

    bool passes_ss() const { return (shapeFlags() & 0x1); }         // standalone shower shape WP
    bool passes_looseTkss() const { return (shapeFlags() & 0x2); }  // loose Tk shower shape WP

    // brems: not saved in the current emulator
    ap_uint<2> brems() const { return ((clusterData >> 52) & 0x3); }

    const int unusedBitsStart() const { return n_bits_unused_start; }

    // Other checks
    bool passNullBitsCheck(void) const { return ((data() >> unusedBitsStart()) == 0x0); }

    // // Get real eta (does not depend on card number). crystal iEta = 0 starts at real eta -1.4841.
    // float realEta() const { return (float)((-1 * ETA_RANGE_ONE_SIDE) + (eta() * LSB_ETA)); }

    // // Get real phi (uses card number).
    // float realPhi() const {
    //   // each card starts at a different real phi
    //   int offset_tower = 0;
    //   if (cardNumber() == 0) {
    //     offset_tower = correlatorCard0_tower_iphi_offset;
    //   } else if (cardNumber() == 1) {
    //     offset_tower = correlatorCard1_tower_iphi_offset;
    //   } else if (cardNumber() == 2) {
    //     offset_tower = correlatorCard2_tower_iphi_offset;
    //   }
    //   int thisPhi = (phi() + (offset_tower * n_crystals_in_tower));
    //   // crystal iPhi = 0 starts at real phi = -180 degrees
    //   return (float)((-1 * M_PI) + (thisPhi * LSB_PHI));
    // }
  };

  // Collection typedef
  typedef std::vector<l1tp2::GCTBarrelDigiClusterToCorrLayer1> GCTBarrelDigiClustersToCorrLayer1Collection;

}  // namespace l1tp2

#endif
