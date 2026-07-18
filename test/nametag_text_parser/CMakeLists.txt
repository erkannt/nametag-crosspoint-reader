add_executable(NametagTextTest
  NametagTextTest.cpp
  ${REPO_ROOT}/lib/Nametag/NametagText.cpp
)

target_include_directories(NametagTextTest PRIVATE
  ${REPO_ROOT}/lib/Nametag
)

target_link_libraries(NametagTextTest PRIVATE
  crosspoint_test_common
  GTest::gtest_main
)

gtest_discover_tests(NametagTextTest)
