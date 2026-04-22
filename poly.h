#ifndef POLY_H
#define POLY_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Polynomial represented as array of coefficients
// coeffs[i] = coefficient of x^i
// e.g., 3x^2 + 2x + 1 => coeffs = {1, 2, 3}, degree = 2
typedef struct {
    double *coeffs;
    int degree;
} Poly;

// Create / Destroy
Poly *poly_create(int degree);
Poly *poly_from_array(double *coeffs, int degree);
Poly *poly_copy(const Poly *p);
void  poly_destroy(Poly *p);

// Display
void poly_print(const Poly *p);
void poly_print_to_file(const Poly *p, FILE *f);

// Basic Arithmetic
Poly *poly_add(const Poly *a, const Poly *b);
Poly *poly_sub(const Poly *a, const Poly *b);
Poly *poly_mul(const Poly *a, const Poly *b);
Poly *poly_scalar_mul(const Poly *a, double scalar);
Poly *poly_negate(const Poly *a);

// Calculus
Poly *poly_differentiate(const Poly *p);
Poly *poly_integrate(const Poly *p, double constant);

// Evaluation
double poly_eval(const Poly *p, double x);
double poly_eval_horner(const Poly *p, double x);

// Properties
int    poly_degree(const Poly *p);
double poly_leading_coeff(const Poly *p);
int    poly_is_zero(const Poly *p);
int    poly_equals(const Poly *a, const Poly *b);
Poly  *poly_normalize(Poly *p);

// Advanced
Poly  *poly_compose(const Poly *f, const Poly *g);
Poly  *poly_gcd(const Poly *a, const Poly *b);
Poly  *poly_mod(const Poly *a, const Poly *b);
Poly **poly_divmod(const Poly *a, const Poly *b);
double poly_newton_root(const Poly *p, double x0, int max_iter, double tol);
double *poly_roots_real(const Poly *p, int *count);

// File I/O
int   poly_save(const Poly *p, const char *filename);
Poly *poly_load(const char *filename);

#endif
