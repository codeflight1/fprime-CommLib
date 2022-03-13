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
  class RMAPPacket : public RMAPPacketBase {
    public:
      U8* encode();
      void decode(const Fw::Buffer& buffer);

      U8 addrPad(SpaceWireAddr& addr) const;

    private:
      static U8 CRC(U8* buf, U32 len);
  };
} // end namespace SpaceWire

#endif /* RMAPPACKET_HPP_ */
