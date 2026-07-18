add_executable(NametagSizeFitTest
  NametagSizeFitTest.cpp
  ${REPO_ROOT}/lib/Nametag/NametagSizeFit.cpp
)

target_include_directories(NametagSizeFitTest PRIVATE
  ${REPO_ROOT}/lib/Nametag
)

target_link_libraries(NametagSizeFitTest PRIVATE
  crosspoint_test_common
  GTest::gtest_main
)

gtest_discover_tests(NametagSizeFitTest)
