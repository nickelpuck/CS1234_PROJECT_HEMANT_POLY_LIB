#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "poly.h"
#include "graph.h"
#include "crypto.h"
#include "codes.h"

void print_separator() { printf("\n========================================\n"); }
void print_header(int n, const char *title) {
    printf("\n========================================\n");
    printf("DEMO %d: %s\n", n, title);
    printf("========================================\n");
}

//Polyn Demos
void demo_basic() {
    print_header(1, "Basic Polynomial Arithmetic");
    double ca[]={1, 2, 3};Poly *a = poly_from_array(ca, 2);
    double cb[]={4, -1, 1};Poly *b = poly_from_array(cb, 2);
    printf("A = "); poly_print(a);
    printf("B = "); poly_print(b);
    Poly *sum = poly_add(a,b), *diff = poly_sub(a,b);
    Poly *prod = poly_mul(a,b), *neg = poly_negate(a);
    Poly *sc = poly_scalar_mul(a, 2.5);
    printf("A+B   = ");poly_print(sum);
    printf("A-B   = ");poly_print(diff);
    printf("A*B   = ");poly_print(prod);
    printf("-A    = ");poly_print(neg);
    printf("2.5*A = ");poly_print(sc);
    poly_destroy(a);poly_destroy(b);poly_destroy(sum);poly_destroy(diff);
    poly_destroy(prod);poly_destroy(neg);poly_destroy(sc);
}

void demo_eval(){
    print_header(2, "Evaluation: Direct & Horner's Method");
    double ca[]={1, 2, 3};
    Poly *p=poly_from_array(ca,2);
    printf("P = ");poly_print(p);
    for(double x=-2.0; x<=2.0; x+=1.0)
        printf("  P(%.1f) = %.4g  [Horner: %.4g]\n",
               x, poly_eval(p, x), poly_eval_horner(p, x));
    poly_destroy(p);
}

void demo_calculus(){
    print_header(3, "Differentiation & Integration");
    double ca[]={1, 2, 3, 4};
    Poly *p=poly_from_array(ca, 3);
    Poly *dp=poly_differentiate(p);
    Poly *ddp=poly_differentiate(dp);
    Poly *ip=poly_integrate(p, 0.0);
    printf("P        = ");poly_print(p);
    printf("P'       = ");poly_print(dp);
    printf("P''      = ");poly_print(ddp);
    printf("integral = ");poly_print(ip);
    poly_destroy(p); poly_destroy(dp); poly_destroy(ddp); poly_destroy(ip);
}

void demo_divmod(){
    print_header(4, "Polynomial Long Division");
    double ca[]={-4, 0, -2, 1}; Poly *a=poly_from_array(ca, 3);
    double cb[]={-3, 1}; Poly *b = poly_from_array(cb, 1);
    printf("A = "); poly_print(a);
    printf("B = "); poly_print(b);
    Poly **dm=poly_divmod(a, b);
    if (dm){
        printf("A/B = "); poly_print(dm[0]);
        printf("A%%B = "); poly_print(dm[1]);
        poly_destroy(dm[0]); poly_destroy(dm[1]); free(dm);
    }
    poly_destroy(a); poly_destroy(b);
}

void demo_gcd(){
    print_header(5, "GCD of Polynomials (Euclidean Algorithm)");
    double ca[]={-1, 0, 1}; Poly *a = poly_from_array(ca, 2);
    double cb[]={2, -3, 1}; Poly *b = poly_from_array(cb, 2);
    printf("A   = "); poly_print(a);
    printf("B   = "); poly_print(b);
    Poly *g=poly_gcd(a, b);
    printf("GCD = "); poly_print(g);
    poly_destroy(a); poly_destroy(b); poly_destroy(g);
}

