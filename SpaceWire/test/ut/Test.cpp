#include <gtest/gtest.h>
#include <SpaceWire/RMAPPacket.cpp>
#include <SpaceWire/SpaceWireAddrSerializableAc.cpp>
#include <SpaceWire/SpaceWireAddrTypeEnumAc.cpp>
#include <Fw/Buffer/Buffer.cpp>

#include <cstdlib>

TEST(RMAPPacketTest, AddrTest) {
  U8* dat = (U8*) malloc(2);

  dat[0] = 0xFF;

  SpaceWire::SpaceWireAddr addr = SpaceWire::SpaceWireAddr(0x54, dat, 0, SpaceWire::SpaceWireAddrType::LOGICAL, 0);
  SpaceWire::SpaceWireAddr addr2 = SpaceWire::SpaceWireAddr(0x76, dat, 0, SpaceWire::SpaceWireAddrType::LOGICAL, 0);

  free(dat);

  U8 data[0x10] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

  Fw::Buffer buff = Fw::Buffer(data, 0x10, 0x01);

  SpaceWire::RMAPPacket packet = SpaceWire::RMAPPacket();
  packet.set(addr, addr2, SpaceWire::RMAPPacketType::WriteReply, false, false, true, SpaceWire::RMAPStatus::Success, (U8) 0x57, (U16) 0x05, (U8) 0x00, (U32) 0x2000, (U32) 0x10, buff);

  std::cout << packet.encode() << '\n';
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
