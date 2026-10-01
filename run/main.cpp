#include <exception>
#include <iostream>
#include <vector>
#include "sweep.h"
#include "sweep_runner.h"

static double exactSolution(double x) {
    return 10.0 + 90.0 * x * x;
}

static TridiagonalSystem makeSystem(int n) {
    const double h = 1.0 / n;
    const double k = 12.0 / (h * h);

    TridiagonalSystem s(n);
    for (int i = 1; i < n; ++i) {
        double xi = i * h;
        s.A[i] = k;
        s.B[i] = k;
        s.C[i] = 2.0 * k + 5.0;
        s.phi[i] = -2110.0 + 450.0 * xi * xi;
    }
    s.mu1 = 10.0;
    s.mu2 = 100.0;
    return s;
}

static TridiagonalSystem makeOptimizedSystem(int n) {
    const double h = 1.0 / n;
    const double k = 12.0 / (h * h);

    TridiagonalSystem s(2);

    s.A[1] = k;
    s.B[1] = k;
    s.C[1] = 2.0 * k + 5.0;
    s.n = n;
    s.phi.assign(n + 1, 0);
    for (int i = 1; i < n; ++i) {
        double xi = i * h;
        s.phi[i] = -2110.0 + 450.0 * xi * xi;
    }
    s.mu1 = 10.0;
    s.mu2 = 100.0;
    return s;
}

int main() {
    int n;
    std::cout << "enter n: ";
    if (!(std::cin >> n) || n < 2) {
        std::cerr << "n must be an integer >= 2\n";
        return 1;
    }

    TridiagonalSystem system = makeSystem(n);
    TridiagonalSystem osystem = makeOptimizedSystem(n);


    GeneralSweepStrategy general;
    OptimizedSweepStrategy optimized;
    const ISweepStrategy* strategies[] = { &general, &optimized };

    try {
        std::vector<SweepResult> results;
        for (const ISweepStrategy* strategy : strategies) {
            TridiagonalSystem tsystem;
//            if (strategy->name() == "optimized") tsystem = makeOptimizedSystem(n);
//            else tsystem = makeSystem(n);
//            results.push_back(runSweep(*strategy, tsystem, exactSolution));
//            printTable(results.back(), exactSolution);
            for(int n = 10; n < 100000000; n *= 10){
                if (strategy->name() == "optimized") tsystem = makeOptimizedSystem(n);
                else tsystem = makeSystem(n);
                results.push_back(runSweep(*strategy, tsystem, exactSolution));
            }
        }
        printSummary(results);
    }
    catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}