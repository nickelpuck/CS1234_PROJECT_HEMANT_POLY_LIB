#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "poly.h"
#include "graph.h"

void print_separator() {
    printf("\n========================================\n");
}

// ─── Polynomial Demos ────────────────────────────────────────────────────────

void demo_basic() {
    print_separator();
    printf("DEMO 1: Basic Arithmetic\n");
    print_separator();

    double ca[] = {1, 2, 3};   // 3x^2 + 2x + 1
    Poly *a = poly_from_array(ca, 2);
    printf("A = "); poly_print(a);

    double cb[] = {4, -1, 1};  // x^2 - x + 4
    Poly *b = poly_from_array(cb, 2);
    printf("B = "); poly_print(b);

    Poly *sum  = poly_add(a, b);
    Poly *diff = poly_sub(a, b);
    Poly *prod = poly_mul(a, b);
    Poly *neg  = poly_negate(a);
    Poly *sc   = poly_scalar_mul(a, 2.5);

    printf("A + B = "); poly_print(sum);
    printf("A - B = "); poly_print(diff);
    printf("A * B = "); poly_print(prod);
    printf("-A    = "); poly_print(neg);
    printf("2.5*A = "); poly_print(sc);

    poly_destroy(a); poly_destroy(b); poly_destroy(sum);
    poly_destroy(diff); poly_destroy(prod); poly_destroy(neg); poly_destroy(sc);
}

void demo_eval() {
    print_separator();
    printf("DEMO 2: Evaluation (Direct & Horner's Method)\n");
    print_separator();

    double ca[] = {1, 2, 3};  // 3x^2 + 2x + 1
    Poly *p = poly_from_array(ca, 2);
    printf("P = "); poly_print(p);

    for (double x = -2.0; x <= 2.0; x += 1.0)
        printf("  P(%.1f) = %.4g  [Horner: %.4g]\n",
               x, poly_eval(p, x), poly_eval_horner(p, x));
    poly_destroy(p);
}

void demo_calculus() {
    print_separator();
    printf("DEMO 3: Differentiation & Integration\n");
    print_separator();

    double ca[] = {1, 2, 3, 4};  // 4x^3 + 3x^2 + 2x + 1
    Poly *p = poly_from_array(ca, 3);
    printf("P        = "); poly_print(p);

    Poly *dp  = poly_differentiate(p);
    Poly *ddp = poly_differentiate(dp);
    Poly *ip  = poly_integrate(p, 0.0);

    printf("P'       = "); poly_print(dp);
    printf("P''      = "); poly_print(ddp);
    printf("integral = "); poly_print(ip);

    poly_destroy(p); poly_destroy(dp); poly_destroy(ddp); poly_destroy(ip);
}

void demo_divmod() {
    print_separator();
    printf("DEMO 4: Polynomial Long Division\n");
    print_separator();

    double ca[] = {-4, 0, -2, 1};  // x^3 - 2x^2 - 4
    Poly *a = poly_from_array(ca, 3);
    double cb[] = {-3, 1};          // x - 3
    Poly *b = poly_from_array(cb, 1);

    printf("A = "); poly_print(a);
    printf("B = "); poly_print(b);

    Poly **dm = poly_divmod(a, b);
    if (dm) {
        printf("A / B = "); poly_print(dm[0]);
        printf("A %% B = "); poly_print(dm[1]);
        poly_destroy(dm[0]); poly_destroy(dm[1]); free(dm);
    }
    poly_destroy(a); poly_destroy(b);
}

void demo_gcd() {
    print_separator();
    printf("DEMO 5: GCD of Polynomials (Euclidean Algorithm)\n");
    print_separator();

    double ca[] = {-1, 0, 1};   // x^2 - 1 = (x-1)(x+1)
    Poly *a = poly_from_array(ca, 2);
    double cb[] = {2, -3, 1};   // x^2 - 3x + 2 = (x-1)(x-2)
    Poly *b = poly_from_array(cb, 2);

    printf("A   = "); poly_print(a);
    printf("B   = "); poly_print(b);

    Poly *g = poly_gcd(a, b);
    printf("GCD = "); poly_print(g);  // x - 1

    poly_destroy(a); poly_destroy(b); poly_destroy(g);
}

void demo_compose() {
    print_separator();
    printf("DEMO 6: Polynomial Composition f(g(x))\n");
    print_separator();

    double cf[] = {1, 0, 1};  // f = x^2 + 1
    Poly *f = poly_from_array(cf, 2);
    double cg[] = {2, 1};     // g = x + 2
    Poly *g = poly_from_array(cg, 1);

    printf("f(x)    = "); poly_print(f);
    printf("g(x)    = "); poly_print(g);

    Poly *fog = poly_compose(f, g);
    printf("f(g(x)) = "); poly_print(fog);  // x^2 + 4x + 5

    poly_destroy(f); poly_destroy(g); poly_destroy(fog);
}

