module SpaceWire {

  @ RMAP packet type enum
  enum RMAPPacketType: U8 {
    WriteCommand = 0x60, # 0b011xxxaa
    WriteReply   = 0x28, # 0b001x1xaa
    ReadCommand  = 0x48, # 0b01001xaa
    ReadReply    = 0x08, # 0b00001xaa
    RMWCommand   = 0x5C, # 0b010111aa
    RMWReply     = 0x1C  # 0b000111aa
  }

  @ RMAP response status enum
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

  @ RMAP packet base class
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

  @ RMAP packet class
  type RMAPPacket
}
