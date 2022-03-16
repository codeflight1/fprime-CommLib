// ======================================================================
// \title  SpaceWireDecoder.cpp
// \author codeflight
// \brief  cpp file for SpaceWireDecoder component implementation class
//
// ======================================================================


#include <SpaceWire/SpaceWireDecoder.hpp>
#include <SpaceWire/SpaceWireProtocolIDEnumAc.cpp>
#include <SpaceWire/RMAPPacket.cpp>
#include <SpaceWire/CCSDSPacket.cpp>
#include "Fw/Types/BasicTypes.hpp"

#include <cstdlib> // malloc

namespace SpaceWire {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  SpaceWireDecoder ::
    SpaceWireDecoder(
        const char *const compName
    ) : SpaceWireDecoderComponentBase(compName)
  {

  }

  void SpaceWireDecoder ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    SpaceWireDecoderComponentBase::init(queueDepth, instance);

    for (NATIVE_INT_TYPE i = 0; i < 32; i++) {
      this->APID[i] = 0;
      this->protocol[i] = 0;
    }
  }

  SpaceWireDecoder ::
    ~SpaceWireDecoder()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void SpaceWireDecoder ::
    CCSDSin_handler(
        const NATIVE_INT_TYPE portNum,
        const SpaceWire::CCSDSPacket &p
    )
  {
    CCSDSPacket packet = p;
    Fw::Buffer buf = Fw::Buffer((U8*) malloc(packet.getLength()), packet.getLength(), 0x01);
    SerDesStatus status = packet.encode(buf);

    if (status == SerDesStatus::SUCCESS) {
      this->dataOut_out(0, buf);
    }
    // TODO: error handling
  }

  void SpaceWireDecoder ::
    RMAPin_handler(
        const NATIVE_INT_TYPE portNum,
        const SpaceWire::RMAPPacket &p
    )
  {
    RMAPPacket packet = p;
    Fw::Buffer buf = Fw::Buffer((U8*) malloc(packet.getLength()), packet.getLength(), 0x01);
    SerDesStatus status = packet.encode(buf);

    if (status == SerDesStatus::SUCCESS) {
      this->dataOut_out(0, buf);
    }
    // TODO: error handling
  }

  void SpaceWireDecoder ::
    dataIn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    U32 loc = 0;

    while (fwBuffer.getData()[loc] < 32) { // skip over path address
      loc++;
    }

    switch (static_cast<SpaceWireProtocolID::t>(fwBuffer.getData()[loc + 1])) {
      case SpaceWireProtocolID::RMAP:
        {
          RMAPPacket packet;
          SerDesStatus status = RMAPPacket::decode(packet, fwBuffer);

          if (status == SerDesStatus::SUCCESS) {
            this->RMAPout_out(0, packet);
          }
        }
        break;

      case SpaceWireProtocolID::CCSDS:
        {
          CCSDSPacket packet;
          SerDesStatus status = CCSDSPacket::decode(packet, fwBuffer);

          if (status != SerDesStatus::SUCCESS) {
            break; // TODO
          }

          for (NATIVE_INT_TYPE i = 0; i < 32; i++) {
            if (this->APID[i] == packet.getAPID()) {
              this->CCSDSout_out(i, packet);
            }
          }
        }
        break;

      default:
        {
          for (NATIVE_INT_TYPE i = 0; i < 32; i++) {
            if (this->protocol[i] == fwBuffer.getData()[loc + 1]) {
              this->rawOut_out(i, fwBuffer);
            }
          }
        }
        break;
    }
  }

  void SpaceWireDecoder ::
    rawIn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    this->dataOut_out(0, fwBuffer);
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void SpaceWireDecoder ::
    registerAPID_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U16 APID,
        U8 portNum
    )
  {
    FW_ASSERT(portNum < 32);

    this->APID[portNum] = APID;

    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void SpaceWireDecoder ::
    registerProtocol_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U8 protocol,
        U8 portNum
    )
  {
    FW_ASSERT(portNum < 32);

    this->protocol[portNum] = protocol;

    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

} // end namespace SpaceWire
