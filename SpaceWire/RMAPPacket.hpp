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
#include <SpaceWire/SerDesStatusEnumAc.hpp>

namespace SpaceWire {
  class RMAPPacket : public RMAPPacketBase {
    public:
      SerDesStatus encode(Fw::Buffer& buffer);
      static SerDesStatus decode(RMAPPacket& packet, const Fw::Buffer& buffer);

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
