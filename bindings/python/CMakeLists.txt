find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(mitl mitl_py.cpp)
target_link_libraries(mitl PRIVATE mitl_core)
target_compile_features(mitl PRIVATE cxx_std_20)

install(TARGETS mitl
	LIBRARY DESTINATION .
	RUNTIME DESTINATION .
)
