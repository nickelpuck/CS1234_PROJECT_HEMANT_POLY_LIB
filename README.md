# Polynomial + Graph Theory + Cryptography Library
## CS1234 Small-Scale Application Development

A comprehensive C library bridging **polynomial algebra**, **graph theory**, **cryptography**, and **coding theory**. Connects CS1234 (C programming, data structures) with CS1202 (Discrete Mathematics, Graph Theory).

**Name:** Hemant Kumar Ujjwal [CS25B031] (Github:nickelpuck)

---

## Modules

### 1. Polynomial Library (`poly.h`, `poly.c`)
- Create, add, subtract, multiply, scalar multiply, negate
- Evaluate: direct method + **Horner's method**
- **Differentiate** and **integrate** (symbolic)
- Polynomial **long division** (quotient + remainder)
- **GCD** via Euclidean algorithm
- **Composition** f(g(x))
- **Root finding**: quadratic formula + Newton's method
- **Modular arithmetic** over Z_p (finite fields)
- **Irreducibility test** over Z_p
- **Cyclotomic polynomials** Phi_n(x)
- **Polynomial hashing** (Horner mod m)
- File save / load

### 2. Graph Theory Library (`graph.h`, `graph.c`)
- Adjacency matrix representation, BFS, DFS
- Connectivity, cycle detection, connected components
- **Characteristic polynomial** det(λI - A)
- **Spectral graph theory**: eigenvalues, spectral gap, algebraic connectivity (Fiedler value), cospectral detection
- **Chromatic polynomial** P(G,k) via deletion-contraction
- **Chromatic number**
- **Tutte polynomial** T(G; x, y) — generalizes chromatic + matching + reliability
  - T(1,1) = number of spanning trees
  - T(2,0) = number of acyclic orientations
  - T(1,2) = number of connected spanning subgraphs
- **Matching polynomial** and k-matching count
- **Reliability polynomial** R(G, p)
- Graph file save / load

### 3. Cryptography (`crypto.h`, `crypto.c`)
- **Shamir's Secret Sharing**: split secret into n shares, reconstruct from any k
- **Lagrange interpolation** (real-valued and over Z_p)
- **Rabin-Karp string search** via polynomial fingerprinting + rolling hash
- **Polynomial file fingerprinting** (checksum)
- Modular arithmetic: fast exponentiation, modular inverse (Fermat's little theorem)

### 4. Error Correcting Codes (`codes.h`, `codes.c`)
- **Reed-Solomon encoding**: message → polynomial → evaluate at n points
- **Reed-Solomon decoding**: recover message from codeword with erasures
- **Berlekamp-Welch error correction**: recover polynomial from corrupted evaluations
- **Polynomial checksum** (CRC-like): compute and verify data integrity

---

## Files

| File | Description |

| `poly.h / poly.c` | Polynomial library |
| `graph.h / graph.c` | Graph theory library |
| `crypto.h / crypto.c` | Cryptography (Shamir, Rabin-Karp, Lagrange) |
| `codes.h / codes.c` | Error correcting codes (Reed-Solomon) |
| `main.c` | Driver with 22 demos |
| `Makefile` | Builds `libpoly.a` + `polydemo` |

---

## Compilation

```bash
make
```

Requires: `gcc`, `make`, standard C + `libm`. No extra installation needed.
Works on Linux / macOS / DCF machines.

---

## Running

### Full demo (all 22 features):
```bash
./polydemo
```

---

## Demo Overview

| # | Demo | Concept |

| 1 | Basic arithmetic | Polynomial algebra |
| 2 | Evaluation | Horner's method |
| 3 | Calculus | Differentiation, integration |
| 4 | Division | Long division algorithm |
| 5 | GCD | Euclidean algorithm |
| 6 | Composition | f(g(x)) |
| 7 | Root finding | Newton's method |
| 8 | File I/O | State persistence |
| 9 | Cyclotomic | Number theory |
| 10 | Finite fields | Z_p arithmetic, irreducibility |
| 11 | Graph traversal | BFS, DFS |
| 12 | Spectral theory | Eigenvalues, Fiedler value |
| 13 | Chromatic poly | Graph coloring |
| 14 | Tutte polynomial | Spanning trees, orientations |
| 15 | Reliability poly | Network reliability |
| 16 | Matching poly | Combinatorics |
| 17 | Lagrange interp | Polynomial reconstruction |
| 18 | Shamir sharing | Cryptographic secret sharing |
| 19 | Rabin-Karp | String search via hashing |
| 20 | Reed-Solomon | Error correcting codes |
| 21 | Checksum | Data integrity (CRC-like) |
| 22 | Berlekamp-Welch | Error correction |

---

## Clean
```bash
make clean
```
