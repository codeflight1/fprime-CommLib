#ifndef CCSDSPACKET_HPP_
#define CCSDSPACKET_HPP_

#include <SpaceWire/CCSDSPacketBaseSerializableAc.hpp>
#include <SpaceWire/SerDesStatusEnumAc.hpp>
#include <Fw/Buffer/Buffer.hpp>

namespace SpaceWire {
  class CCSDSPacket : public CCSDSPacketBase {
    public:
      SerDesStatus encode(Fw::Buffer& buffer);
      static SerDesStatus decode(CCSDSPacket& packet, Fw::Buffer& buffer);

      U32 getLength() {return 7 + this->m_Data.getSize();};

      bool operator==(const CCSDSPacket& src) const;
  };
} /* SpaceWire */

#endif // CCSDSPACKET_HPP_
