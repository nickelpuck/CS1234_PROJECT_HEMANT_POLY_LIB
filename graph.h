#ifndef GRAPH_H
#define GRAPH_H

#include "poly.h"

//Graph Struct
typedef struct {
    int n; // no of vertices
    int **adj;  // adj[i][j] = 1 if edge i->j exists
    char **labels;// vertex labels
} Graph;

//Create/Destroy
Graph *graph_create(int n);
void graph_destroy(Graph *g);
void graph_add_edge(Graph *g, int u, int v);
void graph_add_directed_edge(Graph *g, int u, int v);
void graph_set_label(Graph *g, int v, const char *label);
void graph_print(const Graph *g);
void graph_print_matrix(const Graph *g);

//Graph Properties
int graph_degree(const Graph *g, int v);
int graph_edge_count(const Graph *g);
int graph_is_connected(const Graph *g);
int *graph_bfs_order(const Graph *g, int start, int *count);
int *graph_dfs_order(const Graph *g, int start, int *count);
int graph_has_cycle(const Graph *g);
int *graph_connected_components(const Graph *g, int *num_components);

//Characteristic Polyn
// det(lambda*I - A):roots are eigenvalues of adjacency matrix
Poly  *graph_characteristic_poly(const Graph *g);

//Spectral Graph Theory
// Compute spectrum (eigenvalue approximations) from characteristic poly
double *graph_spectrum(const Graph *g, int *count);
// Algebraic connectivity (2nd smallest eigenvalue of Laplacian ~ Fiedler value)
double  graph_algebraic_connectivity(const Graph *g);
// Check if two graphs are cospectral (same characteristic poly)
int     graph_is_cospectral(const Graph *g, const Graph *h);
// Spectral gap (difference between 2 largest eigenvalues)
double  graph_spectral_gap(const Graph *g);

// ─── Chromatic Polynomial ─────────────────────────────────────────────────────
Poly  *graph_chromatic_poly(const Graph *g);
int    graph_chromatic_number(const Graph *g);

// ─── Matching Polynomial ──────────────────────────────────────────────────────
Poly  *graph_matching_poly(const Graph *g);
int    graph_matching_count(const Graph *g, int k);

// ─── Tutte Polynomial ─────────────────────────────────────────────────────────
// T(G; x, y) generalizes chromatic + matching + reliability polynomials
// Evaluated at specific points gives:
//   T(G; 1, 1) = number of spanning trees
//   T(G; 2, 0) = number of acyclic orientations
//   T(G; 1, 2) = number of connected spanning subgraphs
// We represent T as a 2D array of coefficients: tutte[i][j] = coeff of x^i * y^j
typedef struct {
    int     max_deg;       // max degree in x and y
    double **coeffs;       // coeffs[i][j] for x^i * y^j
} TuttePoly;

TuttePoly *tutte_create(int max_deg);
void       tutte_destroy(TuttePoly *t);
void       tutte_add_term(TuttePoly *t, int i, int j, double val);
double     tutte_eval(const TuttePoly *t, double x, double y);
void       tutte_print(const TuttePoly *t);
TuttePoly *tutte_add(const TuttePoly *a, const TuttePoly *b);
TuttePoly *graph_tutte_poly(const Graph *g);
long       graph_spanning_trees(const Graph *g);   // T(G; 1, 1)
long       graph_acyclic_orientations(const Graph *g); // T(G; 2, 0)

// ─── Reliability Polynomial ───────────────────────────────────────────────────
// R(G, p) = probability graph is connected if each edge survives with prob p
// R(G, p) = sum_{k} c_k * p^k * (1-p)^(e-k)
Poly  *graph_reliability_poly(const Graph *g);
double graph_reliability_eval(const Graph *g, double p);

// ─── File I/O ─────────────────────────────────────────────────────────────────
int    graph_save(const Graph *g, const char *filename);
Graph *graph_load(const char *filename);

#endif
