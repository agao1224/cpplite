test:
	cmake -S . -B build
	cmake --build build
	ctest --test-dir build

test_verbose:
	cmake -S . -B build
	cmake --build build
	ctest --test-dir build --output-on-failure

all:
	cmake -S . -B build
	cmake --build build