void demo_compose(){
    print_header(6, "Polynomial Composition f(g(x))");
    double cf[]={1, 0, 1}; Poly *f = poly_from_array(cf, 2);
    double cg[]={2, 1};Poly *g = poly_from_array(cg, 1);
    printf("f(x)    = "); poly_print(f);
    printf("g(x)    = "); poly_print(g);
    Poly *fog = poly_compose(f, g);
    printf("f(g(x)) = "); poly_print(fog);
    poly_destroy(f); poly_destroy(g); poly_destroy(fog);
}

void demo_roots(){
    print_header(7, "Root Finding: Quadratic Formula + Newton's Method");
    double cq[]={6, -5, 1}; Poly *q=poly_from_array(cq, 2);
    printf("Q = "); poly_print(q);
    int count;
    double *roots=poly_roots_real(q, &count);
    printf("Roots: "); 
    for(int i=0;i<count;i++) printf("%.4g  ",roots[i]); printf("\n");
    free(roots); poly_destroy(q);
    double cn[]={-2, -1, 0, 1}; Poly *p=poly_from_array(cn, 3);
    printf("\nP = "); poly_print(p);
    double root=poly_newton_root(p, 1.5, 100, 1e-10);
    printf("Newton root: %.10g  |  P(root) = %.2e\n", root, poly_eval_horner(p, root));
    poly_destroy(p);
}

void demo_fileio(){
    print_header(8, "File Save & Load (State Persistence)");
    double ca[]={1, -3, 0, 2}; Poly *p=poly_from_array(ca, 3);
    printf("Original: "); poly_print(p);
    poly_save(p, "poly_saved.txt");
    Poly *loaded=poly_load("poly_saved.txt");
    printf("Loaded:   "); poly_print(loaded);
    printf("Equal: %s\n", poly_equals(p, loaded) ? "YES" : "NO");
    poly_destroy(p); poly_destroy(loaded);
}

void demo_cyclotomic(){
    print_header(9, "Cyclotomic Polynomials Phi_n(x)");
    for (int n = 1;n <= 8;n++) {
        Poly *phi=poly_cyclotomic(n);
        printf("Phi_%d(x) = ", n); poly_print(phi);
        poly_destroy(phi);
    }
}

void demo_modular(){
    print_header(10, "Modular Arithmetic over Finite Fields Z_p");
    double ca[]={3, 7, 5, 2}; Poly *p=poly_from_array(ca, 3);
    printf("P       = "); poly_print(p);
    Poly *pm=poly_mod_p(p, 3);
    printf("P mod 3 = "); poly_print(pm);
    double ci[]={1, 1, 0, 1}; Poly *irr=poly_from_array(ci, 3);
    printf("\nQ = "); poly_print(irr);
    printf("Q irreducible over Z_2: %s\n", poly_is_irreducible_zp(irr, 2) ? "YES" : "NO");
    printf("Q irreducible over Z_3: %s\n", poly_is_irreducible_zp(irr, 3) ? "YES" : "NO");
    printf("Hash of P (base=31, mod=1e9+7): %ld\n", poly_hash(p, 31, 1000000007L));
    poly_destroy(p); poly_destroy(pm); poly_destroy(irr);
}

//Graph Th Demos 
void demo_graph_basics() {
    print_header(11, "Graph Construction, BFS & DFS Traversal");
    Graph *g = graph_create(4);
    graph_set_label(g,0,"A"); graph_set_label(g,1,"B");
    graph_set_label(g,2,"C"); graph_set_label(g,3,"D");
    graph_add_edge(g,0,1); graph_add_edge(g,0,2);
    graph_add_edge(g,1,3); graph_add_edge(g,2,3);
    graph_print(g); graph_print_matrix(g);
    printf("Connected: %s | Has cycle: %s\n",
           graph_is_connected(g)?"YES":"NO", graph_has_cycle(g)?"YES":"NO");
    int cnt;
    int *bfs = graph_bfs_order(g,0,&cnt);
    printf("BFS: "); for(int i=0;i<cnt;i++) printf("%s ",g->labels[bfs[i]]); printf("\n");
    free(bfs);
    int *dfs = graph_dfs_order(g,0,&cnt);
    printf("DFS: "); for(int i=0;i<cnt;i++) printf("%s ",g->labels[dfs[i]]); printf("\n");
    free(dfs);
    graph_destroy(g);
}

