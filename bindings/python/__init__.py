# pyright: reportMissingImports=false
from typing import Any

try:
    from .mitl import AllocationError, BigInt, Complex, ConversionError, MitlError, Real
except ImportError:
    MitlError = AllocationError = ConversionError = BigInt = Real = Complex = Any

__all__ = [
    "MitlError",
    "AllocationError",
    "ConversionError",
    "BigInt",
    "Real",
    "Complex",
]

__version__ = "1.0.0"
