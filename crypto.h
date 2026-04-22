#ifndef CRYPTO_H
#define CRYPTO_H

#include "poly.h"

//Shamir's Secret Sharing
//Split a secret into n shares; any k shares reconstruct it
//Works over a prime field Z_p

typedef struct {
    int x; //share index(1 to n)
    long y;  //share value
} Share;
//Split secret into n shares, requiring k to reconstruct, over Z_p
Share *shamir_split(long secret, int n, int k, long prime);

//Reconstruct secret from any k shares using Lagrange interpolation over Z_p
long shamir_reconstruct(Share *shares, int k, long prime);

// Verify a share is consistent with others
int shamir_verify(Share *shares, int k, Share test, long prime);

//Lagrange Interpolation
// Given k (x,y) points, find unique polynomial of degree k-1 passing through them
Poly *lagrange_interpolate(double *xs, double *ys, int k);

//Lagrange interpolation over Z_p(returns coefficients mod p)
long *lagrange_interpolate_zp(long *xs, long *ys, int k, long prime);

//Polynomial Fingerprinting (Rabin-Karp)
//Treat string as polynomial: s[0] + s[1]*x + s[2]*x^2 + ...
//Evaluate mod prime at random point => fingerprint
typedef struct {
    long prime; // large prime modulus
    long base; // random base point
} Fingerprint;

Fingerprint fp_create(long prime, long base);
long fp_of_string(const char *s, int len, Fingerprint fp);
long fp_roll(long prev_fp, char old_char, char new_char,
                     int window, Fingerprint fp, long base_pow);

//Rabin-Karp substring search:find all occur of pattern in txt
//Returns array of starting indices,sets *count
int *rabin_karp_search(const char *text, const char *pattern, int *count);

// Polyn fingerprint of a file(checksum)
long fp_of_file(const char *filename, Fingerprint fp);

//Modular Arithm Helpers
long mod_pow(long base, long exp, long mod);// fast modular exponentiation
long mod_inv(long a, long mod);// modular inverse (Fermat's little theorem)
long mod_mul(long a, long b, long mod);  // safe multiplication avoiding overflow

#endif
