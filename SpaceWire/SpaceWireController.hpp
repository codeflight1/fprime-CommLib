// ======================================================================
// \title  SpaceWireController.hpp
// \author codeflight
// \brief  hpp file for SpaceWireController component implementation class
//
// ======================================================================

#ifndef SpaceWireController_HPP
#define SpaceWireController_HPP

#include "SpaceWire/SpaceWireControllerComponentAc.hpp"

namespace SpaceWire {

  class SpaceWireController :
    public SpaceWireControllerComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object SpaceWireController
      //!
      SpaceWireController(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object SpaceWireController
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object SpaceWireController
      //!
      ~SpaceWireController();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for CCSDSin
      //!
      void CCSDSin_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const SpaceWire::CCSDSPacket &p /*!< Incoming CCSDS packet */
      );

      //! Handler implementation for RMAPin
      //!
      void RMAPin_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const SpaceWire::RMAPPacket &p /*!< Incoming RMAP packet */
      );

      //! Handler implementation for dataIn
      //!
      void dataIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer /*!< Incoming data from SpaceWire driver */
      );

      //! Handler implementation for rawIn
      //!
      void rawIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer /*!< Incoming raw data */
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for registerAPID command handler
      //!
      void registerAPID_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U16 APID, /*!< APID to register */
          U8 portNum /*!< Port number to register APID to */
      );

      //! Implementation for registerProtocol command handler
      //!
      void registerProtocol_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U8 protocol, /*!< Protocol to register */
          U8 portNum /*!< Port number to register protocol to */
      );

    PRIVATE:

      U16 APID[32]; /*!< Registered APIDs */
      U8 protocol[32]; /*!< Registered custom protocols */

    };

} // end namespace SpaceWire

#endif
