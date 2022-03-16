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

  type SpaceWireAddr

  port RMAPPacketPort( p: RMAPPacket )
  port CCSDSPacketPort( p: CCSDSPacket )

  active component SpaceWireDecoder {
    async input port dataIn: Fw.BufferSend

    output port dataOut: Fw.BufferSend

    async input port RMAPin: RMAPPacketPort

    async input port CCSDSin: [32] CCSDSPacketPort

    async input port rawIn: [32] Fw.BufferSend

    output port RMAPout: RMAPPacketPort

    output port CCSDSout: [32] CCSDSPacketPort

    output port rawOut: [32] Fw.BufferSend

    async command registerAPID(APID: U16, portNum: U8) opcode 0x00

    async command registerProtocol(protocol: U8, portNum: U8) opcode 0x01

    @ Command receive port
    command recv port cmdIn

    @ Command registration port
    command reg port cmdRegOut

    @ Command response port
    command resp port cmdResponseOut
  }
}
