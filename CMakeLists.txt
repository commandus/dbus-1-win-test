#
# Windows prerequisites:
# vcpkg install --triplet x64-windows dbus-1
# mkdir build; cd build; cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\git\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
#
cmake_minimum_required(VERSION 3.22)
project(dbus-1-win-test)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_package(DBus1 CONFIG REQUIRED)
    set(OS_SPECIFIC_LIBS wsock32 ws2_32 Userenv)
else()
    set(EXTRA_HEADERS "/usr/include/dbus-1.0" "/usr/lib/x86_64-linux-gnu/dbus-1.0/include")
endif()

set(CFLAGS_GIO
    /usr/include/dbus-1.0 /usr/lib/x86_64-linux-gnu/dbus-1.0/include
)
# /usr/include/libmount /usr/include/blkid /usr/include/glib-2.0 /usr/lib/x86_64-linux-gnu/glib-2.0/include

add_executable(dbus-1-win-test main.cpp)
#target_link_libraries(dbus-1-win-test PRIVATE dbus-1 dbus-glib-1 gio-2.0 gobject-2.0 glib-2.0  ${OS_SPECIFIC_LIBS})
#target_include_directories(dbus-1-win-test PUBLIC ${CFLAGS_GIO} ${EXTRA_HEADERS})
target_link_libraries(dbus-1-win-test PRIVATE dbus-1 ${OS_SPECIFIC_LIBS})
# target_include_directories(dbus-1-win-test PUBLIC ${CFLAGS_GIO} ${EXTRA_HEADERS})

