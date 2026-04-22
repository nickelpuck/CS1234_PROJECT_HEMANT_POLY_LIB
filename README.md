# Polynomial + Graph Theory Library — CS1234 Project

A C library combining **polynomial arithmetic** with **graph theory** via characteristic, chromatic, and matching polynomials. Bridges CS1234 (C programming, data structures) and CS1202 (Discrete Mathematics, Graph Theory).

**Author:** Hemant Kumar (hkujjwal2468)  
**Course:** CS1234 Small-Scale Application Development

---

## Features

### Polynomial Library (`poly.h`, `poly.c`)
- Create polynomials from coefficient arrays
- Add, subtract, multiply, scalar multiply, negate
- Evaluate using direct method and **Horner's method**
- **Differentiate** and **integrate** (symbolic)
- Polynomial **long division** — quotient + remainder
- **GCD** of two polynomials (Euclidean algorithm)
- **Composition**: f(g(x))
- **Root finding**: quadratic formula + Newton's method
- **Modular arithmetic**: reduce coefficients mod prime p
- **Irreducibility test** over finite fields Z_p
- **Cyclotomic polynomials** Phi_n(x)
- **Polynomial hashing** (Horner mod m)
- Save / load polynomials to / from files

### Graph Theory Library (`graph.h`, `graph.c`)
- Adjacency matrix graph representation
- Add edges (directed / undirected), vertex labels
- **BFS** and **DFS** traversal
- Connectivity check, cycle detection
- Connected components
- **Characteristic polynomial** det(λI - A) — eigenvalues of adjacency matrix
- **Chromatic polynomial** P(G, k) via deletion-contraction (counts k-colorings)
- **Chromatic number** (minimum colors needed)
- **Matching polynomial** and k-matching count
- Save / load graphs to / from files

---

## Files

| File | Description |
|------|-------------|
| `poly.h` | Polynomial header — declarations + struct |
| `poly.c` | Polynomial library implementation |
| `graph.h` | Graph header — declarations + struct |
| `graph.c` | Graph library implementation |
| `main.c` | Driver with 15 demos |
| `Makefile` | Builds `libpoly.a` + `polydemo` binary |

---

## Compilation

```bash
make
```

Requires: `gcc`, `make`, standard C library + `libm`.  
No extra installation needed. Works on Linux / macOS / DCF.

---

## Running

### Full demo (all 15 features):

```bash
./polydemo
```

### CLI mode — Polynomial operations:

Polynomial file format (degree on line 1, then coefficients constant-first):
```
2
1
2
3
```
(represents 3x^2 + 2x + 1)

```bash
./polydemo eval  poly_a.txt 5.0       # evaluate at x=5
./polydemo diff  poly_a.txt           # differentiate
./polydemo integ poly_a.txt           # integrate
./polydemo add   poly_a.txt poly_b.txt
./polydemo mul   poly_a.txt poly_b.txt
./polydemo gcd   poly_a.txt poly_b.txt
./polydemo compose poly_a.txt poly_b.txt
```

### CLI mode — Graph operations:

Graph file format (n on line 1, then n x n adjacency matrix):
```
3
0 1 1
1 0 1
1 1 0
```

```bash
./polydemo ginfo     graph.txt    # print graph info, connectivity
./polydemo charpoly  graph.txt    # characteristic polynomial
./polydemo chrompoly graph.txt    # chromatic polynomial + chromatic number
./polydemo matching  graph.txt    # matching polynomial
```

---

## Sample Output

```
╔══════════════════════════════════════╗
║  Polynomial + Graph Library (CS1234) ║
╚══════════════════════════════════════╝

DEMO 1: Basic Arithmetic
A = 3x^2 + 2x + 1
B = x^2 - x + 4
A + B = 4x^2 + x + 5
A * B = 3x^4 - x^3 + 15x^2 + 6x + 4

DEMO 10: Characteristic Polynomial
Graph: Cycle C4
Characteristic poly: x^4 - 4x^2
lambda = -2 is an eigenvalue
lambda =  0 is an eigenvalue
lambda =  2 is an eigenvalue

DEMO 11: Chromatic Polynomial
Graph: Triangle K3
P(G, k) = k^3 - 3k^2 + 2k
k=1: 0 colorings
k=2: 0 colorings
k=3: 6 colorings
Chromatic number: 3

DEMO 14: Cyclotomic Polynomials
Phi_1(x) = x - 1
Phi_2(x) = x + 1
Phi_3(x) = x^2 + x + 1
Phi_4(x) = x^2 + 1
Phi_6(x) = x^2 - x + 1
```

---

## Clean

```bash
make clean
```