void demo_roots() {
    print_separator();
    printf("DEMO 7: Root Finding (Quadratic Formula + Newton's Method)\n");
    print_separator();

    double cq[] = {6, -5, 1};  // x^2 - 5x + 6 = (x-2)(x-3)
    Poly *q = poly_from_array(cq, 2);
    printf("Q = "); poly_print(q);

    int count;
    double *roots = poly_roots_real(q, &count);
    printf("Real roots: ");
    for (int i = 0; i < count; i++) printf("%.4g  ", roots[i]);
    printf("\n");
    free(roots);
    poly_destroy(q);

    double cn[] = {-2, -1, 0, 1};  // x^3 - x - 2
    Poly *p = poly_from_array(cn, 3);
    printf("\nP = "); poly_print(p);
    double root = poly_newton_root(p, 1.5, 100, 1e-10);
    printf("Newton root (x0=1.5): %.10g\n", root);
    printf("Verify P(root) = %.2e\n", poly_eval_horner(p, root));
    poly_destroy(p);
}

void demo_fileio() {
    print_separator();
    printf("DEMO 8: File Save & Load (State Persistence)\n");
    print_separator();

    double ca[] = {1, -3, 0, 2};  // 2x^3 - 3x + 1
    Poly *p = poly_from_array(ca, 3);
    printf("Original: "); poly_print(p);

    poly_save(p, "poly_saved.txt");
    printf("Saved to poly_saved.txt\n");

    Poly *loaded = poly_load("poly_saved.txt");
    printf("Loaded:   "); poly_print(loaded);
    printf("Equal: %s\n", poly_equals(p, loaded) ? "YES" : "NO");

    poly_destroy(p); poly_destroy(loaded);
}

// ─── Graph Theory Demos ──────────────────────────────────────────────────────

void demo_graph_basics() {
    print_separator();
    printf("DEMO 9: Graph Construction & Traversal (BFS / DFS)\n");
    print_separator();

    // Build a simple graph: 0-1-2-3, with extra edge 0-2
    //   0 - 1
    //   |\  |
    //   2 - 3
    Graph *g = graph_create(4);
    graph_set_label(g, 0, "A");
    graph_set_label(g, 1, "B");
    graph_set_label(g, 2, "C");
    graph_set_label(g, 3, "D");
    graph_add_edge(g, 0, 1);
    graph_add_edge(g, 0, 2);
    graph_add_edge(g, 1, 3);
    graph_add_edge(g, 2, 3);

    graph_print(g);
    graph_print_matrix(g);

    printf("\nDegrees: ");
    for (int i = 0; i < g->n; i++)
        printf("%s=%d  ", g->labels[i], graph_degree(g, i));
    printf("\n");

    printf("Connected: %s\n", graph_is_connected(g) ? "YES" : "NO");
    printf("Has cycle: %s\n", graph_has_cycle(g) ? "YES" : "NO");

    int count;
    int *bfs = graph_bfs_order(g, 0, &count);
    printf("BFS from A: ");
    for (int i = 0; i < count; i++) printf("%s ", g->labels[bfs[i]]);
    printf("\n");
    free(bfs);

    int *dfs = graph_dfs_order(g, 0, &count);
    printf("DFS from A: ");
    for (int i = 0; i < count; i++) printf("%s ", g->labels[dfs[i]]);
    printf("\n");
    free(dfs);

    graph_destroy(g);
}

void demo_characteristic_poly() {
    print_separator();
    printf("DEMO 10: Characteristic Polynomial of a Graph\n");
    print_separator();

    // Cycle graph C4: 0-1-2-3-0
    Graph *g = graph_create(4);
    graph_set_label(g, 0, "0"); graph_set_label(g, 1, "1");
    graph_set_label(g, 2, "2"); graph_set_label(g, 3, "3");
    graph_add_edge(g, 0, 1);
    graph_add_edge(g, 1, 2);
    graph_add_edge(g, 2, 3);
    graph_add_edge(g, 3, 0);

    printf("Graph: Cycle C4\n");
    graph_print(g);

    Poly *cp = graph_characteristic_poly(g);
    printf("\nCharacteristic polynomial det(lambda*I - A):\n");
    printf("  "); poly_print(cp);
    printf("(Roots are eigenvalues of the adjacency matrix)\n");

    // Verify: eigenvalues of C4 are 2, 0, 0, -2
    printf("\nEigenvalue check (roots of char. poly):\n");
    for (int x = -3; x <= 3; x++) {
        double val = poly_eval_horner(cp, (double)x);
        if (fabs(val) < 1e-6)
            printf("  lambda = %d is an eigenvalue\n", x);
    }

    poly_destroy(cp);
    graph_destroy(g);
}

