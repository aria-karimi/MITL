cmake_minimum_required(VERSION 3.16)
project(MITL VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CTest)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(MITL_BUILD_PYTHON "Build Python bindings" ON)

add_subdirectory(core)

if (MITL_BUILD_PYTHON)
	set(PYBIND11_FINDPYTHON ON)
	find_package(Python COMPONENTS Interpreter Development QUIET)
	find_package(pybind11 CONFIG QUIET)
	if (Python_FOUND AND pybind11_FOUND)
		add_subdirectory(bindings/python)
	else()
		message(WARNING "Python bindings disabled: Python or pybind11 not found")
	endif()
endif()

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/MITLConfigVersion.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/MITLConfig.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/MITLConfig.cmake"
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MITL
)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/MITLConfig.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/MITLConfigVersion.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MITL
)

