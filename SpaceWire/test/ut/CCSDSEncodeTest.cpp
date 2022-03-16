TEST(CCSDSEncodeTest, SIZE_MISMATCHTest) {
  SpaceWire::CCSDSPacket packet = SpaceWire::CCSDSPacket();

  U8 dat[1] = {0x00};

  U8 data[10] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};

  packet.set(SpaceWire::SpaceWireAddr(0x22, dat, 0, SpaceWire::SpaceWireAddrType::LOGICAL), 0x10, true, false, 0x16, SpaceWire::CCSDSSeqType::UnSegmented, 0x10, Fw::Buffer(data, 1, 1));

  Fw::Buffer outbuf = Fw::Buffer((U8*) malloc(packet.getLength()), packet.getLength()+10, 0x01);

  SpaceWire::SerDesStatus status = packet.encode(outbuf);

  ASSERT_EQ(status, SpaceWire::SerDesStatus::SIZE_MISMATCH);
}

TEST(CCSDSEncodeTest, APID_OVERFLOWTest) {
  SpaceWire::CCSDSPacket packet = SpaceWire::CCSDSPacket();

  U8 dat[1] = {0x00};

  U8 data[10] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};

  packet.set(SpaceWire::SpaceWireAddr(0x22, dat, 0, SpaceWire::SpaceWireAddrType::LOGICAL), 0x10, true, false, 0xFFFF, SpaceWire::CCSDSSeqType::UnSegmented, 0x10, Fw::Buffer(data, 1, 1));

  Fw::Buffer outbuf = Fw::Buffer((U8*) malloc(packet.getLength()), packet.getLength(), 0x01);

  SpaceWire::SerDesStatus status = packet.encode(outbuf);

  ASSERT_EQ(status, SpaceWire::SerDesStatus::APID_OVERFLOW);
}

TEST(CCSDSEncodeTest, SEQCNT_OVERFLOWTest) {
  SpaceWire::CCSDSPacket packet = SpaceWire::CCSDSPacket();

  U8 dat[1] = {0x00};

  U8 data[10] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};

  packet.set(SpaceWire::SpaceWireAddr(0x22, dat, 0, SpaceWire::SpaceWireAddrType::LOGICAL), 0x10, true, false, 0x16, SpaceWire::CCSDSSeqType::UnSegmented, 0xFFFF, Fw::Buffer(data, 1, 1));

  Fw::Buffer outbuf = Fw::Buffer((U8*) malloc(packet.getLength()), packet.getLength(), 0x01);

  SpaceWire::SerDesStatus status = packet.encode(outbuf);

  ASSERT_EQ(status, SpaceWire::SerDesStatus::SEQCNT_OVERFLOW);
}

TEST(CCSDSEncodeTest, DATA_OVERFLOWTest) {
  SpaceWire::CCSDSPacket packet = SpaceWire::CCSDSPacket();

  U8 dat[1] = {0x00};

  U8 data[10] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};

  packet.set(SpaceWire::SpaceWireAddr(0x22, dat, 0, SpaceWire::SpaceWireAddrType::LOGICAL), 0x10, true, false, 0x16, SpaceWire::CCSDSSeqType::UnSegmented, 0x10, Fw::Buffer(data, 0x1FFFF, 1));

  Fw::Buffer outbuf = Fw::Buffer((U8*) malloc(packet.getLength()), packet.getLength(), 0x01);

  SpaceWire::SerDesStatus status = packet.encode(outbuf);

  ASSERT_EQ(status, SpaceWire::SerDesStatus::DATA_OVERFLOW);
}
