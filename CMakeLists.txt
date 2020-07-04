cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
# CMP0000: Call the cmake_minimum_required() command at the beginning of the top-level
# CMakeLists.txt file even before calling the project() command.
# The cmake_minimum_required(VERSION) command implicitly invokes the cmake_policy(VERSION)
# command to specify that the current project code is written for the given range of CMake
# versions.
project(qtsni-theme)

include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Minimum Versions
set(QT_MINIMUM_VERSION "5.10.0")

find_package(Qt5DBus ${QT_MINIMUM_VERSION} REQUIRED)
find_package(Qt5Widgets ${QT_MINIMUM_VERSION} REQUIRED)
find_package(dbusmenu-qt5 REQUIRED)

add_subdirectory(src)
