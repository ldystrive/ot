add_executable(
    unit_tests
    operation_test.cpp
    )

target_link_libraries(
    unit_tests
    gtest_main
    ot
    )

add_test(
  NAME
    unit
  COMMAND
    ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/unit_tests
  )
