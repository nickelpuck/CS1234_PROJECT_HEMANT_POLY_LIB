#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ─── Create / Destroy ────────────────────────────────────────────────────────

Graph *graph_create(int n) {
    if (n <= 0) return NULL;
    Graph *g = (Graph *)malloc(sizeof(Graph));
    if (!g) return NULL;
    g->n = n;

    g->adj = (int **)calloc(n, sizeof(int *));
    g->labels = (char **)calloc(n, sizeof(char *));
    if (!g->adj || !g->labels) { free(g); return NULL; }

    for (int i = 0; i < n; i++) {
        g->adj[i] = (int *)calloc(n, sizeof(int));
        char buf[16];
        snprintf(buf, sizeof(buf), "v%d", i);
        g->labels[i] = strdup(buf);
    }
    return g;
}

void graph_destroy(Graph *g) {
    if (!g) return;
    for (int i = 0; i < g->n; i++) {
        free(g->adj[i]);
        free(g->labels[i]);
    }
    free(g->adj);
    free(g->labels);
    free(g);
}

void graph_add_edge(Graph *g, int u, int v) {
    if (!g || u < 0 || v < 0 || u >= g->n || v >= g->n) return;
    g->adj[u][v] = 1;
    g->adj[v][u] = 1;
}

void graph_add_directed_edge(Graph *g, int u, int v) {
    if (!g || u < 0 || v < 0 || u >= g->n || v >= g->n) return;
    g->adj[u][v] = 1;
}

void graph_set_label(Graph *g, int v, const char *label) {
    if (!g || v < 0 || v >= g->n || !label) return;
    free(g->labels[v]);
    g->labels[v] = strdup(label);
}

void graph_print(const Graph *g) {
    if (!g) return;
    printf("Graph (%d vertices, %d edges):\n", g->n, graph_edge_count(g));
    for (int u = 0; u < g->n; u++) {
        printf("  %s -> ", g->labels[u]);
        int first = 1;
        for (int v = 0; v < g->n; v++) {
            if (g->adj[u][v]) {
                if (!first) printf(", ");
                printf("%s", g->labels[v]);
                first = 0;
            }
        }
        if (first) printf("(none)");
        printf("\n");
    }
}

void graph_print_matrix(const Graph *g) {
    if (!g) return;
    printf("Adjacency Matrix:\n   ");
    for (int j = 0; j < g->n; j++) printf("%3s", g->labels[j]);
    printf("\n");
    for (int i = 0; i < g->n; i++) {
        printf("%3s", g->labels[i]);
        for (int j = 0; j < g->n; j++)
            printf("%3d", g->adj[i][j]);
        printf("\n");
    }
}

// ─── Graph Properties ────────────────────────────────────────────────────────

int graph_degree(const Graph *g, int v) {
    if (!g || v < 0 || v >= g->n) return -1;
    int deg = 0;
    for (int j = 0; j < g->n; j++) deg += g->adj[v][j];
    return deg;
}

int graph_edge_count(const Graph *g) {
    if (!g) return 0;
    int count = 0;
    for (int i = 0; i < g->n; i++)
        for (int j = i + 1; j < g->n; j++)
            if (g->adj[i][j]) count++;
    return count;
}

// BFS traversal order from start vertex
int *graph_bfs_order(const Graph *g, int start, int *count) {
    if (!g || start < 0 || start >= g->n) return NULL;
    int *order   = (int *)malloc(g->n * sizeof(int));
    int *visited = (int *)calloc(g->n, sizeof(int));
    int *queue   = (int *)malloc(g->n * sizeof(int));
    if (!order || !visited || !queue) {
        free(order); free(visited); free(queue); return NULL;
    }
    int head = 0, tail = 0, idx = 0;
    queue[tail++] = start;
    visited[start] = 1;
    while (head < tail) {
        int u = queue[head++];
        order[idx++] = u;
        for (int v = 0; v < g->n; v++) {
            if (g->adj[u][v] && !visited[v]) {
                visited[v] = 1;
                queue[tail++] = v;
            }
        }
    }
    *count = idx;
    free(visited); free(queue);
    return order;
}

