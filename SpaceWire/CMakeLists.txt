set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SpaceWire.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/SpaceWireAddr.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/RMAP.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/RMAPPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/CCSDS.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/CCSDSPacket.cpp"
)

register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Test.cpp"
)

register_fprime_ut()
