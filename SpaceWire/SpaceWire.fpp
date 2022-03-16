module SpaceWire {

  @ SpaceWire protocol ID
  enum SpaceWireProtocolID: U8 {
    Extended = 0,
    RMAP = 1,
    CCSDS = 2,
    RDDP = 238,
    STUP = 239
  }

  @ SpaceWire address type
  enum SpaceWireAddrType {
    PHYSICAL,
    LOGICAL
  }

  @ SpaceWire packet serialize/deserialize status
  enum SerDesStatus {
    SUCCESS,
    ADDR_TYPE_MISMATCH,
    DATA_LEN_OVERRUN,
    DATA_LEN_MISMATCH,
    BUFFER_LEN_MISMATCH,
    RMW_DATA_MISMATCH,
    WRONG_PROTOCOL,
    BAD_HEADER_CRC,
    BAD_DATA_CRC,
    RSRVD_NZ,
    SIZE_MISMATCH,
    APID_OVERFLOW,
    SEQCNT_OVERFLOW,
    DATA_OVERFLOW
  }

  @ SpaceWire address type
  type SpaceWireAddr

  port RMAPPacketPort( p: RMAPPacket )
  port CCSDSPacketPort( p: CCSDSPacket )

  @ SpaceWire controller component
  active component SpaceWireController {

    @ Input data port from SpaceWire driver
    async input port dataIn: Fw.BufferSend

    @ Output data port to SpaceWire driver
    output port dataOut: Fw.BufferSend

    @ Input data port from RMAP controller
    async input port RMAPin: RMAPPacketPort

    @ Input data port from CCSDS components
    async input port CCSDSin: [32] CCSDSPacketPort

    @ Raw input data port
    async input port rawIn: [32] Fw.BufferSend

    @ Output data port to RMAP controller
    output port RMAPout: RMAPPacketPort

    @ Output data port to CCSDS components
    output port CCSDSout: [32] CCSDSPacketPort

    @ Raw output data port
    output port rawOut: [32] Fw.BufferSend

    @ Registers APID to port number
    async command registerAPID(APID: U16, portNum: U8) opcode 0x00

    @ Registers custom protocol ID to port number
    async command registerProtocol(protocol: U8, portNum: U8) opcode 0x01

    @ Command receive port
    command recv port cmdIn

    @ Command registration port
    command reg port cmdRegOut

    @ Command response port
    command resp port cmdResponseOut
  }
}