// DFS traversal (iterative using stack)
int *graph_dfs_order(const Graph *g, int start, int *count) {
    if (!g || start < 0 || start >= g->n) return NULL;
    int *order   = (int *)malloc(g->n * sizeof(int));
    int *visited = (int *)calloc(g->n, sizeof(int));
    int *stack   = (int *)malloc(g->n * sizeof(int));
    if (!order || !visited || !stack) {
        free(order); free(visited); free(stack); return NULL;
    }
    int top = 0, idx = 0;
    stack[top++] = start;
    while (top > 0) {
        int u = stack[--top];
        if (visited[u]) continue;
        visited[u] = 1;
        order[idx++] = u;
        for (int v = g->n - 1; v >= 0; v--)
            if (g->adj[u][v] && !visited[v])
                stack[top++] = v;
    }
    *count = idx;
    free(visited); free(stack);
    return order;
}

int graph_is_connected(const Graph *g) {
    if (!g || g->n == 0) return 1;
    int count;
    int *order = graph_bfs_order(g, 0, &count);
    int result = (count == g->n);
    free(order);
    return result;
}

// DFS-based cycle detection
static int _dfs_cycle(const Graph *g, int u, int parent, int *visited) {
    visited[u] = 1;
    for (int v = 0; v < g->n; v++) {
        if (!g->adj[u][v]) continue;
        if (!visited[v]) {
            if (_dfs_cycle(g, v, u, visited)) return 1;
        } else if (v != parent) {
            return 1;
        }
    }
    return 0;
}

int graph_has_cycle(const Graph *g) {
    if (!g) return 0;
    int *visited = (int *)calloc(g->n, sizeof(int));
    int result = 0;
    for (int i = 0; i < g->n && !result; i++)
        if (!visited[i])
            result = _dfs_cycle(g, i, -1, visited);
    free(visited);
    return result;
}

// Connected components via BFS
int *graph_connected_components(const Graph *g, int *num_components) {
    if (!g) { *num_components = 0; return NULL; }
    int *comp    = (int *)malloc(g->n * sizeof(int));
    int *visited = (int *)calloc(g->n, sizeof(int));
    int *queue   = (int *)malloc(g->n * sizeof(int));
    if (!comp || !visited || !queue) {
        free(comp); free(visited); free(queue); return NULL;
    }
    int c = 0;
    for (int i = 0; i < g->n; i++) {
        if (!visited[i]) {
            int head = 0, tail = 0;
            queue[tail++] = i;
            visited[i] = 1;
            while (head < tail) {
                int u = queue[head++];
                comp[u] = c;
                for (int v = 0; v < g->n; v++) {
                    if (g->adj[u][v] && !visited[v]) {
                        visited[v] = 1;
                        queue[tail++] = v;
                    }
                }
            }
            c++;
        }
    }
    *num_components = c;
    free(visited); free(queue);
    return comp;
}

// ─── Characteristic Polynomial ───────────────────────────────────────────────
// Computes det(lambda*I - A) using cofactor expansion
// We work with polynomial matrices: matrix of Poly*

// Helper: allocate n x n polynomial matrix
static Poly ***_poly_matrix_alloc(int n) {
    Poly ***M = (Poly ***)malloc(n * sizeof(Poly **));
    for (int i = 0; i < n; i++)
        M[i] = (Poly **)calloc(n, sizeof(Poly *));
    return M;
}

static void _poly_matrix_free(Poly ***M, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            if (M[i][j]) poly_destroy(M[i][j]);
        free(M[i]);
    }
    free(M);
}

// Compute determinant of n x n polynomial matrix via cofactor expansion
static Poly *_poly_matrix_det(Poly ***M, int n) {
    if (n == 1) return poly_copy(M[0][0]);

    Poly *det = poly_create(0);
    det->coeffs[0] = 0.0;

    for (int col = 0; col < n; col++) {
        // Build (n-1) x (n-1) submatrix excluding row 0 and col
        Poly ***sub = _poly_matrix_alloc(n - 1);
        for (int i = 1; i < n; i++) {
            int jj = 0;
            for (int j = 0; j < n; j++) {
                if (j == col) continue;
                sub[i - 1][jj++] = poly_copy(M[i][j]);
            }
        }
        Poly *sub_det = _poly_matrix_det(sub, n - 1);
        _poly_matrix_free(sub, n - 1);

        Poly *term = poly_mul(M[0][col], sub_det);
        poly_destroy(sub_det);

        if (col % 2 == 0) {
            Poly *tmp = poly_add(det, term);
            poly_destroy(det); poly_destroy(term);
            det = tmp;
        } else {
            Poly *tmp = poly_sub(det, term);
            poly_destroy(det); poly_destroy(term);
            det = tmp;
        }
    }
    return det;
}

