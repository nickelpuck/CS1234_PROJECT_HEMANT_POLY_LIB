#include "codes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "crypto.h"

//Reed-Solomon 
RSCode *rs_create(int n, int k) {
    if (n <= 0 || k <= 0 || k > n) return NULL;
    RSCode *rs = (RSCode *)malloc(sizeof(RSCode));
    if (!rs) return NULL;
    rs->n = n;
    rs->k = k;
    rs->points = (double *)malloc(n * sizeof(double));
    if (!rs->points) { free(rs); return NULL; }
    for (int i = 0; i < n; i++)
        rs->points[i] = (double)(i + 1);  //evaluatn pts:1,2,..,n
    return rs;
}
void rs_destroy(RSCode *rs) {
    if (!rs) return;
    free(rs->points);
    free(rs);
}

double *rs_encode(const RSCode *rs, double *message) {
    if (!rs || !message) return NULL;

    //Build poly from msg coeff
    Poly *p = poly_from_array(message, rs->k - 1);
    if (!p) return NULL;
    double *codeword = (double *)malloc(rs->n * sizeof(double));
    if (!codeword) { poly_destroy(p); return NULL; }

    //Evaluate at each pt
    for (int i = 0; i < rs->n; i++)
        codeword[i] = poly_eval_horner(p, rs->points[i]);
    poly_destroy(p);
    return codeword;
}

double *rs_decode(const RSCode *rs, double *codeword) {
    if (!rs || !codeword) return NULL;

    // Collect non-erased positions (not NaN)
    double *xs = (double *)malloc(rs->n * sizeof(double));
    double *ys = (double *)malloc(rs->n * sizeof(double));
    int valid = 0;
    for (int i = 0; i < rs->n; i++) {
        if (!isnan(codeword[i])) {
            xs[valid] = rs->points[i];
            ys[valid] = codeword[i];
            valid++;
        }
    }

    if (valid < rs->k) {
        free(xs); free(ys);
        fprintf(stderr, "Too many erasures: need %d, have %d\n", rs->k, valid);
        return NULL;
    }

    // Use 1st k valid pts for Lagrange interpolatn
    Poly *p = lagrange_interpolate(xs, ys, rs->k);
    free(xs); free(ys);
    if (!p) return NULL;

    // Extract msg coeff
    double *message = (double *)calloc(rs->k, sizeof(double));
    if (!message) { poly_destroy(p); return NULL; }
    for (int i = 0; i < rs->k && i <= p->degree; i++)
        message[i] = round(p->coeffs[i]);
    poly_destroy(p);
    return message;
}

void rs_corrupt(double *codeword, int n, int num_errors) {
    if (!codeword || num_errors <= 0) return;
    srand((unsigned)time(NULL) ^ 0xDEAD);
    int *positions = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) positions[i] = i;
    // Shuffle
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = positions[i]; positions[i] = positions[j]; positions[j] = tmp;
    }
    for (int i = 0; i < num_errors && i < n; i++)
        codeword[positions[i]] = NAN;  // mark as erasure
    free(positions);
}

int rs_is_valid(const RSCode *rs, double *codeword) {
    if (!rs || !codeword) return 0;
    // Interpolate from 1st k pts, check rest
    double *xs = (double *)malloc(rs->k * sizeof(double));
    double *ys = (double *)malloc(rs->k * sizeof(double));
    for (int i = 0; i < rs->k; i++) {
        xs[i] = rs->points[i];
        ys[i] = codeword[i];
    }
    Poly *p = lagrange_interpolate(xs, ys, rs->k);
    free(xs); free(ys);
    if (!p) return 0;

    int valid = 1;
    for (int i = rs->k; i < rs->n; i++) {
        double expected = poly_eval_horner(p, rs->points[i]);
        if (fabs(expected - codeword[i]) > 0.5) { valid = 0; break; }
    }
    poly_destroy(p);
    return valid;
}

void rs_print_codeword(double *codeword, int n) {
    printf("[");
    for (int i = 0; i < n; i++) {
        if (isnan(codeword[i])) printf(" ERR");
        else printf(" %.0f", codeword[i]);
        if (i < n - 1) printf(",");
    }
    printf(" ]\n");
}

// Polyn Checksum 
Poly *checksum_generator(int degree) {
    // Simple generator: x^degree + x + 1
    Poly *g = poly_create(degree);
    g->coeffs[degree] = 1.0;
    g->coeffs[1]      = 1.0;
    g->coeffs[0]      = 1.0;
    return g;
}

