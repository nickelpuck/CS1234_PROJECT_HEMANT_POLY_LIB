#ifndef CODES_H
#define CODES_H

#include "poly.h"

// ─── Reed-Solomon Error Correcting Codes ─────────────────────────────────────
// Encode a message of k symbols as a polynomial of degree k-1
// Evaluate at n points => n codeword symbols
// Can recover original message even if up to (n-k)/2 symbols are corrupted
//
// We work over integers with floating-point arithmetic (simplified RS)
// for demonstration purposes.

typedef struct {
    int    n;          // total codeword length (number of evaluation points)
    int    k;          // message length (number of data symbols)
    double *points;    // evaluation points x_0, x_1, ..., x_{n-1}
} RSCode;

// Create RS code with n codeword symbols, k message symbols
// Evaluation points are 1, 2, ..., n
RSCode *rs_create(int n, int k);
void    rs_destroy(RSCode *rs);

// Encode: message[0..k-1] -> codeword[0..n-1]
// Treats message as coefficients of polynomial, evaluates at n points
double *rs_encode(const RSCode *rs, double *message);

// Decode: recover message from codeword (with possible erasures marked as NaN)
// Uses Lagrange interpolation on non-erased positions
// Returns NULL if too many erasures (> n-k)
double *rs_decode(const RSCode *rs, double *codeword);

// Simulate corruption: randomly corrupt 'num_errors' positions
void    rs_corrupt(double *codeword, int n, int num_errors);

// Check if codeword is valid (all evaluations consistent with a degree k-1 poly)
int     rs_is_valid(const RSCode *rs, double *codeword);

// Print codeword
void    rs_print_codeword(double *codeword, int n);

// ─── Polynomial-based Checksum / CRC-like ────────────────────────────────────
// Treat data as polynomial, divide by generator polynomial
// Remainder is the checksum

// Generator polynomial for checksum (e.g., x^4 + x + 1)
Poly   *checksum_generator(int degree);
Poly   *checksum_compute(const char *data, int len, Poly *generator);
int     checksum_verify(const char *data, int len, Poly *generator, Poly *checksum);

// ─── Berlekamp-Welch style error locator (simplified) ────────────────────────
// Given n points where up to e are wrong, find the correct polynomial
// Works when n >= k + 2e
Poly   *rs_error_correct(double *xs, double *ys, int n, int k, int max_errors);

#endif
