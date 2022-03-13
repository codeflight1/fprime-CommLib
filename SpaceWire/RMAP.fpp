module SpaceWire {

  enum RMAPPacketType: U8 {
    WriteCommand = 0x60, # 0b011xxx00
    WriteReply   = 0x28, # 0b001x1x00
    ReadCommand  = 0x48, # 0b01001x00
    ReadReply    = 0x08, # 0b00001x00
    RMWCommand   = 0x5C, # 0b01011100
    RMWReply     = 0x1C  # 0b00011100
  }

  # 0 C/R R/W Ver Ack Inc SPL

  enum RMAPStatus: U8 {
    Success                = 0,
    GeneralError           = 1,
    CmdNotSupported        = 2,
    InvalidDestKey         = 3,
    InvalidDataCRC         = 4,
    EarlyEOP               = 5,
    LateEOP                = 6,
    EarlyEEP               = 7,
    LateEEP                = 8,
    VerifyBufOverrun       = 9,
    AuthFailure            = 10,
    RMWDataLenError        = 11,
    InvalidDestLogicalAddr = 12
  }

  struct RMAPPacketBase {
    DestAddr: SpaceWireAddr,
    SourceAddr: SpaceWireAddr,
    Type: RMAPPacketType,
    Verify: bool,
    Ack: bool,
    Increment: bool,
    Status: RMAPStatus,
    DestKey: U8
    TransID: U16,
    ExtDataAddr: U8,
    DataAddr: U32,
    DataLen: U32, # serialized as U24
    Data: Fw.Buffer
  }
}
