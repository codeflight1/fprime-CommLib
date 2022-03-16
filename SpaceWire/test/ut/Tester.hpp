// ======================================================================
// \title  Tester.hpp
// \author codeflight
// \brief  hpp file for SpaceWireController test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "GTestBase.hpp"
#include "SpaceWire/SpaceWireController.hpp"

namespace SpaceWire {

  class Tester :
    public SpaceWireControllerGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object Tester
      //!
      Tester();

      //! Destroy object Tester
      //!
      ~Tester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      void RMAPDispatchTest();
      void CCSDSDispatchTest();
      void rawDispatchTest();
      void RMAPinTest();
      void CCSDSinTest();

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_CCSDSout
      //!
      void from_CCSDSout_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const SpaceWire::CCSDSPacket &p
      );

      //! Handler for from_RMAPout
      //!
      void from_RMAPout_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const SpaceWire::RMAPPacket &p
      );

      //! Handler for from_dataOut
      //!
      void from_dataOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer
      );

      //! Handler for from_rawOut
      //!
      void from_rawOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer
      );

    private:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //! Connect ports
      //!
      void connectPorts();

      //! Initialize components
      //!
      void initComponents();

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      SpaceWireController component;

  };

} // end namespace SpaceWire

#endif
