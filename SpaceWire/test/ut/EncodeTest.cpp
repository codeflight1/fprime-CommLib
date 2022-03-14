TEST(EncodeTest, ADDR_TYPE_MISMATCHTest) {
  U8 dat[1] = {0xFF};

  SpaceWire::RMAPPacket packet = SpaceWire::RMAPPacket();
  packet.set(SpaceWire::SpaceWireAddr(0x54, dat, 0, SpaceWire::SpaceWireAddrType::LOGICAL),
             SpaceWire::SpaceWireAddr(0x76, dat, 0, SpaceWire::SpaceWireAddrType::PHYSICAL),
             SpaceWire::RMAPPacketType::WriteReply, false, false, true, SpaceWire::RMAPStatus::Success, (U8) 0x57, (U16) 0x05, (U8) 0x00, (U32) 0x2000, (U32) 0, Fw::Buffer(nullptr, 0, 0));

  Fw::Buffer outbuf = Fw::Buffer((U8*) malloc(packet.getLength()), packet.getLength(), 0x02);

  SpaceWire::RMAPEncodeStatus status = packet.encode(outbuf);

  ASSERT_EQ(status, SpaceWire::RMAPEncodeStatus::ADDR_TYPE_MISMATCH);
}

TEST(EncodeTest, DATA_LEN_OVERRUNTest) {
  U8 dat[1] = {0xFF};

  SpaceWire::RMAPPacket packet = SpaceWire::RMAPPacket();
  packet.set(SpaceWire::SpaceWireAddr(0x54, dat, 0, SpaceWire::SpaceWireAddrType::LOGICAL),
             SpaceWire::SpaceWireAddr(0x76, dat, 0, SpaceWire::SpaceWireAddrType::LOGICAL),
             SpaceWire::RMAPPacketType::WriteReply, false, false, true, SpaceWire::RMAPStatus::Success, (U8) 0x57, (U16) 0x05, (U8) 0x00, (U32) 0x2000, (U32) 0xFFFFFFFF, Fw::Buffer(nullptr, 0, 0));

  Fw::Buffer outbuf = Fw::Buffer((U8*) malloc(packet.getLength()), packet.getLength(), 0x02);

  SpaceWire::RMAPEncodeStatus status = packet.encode(outbuf);

  ASSERT_EQ(status, SpaceWire::RMAPEncodeStatus::DATA_LEN_OVERRUN);
}

TEST(EncodeTest, DATA_LEN_MISMATCHTest) {
  U8 dat[1] = {0xFF};

  SpaceWire::RMAPPacket packet = SpaceWire::RMAPPacket();
  packet.set(SpaceWire::SpaceWireAddr(0x54, dat, 0, SpaceWire::SpaceWireAddrType::LOGICAL),
             SpaceWire::SpaceWireAddr(0x76, dat, 0, SpaceWire::SpaceWireAddrType::LOGICAL),
             SpaceWire::RMAPPacketType::WriteCommand, false, false, true, SpaceWire::RMAPStatus::Success, (U8) 0x57, (U16) 0x05, (U8) 0x00, (U32) 0x2000, (U32) 0xFF, Fw::Buffer(nullptr, 0, 0));

  Fw::Buffer outbuf = Fw::Buffer((U8*) malloc(packet.getLength()), packet.getLength(), 0x02);

  SpaceWire::RMAPEncodeStatus status = packet.encode(outbuf);

  ASSERT_EQ(status, SpaceWire::RMAPEncodeStatus::DATA_LEN_MISMATCH);
}

TEST(EncodeTest, BUFFER_LEN_MISMATCHTest) {
  U8 dat[1] = {0xFF};

  SpaceWire::RMAPPacket packet = SpaceWire::RMAPPacket();
  packet.set(SpaceWire::SpaceWireAddr(0x54, dat, 0, SpaceWire::SpaceWireAddrType::LOGICAL),
             SpaceWire::SpaceWireAddr(0x76, dat, 0, SpaceWire::SpaceWireAddrType::LOGICAL),
             SpaceWire::RMAPPacketType::WriteCommand, false, false, true, SpaceWire::RMAPStatus::Success, (U8) 0x57, (U16) 0x05, (U8) 0x00, (U32) 0x2000, (U32) 0, Fw::Buffer(nullptr, 0, 0));

  Fw::Buffer outbuf = Fw::Buffer((U8*) malloc(packet.getLength()), 10, 0x02);

  SpaceWire::RMAPEncodeStatus status = packet.encode(outbuf);

  ASSERT_EQ(status, SpaceWire::RMAPEncodeStatus::BUFFER_LEN_MISMATCH);
}

/* // Not yet implemented
TEST(EncodeTest, RMW_DATA_MISMATCHTest) {
  U8 dat[1] = {0xFF};

  SpaceWire::RMAPPacket packet = SpaceWire::RMAPPacket();
  packet.set(SpaceWire::SpaceWireAddr(0x54, dat, 0, SpaceWire::SpaceWireAddrType::LOGICAL),
             SpaceWire::SpaceWireAddr(0x76, dat, 0, SpaceWire::SpaceWireAddrType::LOGICAL),
             SpaceWire::RMAPPacketType::WriteCommand, false, false, true, SpaceWire::RMAPStatus::Success, (U8) 0x57, (U16) 0x05, (U8) 0x00, (U32) 0x2000, (U32) 0xFF, Fw::Buffer(nullptr, 0, 0));

  Fw::Buffer outbuf = Fw::Buffer((U8*) malloc(packet.getLength()), packet.getLength(), 0x02);

  SpaceWire::RMAPEncodeStatus status = packet.encode(outbuf);

  ASSERT_EQ(status, SpaceWire::RMAPEncodeStatus::RMW_DATA_MISMATCH);
}
*/
