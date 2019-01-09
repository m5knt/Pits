# @author Yukio KANEDA

cmake_minimum_required(VERSION 3.1)
project(PitsTest CXX)

include_directories(source "../Pits/..")

add_executable(TestTimer TestTimer.cpp)
target_link_libraries(TestTimer Pits)

add_executable(TestStringLiteral TestStringLiteral.cpp)
target_link_libraries(TestStringLiteral Pits)

add_executable(TestEncoding TestEncoding.cpp)
target_link_libraries(TestEncoding Pits)

add_executable(TestPits TestPits.cpp)
target_link_libraries(TestPits Pits)

add_executable(TestStringStream TestStringStream.cpp)
target_link_libraries(TestStringStream Pits)


# add_executable(TestFormat TestFormat.cpp)
# target_link_libraries(TestFormat Pits)

# set(CMAKE_CXX_FLAGS "/std:c++17")
# install(TARGETS Pits DESTINATION lib)
# install(DIRECTORY include/Pits DESTINATION include)
# install(DIRECTORY test/packages/EABase/include/Common/EABase DESTINATION include)
# set(CMAKE_CXX_FLAGS_DEBUG "-g3 /O0 -pg")
# set(CMAKE_CXX_FLAGS_RELEASE "/O2 -s /DNDEBUG")
# set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 /Og -pg")
# set(CMAKE_CXX_FLAGS_MINSIZEREL "/Os -s /DNDEBUG")
# add_library(A STATIC a.c)
# add_library(B STATIC b.c)
# target_link_libraries(A B)
# target_link_libraries(B A)
# add_executable(main main.c)
# target_link_libraries(main A)