void demo_chromatic_poly() {
    print_separator();
    printf("DEMO 11: Chromatic Polynomial (Graph Coloring)\n");
    print_separator();

    // Triangle K3
    Graph *k3 = graph_create(3);
    graph_set_label(k3, 0, "A"); graph_set_label(k3, 1, "B"); graph_set_label(k3, 2, "C");
    graph_add_edge(k3, 0, 1);
    graph_add_edge(k3, 1, 2);
    graph_add_edge(k3, 0, 2);

    printf("Graph: Triangle K3\n");
    graph_print(k3);

    Poly *cp = graph_chromatic_poly(k3);
    printf("\nChromatic polynomial P(G, k):\n");
    printf("  "); poly_print(cp);
    printf("  (should be k*(k-1)*(k-2) = k^3 - 3k^2 + 2k)\n");

    printf("\nColorings with k colors:\n");
    for (int k = 1; k <= 5; k++)
        printf("  k=%d: %.0f colorings\n", k, poly_eval_horner(cp, (double)k));

    printf("Chromatic number: %d\n", graph_chromatic_number(k3));
    poly_destroy(cp);
    graph_destroy(k3);

    // Path graph P4: 0-1-2-3
    printf("\nGraph: Path P4\n");
    Graph *p4 = graph_create(4);
    graph_add_edge(p4, 0, 1);
    graph_add_edge(p4, 1, 2);
    graph_add_edge(p4, 2, 3);
    graph_print(p4);

    Poly *cp2 = graph_chromatic_poly(p4);
    printf("Chromatic polynomial: "); poly_print(cp2);
    printf("Chromatic number: %d\n", graph_chromatic_number(p4));
    poly_destroy(cp2);
    graph_destroy(p4);
}

void demo_matching_poly() {
    print_separator();
    printf("DEMO 12: Matching Polynomial\n");
    print_separator();

    // Complete graph K4
    Graph *k4 = graph_create(4);
    for (int i = 0; i < 4; i++)
        for (int j = i + 1; j < 4; j++)
            graph_add_edge(k4, i, j);

    printf("Graph: Complete K4\n");
    graph_print(k4);

    printf("\nMatchings of size k:\n");
    for (int k = 0; k <= 2; k++)
        printf("  m(K4, %d) = %d\n", k, graph_matching_count(k4, k));

    Poly *mp = graph_matching_poly(k4);
    printf("\nMatching polynomial mu(G, x):\n");
    printf("  "); poly_print(mp);

    poly_destroy(mp);
    graph_destroy(k4);
}

void demo_modular_poly() {
    print_separator();
    printf("DEMO 13: Modular Arithmetic & Finite Fields (Z_p)\n");
    print_separator();

    double ca[] = {3, 7, 5, 2};  // 2x^3 + 5x^2 + 7x + 3
    Poly *p = poly_from_array(ca, 3);
    printf("P = "); poly_print(p);

    Poly *pm = poly_mod_p(p, 3);
    printf("P mod 3 = "); poly_print(pm);

    // Check irreducibility over Z_2 and Z_3
    double ci[] = {1, 1, 0, 1};  // x^3 + x + 1 (irreducible over Z_2)
    Poly *irr = poly_from_array(ci, 3);
    printf("\nQ = "); poly_print(irr);
    printf("Q irreducible over Z_2: %s\n",
           poly_is_irreducible_zp(irr, 2) ? "YES" : "NO");
    printf("Q irreducible over Z_3: %s\n",
           poly_is_irreducible_zp(irr, 3) ? "YES" : "NO");

    printf("\nPolynomial hash of P (x=31, m=1e9+7): %ld\n",
           poly_hash(p, 31, 1000000007L));

    poly_destroy(p); poly_destroy(pm); poly_destroy(irr);
}

void demo_cyclotomic() {
    print_separator();
    printf("DEMO 14: Cyclotomic Polynomials Phi_n(x)\n");
    print_separator();

    for (int n = 1; n <= 8; n++) {
        Poly *phi = poly_cyclotomic(n);
        printf("Phi_%d(x) = ", n); poly_print(phi);
        poly_destroy(phi);
    }
}