Poly *checksum_compute(const char *data, int len, Poly *generator) {
    if (!data || !generator) return NULL;
    // Build polyn from data bytes
    Poly *msg = poly_create(len - 1);
    for (int i = 0; i < len; i++)
        msg->coeffs[i] = (double)(unsigned char)data[i];
    poly_normalize(msg);

    // Rem = msg mod generator
    Poly *rem = poly_mod(msg, generator);
    poly_destroy(msg);
    return rem;
}

int checksum_verify(const char *data, int len, Poly *generator, Poly *checksum) {
    Poly *computed = checksum_compute(data, len, generator);
    if (!computed) return 0;
    int ok = poly_equals(computed, checksum);
    poly_destroy(computed);
    return ok;
}

//Simplified Error Correctn (Berlekamp-Welch style)
// Given n(x,y) pairs where at most e are wrong,
// find polyn of deg< k passing through at least n-e points.
// Uses the key equation: E(x)*f(x) = Q(x) where deg(E)=e, deg(Q)=e+k-1
// Solve linear syst for coeff.

Poly *rs_error_correct(double *xs, double *ys, int n, int k, int max_errors) {
    if (!xs || !ys || n <= 0) return NULL;
    int e = max_errors;
    // Deg(Q)=e+k-1,deg(E)=e
    // Unknowns:Q has e+k coeff, E has e coeff(leading coeff = 1)
    // Total unknowns =e+k+e=2e+k
    int q_deg = e + k - 1;
    int unknowns = (q_deg + 1) + e;  //Q coeffs+E coeffs(excl. leading)

    if (n < unknowns) {
        // Not enough pts,fall back to plain Lagrange
        return lagrange_interpolate(xs, ys, k);
    }

    // Build linear syst:for each point i:
    // Q(xi)-yi*E(xi)=0
    // sum_{j=0}^{q_deg} q_j * xi^j - yi * (xi^e + sum_{j=0}^{e-1} e_j * xi^j) = 0
    // Rearranged: sum q_j * xi^j - yi * sum_{j=0}^{e-1} e_j * xi^j = yi * xi^e

    // Allocate matrix A (n x unknowns) and vector b (n)
    double **A = (double **)malloc(n * sizeof(double *));
    double  *b = (double  *)malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) {
        A[i] = (double *)calloc(unknowns, sizeof(double));
        // Q coefficients: columns 0..q_deg
        double xpow = 1.0;
        for (int j = 0; j <= q_deg; j++) {
            A[i][j] = xpow;
            xpow *= xs[i];
        }
        // E coefficients (j=0..e-1): columns q_deg+1 .. q_deg+e
        xpow = 1.0;
        for (int j = 0; j < e; j++) {
            A[i][q_deg + 1 + j] = -ys[i] * xpow;
            xpow *= xs[i];
        }
        //RHS:yi*xi^e
        b[i] = ys[i] * pow(xs[i], e);
    }

    //Gaussian eliminatn
    double *sol = (double *)calloc(unknowns, sizeof(double));
    int rows = n < unknowns ? n : unknowns;
    for (int col = 0; col < rows; col++) {
        //Find pivot
        int pivot = -1;
        for (int row = col; row < n; row++) {
            if (fabs(A[row][col]) > 1e-9) { pivot = row; break; }
        }
        if (pivot < 0) continue;
        //Swap
        double *tmp = A[col]; A[col] = A[pivot]; A[pivot] = tmp;
        double tb = b[col]; b[col] = b[pivot]; b[pivot] = tb;
        //Eliminate
        double scale = A[col][col];
        for (int j = col; j < unknowns; j++) A[col][j] /= scale;
        b[col] /= scale;
        for (int row = 0; row < n; row++) {
            if (row == col || fabs(A[row][col]) < 1e-12) continue;
            double f = A[row][col];
            for (int j = col; j < unknowns; j++) A[row][j] -= f * A[col][j];
            b[row] -= f * b[col];
        }
    }
    for (int i = 0; i < rows; i++) sol[i] = b[i];

    //Extr Q&E poly
    Poly *Q = poly_create(q_deg);
    for (int j = 0; j <= q_deg; j++) Q->coeffs[j] = sol[j];
    poly_normalize(Q);

    Poly *E = poly_create(e);
    E->coeffs[e] = 1.0;  //monic
    for (int j = 0; j < e; j++) E->coeffs[j] = sol[q_deg + 1 + j];
    poly_normalize(E);

    //f=Q/E
    Poly **dm = poly_divmod(Q, E);
    Poly *f = NULL;
    if (dm) {
        f = dm[0];
        poly_destroy(dm[1]);
        free(dm);
    }

    poly_destroy(Q); poly_destroy(E);
    for (int i = 0; i < n; i++) free(A[i]);
    free(A); free(b); free(sol);

    return f ? poly_normalize(f) : lagrange_interpolate(xs, ys, k);
}
