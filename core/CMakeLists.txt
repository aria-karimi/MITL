add_library(mitl_core STATIC
    src/Real.cpp
)

target_include_directories(mitl_core PUBLIC include)
target_compile_features(mitl_core PUBLIC cxx_std_20)
