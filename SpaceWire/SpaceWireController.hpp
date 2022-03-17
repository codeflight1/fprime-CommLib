// ======================================================================
// \title  SpaceWireController.hpp
// \author codeflight
// \brief  hpp file for SpaceWireController component implementation class
//
// ======================================================================

#ifndef SpaceWireController_HPP
#define SpaceWireController_HPP

#include "SpaceWire/SpaceWireControllerComponentAc.hpp"

#include <SpaceWire/FppConstantsAc.hpp>

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

      void registerAPID_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U16 APID, /*!< APID to register */
          bool used /*!< Register or deregister */
      );

      void registerTransID_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U16 TransID, /*!< Transaction ID to register */
          bool used /*!< Register or deregister */
      );

      void registerProtocol_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U8 protocol, /*!< Protocol to register */
          bool used /*!< Register or deregister */
      );

    PRIVATE:

      struct {
        U16 APID;
        bool used = false;
      } APIDs[numCCSDSPorts]; /*!< Registered APIDs */

      struct {
        U8 protocol;
        bool used = false;
      } protocols[numRawPorts]; /*!< Registered custom protocols */

      struct {
        U8 portNum;
        bool used = false;
      } TransIDs[65536]; /*!< Registered transaction IDs */

    };

} // end namespace SpaceWire

#endif
