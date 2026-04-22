# Polynomial Library — CS1234 Project

A C library for polynomial arithmetic, calculus, root-finding, and file I/O.

**Author:** Hemant Kumar (hkujjwal2468)  
**Course:** CS1234 Small-Scale Application Development

## Features

- Create polynomials from coefficient arrays
- Add, subtract, multiply, scalar multiply, negate
- Evaluate using direct method and Horner's method
- Differentiate and integrate (symbolic)
- Polynomial long division — quotient + remainder
- GCD of two polynomials (Euclidean algorithm)
- Composition: f(g(x))
- Root finding: quadratic formula + Newton's method
- Save / load polynomials to / from files (state persistence)
- CLI mode: operate on saved polynomial files directly

## Files

| File | Description |
|------|-------------|
| `poly.h` | Header — all declarations and struct definition |
| `poly.c` | Library implementation (~400 lines) |
| `main.c` | Driver / demo program (~250 lines) |
| `Makefile` | Builds `libpoly.a` static library and `polydemo` binary |

## Compilation

```bash
make
```

Requires: `gcc`, `make`, standard C library + `libm` (math).  
Works on Linux / macOS / DCF machines — no extra installation needed.

## Running

### Full demo (all 8 features):

```bash
./polydemo
```

### CLI mode (operate on polynomial files):

Polynomial files store degree on line 1, then coefficients `coeffs[0]..coeffs[degree]` (constant term first).

Example `poly_a.txt` for `3x^2 + 2x + 1`:
```
2
1
2
3
```

```bash
# Evaluate at x=5
./polydemo eval poly_a.txt 5.0

# Differentiate
./polydemo diff poly_a.txt

# Integrate
./polydemo integ poly_a.txt

# Add two polynomials
./polydemo add poly_a.txt poly_b.txt

# Multiply
./polydemo mul poly_a.txt poly_b.txt

# GCD
./polydemo gcd poly_a.txt poly_b.txt

# Compose: f(g(x))
./polydemo compose poly_a.txt poly_b.txt
```

## Sample Output

```
╔══════════════════════════════════════╗
║   Polynomial Library Demo (CS1234)   ║
╚══════════════════════════════════════╝

========================================
DEMO 1: Basic Arithmetic
========================================
A = 3x^2 + 2x + 1
B = x^2 - x + 4
A + B = 4x^2 + x + 5
A - B = 2x^2 + 3x - 3
A * B = 3x^4 - x^3 + 15x^2 + 6x + 4
-A    = -3x^2 - 2x - 1
2.5*A = 7.5x^2 + 5x + 2.5

========================================
DEMO 5: GCD of Polynomials
========================================
A   = x^2 - 1
B   = x^2 - 3x + 2
GCD = x - 1

========================================
DEMO 7: Root Finding
========================================
Q = x^2 - 5x + 6
Real roots: 3  2
Newton root (x0=1.5): 1.324717957
```

## Clean

```bash
make clean
```
