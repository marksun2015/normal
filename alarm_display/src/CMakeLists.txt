CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(alarm)
SET(VERSION "1.0.0")

find_package(PkgConfig)
pkg_check_modules(GLIB-2.0 glib-2.0)

#ADD_EXECUTABLE(upgradeOS main.cpp nandwrite.cpp mtd_debug.cpp flash_eraseall.cpp)
ADD_EXECUTABLE(alarm main.cpp )
set(CMAKE_EXE_LINKER_FLAGS -static)
#set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
#set(CMAKE_EXE_LINK_DYNAMIC_C_FLAGS)       # remove -Wl,-Bdynamic
#set(CMAKE_EXE_LINK_DYNAMIC_CXX_FLAGS)
#set(CMAKE_SHARED_LIBRARY_C_FLAGS)         # remove -fPIC
#set(CMAKE_SHARED_LIBRARY_CXX_FLAGS)
#set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)    # remove -dynamic
#set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)

TARGET_LINK_LIBRARIES(
  alarm
  display
  pthread
  # ${GLIB-2.0_LIBRARIES}
)

# Installation directories
INCLUDE(GNUInstallDirs)
INSTALL(TARGETS alarm RUNTIME DESTINATION ${CMAKE_INSTALL_SBINDIR})
