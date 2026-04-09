import mitl


def main() -> None:
    a = mitl.BigInt("12345678901234567890")
    b = mitl.BigInt("1000000007")
    q, r = a.divmod(b)

    print("BigInt demo")
    print("a      =", a)
    print("b      =", b)
    print("a / b  =", q)
    print("a % b  =", r)

    x = mitl.Real(1.0)
    y = mitl.Real(3.0)
    z = x.divide_by(y, 64)
    z_trunc = mitl.Real(22.0).divide_by(mitl.Real(7.0), 0)

    print("\nReal demo")
    print("x / y  =", z)
    print("trunc(22/7) =", z_trunc)

    c = mitl.Complex.parse("1.5-2i")
    d = mitl.Complex(3.0, -4.0)
    print("\nComplex demo")
    print("c      =", c)
    print("d      =", d)
    print("c / d  =", c / d)


if __name__ == "__main__":
    main()