void demo_spectral() {
    print_header(12, "Spectral Graph Theory");
    // Cycle C4
    Graph *c4 = graph_create(4);
    graph_add_edge(c4,0,1); graph_add_edge(c4,1,2);
    graph_add_edge(c4,2,3); graph_add_edge(c4,3,0);
    printf("Graph: Cycle C4\n");
    Poly *cp = graph_characteristic_poly(c4);
    printf("Characteristic poly: "); poly_print(cp);
    poly_destroy(cp);
    int cnt;
    double *spec = graph_spectrum(c4, &cnt);
    printf("Spectrum (eigenvalues): ");
    for (int i=0;i<cnt;i++) printf("%.4g  ", spec[i]);
    printf("\n");
    free(spec);
    printf("Spectral gap: %.4g\n", graph_spectral_gap(c4));
    printf("Algebraic connectivity (Fiedler): %.4g\n", graph_algebraic_connectivity(c4));

    // Cospectral check: two non-isomorphic graphs with same spectrum
    Graph *c4b = graph_create(4);
    graph_add_edge(c4b,0,1); graph_add_edge(c4b,1,2);
    graph_add_edge(c4b,2,3); graph_add_edge(c4b,3,0);
    printf("C4 cospectral with itself: %s\n",
           graph_is_cospectral(c4, c4b) ? "YES" : "NO");
    graph_destroy(c4); graph_destroy(c4b);
}

void demo_chromatic() {
    print_header(13, "Chromatic Polynomial & Graph Coloring");
    Graph *k3 = graph_create(3);
    graph_add_edge(k3,0,1); graph_add_edge(k3,1,2); graph_add_edge(k3,0,2);
    printf("Graph: Triangle K3\n"); graph_print(k3);
    Poly *cp = graph_chromatic_poly(k3);
    printf("Chromatic poly P(G,k) = "); poly_print(cp);
    for (int k=1;k<=5;k++)
        printf("  k=%d: %.0f colorings\n", k, poly_eval_horner(cp,(double)k));
    printf("Chromatic number: %d\n", graph_chromatic_number(k3));
    poly_destroy(cp); graph_destroy(k3);
}

void demo_tutte() {
    print_header(14, "Tutte Polynomial T(G; x, y)");
    // Triangle K3
    Graph *k3 = graph_create(3);
    graph_add_edge(k3,0,1); graph_add_edge(k3,1,2); graph_add_edge(k3,0,2);
    printf("Graph: Triangle K3\n");
    TuttePoly *t = graph_tutte_poly(k3);
    printf("Tutte polynomial T(G; x, y) = "); tutte_print(t);
    printf("T(1,1) = spanning trees     = %.0f\n", tutte_eval(t,1,1));
    printf("T(2,0) = acyclic orient.    = %.0f\n", tutte_eval(t,2,0));
    printf("T(1,2) = connected subgraphs= %.0f\n", tutte_eval(t,1,2));
    printf("Spanning trees (direct):      %ld\n", graph_spanning_trees(k3));
    printf("Acyclic orientations:         %ld\n", graph_acyclic_orientations(k3));
    tutte_destroy(t); graph_destroy(k3);
}

void demo_reliability(){
    print_header(15, "Reliability Polynomial R(G, p)");
    Graph *g=graph_create(3);
    graph_add_edge(g,0,1); graph_add_edge(g,1,2); graph_add_edge(g,0,2);
    printf("Graph: Triangle K3\n");
    Poly *rp=graph_reliability_poly(g);
    printf("Reliability poly R(G,p) = "); poly_print(rp);
    printf("Connectivity probability:\n");
    for (int i=0;i<=10;i++){
        double p=i / 10.0;
        printf("  p=%.1f: R=%.4f\n", p, poly_eval_horner(rp, p));
    }
    poly_destroy(rp); graph_destroy(g);
}

