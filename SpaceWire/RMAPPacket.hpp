/*
 * RMAPPacket.hpp
 *
 *  Author:     codeflight
 *
 */
#ifndef RMAPPACKET_HPP_
#define RMAPPACKET_HPP_

#include <SpaceWire/SpaceWireAddrSerializableAc.hpp>
#include <SpaceWire/RMAPPacketBaseSerializableAc.hpp>
#include <Fw/Buffer/Buffer.hpp>

namespace SpaceWire {
  enum RMAPEncodeStatus {
    SUCCESS,
    ADDR_TYPE_MISMATCH,
    DATA_LEN_OVERRUN,
    DATA_LEN_MISMATCH,
    BUFFER_LEN_MISMATCH,
    RMW_DATA_MISMATCH
  };

  enum RMAPDecodeStatus {

  };

  class RMAPPacket : public RMAPPacketBase {
    public:
      RMAPEncodeStatus encode(Fw::Buffer& buffer);
      RMAPDecodeStatus decode(const Fw::Buffer& buffer);

      U32 getLength();

    private:
      static U8 addrPad(SpaceWireAddr& addr);

      static U8 CRC(U8* buf, U32 len);
  };
} // end namespace SpaceWire

#endif /* RMAPPACKET_HPP_ */
