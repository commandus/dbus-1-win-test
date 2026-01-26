#
# Windows prerequisites:
# vcpkg install --triplet x64-windows dbus-1
# mkdir build; cd build; cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\git\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
#
cmake_minimum_required(VERSION 3.28)
project(dbus-1-win-test)

set(CMAKE_CXX_STANDARD 17)

find_package(DBus1 CONFIG REQUIRED)

set(OS_SPECIFIC_LIBS wsock32 ws2_32 Userenv WindowsApp.lib)

add_executable(dbus-1-win-test main.cpp)
target_link_libraries(dbus-1-win-test PRIVATE dbus-1 ${OS_SPECIFIC_LIBS})
