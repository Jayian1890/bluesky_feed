add_executable(https_client_test test_https_client.cpp)
target_link_libraries(https_client_test PRIVATE HTTPSClient gtest_main gtest)
add_test(NAME HTTPSClientTest COMMAND https_client_test)