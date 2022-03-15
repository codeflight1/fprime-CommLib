TEST(CCSDSDecodeTest, BasicTest) {
  SpaceWire::CCSDSPacket packet = SpaceWire::CCSDSPacket();

  U8 dat[1] = {0x00};

  U8 data[10] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};

  packet.set(SpaceWire::SpaceWireAddr(0x22, dat, 0, SpaceWire::SpaceWireAddrType::LOGICAL), 0x10, true, false, 0x16, SpaceWire::CCSDSSeqType::UnSegmented, 0x10, Fw::Buffer(data, 1, 1));

  Fw::Buffer outbuf = Fw::Buffer((U8*) malloc(packet.getLength()), packet.getLength(), 0x01);

  SpaceWire::CCSDSEncodeStatus status = packet.encode(outbuf);

  SpaceWire::CCSDSPacket decoded;
  SpaceWire::CCSDSDecodeStatus status2 = packet.decode(decoded, outbuf);

  ASSERT_EQ(status2, SpaceWire::CCSDSDecodeStatus::SUCCESS);
  ASSERT_EQ(packet, decoded);
}

TEST(CCSDSDecodeTest, RSRVD_NZTest) {
  SpaceWire::CCSDSPacket packet = SpaceWire::CCSDSPacket();

  U8 data[8] = {0x22, 0x02, 0x10, 0x10, 0x00, 0x16, 0xc0, 0x10};

  Fw::Buffer buf = Fw::Buffer(data, 8, 0x01);

  SpaceWire::CCSDSPacket decoded;
  SpaceWire::CCSDSDecodeStatus status2 = packet.decode(decoded, buf);

  ASSERT_EQ(status2, SpaceWire::CCSDSDecodeStatus::RSRVD_NZ);
}

TEST(CCSDSDecodeTest, WRONG_PROTOCOLTest) {
  SpaceWire::CCSDSPacket packet = SpaceWire::CCSDSPacket();

  U8 data[8] = {0x22, 0x03, 0x00, 0x10, 0x00, 0x16, 0xc0, 0x10};

  Fw::Buffer buf = Fw::Buffer(data, 10, 0x01);

  SpaceWire::CCSDSPacket decoded;
  SpaceWire::CCSDSDecodeStatus status2 = packet.decode(decoded, buf);

  ASSERT_EQ(status2, SpaceWire::CCSDSDecodeStatus::WRONG_PROTOCOL);
}
