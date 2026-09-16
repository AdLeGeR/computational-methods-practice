#include <iostream>
#include <chrono>
#include <cmath>
#include <sweep.h>

int main() {
    int n;
    std::cout << "enter n: ";
    std::cin >> n;
    if (n < 2) {
        std::cout << "n < 2" << std::endl;
        return 1;
    }

    const double h = 1.0 / static_cast<double>(n);

    TridiagonalSystem s(n);
    for (int i = 1; i <= n - 1; i++) {
        s.A[i] = 12.0 / (h * h);
        s.C[i] = 24.0 / (h * h) + 5.0;
        s.B[i] = 12.0 / (h * h);
        double xi = static_cast<double>(i) * h;
        s.phi[i] = -2110.0 + 450.0 * xi * xi;
    }
    s.mu1 = 10;
    s.mu2 = 100;

//время для ген прогонки
    auto tGenStart = std::chrono::high_resolution_clock::now();
    auto v = GeneralSweepStrategy().sweep(s);
    auto tGenEnd = std::chrono::high_resolution_clock::now();
    double genTime = std::chrono::duration<double, std::micro>(tGenEnd - tGenStart).count();

    double maxGen = 0.0;
    std::cout << "\n--- general sweep ---" << std::endl;
    std::cout << "i\txi\tvti\tvi\tvti-vi" << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    for (int i = 0; i <= n; i++) {
        double xi  = i * h;
        double vti = 10.0 + 90.0 * xi * xi;
        double maxr = std::abs(vti - v[i]);
        if (maxr > maxGen) maxGen = maxr;
        std::cout << i << "\t" << xi << "\t" << vti << "\t" << v[i]
                  << "\t" << vti - v[i] << std::endl;
    }
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "max|vti-vi|: " << maxGen << std::endl;

//время для опт прогонки
    auto tOptStart = std::chrono::high_resolution_clock::now();
    auto vo = OptimizedSweepStrategy().sweep(s);
    auto tOptEnd = std::chrono::high_resolution_clock::now();
    double optTime = std::chrono::duration<double, std::micro>(tOptEnd - tOptStart).count();

    double maxOpt = 0.0;
    std::cout << "\n--- optimized sweep ---" << std::endl;
    std::cout << "i\txi\tvti\tvi\tvti-vi" << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    for (int i = 0; i <= n; i++) {
        double xi  = i * h;
        double vti = 10.0 + 90.0 * xi * xi;
        double maxr = std::abs(vti - vo[i]);
        if (maxr > maxOpt) maxOpt = maxr;
        std::cout << i << "\t" << xi << "\t" << vti << "\t" << vo[i]
                  << "\t" << vti - vo[i] << std::endl;
    }
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "max|vti-vi|: " << maxOpt << std::endl;

    std::cout << "\ntime general: " << genTime << " us" << std::endl;
    std::cout << "time optimized: " << optTime << " us" << std::endl;

    std::cout << "\n--for the report table--" << std::endl;
    ISweepStrategy* strategies[] = {
        new GeneralSweepStrategy(),
        new OptimizedSweepStrategy()
    };

    for (auto* strategy : strategies) {
        auto vpoly = strategy->sweep(s);
        double maxpoly = 0.0;
        for (int i = 0; i <= n; i++) {
            double xi  = i * h;
            double vti = 10.0 + 90.0 * xi * xi;
            double maxr = std::abs(vti - vpoly[i]);
            if (maxr > maxpoly) maxpoly = maxr;
        }
        std::cout <<  "version of the sweep method: " << (dynamic_cast<GeneralSweepStrategy*>(strategy) ? "general" : "optimized")
                  << ", max|vti-vi|: " << maxpoly << std::endl;
        delete strategy;
    }

    return 0;
}