void demo_matching(){
    print_header(16, "Matching Polynomial");
    Graph *k4= graph_create(4);
    for(int i=0;i<4;i++) for(int j=i+1;j<4;j++) graph_add_edge(k4,i,j);
    printf("Graph: Complete K4\n");
    for(int k=0;k<=2;k++)
        printf("  m(K4,%d) = %d\n", k, graph_matching_count(k4,k));
    Poly *mp=graph_matching_poly(k4);
    printf("Matching poly: "); poly_print(mp);
    poly_destroy(mp); graph_destroy(k4);
}

//Crypto & Coding Demos
void demo_lagrange() {
    print_header(17, "Lagrange Interpolation");
    //4 pts on y=x^3-2x+1
    double xs[] = {1, 2, 3, 4};
    double ys[] = {0, 5, 22, 57};
    printf("Points: ");
    for (int i=0;i<4;i++) printf("(%.0f,%.0f) ", xs[i], ys[i]);
    printf("\n");
    Poly *p = lagrange_interpolate(xs, ys, 4);
    printf("Interpolated poly: "); poly_print(p);
    printf("Verify at x=5: %.4g\n", poly_eval_horner(p, 5.0));
    poly_destroy(p);
}

void demo_shamir() {
    print_header(18, "Shamir's Secret Sharing");
    long secret = 1234567L;
    int n = 5, k = 3;
    long prime = 1000000007L;
    printf("Secret: %ld\n", secret);
    printf("Split into n=%d shares, need k=%d to reconstruct\n", n, k);
    Share *shares = shamir_split(secret, n, k, prime);
    printf("Shares:\n");
    for (int i=0;i<n;i++) printf("  Share %d: (%d, %ld)\n", i+1, shares[i].x, shares[i].y);

    //Reconst first k shares
    long recovered=shamir_reconstruct(shares, k, prime);
    printf("\nReconstructed from shares 1..%d: %ld\n", k, recovered);
    printf("Match: %s\n", recovered == secret ? "YES" : "NO");

    //Reconstr from diff k shares
    Share subset[3]={shares[1],shares[3],shares[4]};
    long recovered2= shamir_reconstruct(subset, k, prime);
    printf("Reconstructed from shares 2,4,5: %ld\n", recovered2);
    printf("Match: %s\n", recovered2 == secret ? "YES" : "NO");

    //Verify a share
    printf("\nVerify share 3 is consistent: %s\n",
           shamir_verify(shares, k, shares[2], prime) ? "YES" : "NO");
    free(shares);
}

void demo_rabin_karp(){
    print_header(19, "Rabin-Karp Polynomial Fingerprinting & String Search");
    const char *text="the quick brown fox jumps over the lazy dog the fox";
    const char *pattern="fox";
    printf("Text:    \"%s\"\n", text);
    printf("Pattern: \"%s\"\n", pattern);
    int count;
    int *positions=rabin_karp_search(text, pattern, &count);
    printf("Found %d occurrence(s) at positions: ", count);
    for(int i=0;i<count;i++) printf("%d ", positions[i]);
    printf("\n");
    free(positions);

    //Fingerprint demo
    Fingerprint fp=fp_create(1000000007L, 31L);
    printf("\nFingerprint of \"hello\": %ld\n", fp_of_string("hello", 5, fp));
    printf("Fingerprint of \"world\": %ld\n", fp_of_string("world", 5, fp));
    printf("Fingerprint of \"hello\": %ld (same as before)\n", fp_of_string("hello", 5, fp));
}