Poly *graph_characteristic_poly(const Graph *g) {
    if (!g) return NULL;
    int n = g->n;

    // Build matrix M where M[i][j] = lambda - A[i][i] if i==j, else -A[i][j]
    // lambda is represented as polynomial {0, 1} (0 + 1*x)
    Poly ***M = _poly_matrix_alloc(n);
    double lambda_coeffs[] = {0.0, 1.0};

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {
                // lambda - A[i][i]
                Poly *lam = poly_from_array(lambda_coeffs, 1);
                if (g->adj[i][i]) {
                    double c[] = {(double)g->adj[i][i]};
                    Poly *aij = poly_from_array(c, 0);
                    M[i][j] = poly_sub(lam, aij);
                    poly_destroy(aij);
                    poly_destroy(lam);
                } else {
                    M[i][j] = lam;
                }
            } else {
                // -A[i][j]
                double c[] = {-(double)g->adj[i][j]};
                M[i][j] = poly_from_array(c, 0);
            }
        }
    }

    Poly *result = _poly_matrix_det(M, n);
    _poly_matrix_free(M, n);
    return result;
}

// ─── Chromatic Polynomial ────────────────────────────────────────────────────
// Deletion-contraction: P(G,k) = P(G-e, k) - P(G/e, k)
// Base cases: empty graph on n vertices => k^n
//             complete graph K_n => k*(k-1)*...*(k-n+1)

// Check if graph is complete
static int _is_complete(const Graph *g) {
    for (int i = 0; i < g->n; i++)
        for (int j = 0; j < g->n; j++)
            if (i != j && !g->adj[i][j]) return 0;
    return 1;
}

// Find first edge (u,v) with u < v
static int _first_edge(const Graph *g, int *u, int *v) {
    for (int i = 0; i < g->n; i++)
        for (int j = i + 1; j < g->n; j++)
            if (g->adj[i][j]) { *u = i; *v = j; return 1; }
    return 0;
}

// Delete edge (u,v) from graph (returns new graph)
static Graph *_graph_delete_edge(const Graph *g, int u, int v) {
    Graph *h = graph_create(g->n);
    for (int i = 0; i < g->n; i++)
        for (int j = 0; j < g->n; j++)
            h->adj[i][j] = g->adj[i][j];
    h->adj[u][v] = 0;
    h->adj[v][u] = 0;
    return h;
}

// Contract edge (u,v): merge v into u, remove self-loops
static Graph *_graph_contract_edge(const Graph *g, int u, int v) {
    // New graph has n-1 vertices; vertex v is merged into u
    // Vertices > v are shifted down by 1
    int n = g->n - 1;
    Graph *h = graph_create(n);
    for (int i = 0; i < g->n; i++) {
        if (i == v) continue;
        int ni = i > v ? i - 1 : i;
        for (int j = 0; j < g->n; j++) {
            if (j == v) continue;
            int nj = j > v ? j - 1 : j;
            // merge: if original had edge to v, redirect to u
            int src = (i == u) ? (g->adj[i][j] || g->adj[v][j]) : g->adj[i][j];
            if (ni != nj && src)
                h->adj[ni][nj] = 1;
        }
    }
    return h;
}

// k^n as polynomial
static Poly *_k_power_n(int n) {
    // (x)^n via repeated multiplication
    double base[] = {0.0, 1.0};  // x
    Poly *result = poly_create(0);
    result->coeffs[0] = 1.0;
    Poly *x = poly_from_array(base, 1);
    for (int i = 0; i < n; i++) {
        Poly *tmp = poly_mul(result, x);
        poly_destroy(result);
        result = tmp;
    }
    poly_destroy(x);
    return result;
}