void demo_graph_fileio() {
    print_separator();
    printf("DEMO 15: Graph File Save & Load\n");
    print_separator();

    Graph *g = graph_create(3);
    graph_add_edge(g, 0, 1);
    graph_add_edge(g, 1, 2);
    graph_add_edge(g, 0, 2);

    printf("Original: "); graph_print(g);
    graph_save(g, "graph_saved.txt");
    printf("Saved to graph_saved.txt\n");

    Graph *loaded = graph_load("graph_saved.txt");
    printf("Loaded:   "); graph_print(loaded);

    graph_destroy(g); graph_destroy(loaded);
}

void cli_mode(int argc, char *argv[]) {
    if (argc < 3) {
        printf("CLI Usage: %s <op> <file1> [file2/value]\n", argv[0]);
        printf("  Poly ops : add sub mul diff integ eval gcd compose\n");
        printf("  Graph ops: ginfo charpoly chrompoly matching\n");
        return;
    }

    char *op = argv[1];

    // Graph operations
    if (strcmp(op, "ginfo") == 0 || strcmp(op, "charpoly") == 0 ||
        strcmp(op, "chrompoly") == 0 || strcmp(op, "matching") == 0) {
        Graph *g = graph_load(argv[2]);
        if (!g) { fprintf(stderr, "Error loading graph %s\n", argv[2]); return; }
        graph_print(g);
        if (strcmp(op, "ginfo") == 0) {
            printf("Connected: %s\n", graph_is_connected(g) ? "YES" : "NO");
            printf("Has cycle: %s\n", graph_has_cycle(g) ? "YES" : "NO");
            int nc;
            int *comp = graph_connected_components(g, &nc);
            printf("Components: %d\n", nc);
            free(comp);
        } else if (strcmp(op, "charpoly") == 0) {
            Poly *cp = graph_characteristic_poly(g);
            printf("Characteristic poly: "); poly_print(cp);
            poly_destroy(cp);
        } else if (strcmp(op, "chrompoly") == 0) {
            Poly *cp = graph_chromatic_poly(g);
            printf("Chromatic poly: "); poly_print(cp);
            printf("Chromatic number: %d\n", graph_chromatic_number(g));
            poly_destroy(cp);
        } else if (strcmp(op, "matching") == 0) {
            Poly *mp = graph_matching_poly(g);
            printf("Matching poly: "); poly_print(mp);
            poly_destroy(mp);
        }
        graph_destroy(g);
        return;
    }

    // Polynomial operations
    Poly *a = poly_load(argv[2]);
    if (!a) { fprintf(stderr, "Error loading poly %s\n", argv[2]); return; }
    printf("A = "); poly_print(a);

    if (strcmp(op, "diff") == 0) {
        Poly *d = poly_differentiate(a); printf("d/dx A = "); poly_print(d); poly_destroy(d);
    } else if (strcmp(op, "integ") == 0) {
        Poly *ig = poly_integrate(a, 0.0); printf("integral A = "); poly_print(ig); poly_destroy(ig);
    } else if (strcmp(op, "eval") == 0 && argc >= 4) {
        double x = atof(argv[3]);
        printf("A(%.4g) = %.10g\n", x, poly_eval_horner(a, x));
    } else if (argc >= 4) {
        Poly *b = poly_load(argv[3]);
        if (!b) { fprintf(stderr, "Error loading %s\n", argv[3]); poly_destroy(a); return; }
        printf("B = "); poly_print(b);
        Poly *res = NULL;
        if      (strcmp(op, "add")     == 0) res = poly_add(a, b);
        else if (strcmp(op, "sub")     == 0) res = poly_sub(a, b);
        else if (strcmp(op, "mul")     == 0) res = poly_mul(a, b);
        else if (strcmp(op, "gcd")     == 0) res = poly_gcd(a, b);
        else if (strcmp(op, "compose") == 0) res = poly_compose(a, b);
        if (res) { printf("Result = "); poly_print(res); poly_destroy(res); }
        poly_destroy(b);
    }
    poly_destroy(a);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        cli_mode(argc, argv);
        return 0;
    }

    printf("╔══════════════════════════════════════╗\n");
    printf("║  Polynomial + Graph Library (CS1234)  ║\n");
    printf("╚══════════════════════════════════════╝\n");

    // Polynomial demos
    demo_basic();
    demo_eval();
    demo_calculus();
    demo_divmod();
    demo_gcd();
    demo_compose();
    demo_roots();
    demo_fileio();

    // Graph theory + discrete math demos
    demo_graph_basics();
    demo_characteristic_poly();
    demo_chromatic_poly();
    demo_matching_poly();
    demo_modular_poly();
    demo_cyclotomic();
    demo_graph_fileio();

    print_separator();
    printf("All 15 demos complete!\n");
    return 0;
}
