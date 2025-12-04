add_library(mitl_core STATIC
    src/Real.cpp
    src/BigInt.cpp
)

target_include_directories(mitl_core PUBLIC include)
target_compile_features(mitl_core PUBLIC cxx_std_20)

add_executable(mitl_real_test
    src/RealTest.cpp
)

target_link_libraries(mitl_real_test PRIVATE mitl_core)
