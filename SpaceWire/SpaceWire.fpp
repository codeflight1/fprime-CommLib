module SpaceWire {

  enum SpaceWireProtocolID: U8 {
    Extended = 0,
    RMAP = 1,
    CCSDS = 2,
    RDDP = 238,
    STUP = 239
  }

  enum SpaceWireAddrType {
    PHYSICAL,
    LOGICAL
  }

  struct SpaceWireAddr {
    LogicalAddr: U8,
    PhysicalAddr: [8] U8,
    Type: SpaceWireAddrType,
    Length: U8
  }

  struct SpaceWirePacket {
    LogicalAddr: U8,
    Protocol: SpaceWireProtocolID,
    Data: Fw.Buffer
  }

  struct CCSDSPacket {
    Header: U16,
    ControlSequence: U16,
    Length: U16,
    Data: Fw.Buffer
  }
}
