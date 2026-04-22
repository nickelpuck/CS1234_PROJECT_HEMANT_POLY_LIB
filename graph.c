#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ─── Create / Destroy ─────────────────────────────────────────────────────────

Graph *graph_create(int n) {
    if (n <= 0) return NULL;
    Graph *g = (Graph *)malloc(sizeof(Graph));
    if (!g) return NULL;
    g->n = n;
    g->adj    = (int  **)calloc(n, sizeof(int  *));
    g->labels = (char **)calloc(n, sizeof(char *));
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
    g->adj[u][v] = g->adj[v][u] = 1;
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
        for (int j = 0; j < g->n; j++) printf("%3d", g->adj[i][j]);
        printf("\n");
    }
}

// ─── Graph Properties ─────────────────────────────────────────────────────────

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

int *graph_bfs_order(const Graph *g, int start, int *count) {
    if (!g || start < 0 || start >= g->n) return NULL;
    int *order   = (int *)malloc(g->n * sizeof(int));
    int *visited = (int *)calloc(g->n, sizeof(int));
    int *queue   = (int *)malloc(g->n * sizeof(int));
    int head = 0, tail = 0, idx = 0;
    queue[tail++] = start;
    visited[start] = 1;
    while (head < tail) {
        int u = queue[head++];
        order[idx++] = u;
        for (int v = 0; v < g->n; v++)
            if (g->adj[u][v] && !visited[v]) {
                visited[v] = 1;
                queue[tail++] = v;
            }
    }
    *count = idx;
    free(visited); free(queue);
    return order;
}

int *graph_dfs_order(const Graph *g, int start, int *count) {
    if (!g || start < 0 || start >= g->n) return NULL;
    int *order   = (int *)malloc(g->n * sizeof(int));
    int *visited = (int *)calloc(g->n, sizeof(int));
    int *stack   = (int *)malloc(g->n * sizeof(int));
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

static int _dfs_cycle(const Graph *g, int u, int parent, int *visited) {
    visited[u] = 1;
    for (int v = 0; v < g->n; v++) {
        if (!g->adj[u][v]) continue;
        if (!visited[v]) {
            if (_dfs_cycle(g, v, u, visited)) return 1;
        } else if (v != parent) return 1;
    }
    return 0;
}

int graph_has_cycle(const Graph *g) {
    if (!g) return 0;
    int *visited = (int *)calloc(g->n, sizeof(int));
    int result = 0;
    for (int i = 0; i < g->n && !result; i++)
        if (!visited[i]) result = _dfs_cycle(g, i, -1, visited);
    free(visited);
    return result;
}

int *graph_connected_components(const Graph *g, int *num_components) {
    if (!g) { *num_components = 0; return NULL; }
    int *comp    = (int *)malloc(g->n * sizeof(int));
    int *visited = (int *)calloc(g->n, sizeof(int));
    int *queue   = (int *)malloc(g->n * sizeof(int));
    int c = 0;
    for (int i = 0; i < g->n; i++) {
        if (!visited[i]) {
            int head = 0, tail = 0;
            queue[tail++] = i; visited[i] = 1;
            while (head < tail) {
                int u = queue[head++];
                comp[u] = c;
                for (int v = 0; v < g->n; v++)
                    if (g->adj[u][v] && !visited[v]) {
                        visited[v] = 1; queue[tail++] = v;
                    }
            }
            c++;
        }
    }
    *num_components = c;
    free(visited); free(queue);
    return comp;
}

// ─── Characteristic Polynomial ────────────────────────────────────────────────

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

static Poly *_poly_matrix_det(Poly ***M, int n) {
    if (n == 1) return poly_copy(M[0][0]);
    Poly *det = poly_create(0);
    det->coeffs[0] = 0.0;
    for (int col = 0; col < n; col++) {
        Poly ***sub = _poly_matrix_alloc(n - 1);
        for (int i = 1; i < n; i++) {
            int jj = 0;
            for (int j = 0; j < n; j++) {
                if (j == col) continue;
                sub[i-1][jj++] = poly_copy(M[i][j]);
            }
        }
        Poly *sub_det = _poly_matrix_det(sub, n - 1);
        _poly_matrix_free(sub, n - 1);
        Poly *term = poly_mul(M[0][col], sub_det);
        poly_destroy(sub_det);
        if (col % 2 == 0) {
            Poly *tmp = poly_add(det, term); poly_destroy(det); poly_destroy(term); det = tmp;
        } else {
            Poly *tmp = poly_sub(det, term); poly_destroy(det); poly_destroy(term); det = tmp;
        }
    }
    return det;
}

Poly *graph_characteristic_poly(const Graph *g) {
    if (!g) return NULL;
    int n = g->n;
    Poly ***M = _poly_matrix_alloc(n);
    double lc[] = {0.0, 1.0};
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {
                Poly *lam = poly_from_array(lc, 1);
                if (g->adj[i][i]) {
                    double c[] = {(double)g->adj[i][i]};
                    Poly *a = poly_from_array(c, 0);
                    M[i][j] = poly_sub(lam, a);
                    poly_destroy(a); poly_destroy(lam);
                } else M[i][j] = lam;
            } else {
                double c[] = {-(double)g->adj[i][j]};
                M[i][j] = poly_from_array(c, 0);
            }
        }
    }
    Poly *result = _poly_matrix_det(M, n);
    _poly_matrix_free(M, n);
    return result;
}

