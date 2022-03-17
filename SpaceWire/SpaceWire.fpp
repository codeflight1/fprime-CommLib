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

  port registerAPID(APID: U16, used: bool)
  port registerProtocol(protocol: U8, used: bool)
  port registerTransID(TransID: U16, used: bool)

  constant numCCSDSPorts = 32;
  constant numRMAPPorts = 32;
  constant numRawPorts = 32;

  @ SpaceWire controller component
  active component SpaceWireController {

    @ Input data port from SpaceWire driver
    async input port dataIn: Fw.BufferSend

    @ Output data port to SpaceWire driver
    output port dataOut: Fw.BufferSend

    @ Input data port from RMAP components
    async input port RMAPin: [numRMAPPorts] RMAPPacketPort

    @ Output data port to RMAP components
    output port RMAPout: [numRMAPPorts] RMAPPacketPort

    @ Registers RMAP transaction ID to port number
    async input port registerTransID: [numRMAPPorts] registerTransID

    match RMAPin with RMAPout
    match RMAPin with registerTransID

    @ Input data port from CCSDS components
    async input port CCSDSin: [numCCSDSPorts] CCSDSPacketPort

    @ Output data port to CCSDS components
    output port CCSDSout: [numCCSDSPorts] CCSDSPacketPort

    @ Registers APID to port number
    async input port registerAPID: [numCCSDSPorts] registerAPID

    match CCSDSin with CCSDSout
    match CCSDSin with registerAPID

    @ Raw input data port
    async input port rawIn: [numRawPorts] Fw.BufferSend

    @ Raw output data port
    output port rawOut: [numRawPorts] Fw.BufferSend

    @ Registers custom protocol ID to port number
    async input port registerProtocol: [numRawPorts] registerProtocol

    match rawIn with rawOut
    match rawIn with registerProtocol
  }
}
