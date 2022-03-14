/*
 * RMAPPacket.hpp
 *
 *  Author:     codeflight
 *
 */
#ifndef RMAPPACKET_HPP_
#define RMAPPACKET_HPP_

#include <SpaceWire/SpaceWireAddr.hpp>
#include <SpaceWire/RMAPPacketBaseSerializableAc.hpp>
#include <Fw/Buffer/Buffer.hpp>

namespace SpaceWire {
  enum class RMAPEncodeStatus {
    SUCCESS,
    ADDR_TYPE_MISMATCH,
    DATA_LEN_OVERRUN,
    DATA_LEN_MISMATCH,
    BUFFER_LEN_MISMATCH,
    RMW_DATA_MISMATCH
  };

  enum class RMAPDecodeStatus {
    SUCCESS,
    WRONG_PROTOCOL,
    UNK_PACKET_TYPE,
    BAD_HEADER_CRC,
    BAD_DATA_CRC
  };

  class RMAPPacket : public RMAPPacketBase {
    public:
      RMAPEncodeStatus encode(Fw::Buffer& buffer);
      static RMAPDecodeStatus decode(RMAPPacket& packet, const Fw::Buffer& buffer);

      U32 getLength();

      bool getVerify() const;
      bool getAck() const;
      bool getIncrement() const;

      bool operator==(const RMAPPacket& src) const;

#if FW_SERIALIZABLE_TO_STRING || BUILD_UT
      void toString(Fw::StringBase& text) const;
#endif

    private:
      static U8 addrPad(SpaceWireAddr& addr);

      static U8 CRC(U8* buf, U32 len);
  };
} // end namespace SpaceWire

#endif /* RMAPPACKET_HPP_ */
