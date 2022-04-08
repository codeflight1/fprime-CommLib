// ======================================================================
// \title  SpaceWireController.cpp
// \author codeflight
// \brief  cpp file for SpaceWireController component implementation class
//
// ======================================================================


#include "SpaceWireController.hpp"
#include <SpaceWire/SpaceWireProtocolIDEnumAc.cpp>
#include "RMAPPacket.cpp"
#include "CCSDSPacket.cpp"
#include "Fw/Types/BasicTypes.hpp"

#include <cstdlib> // malloc

namespace SpaceWire {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  SpaceWireController ::
    SpaceWireController(
        const char *const compName
    ) : SpaceWireControllerComponentBase(compName)
  {

  }

  void SpaceWireController ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    SpaceWireControllerComponentBase::init(queueDepth, instance);
  }

  SpaceWireController ::
    ~SpaceWireController()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void SpaceWireController ::
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

  void SpaceWireController ::
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

  void SpaceWireController ::
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

          if (status != SerDesStatus::SUCCESS) {
            break; // TODO
          }

          if (this->TransIDs[packet.getTransID()].used == true) {
            this->RMAPout_out(this->TransIDs[packet.getTransID()].portNum, packet);
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

          for (NATIVE_INT_TYPE i = 0; i < numCCSDSPorts; i++) {
            if (this->APIDs[i].APID == packet.getAPID()) {
              this->CCSDSout_out(i, packet);
              break;
            }
          }
        }
        break;

      default:
        {
          for (NATIVE_INT_TYPE i = 0; i < numRawPorts; i++) {
            if (this->protocols[i].protocol == fwBuffer.getData()[loc + 1]) {
              this->rawOut_out(i, fwBuffer);
              break;
            }
          }
        }
        break;
    }
  }

  void SpaceWireController ::
    rawIn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    this->dataOut_out(0, fwBuffer);
  }

  void SpaceWireController ::
    registerAPID_handler(
        const NATIVE_INT_TYPE portNum,
        U16 APID,
        bool used
    )
  {
    FW_ASSERT(portNum < numCCSDSPorts);

    this->APIDs[portNum].APID = APID;
    this->APIDs[portNum].used = used;
  }

  void SpaceWireController ::
    registerTransID_handler(
        const NATIVE_INT_TYPE portNum,
        U16 TransID,
        bool used
    )
  {
    FW_ASSERT(portNum < numRMAPPorts);

    this->TransIDs[TransID].portNum = portNum;
    this->TransIDs[TransID].used = used;
  }

  void SpaceWireController ::
    registerProtocol_handler(
        const NATIVE_INT_TYPE portNum,
        U8 protocol,
        bool used
    )
  {
    FW_ASSERT(portNum < numRawPorts);

    this->protocols[portNum].protocol = protocol;
    this->protocols[portNum].used = used;
  }

} // end namespace SpaceWire
