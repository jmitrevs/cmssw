#ifndef DataFormats_L1TCalorimeterPhase2_CaloPFDigiClusterToCorrLayer1_h
#define DataFormats_L1TCalorimeterPhase2_CaloPFDigiClusterToCorrLayer1_h

#include <ap_int.h>
#include <vector>

namespace l1tp2 {

  class CaloPFDigiClusterToCorrLayer1 {
  private:
    // Data (note: positional information is entirely encoded in the location in the output array)
    unsigned long long int clusterData;

    // Constants
    static constexpr float LSB_PT = 0.5;                 // 0.5 GeV

    // start of the unused bits 
    static constexpr int n_bits_unused_start = 31; 

  public:
    CaloPFDigiClusterToCorrLayer1() { clusterData = 0; }

    CaloPFDigiClusterToCorrLayer1(ap_uint<64> data) { clusterData = data; }

    // Note types of the constructor
    CaloPFDigiClusterToCorrLayer1(
                               ap_uint<12> pt,
                               int etaCr,
                               int phiCr,
                               ap_uint<4> hoe) {
      // To use .range() we need an ap class member
      ap_uint<64> temp_data; 
      
      ap_uint<7> etaCrDigitized = abs(etaCr);
      ap_int<7> phiCrDigitized = phiCr;

      temp_data.range(11, 0) = pt.range();
      temp_data.range(18, 12) = etaCrDigitized.range();
      temp_data.range(25, 19) = phiCrDigitized.range();

      clusterData = temp_data;
    }

    // Getters
    ap_uint<64> data() const { return clusterData; }

    // Other getters
    float ptLSB() const { return LSB_PT; }
    ap_uint<12> pt() const { return data().range(11, 0); }
    float ptFloat() const { return pt() * ptLSB(); }

    // crystal eta (unsigned 7 bits)
    int eta() const { return (ap_uint<7>) data().range(18, 12); }

    // crystal phi (signed 7 bits)
    int phi() const { return (ap_int<7>) data().range(25, 19); }

    // HoE value 
    ap_uint<4> hoe() const { return data().range(30, 26); }   

    // Check that unused bits are zero
    const int unusedBitsStart() const { return n_bits_unused_start; }
    bool passNullBitsCheck(void) const { return ((data() >> unusedBitsStart()) == 0); }

  };

  // Collection typedef
  typedef std::vector<l1tp2::CaloPFDigiClusterToCorrLayer1> CaloPFDigiClusterToCorrLayer1Collection;
  typedef std::vector<l1tp2::CaloPFDigiClusterToCorrLayer1Collection> CaloPFDigiClusterToCorrLayer1CollectionFullDetector;

}  // namespace l1tp2

#endif