// k*(k-1)*...*(k-n+1) as polynomial (falling factorial)
static Poly *_falling_factorial(int n) {
    Poly *result = poly_create(0);
    result->coeffs[0] = 1.0;
    for (int i = 0; i < n; i++) {
        // multiply by (x - i)
        double c[] = {(double)(-i), 1.0};
        Poly *factor = poly_from_array(c, 1);
        Poly *tmp = poly_mul(result, factor);
        poly_destroy(result);
        poly_destroy(factor);
        result = tmp;
    }
    return result;
}

Poly *graph_chromatic_poly(const Graph *g) {
    if (!g) return NULL;

    // Base case 1: no edges => k^n
    if (graph_edge_count(g) == 0)
        return _k_power_n(g->n);

    // Base case 2: complete graph => k*(k-1)*...*(k-n+1)
    if (_is_complete(g))
        return _falling_factorial(g->n);

    // Deletion-contraction
    int u, v;
    _first_edge(g, &u, &v);

    Graph *g_del  = _graph_delete_edge(g, u, v);
    Graph *g_con  = _graph_contract_edge(g, u, v);

    Poly *p_del = graph_chromatic_poly(g_del);
    Poly *p_con = graph_chromatic_poly(g_con);

    Poly *result = poly_sub(p_del, p_con);

    graph_destroy(g_del);
    graph_destroy(g_con);
    poly_destroy(p_del);
    poly_destroy(p_con);

    return result;
}

int graph_chromatic_number(const Graph *g) {
    if (!g) return 0;
    Poly *cp = graph_chromatic_poly(g);
    if (!cp) return 0;
    // Find smallest k >= 1 such that P(G, k) > 0
    int chi = 1;
    for (chi = 1; chi <= g->n + 1; chi++) {
        double val = poly_eval_horner(cp, (double)chi);
        if (val > 0.5) break;
    }
    poly_destroy(cp);
    return chi;
}

// ─── Matching Polynomial ─────────────────────────────────────────────────────
// Count k-matchings using inclusion over edge subsets (for small graphs)
// m(G, k) = number of ways to choose k independent edges

int graph_matching_count(const Graph *g, int k) {
    if (!g || k < 0) return 0;
    if (k == 0) return 1;

    int e = graph_edge_count(g);
    if (k > e || k > g->n / 2) return 0;

    // Collect all edges
    int (*edges)[2] = malloc(e * sizeof(*edges));
    int idx = 0;
    for (int i = 0; i < g->n; i++)
        for (int j = i + 1; j < g->n; j++)
            if (g->adj[i][j]) {
                edges[idx][0] = i;
                edges[idx][1] = j;
                idx++;
            }

    // Enumerate all subsets of size k
    int count = 0;
    // Use bitmask over edges (works for e <= 30)
    if (e <= 30) {
        for (int mask = 0; mask < (1 << e); mask++) {
            if (__builtin_popcount(mask) != k) continue;
            // Check if selected edges form a matching (no shared vertex)
            int used[64] = {0};
            int valid = 1;
            for (int b = 0; b < e && valid; b++) {
                if (!(mask & (1 << b))) continue;
                int u = edges[b][0], v = edges[b][1];
                if (used[u] || used[v]) valid = 0;
                used[u] = used[v] = 1;
            }
            if (valid) count++;
        }
    }
    free(edges);
    return count;
}

Poly *graph_matching_poly(const Graph *g) {
    if (!g) return NULL;
    int n = g->n;
    int max_k = n / 2;

    // mu(G, x) = sum_{k=0}^{max_k} (-1)^k * m(G,k) * x^(n-2k)
    Poly *result = poly_create(n);
    for (int k = 0; k <= max_k; k++) {
        int mk = graph_matching_count(g, k);
        double coeff = (k % 2 == 0) ? (double)mk : -(double)mk;
        result->coeffs[n - 2 * k] += coeff;
    }
    return poly_normalize(result);
}

// ─── Modular Polynomial Arithmetic ───────────────────────────────────────────

static int _mod(int a, int p) {
    return ((a % p) + p) % p;
}

Poly *poly_mod_p(const Poly *p, int prime) {
    if (!p || prime <= 0) return NULL;
    Poly *res = poly_copy(p);
    for (int i = 0; i <= res->degree; i++) {
        long c = (long)round(res->coeffs[i]);
        res->coeffs[i] = (double)_mod((int)c, prime);
    }
    return poly_normalize(res);
}