void demo_reed_solomon(){
    print_header(20, "Reed-Solomon Error Correcting Codes");
    //k=4 msg sym,n=7 codeword symb=>can fix upto 1 erasure
    int k=4, n=7;
    RSCode *rs=rs_create(n, k);
    double message[]={3, 1, 4, 1};  //x^3+4x^2+x+3
    printf("Message (poly coeffs): ");
    for(int i=0;i<k;i++) printf("%.0f ", message[i]); printf("\n");

    double *codeword=rs_encode(rs, message);
    printf("Encoded codeword: "); rs_print_codeword(codeword, n);
    printf("Valid codeword: %s\n", rs_is_valid(rs, codeword) ? "YES" : "NO");

    //Corrupt 1 symb(mrk as erasure)
    double *corrupted=(double *)malloc(n * sizeof(double));
    for (int i=0;i<n;i++) corrupted[i] = codeword[i];
    corrupted[2]=NAN;  //erase @nd pos
    printf("\nAfter erasure at position 2: "); rs_print_codeword(corrupted, n);

    double *recovered=rs_decode(rs, corrupted);
    printf("Recovered message: ");
    for(int i=0;i<k;i++) printf("%.0f ", recovered[i]); printf("\n");
    int match = 1;
    for(int i=0;i<k;i++) if(fabs(recovered[i]-message[i])>0.5) match=0;
    printf("Match original: %s\n", match ? "YES" : "NO");

    free(codeword); free(corrupted); free(recovered);
    rs_destroy(rs);
}

void demo_checksum(){
    print_header(21, "Polynomial Checksum (CRC-like)");
    const char *data="Hello, CS1234!";
    Poly *gen=checksum_generator(4);
    printf("Generator poly: "); poly_print(gen);
    printf("Data: \"%s\"\n", data);
    Poly *chk=checksum_compute(data, (int)strlen(data), gen);
    printf("Checksum poly: "); poly_print(chk);
    printf("Verify (original): %s\n",
           checksum_verify(data, (int)strlen(data), gen, chk) ? "PASS" : "FAIL");
    const char *tampered="Hello, CS1235!";
    printf("Verify (tampered): %s\n",
           checksum_verify(tampered, (int)strlen(tampered), gen, chk) ? "PASS" : "FAIL");
    poly_destroy(gen); poly_destroy(chk);
}

void demo_error_correct(){
    print_header(22, "Berlekamp-Welch Error Correction");
    //f(x)=x^2+2x+3, at 6pts
    double xs[]={1,2,3,4,5,6};
    double ys[]={6,11,18,27,38,51};  // f(x) = x^2 + 2x + 3
    printf("Original points (f(x) = x^2 + 2x + 3):\n");
    for(int i=0;i<6;i++) printf("  f(%.0f) = %.0f\n", xs[i], ys[i]);

    // Corrupt 1 pt
    double corrupted_ys[]={6,11,99,27,38,51};  // ys[2] corrupted
    printf("\nCorrupted points (f(3) changed to 99):\n");
    for(int i=0;i<6;i++) printf("  f(%.0f) = %.0f\n", xs[i], corrupted_ys[i]);

    Poly *recovered=rs_error_correct(xs, corrupted_ys, 6, 3, 1);
    printf("\nRecovered polynomial: "); poly_print(recovered);
    printf("f(3) recovered = %.4g (should be 18)\n", poly_eval_horner(recovered, 3.0));
    poly_destroy(recovered);
}

int main(int argc,char *argv[]){
    (void)argc;(void)argv;
    printf("╔══════════════════════════════════════╗\n");
    printf("║ Poly + Graph + Crypto Library CS1234 ║\n");
    printf("╚══════════════════════════════════════╝\n");

    // Polyn
    demo_basic();
    demo_eval();
    demo_calculus();
    demo_divmod();
    demo_gcd();
    demo_compose();
    demo_roots();
    demo_fileio();
    demo_cyclotomic();
    demo_modular();

    // Graph Theory
    demo_graph_basics();
    demo_spectral();
    demo_chromatic();
    demo_tutte();
    demo_reliability();
    demo_matching();

    //Crypto&Coding Theory
    demo_lagrange();
    demo_shamir();
    demo_rabin_karp();
    demo_reed_solomon();
    demo_checksum();
    demo_error_correct();

    print_separator();
    printf("All 22 demos complete!\n");
    return 0;
}
