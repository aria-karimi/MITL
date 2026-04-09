# MITL - Memory Is The Limit

MITL is a C++20 arbitrary-precision math library with three core numeric types:

- `mitl::BigInt`: signed arbitrary-precision integer using base `10^9` limbs.
- `mitl::Real`: arbitrary-precision binary floating-point style value.
- `mitl::Complex`: complex number type backed by `mitl::Real` components.

Python bindings are provided through `pybind11`.

MITL exposes C++ and Python APIs with the same core behavior for integer, real, and complex arithmetic.

## Features

- Arbitrary-precision integer arithmetic (`+`, `-`, `*`, `/`, `%`).
- Signed division returning quotient and remainder.
- Arbitrary-precision real arithmetic with NaN/Infinity support.
- Complex arithmetic (`+`, `-`, `*`, `/`) with conjugate, norm, magnitude, and phase.
- CMake-based C++ build and install flow.
- Python extension module build via scikit-build-core.

## Behavioral Notes

- `BigInt` parsing is strict: accepted forms are `[+|-]?[0-9]+` with no embedded whitespace.
- `BigInt::div` uses truncated division semantics.
	- Quotient truncates toward zero.
	- Remainder keeps the dividend sign.
- `Real` models finite values, infinities, and NaN.
- `Real::divideBy(divisor, precision)` quantizes the quotient to decimal digits via fixed formatting.
	- `precision == 0` returns truncated integer quotient.
	- Precision is capped internally at 256 digits.
- `Real` relational operators return `false` whenever NaN is involved.
- `Real` `%` uses integer-style remainder over integer-converted operands.
- `Complex::toString()` emits canonical `a+bi` or `a-bi` text.
- `Complex::parse()` accepts `a+bi` / `a-bi` with optional whitespace and scientific notation.
- `Complex` equality is approximate (double-based tolerance), not exact symbolic equality.

## Requirements

- CMake 3.16+
- C++20 compiler
	- MSVC 2022+, GCC 11+, or Clang 14+
- Python 3.9+ (for Python bindings)
- `pybind11` (for Python bindings)

## Build (C++)

From repository root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The project enforces strict warnings (`/W4 /WX` on MSVC, `-Wall -Wextra -Wpedantic -Werror` on GCC/Clang).

## Run Tests

```bash
ctest --test-dir build --output-on-failure
```

Or directly:

```bash
./build/core/mitl_tests
```

On Windows multi-config generators, the test executable may be under `build/core/Release/`.

## Install (C++)

```bash
cmake --install build --prefix ./install
```

Installed artifacts include headers and `MITLTargets.cmake` for downstream CMake usage.

## Python Build and Install

Build wheel/sdist:

```bash
python -m pip install --upgrade pip build
python -m build
```

Editable install (development):

```bash
python -m pip install -e .
```

## Usage Examples

### C++ BigInt

```cpp
#include "mitl/BigInt.hpp"

mitl::BigInt a("12345678901234567890");
mitl::BigInt b(7);
auto qr = a.div(b);

// Truncated division: remainder follows dividend sign
auto neg = mitl::BigInt(-17).div(mitl::BigInt(5)); // q = -3, r = -2
```

### C++ Real

```cpp
#include "mitl/Real.hpp"

mitl::Real x(1.0);
mitl::Real y(3.0);
mitl::Real z = x.divideBy(y, 64);

// precision == 0 means truncation toward zero
mitl::Real t = mitl::Real(22.0).divideBy(mitl::Real(7.0), 0); // 3
```

### C++ Complex

```cpp
#include "mitl/Complex.hpp"

mitl::Complex c = mitl::Complex::parse("1.5-2e0i");
mitl::Complex d(3.0, 4.0);
auto q = c / d;
```

### Python

```python
import mitl

a = mitl.BigInt("12345678901234567890")
b = mitl.BigInt("1000000007")
q, r = a.divmod(b)

x = mitl.Real(1.0)
y = mitl.Real(3.0)
z = x.divide_by(y, 64)

c1 = mitl.Complex(1.0, 2.0)
c2 = mitl.Complex(3.0, -4.0)
q = c1 / c2

parsed = mitl.Complex.parse("1.5-2i")
```

See the `examples/` directory for runnable samples.

## Mathematical Notes

- `BigInt` stores little-endian limbs in base `10^9`.
- `Real` stores base-`2^64` limbs and a binary exponent.
- `BigInt::div` follows truncated division semantics.
- `Complex` arithmetic is implemented on top of `Real` components.

## Documentation

Generate API docs with Doxygen:

```bash
doxygen Doxyfile
```

Output is written to `docs/html/`.

In this branch, generated API documentation under `docs/html/` is intentionally tracked.

## Project Status

Version: `1.0.0`

## License

MIT License. See `LICENSE`.
