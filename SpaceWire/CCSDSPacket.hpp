#ifndef CCSDSPACKET_HPP_
#define CCSDSPACKET_HPP_

#include <SpaceWire/CCSDSPacketBaseSerializableAc.hpp>
#include <Fw/Buffer/Buffer.hpp>

namespace SpaceWire {
  enum class CCSDSEncodeStatus {
    SUCCESS,
    SIZE_MISMATCH,
    APID_OVERFLOW,
    SEQCNT_OVERFLOW,
    DATA_OVERFLOW
  };

  enum class CCSDSDecodeStatus {
    SUCCESS,
    WRONG_PROTOCOL,
    RSRVD_NZ
  };

  class CCSDSPacket : public CCSDSPacketBase {
    public:
      CCSDSEncodeStatus encode(Fw::Buffer& buffer);
      static CCSDSDecodeStatus decode(CCSDSPacket& packet, Fw::Buffer& buffer);

      U32 getLength() {return 7 + this->m_Data.getSize();};

      bool operator==(const CCSDSPacket& src) const;
  };
} /* SpaceWire */

#endif // CCSDSPACKET_HPP_
