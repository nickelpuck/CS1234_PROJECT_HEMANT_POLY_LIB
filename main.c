#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "poly.h"

void print_separator() {
    printf("\n========================================\n");
}

void demo_basic() {
    print_separator();
    printf("DEMO 1: Basic Arithmetic\n");
    print_separator();

    // p = 3x^2 + 2x + 1
    double ca[] = {1, 2, 3};
    Poly *a = poly_from_array(ca, 2);
    printf("A = "); poly_print(a);

    // q = x^2 - x + 4
    double cb[] = {4, -1, 1};
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

    poly_destroy(a); poly_destroy(b);
    poly_destroy(sum); poly_destroy(diff);
    poly_destroy(prod); poly_destroy(neg); poly_destroy(sc);
}

void demo_eval() {
    print_separator();
    printf("DEMO 2: Evaluation\n");
    print_separator();

    double ca[] = {1, 2, 3};  // 3x^2 + 2x + 1
    Poly *p = poly_from_array(ca, 2);
    printf("P = "); poly_print(p);

    for (double x = -2.0; x <= 2.0; x += 1.0) {
        printf("  P(%.1f) = %.4g  [Horner: %.4g]\n",
               x, poly_eval(p, x), poly_eval_horner(p, x));
    }
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

    poly_destroy(p); poly_destroy(dp);
    poly_destroy(ddp); poly_destroy(ip);
}

void demo_divmod() {
    print_separator();
    printf("DEMO 4: Division (Quotient & Remainder)\n");
    print_separator();

    // a = x^3 - 2x^2 - 4
    double ca[] = {-4, 0, -2, 1};
    Poly *a = poly_from_array(ca, 3);

    // b = x - 3
    double cb[] = {-3, 1};
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
    printf("DEMO 5: GCD of Polynomials\n");
    print_separator();

    // a = x^2 - 1 = (x-1)(x+1)
    double ca[] = {-1, 0, 1};
    Poly *a = poly_from_array(ca, 2);

    // b = x^2 - 3x + 2 = (x-1)(x-2)
    double cb[] = {2, -3, 1};
    Poly *b = poly_from_array(cb, 2);

    printf("A   = "); poly_print(a);
    printf("B   = "); poly_print(b);

    Poly *g = poly_gcd(a, b);
    printf("GCD = "); poly_print(g);  // should be x - 1

    poly_destroy(a); poly_destroy(b); poly_destroy(g);
}

void demo_compose() {
    print_separator();
    printf("DEMO 6: Composition f(g(x))\n");
    print_separator();

    // f = x^2 + 1
    double cf[] = {1, 0, 1};
    Poly *f = poly_from_array(cf, 2);

    // g = x + 2
    double cg[] = {2, 1};
    Poly *g = poly_from_array(cg, 1);

    printf("f(x)    = "); poly_print(f);
    printf("g(x)    = "); poly_print(g);

    Poly *fog = poly_compose(f, g);
    printf("f(g(x)) = "); poly_print(fog);  // (x+2)^2 + 1 = x^2 + 4x + 5

    poly_destroy(f); poly_destroy(g); poly_destroy(fog);
}

void demo_roots() {
    print_separator();
    printf("DEMO 7: Root Finding\n");
    print_separator();

    // Quadratic: x^2 - 5x + 6 = (x-2)(x-3)
    double cq[] = {6, -5, 1};
    Poly *q = poly_from_array(cq, 2);
    printf("Q = "); poly_print(q);

    int count;
    double *roots = poly_roots_real(q, &count);
    printf("Real roots: ");
    for (int i = 0; i < count; i++) printf("%.4g  ", roots[i]);
    printf("\n");
    free(roots);
    poly_destroy(q);

    // Newton's method: x^3 - x - 2 (root near x=1.5)
    double cn[] = {-2, -1, 0, 1};
    Poly *p = poly_from_array(cn, 3);
    printf("\nP = "); poly_print(p);
    double root = poly_newton_root(p, 1.5, 100, 1e-10);
    printf("Newton root (x0=1.5): %.10g\n", root);
    printf("Verify P(%.10g) = %.2e\n", root, poly_eval_horner(p, root));
    poly_destroy(p);
}

void demo_fileio() {
    print_separator();
    printf("DEMO 8: File Save & Load\n");
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

void cli_mode(int argc, char *argv[]) {
    if (argc < 3) {
        printf("CLI Usage: %s <op> <file1> [file2] [value]\n", argv[0]);
        printf("  ops: add sub mul diff integ eval gcd compose\n");
        return;
    }

    char *op = argv[1];
    Poly *a  = poly_load(argv[2]);
    if (!a) { fprintf(stderr, "Error loading %s\n", argv[2]); return; }

    printf("A = "); poly_print(a);

    if (strcmp(op, "diff") == 0) {
        Poly *d = poly_differentiate(a);
        printf("d/dx A = "); poly_print(d);
        poly_destroy(d);
    } else if (strcmp(op, "integ") == 0) {
        Poly *ig = poly_integrate(a, 0.0);
        printf("integral A = "); poly_print(ig);
        poly_destroy(ig);
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
    printf("║   Polynomial Library Demo (CS1234)   ║\n");
    printf("╚══════════════════════════════════════╝\n");

    demo_basic();
    demo_eval();
    demo_calculus();
    demo_divmod();
    demo_gcd();
    demo_compose();
    demo_roots();
    demo_fileio();

    print_separator();
    printf("All demos complete!\n");
    return 0;
}