// ─── Spectral Graph Theory ────────────────────────────────────────────────────

// Find real roots of characteristic poly using Newton's method with multiple starts
double *graph_spectrum(const Graph *g, int *count) {
    if (!g) { *count = 0; return NULL; }
    Poly *cp = graph_characteristic_poly(g);
    if (!cp) { *count = 0; return NULL; }

    int n = g->n;
    double *eigenvalues = (double *)malloc(n * sizeof(double));
    int found = 0;

    // Try multiple starting points to find all real roots
    double starts[] = {-10,-5,-3,-2,-1,0,1,2,3,5,10};
    int nstarts = 11;
    for (int s = 0; s < nstarts && found < n; s++) {
        double root = poly_newton_root(cp, starts[s], 200, 1e-10);
        if (fabs(poly_eval_horner(cp, root)) > 1e-4) continue;
        // Check not duplicate
        int dup = 0;
        for (int i = 0; i < found; i++)
            if (fabs(eigenvalues[i] - root) < 1e-6) { dup = 1; break; }
        if (!dup) eigenvalues[found++] = root;
    }
    // Sort ascending
    for (int i = 0; i < found - 1; i++)
        for (int j = i + 1; j < found; j++)
            if (eigenvalues[i] > eigenvalues[j]) {
                double tmp = eigenvalues[i];
                eigenvalues[i] = eigenvalues[j];
                eigenvalues[j] = tmp;
            }
    *count = found;
    poly_destroy(cp);
    return eigenvalues;
}

// Laplacian matrix: L = D - A, algebraic connectivity = 2nd smallest eigenvalue
double graph_algebraic_connectivity(const Graph *g) {
    if (!g) return 0.0;
    // Build Laplacian as polynomial matrix and find its char poly
    int n = g->n;
    Poly ***M = _poly_matrix_alloc(n);
    double lc[] = {0.0, 1.0};
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {
                // lambda - degree(i)
                Poly *lam = poly_from_array(lc, 1);
                double c[] = {(double)graph_degree(g, i)};
                Poly *d = poly_from_array(c, 0);
                M[i][j] = poly_sub(lam, d);
                poly_destroy(d); poly_destroy(lam);
            } else {
                double c[] = {(double)g->adj[i][j]};
                M[i][j] = poly_from_array(c, 0);
            }
        }
    }
    Poly *lap_cp = _poly_matrix_det(M, n);
    _poly_matrix_free(M, n);

    // Find eigenvalues of Laplacian
    double best = 1e18;
    double starts[] = {-1, 0, 0.1, 0.5, 1, 2, 3, 5};
    int nstarts = 8;
    for (int s = 0; s < nstarts; s++) {
        double root = poly_newton_root(lap_cp, starts[s], 200, 1e-10);
        if (fabs(poly_eval_horner(lap_cp, root)) > 1e-4) continue;
        if (root > 1e-6 && root < best) best = root;
    }
    poly_destroy(lap_cp);
    return best > 1e17 ? 0.0 : best;
}

