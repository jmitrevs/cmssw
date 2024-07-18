#ifndef DataFormats_L1TCalorimeterPhase2_GCTBarrelDigiClusterToCorrLayer1_h
#define DataFormats_L1TCalorimeterPhase2_GCTBarrelDigiClusterToCorrLayer1_h

#include <ap_int.h>
#include <vector>

namespace l1tp2 {

  class GCTBarrelDigiClusterToCorrLayer1 {
  private:
    // Data (note: positional information is entirely encoded in the location in the output array)
    unsigned long long int clusterData;

    // Constants
    static constexpr float LSB_PT = 0.5;                 // 0.5 GeV

    // start of the unused bits 
    static constexpr int n_bits_unused_start = 52; 

  public:
    GCTBarrelDigiClusterToCorrLayer1() { clusterData = 0x0; }

    GCTBarrelDigiClusterToCorrLayer1(ap_uint<64> data) { clusterData = data; }

    GCTBarrelDigiClusterToCorrLayer1(
                               ap_uint<12> pt,
                               int etaCr,
                               int phiCr,
                               ap_uint<4> hoe,
                               ap_uint<2> hoeFlag,
                               ap_uint<3> iso,
                               ap_uint<2> isoFlag,
                               ap_uint<6> fb,
                               ap_uint<5> timing,
                               ap_uint<2> shapeFlag,
                               ap_uint<2> brems) {
      std::cout << "... In constructor: etaCr and phiCr are " << etaCr << ", " << phiCr << std::endl;
      // iEta is a signed quantity, in bits 12 through 19 (8 bits). Bit 19 stores the sign of iEta (1 if iEta is positive, -1 if iEta is negative)
      // This leaves 7 bits for the magnitude (bits 12 through 18)
      ap_uint<64> temp_data_eta = 0x0; 
      if (etaCr > 0) temp_data_eta |= (0x1 << 19);   // set bit 19 to 1 if iEta is positive 
      temp_data_eta |= ((0x7F & abs(etaCr)) << 12);  

      // Repeat for phi, which is bits 20 through 26 (7 bits). The sign bit is 26, leaving 6 bits for the magnitude
      ap_uint<64> temp_data_phi = 0x0;
      if (phiCr > 0) temp_data_phi |= (0x1 << 26);   // set bit 26 to 1 if iPhi is positive 
      temp_data_phi |= ((0x3F & abs(phiCr)) << 20);   // 0x3F is 0b111111 (six 1's)

      clusterData = ((ap_uint<64>)pt) | temp_data_eta | temp_data_phi |
                    (((ap_uint<64>)hoe) << 27) | (((ap_uint<64>)hoeFlag) << 31) | (((ap_uint<64>)iso) << 36) |
                    (((ap_uint<64>)isoFlag) << 38) | (((ap_uint<64>)fb) << 44) | (((ap_uint<64>)timing) << 49) |
                    (((ap_uint<64>)shapeFlag << 51)) | (((ap_uint<64>)brems << 53));
    }

    // Getters
    ap_uint<64> data() const { return clusterData; }

    // Other getters
    float ptLSB() const { return LSB_PT; }
    ap_uint<12> pt() const { return (clusterData & 0xFFF); }
    float ptFloat() const { return pt() * ptLSB(); }

    // crystal eta (signed quantity)
    int eta() const { 
      int signed_val; 
      // get the sign bit (the 19th bit). If it is 1, eta is positive. If it is 0, eta is negative
      // Magnitude is seven bits: bits 12 through 18.
      if (clusterData & (0b1 << 19)) signed_val = ((clusterData >> 12) & 0x7F); 
      else signed_val = - ((clusterData >> 12) & 0x7F); 

      return signed_val;
    }  

    // crystal phi (signed quantity)
    int phi() const { 
      int signed_val; 
      // get the sign bit (the 26th bit). If it is 1, phi is positive. If it is 0, phi is negative
      // Magnitude is bits 20 through 25
      if (clusterData & (0b1 << 26)) signed_val = ((clusterData >> 20) & 0x3F);  // 0x3F is six 1's
      else signed_val = - ((clusterData >> 20) & 0x3F); 

      return signed_val;
    }  

    // HoE value and flag: not defined yet in the emulator
    ap_uint<4> hoe() const { return ((clusterData >> 27) & 0xF); }      // (four 1's) 0b1111 = 0xF
    ap_uint<2> hoeFlag() const { return ((clusterData >> 31) & 0x3); }  // (two 1's) 0b11 = 0x3

    // Raw isolation sum: not saved in the emulator
    ap_uint<3> iso() const { return ((clusterData >> 36) & 0x7); }

    // iso flag: two bits, least significant bit is the standalone WP (true or false), second bit is the looseTk WP (true or false)
    // e.g. 0b01 : standalone iso flag passed, loose Tk iso flag did not pass
    ap_uint<2> isoFlags() const { return ((clusterData >> 38) & 0x3); }  // (two 1's) 0b11 = 0x3
    bool passes_iso() const { return (isoFlags() & 0x1); }               // standalone iso WP
    bool passes_looseTkiso() const { return (isoFlags() & 0x2); }        // loose Tk iso WP

    // fb and timing: not saved in the current emulator
    ap_uint<6> fb() const { return ((clusterData >> 44) & 0x3F); }
    ap_uint<5> timing() const { return ((clusterData >> 49) & 0x1F); }

    // shower shape shape flag: two bits, least significant bit is the standalone WP, second bit is the looseTk WP
    // e.g. 0b01 : standalone shower shape flag passed, loose Tk shower shape flag did not pass
    ap_uint<2> shapeFlags() const { return ((clusterData >> 51) & 0x3); }

    bool passes_ss() const { return (shapeFlags() & 0x1); }         // standalone shower shape WP
    bool passes_looseTkss() const { return (shapeFlags() & 0x2); }  // loose Tk shower shape WP

    // brems: not saved in the current emulator
    ap_uint<2> brems() const { return ((clusterData >> 53) & 0x3); }

    const int unusedBitsStart() const { return n_bits_unused_start; }

    // Other checks
    bool passNullBitsCheck(void) const { return ((data() >> unusedBitsStart()) == 0x0); }

  };

  // Collection typedef
  typedef std::vector<l1tp2::GCTBarrelDigiClusterToCorrLayer1> GCTBarrelDigiClusterToCorrLayer1Collection;
  typedef std::vector<l1tp2::GCTBarrelDigiClusterToCorrLayer1Collection> GCTBarrelDigiClusterToCorrLayer1CollectionFullDetector;

}  // namespace l1tp2

#endif
