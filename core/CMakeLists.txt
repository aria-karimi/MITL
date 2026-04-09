add_library(mitl_core STATIC
    src/Real.cpp
    src/BigInt.cpp
    src/Complex.cpp
)

target_include_directories(mitl_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(mitl_core PUBLIC cxx_std_20)

if (MSVC)
    target_compile_options(mitl_core PRIVATE /W4 /WX)
else()
    target_compile_options(mitl_core PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(mitl_real_test
    src/RealTest.cpp
)

target_link_libraries(mitl_real_test PRIVATE mitl_core)

add_executable(mitl_tests
    ../tests/test_all.cpp
)

target_link_libraries(mitl_tests PRIVATE mitl_core)

if (BUILD_TESTING)
    add_test(NAME mitl_tests COMMAND mitl_tests)
    add_test(NAME mitl_real_test COMMAND mitl_real_test)
endif()

install(TARGETS mitl_core
    EXPORT MITLTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT MITLTargets
    FILE MITLTargets.cmake
    NAMESPACE MITL::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MITL
)
