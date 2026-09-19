#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

using namespace std;
using namespace std::chrono;

// ============================================================================
// THE INVARIANT MASTER ENGINE
// ============================================================================
long long execute_master_convolution(const vector<long long>& f_base,
    const vector<long long>& F_target,
    long long r2) {
    long long total_lattice_points = 0;
    for (long long v = 0; v <= r2; ++v) {
        if (f_base[v] != 0) {
            total_lattice_points += f_base[v] * F_target[r2 - v];
        }
    }
    return total_lattice_points;
}

// ============================================================================
// MODULE A: D_3 LATTICE (FCC / Rhombic Dodecahedron)
// ============================================================================
void build_D3_profiles(long long r, long long r2,
    vector<long long>& f1_even, vector<long long>& f1_odd,
    vector<long long>& f2_even, vector<long long>& f2_odd) {
    for (long long z = -r; z <= r; ++z) {
        long long m = z * z;
        if (m <= r2) {
            if (abs(z) % 2 == 0) f1_even[m]++;
            else f1_odd[m]++;
        }
    }
    for (long long x = -r; x <= r; ++x) {
        for (long long y = -r; y <= r; ++y) {
            long long m = x * x + y * y;
            if (m <= r2) {
                if (abs(x + y) % 2 == 0) f2_even[m]++;
                else f2_odd[m]++;
            }
        }
    }
}

long long execute_D3_convolution(long long r2,
    const vector<long long>& f1_even,
    const vector<long long>& f1_odd,
    const vector<long long>& f2_even,
    const vector<long long>& f2_odd) {
    vector<long long> F2_even(r2 + 1, 0), F2_odd(r2 + 1, 0);
    long long sum_e = 0, sum_o = 0;

    for (long long m = 0; m <= r2; ++m) {
        sum_e += f2_even[m]; F2_even[m] = sum_e;
        sum_o += f2_odd[m];  F2_odd[m] = sum_o;
    }

    long long total_D3 = 0;
    for (long long v = 0; v <= r2; ++v) {
        if (f1_even[v] != 0) total_D3 += f1_even[v] * F2_even[r2 - v];
        if (f1_odd[v] != 0)  total_D3 += f1_odd[v] * F2_odd[r2 - v];
    }
    return total_D3;
}

// ============================================================================
// MODULE B: D_4 LATTICE (24-Cell Honeycomb)
// ============================================================================
void build_D4_parity_profiles(long long r, long long r2,
    vector<long long>& f_even,
    vector<long long>& f_odd) {
    for (long long x = -r; x <= r; ++x) {
        for (long long y = -r; y <= r; ++y) {
            long long m = x * x + y * y;
            if (m <= r2) {
                if (abs(x + y) % 2 == 0) f_even[m]++;
                else f_odd[m]++;
            }
        }
    }
}

long long execute_D4_volume(long long r2,
    const vector<long long>& f_even,
    const vector<long long>& f_odd) {
    vector<long long> F_even(r2 + 1, 0), F_odd(r2 + 1, 0);
    long long sum_e = 0, sum_o = 0;

    for (long long m = 0; m <= r2; ++m) {
        sum_e += f_even[m]; F_even[m] = sum_e;
        sum_o += f_odd[m];  F_odd[m] = sum_o;
    }

    long long total_D4 = 0;
    for (long long v = 0; v <= r2; ++v) {
        if (f_even[v] != 0) total_D4 += f_even[v] * F_even[r2 - v];
        if (f_odd[v] != 0)  total_D4 += f_odd[v] * F_odd[r2 - v];
    }
    return total_D4;
}

// ============================================================================
// MODULE C: E_8 LATTICE (Gosset Lattice)
// ============================================================================
// Helper function to build the full 4D array required for 8D E_8 convolution
vector<long long> build_D4_array(long long r2, const vector<long long>& f_even, const vector<long long>& f_odd) {
    vector<long long> f4(r2 + 1, 0);
    for (long long u = 0; u <= r2; ++u) {
        if (f_even[u] == 0 && f_odd[u] == 0) continue;
        for (long long v = 0; v <= r2 - u; ++v) {
            if (f_even[u] != 0 && f_even[v] != 0) f4[u + v] += f_even[u] * f_even[v];
            if (f_odd[u] != 0 && f_odd[v] != 0) f4[u + v] += f_odd[u] * f_odd[v];
        }
    }
    return f4;
}

long long evaluate_E8_lattice(long long r2, const vector<long long>& f4_D4) {
    vector<long long> F4_D4(r2 + 1, 0);
    long long running_sum = 0;
    for (long long m = 0; m <= r2; ++m) {
        running_sum += f4_D4[m];
        F4_D4[m] = running_sum;
    }
    return execute_master_convolution(f4_D4, F4_D4, r2);
}

// ============================================================================
// MAIN DRIVER & LATTICE SELECTION
// ============================================================================
int main() {
    long long R = 100;  // <--- SET YOUR TARGET RADIUS HERE
    long long r2 = R * R;
    long long total_points = 0;

    auto start_time = high_resolution_clock::now();

    // ========================================================================
    // LATTICE SELECTION ZONE
    // Uncomment EXACTLY ONE of the options below to evaluate your lattice.
    // ========================================================================

    // ------------------------------------------------------------------------
    // [ OPTION 1 ] : D_3 LATTICE (FCC / Rhombic Dodecahedron)
    // ------------------------------------------------------------------------
    /*
    cout << "Evaluating D_3 (FCC) Lattice for R = " << R << "\n";
    vector<long long> f1_e(r2 + 1, 0), f1_o(r2 + 1, 0);
    vector<long long> f2_e(r2 + 1, 0), f2_o(r2 + 1, 0);
    build_D3_profiles(R, r2, f1_e, f1_o, f2_e, f2_o);
    total_points = execute_D3_convolution(r2, f1_e, f1_o, f2_e, f2_o);
    */

    // ------------------------------------------------------------------------
    // [ OPTION 2 ] : D_4 LATTICE (24-Cell Honeycomb) --- [CURRENTLY ACTIVE]
    // ------------------------------------------------------------------------

    cout << "Evaluating D_4 (24-Cell) Lattice for R = " << R << "\n";
    vector<long long> f_even(r2 + 1, 0), f_odd(r2 + 1, 0);
    build_D4_parity_profiles(R, r2, f_even, f_odd);
    total_points = execute_D4_volume(r2, f_even, f_odd);


    // ------------------------------------------------------------------------
    // [ OPTION 3 ] : E_8 LATTICE (Gosset Lattice / 8-Space)
    // ------------------------------------------------------------------------
    /*
    cout << "Evaluating E_8 (Gosset) Lattice for R = " << R << "\n";
    vector<long long> f_even_4(r2 + 1, 0), f_odd_4(r2 + 1, 0);
    build_D4_parity_profiles(R, r2, f_even_4, f_odd_4);
    vector<long long> f4_D4 = build_D4_array(r2, f_even_4, f_odd_4);
    total_points = evaluate_E8_lattice(r2, f4_D4);
    */

    // ========================================================================

    auto stop_time = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop_time - start_time);

    cout << "========================================\n";
    cout << "Total Lattice Points : " << total_points << "\n";
    cout << "Execution Time       : " << duration.count() << " ms\n";
    cout << "========================================\n";

    return 0;
}