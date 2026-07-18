add_executable(NametagLayoutTest
  NametagLayoutTest.cpp
)

target_include_directories(NametagLayoutTest PRIVATE
  ${REPO_ROOT}/lib/Nametag
)

target_link_libraries(NametagLayoutTest PRIVATE
  crosspoint_test_common
  GTest::gtest_main
)

gtest_discover_tests(NametagLayoutTest)
