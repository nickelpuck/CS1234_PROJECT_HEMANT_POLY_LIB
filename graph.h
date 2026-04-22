#ifndef GRAPH_H
#define GRAPH_H

#include "poly.h"

// ─── Graph Structure ─────────────────────────────────────────────────────────
// Adjacency matrix representation (undirected/directed, unweighted)
typedef struct {
    int   n;          // number of vertices
    int **adj;        // adj[i][j] = 1 if edge i->j exists
    char **labels;    // optional vertex labels
} Graph;

// ─── Create / Destroy ────────────────────────────────────────────────────────
Graph *graph_create(int n);
void   graph_destroy(Graph *g);
void   graph_add_edge(Graph *g, int u, int v);       // undirected
void   graph_add_directed_edge(Graph *g, int u, int v);
void   graph_set_label(Graph *g, int v, const char *label);
void   graph_print(const Graph *g);
void   graph_print_matrix(const Graph *g);

// ─── Graph Properties ────────────────────────────────────────────────────────
int    graph_degree(const Graph *g, int v);
int    graph_edge_count(const Graph *g);
int    graph_is_connected(const Graph *g);           // BFS-based
int   *graph_bfs_order(const Graph *g, int start, int *count);
int   *graph_dfs_order(const Graph *g, int start, int *count);
int    graph_has_cycle(const Graph *g);              // DFS-based
int   *graph_connected_components(const Graph *g, int *num_components);

// ─── Characteristic Polynomial ───────────────────────────────────────────────
// det(lambda*I - A) computed via cofactor expansion (recursive)
// Returns polynomial in lambda whose roots are eigenvalues of A
Poly  *graph_characteristic_poly(const Graph *g);

// ─── Chromatic Polynomial ────────────────────────────────────────────────────
// P(G, k) = number of proper k-colorings of graph G
// Uses deletion-contraction algorithm (recursive)
Poly  *graph_chromatic_poly(const Graph *g);
int    graph_chromatic_number(const Graph *g);       // min k: P(G,k) > 0

// ─── Matching Polynomial ─────────────────────────────────────────────────────
// mu(G, x) = sum_{k=0}^{floor(n/2)} (-1)^k * m(G,k) * x^(n-2k)
// where m(G,k) = number of matchings of size k
Poly  *graph_matching_poly(const Graph *g);
int    graph_matching_count(const Graph *g, int k);  // count k-matchings

// ─── Modular Polynomial Arithmetic (Finite Fields) ───────────────────────────
// Reduce polynomial coefficients mod prime p
Poly  *poly_mod_p(const Poly *poly, int p);
// Multiply two polynomials mod p
Poly  *poly_mul_mod_p(const Poly *a, const Poly *b, int p);
// Check if polynomial is irreducible over Z_p
int    poly_is_irreducible_zp(const Poly *poly, int p);
// Compute cyclotomic polynomial Phi_n(x)
Poly  *poly_cyclotomic(int n);
// Polynomial hashing: evaluate at x mod m
long   poly_hash(const Poly *poly, long x, long m);

// ─── File I/O for Graphs ─────────────────────────────────────────────────────
int    graph_save(const Graph *g, const char *filename);
Graph *graph_load(const char *filename);

#endif