Poly *poly_mul_mod_p(const Poly *a, const Poly *b, int prime) {
    Poly *prod = poly_mul(a, b);
    Poly *res  = poly_mod_p(prod, prime);
    poly_destroy(prod);
    return res;
}

// Irreducibility test over Z_p using trial division by all polys of lower degree
int poly_is_irreducible_zp(const Poly *p, int prime) {
    if (!p || prime <= 0) return 0;
    if (p->degree <= 0) return 0;
    if (p->degree == 1) return 1;  // linear polynomials are always irreducible

    // Check if p has any root in Z_p
    for (int x = 0; x < prime; x++) {
        double val = poly_eval_horner(p, (double)x);
        long ival = (long)round(val);
        if (_mod((int)ival, prime) == 0) return 0;  // root found => reducible
    }

    // For degree > 2, also check divisibility by all monic polys of degree 2..deg/2
    // (simplified check for small primes and degrees)
    if (p->degree >= 4) {
        // Build all monic polynomials of degree 2 over Z_p and try division
        for (int a0 = 0; a0 < prime; a0++) {
            for (int a1 = 0; a1 < prime; a1++) {
                double c[] = {(double)a0, (double)a1, 1.0};
                Poly *d = poly_from_array(c, 2);
                Poly *rem = poly_mod(p, d);
                Poly *rem_mod = poly_mod_p(rem, prime);
                int is_zero = poly_is_zero(rem_mod);
                poly_destroy(d); poly_destroy(rem); poly_destroy(rem_mod);
                if (is_zero) return 0;
            }
        }
    }
    return 1;
}

// Cyclotomic polynomial Phi_n(x)
// Phi_n(x) = product of (x - e^{2*pi*i*k/n}) for gcd(k,n)=1
// Computed via: x^n - 1 = product of Phi_d(x) for d | n
// => Phi_n(x) = (x^n - 1) / product of Phi_d(x) for d | n, d < n
static int _gcd(int a, int b) { return b == 0 ? a : _gcd(b, a % b); }

Poly *poly_cyclotomic(int n) {
    if (n <= 0) return NULL;

    // Build x^n - 1
    Poly *xn_minus_1 = poly_create(n);
    xn_minus_1->coeffs[n] =  1.0;
    xn_minus_1->coeffs[0] = -1.0;

    if (n == 1) return xn_minus_1;

    // Divide by Phi_d for all proper divisors d of n
    Poly *result = xn_minus_1;
    for (int d = 1; d < n; d++) {
        if (n % d != 0) continue;
        Poly *phi_d = poly_cyclotomic(d);
        Poly **dm   = poly_divmod(result, phi_d);
        poly_destroy(phi_d);
        if (dm) {
            poly_destroy(result);
            poly_destroy(dm[1]);
            result = dm[0];
            free(dm);
        }
    }
    return poly_normalize(result);
}

long poly_hash(const Poly *p, long x, long m) {
    if (!p || m <= 0) return 0;
    // Horner's method mod m
    long result = (long)round(p->coeffs[p->degree]) % m;
    for (int i = p->degree - 1; i >= 0; i--) {
        result = (result * x + (long)round(p->coeffs[i])) % m;
        if (result < 0) result += m;
    }
    return result;
}

// ─── File I/O for Graphs ─────────────────────────────────────────────────────

int graph_save(const Graph *g, const char *filename) {
    if (!g || !filename) return -1;
    FILE *f = fopen(filename, "w");
    if (!f) return -1;
    fprintf(f, "%d\n", g->n);
    for (int i = 0; i < g->n; i++) {
        for (int j = 0; j < g->n; j++)
            fprintf(f, "%d ", g->adj[i][j]);
        fprintf(f, "\n");
    }
    fclose(f);
    return 0;
}

Graph *graph_load(const char *filename) {
    if (!filename) return NULL;
    FILE *f = fopen(filename, "r");
    if (!f) return NULL;
    int n;
    if (fscanf(f, "%d", &n) != 1 || n <= 0) { fclose(f); return NULL; }
    Graph *g = graph_create(n);
    if (!g) { fclose(f); return NULL; }
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            fscanf(f, "%d", &g->adj[i][j]);
    fclose(f);
    return g;
}
