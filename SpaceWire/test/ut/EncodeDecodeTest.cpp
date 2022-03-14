TEST(EncodeDecodeTest, BasicTest) {
  U8 dat[1] = {0x02};

  SpaceWire::RMAPPacket packet = SpaceWire::RMAPPacket();
  packet.set(SpaceWire::SpaceWireAddr(0x54, dat, 1, SpaceWire::SpaceWireAddrType::PHYSICAL),
             SpaceWire::SpaceWireAddr(0x76, dat, 1, SpaceWire::SpaceWireAddrType::PHYSICAL),
             SpaceWire::RMAPPacketType::WriteReply, false, false, false, SpaceWire::RMAPStatus::Success, (U8) 0x0, (U16) 0x05, (U8) 0x00, (U32) 0x0, (U32) 0x0, Fw::Buffer(nullptr, 0, 0));

  Fw::Buffer outbuf = Fw::Buffer((U8*) malloc(packet.getLength()), packet.getLength(), 0x02);

  SpaceWire::RMAPEncodeStatus status = packet.encode(outbuf);

  SpaceWire::RMAPPacket decoded;

  SpaceWire::RMAPDecodeStatus status2 = packet.decode(decoded, outbuf);

  ASSERT_EQ(packet, decoded);
}