int graph_is_cospectral(const Graph *g, const Graph *h) {
    if (!g || !h || g->n != h->n) return 0;
    Poly *cpg = graph_characteristic_poly(g);
    Poly *cph = graph_characteristic_poly(h);
    int result = poly_equals(cpg, cph);
    poly_destroy(cpg); poly_destroy(cph);
    return result;
}

double graph_spectral_gap(const Graph *g) {
    int count;
    double *spec = graph_spectrum(g, &count);
    if (!spec || count < 2) { free(spec); return 0.0; }
    // Sort descending, gap = spec[0] - spec[1]
    double gap = spec[count-1] - spec[count-2];
    free(spec);
    return gap;
}

// ─── Chromatic Polynomial ─────────────────────────────────────────────────────

static int _is_complete(const Graph *g) {
    for (int i = 0; i < g->n; i++)
        for (int j = 0; j < g->n; j++)
            if (i != j && !g->adj[i][j]) return 0;
    return 1;
}

static int _first_edge(const Graph *g, int *u, int *v) {
    for (int i = 0; i < g->n; i++)
        for (int j = i+1; j < g->n; j++)
            if (g->adj[i][j]) { *u = i; *v = j; return 1; }
    return 0;
}

static Graph *_graph_delete_edge(const Graph *g, int u, int v) {
    Graph *h = graph_create(g->n);
    for (int i = 0; i < g->n; i++)
        for (int j = 0; j < g->n; j++)
            h->adj[i][j] = g->adj[i][j];
    h->adj[u][v] = h->adj[v][u] = 0;
    return h;
}

static Graph *_graph_contract_edge(const Graph *g, int u, int v) {
    int n = g->n - 1;
    Graph *h = graph_create(n);
    for (int i = 0; i < g->n; i++) {
        if (i == v) continue;
        int ni = i > v ? i-1 : i;
        for (int j = 0; j < g->n; j++) {
            if (j == v) continue;
            int nj = j > v ? j-1 : j;
            int src = (i == u) ? (g->adj[i][j] || g->adj[v][j]) : g->adj[i][j];
            if (ni != nj && src) h->adj[ni][nj] = 1;
        }
    }
    return h;
}

static Poly *_k_power_n(int n) {
    double base[] = {0.0, 1.0};
    Poly *result = poly_create(0); result->coeffs[0] = 1.0;
    Poly *x = poly_from_array(base, 1);
    for (int i = 0; i < n; i++) {
        Poly *tmp = poly_mul(result, x); poly_destroy(result); result = tmp;
    }
    poly_destroy(x);
    return result;
}

static Poly *_falling_factorial(int n) {
    Poly *result = poly_create(0); result->coeffs[0] = 1.0;
    for (int i = 0; i < n; i++) {
        double c[] = {(double)(-i), 1.0};
        Poly *factor = poly_from_array(c, 1);
        Poly *tmp = poly_mul(result, factor);
        poly_destroy(result); poly_destroy(factor); result = tmp;
    }
    return result;
}

