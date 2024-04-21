cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


enable_testing()

add_executable(
  hello_test
  hellotest.cpp
  HttpBlock.cpp
  ServerBlock.cpp
  ../AConfig.cpp 
  ../AConfigs.cpp 
  ../AGroup.cpp 
  ../AConfigString.cpp 
  ../AConfigInt.cpp 
  ../Configs.cpp 
  ../Worker_processes.cpp 
  ../Client_max_body.cpp 
  ../Http.cpp 
  ../Server.cpp
  ../Server_name.cpp 
  ../Listen.cpp 
  ../Index.cpp 
  ../Root.cpp 
  ../Redirect.cpp 
  ../Error_page.cpp 
  ../Location.cpp
  ../Allow.cpp
  ../AConfigVectorString.cpp 
  ../TestConfigMethod/GetRoot.cpp
  libparser.a
)
target_link_libraries(
  hello_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(hello_test)