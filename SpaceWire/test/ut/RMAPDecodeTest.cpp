TEST(RMAPDecodeTest, BasicTest) {
  U8 dat[1] = {0x02};

  SpaceWire::RMAPPacket packet = SpaceWire::RMAPPacket();
  packet.set(SpaceWire::SpaceWireAddr(0x54, dat, 1, SpaceWire::SpaceWireAddrType::LOGICAL),
             SpaceWire::SpaceWireAddr(0x76, dat, 1, SpaceWire::SpaceWireAddrType::LOGICAL),
             SpaceWire::RMAPPacketType::WriteCommand, false, false, false, SpaceWire::RMAPStatus::Success, (U8) 0x0, (U16) 0x05, (U8) 0x00, (U32) 0x01, (U32) 0x1, Fw::Buffer(dat, 1, 0));

  Fw::Buffer outbuf = Fw::Buffer((U8*) malloc(packet.getLength()), packet.getLength(), 0x02);

  SpaceWire::RMAPEncodeStatus status = packet.encode(outbuf);

  SpaceWire::RMAPPacket decoded;

  SpaceWire::RMAPDecodeStatus status2 = packet.decode(decoded, outbuf);

  ASSERT_EQ(status2, SpaceWire::RMAPDecodeStatus::SUCCESS);
  ASSERT_EQ(packet, decoded);
}

TEST(RMAPDecodeTest, WRONG_PROTOCOLTest) {
  U8 in[8] = {0x54, 0x02, 0x28, 0x00, 0x76, 0x00, 0x05, 0xc4};

  SpaceWire::RMAPPacket packet;

  SpaceWire::RMAPDecodeStatus status = SpaceWire::RMAPPacket::decode(packet, Fw::Buffer(in, 8, 0x02));

  ASSERT_EQ(status, SpaceWire::RMAPDecodeStatus::WRONG_PROTOCOL);
}

TEST(RMAPDecodeTest, UNK_PACKET_TYPETest) {
  U8 in[8] = {0x54, 0x01, 0xFF, 0x00, 0x76, 0x00, 0x05, 0xc4};

  SpaceWire::RMAPPacket packet;

  SpaceWire::RMAPDecodeStatus status = SpaceWire::RMAPPacket::decode(packet, Fw::Buffer(in, 8, 0x02));

  ASSERT_EQ(status, SpaceWire::RMAPDecodeStatus::UNK_PACKET_TYPE);
}

TEST(RMAPDecodeTest, BAD_HEADER_CRCTest) {
  U8 in[8] = {0x54, 0x01, 0x28, 0x00, 0x76, 0x00, 0x05, 0xFF};

  SpaceWire::RMAPPacket packet;

  SpaceWire::RMAPDecodeStatus status = SpaceWire::RMAPPacket::decode(packet, Fw::Buffer(in, 8, 0x02));

  ASSERT_EQ(status, SpaceWire::RMAPDecodeStatus::BAD_HEADER_CRC);
}

TEST(RMAPDecodeTest, BAD_DATA_CRCTest) {
  U8 in[18] = {0x54, 0x01, 0x70, 0x00, 0x76, 0x00, 0x05, 0x00, 0, 0, 0, 1, 0, 0, 1, 0xF0, 0x02, 0x10};

  SpaceWire::RMAPPacket packet;

  SpaceWire::RMAPDecodeStatus status = SpaceWire::RMAPPacket::decode(packet, Fw::Buffer(in, 18, 0x02));

  ASSERT_EQ(status, SpaceWire::RMAPDecodeStatus::BAD_DATA_CRC);
}