Poly *graph_chromatic_poly(const Graph *g) {
    if (!g) return NULL;
    if (graph_edge_count(g) == 0) return _k_power_n(g->n);
    if (_is_complete(g)) return _falling_factorial(g->n);
    int u, v;
    _first_edge(g, &u, &v);
    Graph *g_del = _graph_delete_edge(g, u, v);
    Graph *g_con = _graph_contract_edge(g, u, v);
    Poly *p_del = graph_chromatic_poly(g_del);
    Poly *p_con = graph_chromatic_poly(g_con);
    Poly *result = poly_sub(p_del, p_con);
    graph_destroy(g_del); graph_destroy(g_con);
    poly_destroy(p_del); poly_destroy(p_con);
    return result;
}

int graph_chromatic_number(const Graph *g) {
    if (!g) return 0;
    Poly *cp = graph_chromatic_poly(g);
    if (!cp) return 0;
    int chi;
    for (chi = 1; chi <= g->n + 1; chi++)
        if (poly_eval_horner(cp, (double)chi) > 0.5) break;
    poly_destroy(cp);
    return chi;
}

// ─── Matching Polynomial ──────────────────────────────────────────────────────

int graph_matching_count(const Graph *g, int k) {
    if (!g || k < 0) return 0;
    if (k == 0) return 1;
    int e = graph_edge_count(g);
    if (k > e || k > g->n / 2) return 0;
    int (*edges)[2] = malloc(e * sizeof(*edges));
    int idx = 0;
    for (int i = 0; i < g->n; i++)
        for (int j = i+1; j < g->n; j++)
            if (g->adj[i][j]) { edges[idx][0]=i; edges[idx][1]=j; idx++; }
    int count = 0;
    if (e <= 30) {
        for (int mask = 0; mask < (1 << e); mask++) {
            if (__builtin_popcount(mask) != k) continue;
            int used[64] = {0}; int valid = 1;
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
    int n = g->n, max_k = n / 2;
    Poly *result = poly_create(n);
    for (int k = 0; k <= max_k; k++) {
        int mk = graph_matching_count(g, k);
        result->coeffs[n - 2*k] += (k % 2 == 0) ? (double)mk : -(double)mk;
    }
    return poly_normalize(result);
}

// ─── Tutte Polynomial ─────────────────────────────────────────────────────────

TuttePoly *tutte_create(int max_deg) {
    TuttePoly *t = (TuttePoly *)malloc(sizeof(TuttePoly));
    t->max_deg = max_deg;
    t->coeffs = (double **)calloc(max_deg + 1, sizeof(double *));
    for (int i = 0; i <= max_deg; i++)
        t->coeffs[i] = (double *)calloc(max_deg + 1, sizeof(double));
    return t;
}

void tutte_destroy(TuttePoly *t) {
    if (!t) return;
    for (int i = 0; i <= t->max_deg; i++) free(t->coeffs[i]);
    free(t->coeffs); free(t);
}

void tutte_add_term(TuttePoly *t, int i, int j, double val) {
    if (!t || i > t->max_deg || j > t->max_deg) return;
    t->coeffs[i][j] += val;
}

double tutte_eval(const TuttePoly *t, double x, double y) {
    if (!t) return 0.0;
    double result = 0.0;
    double xpow = 1.0;
    for (int i = 0; i <= t->max_deg; i++) {
        double ypow = 1.0;
        for (int j = 0; j <= t->max_deg; j++) {
            result += t->coeffs[i][j] * xpow * ypow;
            ypow *= y;
        }
        xpow *= x;
    }
    return result;
}

void tutte_print(const TuttePoly *t) {
    if (!t) return;
    int printed = 0;
    for (int i = t->max_deg; i >= 0; i--) {
        for (int j = t->max_deg; j >= 0; j--) {
            double c = t->coeffs[i][j];
            if (fabs(c) < 1e-9) continue;
            if (printed) printf(c < 0 ? " - " : " + ");
            else if (c < 0) printf("-");
            double ac = fabs(c);
            if (fabs(ac - 1.0) > 1e-9) printf("%.4g", ac);
            if (i > 0 && j > 0) printf("x^%dy^%d", i, j);
            else if (i > 0) printf("x^%d", i);
            else if (j > 0) printf("y^%d", j);
            else if (fabs(ac - 1.0) <= 1e-9) printf("1");
            printed = 1;
        }
    }
    if (!printed) printf("0");
    printf("\n");
}

TuttePoly *tutte_add(const TuttePoly *a, const TuttePoly *b) {
    if (!a || !b) return NULL;
    int md = a->max_deg > b->max_deg ? a->max_deg : b->max_deg;
    TuttePoly *res = tutte_create(md);
    for (int i = 0; i <= a->max_deg; i++)
        for (int j = 0; j <= a->max_deg; j++)
            res->coeffs[i][j] += a->coeffs[i][j];
    for (int i = 0; i <= b->max_deg; i++)
        for (int j = 0; j <= b->max_deg; j++)
            res->coeffs[i][j] += b->coeffs[i][j];
    return res;
}

// Tutte polynomial via deletion-contraction
// T(G) = T(G-e) + T(G/e)  for non-loop, non-bridge e
// T(G) = y * T(G-e)        for loop e
// T(G) = x * T(G/e)        for bridge e
static int _is_bridge(const Graph *g, int u, int v) {
    // Remove edge, check connectivity
    Graph *h = _graph_delete_edge(g, u, v);
    int connected = graph_is_connected(h);
    graph_destroy(h);
    return !connected;
}

TuttePoly *graph_tutte_poly(const Graph *g) {
    if (!g) return NULL;
    int e = graph_edge_count(g);
    int md = e + 2;

    // Base case: no edges
    if (e == 0) {
        TuttePoly *t = tutte_create(md);
        tutte_add_term(t, 0, 0, 1.0);  // T = 1
        return t;
    }

    // Find first edge
    int u = -1, v = -1;
    _first_edge(g, &u, &v);

    if (_is_bridge(g, u, v)) {
        // T(G) = x * T(G/e)
        Graph *g_con = _graph_contract_edge(g, u, v);
        TuttePoly *t_con = graph_tutte_poly(g_con);
        graph_destroy(g_con);
        // Multiply by x: shift all x-degrees up by 1
        TuttePoly *result = tutte_create(t_con->max_deg + 1);
        for (int i = 0; i <= t_con->max_deg; i++)
            for (int j = 0; j <= t_con->max_deg; j++)
                if (i + 1 <= result->max_deg)
                    result->coeffs[i+1][j] += t_con->coeffs[i][j];
        tutte_destroy(t_con);
        return result;
    } else {
        // Regular edge: T(G) = T(G-e) + T(G/e)
        Graph *g_del = _graph_delete_edge(g, u, v);
        Graph *g_con = _graph_contract_edge(g, u, v);
        TuttePoly *t_del = graph_tutte_poly(g_del);
        TuttePoly *t_con = graph_tutte_poly(g_con);
        graph_destroy(g_del); graph_destroy(g_con);
        TuttePoly *result = tutte_add(t_del, t_con);
        tutte_destroy(t_del); tutte_destroy(t_con);
        return result;
    }
}

long graph_spanning_trees(const Graph *g) {
    TuttePoly *t = graph_tutte_poly(g);
    if (!t) return 0;
    long result = (long)round(tutte_eval(t, 1.0, 1.0));
    tutte_destroy(t);
    return result;
}

long graph_acyclic_orientations(const Graph *g) {
    TuttePoly *t = graph_tutte_poly(g);
    if (!t) return 0;
    long result = (long)round(tutte_eval(t, 2.0, 0.0));
    tutte_destroy(t);
    return result;
}

// ─── Reliability Polynomial ───────────────────────────────────────────────────
// R(G, p) = sum over all spanning subgraphs H that are connected:
//           p^|E(H)| * (1-p)^(|E(G)| - |E(H)|)
// We enumerate all edge subsets (works for small graphs)

static int _subgraph_connected(const Graph *g, int mask, int *edges, int e) {
    // Build subgraph from edge mask and check connectivity
    int n = g->n;
    int **adj = (int **)calloc(n, sizeof(int *));
    for (int i = 0; i < n; i++) adj[i] = (int *)calloc(n, sizeof(int));
    // edges stored as pairs
    int idx = 0;
    for (int i = 0; i < g->n; i++)
        for (int j = i+1; j < g->n; j++)
            if (g->adj[i][j]) {
                if (mask & (1 << idx)) { adj[i][j] = adj[j][i] = 1; }
                idx++;
            }
    // BFS
    int *visited = (int *)calloc(n, sizeof(int));
    int *queue   = (int *)malloc(n * sizeof(int));
    int head = 0, tail = 0, count = 0;
    queue[tail++] = 0; visited[0] = 1;
    while (head < tail) {
        int u = queue[head++]; count++;
        for (int v = 0; v < n; v++)
            if (adj[u][v] && !visited[v]) { visited[v]=1; queue[tail++]=v; }
    }
    for (int i = 0; i < n; i++) free(adj[i]);
    free(adj); free(visited); free(queue);
    return count == n;
}

Poly *graph_reliability_poly(const Graph *g) {
    if (!g) return NULL;
    int e = graph_edge_count(g);
    if (e > 20) return NULL;  // too large for enumeration

    // R(G,p) = sum_{k=0}^{e} c_k * p^k * (1-p)^(e-k)
    // where c_k = number of connected spanning subgraphs with exactly k edges
    int *ck = (int *)calloc(e + 1, sizeof(int));
    for (int mask = 0; mask < (1 << e); mask++) {
        int k = __builtin_popcount(mask);
        if (_subgraph_connected(g, mask, NULL, e))
            ck[k]++;
    }

    // Build polynomial in p:
    // R(p) = sum_k ck * p^k * (1-p)^(e-k)
    // Expand (1-p)^(e-k) = sum_j C(e-k,j)*(-1)^j * p^j
    // So coeff of p^m = sum_{k=0}^{m} ck * C(e-k, m-k) * (-1)^(m-k)
    Poly *result = poly_create(e);
    // Precompute binomial coefficients
    long **C = (long **)malloc((e+2) * sizeof(long *));
    for (int i = 0; i <= e+1; i++) {
        C[i] = (long *)calloc(e+2, sizeof(long));
        C[i][0] = 1;
        for (int j = 1; j <= i; j++) C[i][j] = C[i-1][j-1] + C[i-1][j];
    }
    for (int m = 0; m <= e; m++) {
        double coeff = 0.0;
        for (int k = 0; k <= m; k++) {
            int rem = e - k;
            int pick = m - k;
            if (pick < 0 || pick > rem) continue;
            double sign = ((m - k) % 2 == 0) ? 1.0 : -1.0;
            coeff += ck[k] * sign * (double)C[rem][pick];
        }
        result->coeffs[m] = coeff;
    }
    for (int i = 0; i <= e+1; i++) free(C[i]);
    free(C); free(ck);
    return poly_normalize(result);
}

double graph_reliability_eval(const Graph *g, double p) {
    Poly *rp = graph_reliability_poly(g);
    if (!rp) return 0.0;
    double val = poly_eval_horner(rp, p);
    poly_destroy(rp);
    return val;
}

// ─── File I/O ─────────────────────────────────────────────────────────────────

int graph_save(const Graph *g, const char *filename) {
    if (!g || !filename) return -1;
    FILE *f = fopen(filename, "w");
    if (!f) return -1;
    fprintf(f, "%d\n", g->n);
    for (int i = 0; i < g->n; i++) {
        for (int j = 0; j < g->n; j++) fprintf(f, "%d ", g->adj[i][j]);
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
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            fscanf(f, "%d", &g->adj[i][j]);
    fclose(f);
    return g;
}
