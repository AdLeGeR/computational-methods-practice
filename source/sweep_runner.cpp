#include "sweep_runner.h"
#include <chrono>
#include <cmath>
#include <iostream>

SweepResult runSweep(const ISweepStrategy& strategy, const TridiagonalSystem& s, ExactFn exact) {
    SweepResult r;
    r.name = strategy.name();

    auto start = std::chrono::steady_clock::now();
    r.v = strategy.sweep(s);
    auto end = std::chrono::steady_clock::now();
    r.timeUs = std::chrono::duration<double, std::micro>(end - start).count();

    const double h = 1.0 / s.n;
    for (std::size_t i = 0; i < r.v.size(); ++i) {
        double err = std::abs(exact(i * h) - r.v[i]);
        if (err > r.maxError) r.maxError = err;
    }
    return r;
}

void printTable(const SweepResult& r, ExactFn exact) {
    const double h = 1.0 / (r.v.size() - 1);

    std::cout << "\n--- " << r.name << " sweep ---\n"
        << "i\txi\tvti\tvi\tvti-vi\n"
        << "---------------------------------------------\n";
    for (std::size_t i = 0; i < r.v.size(); ++i) {
        double xi = i * h;
        double vti = exact(xi);
        std::cout << i << '\t' << xi << '\t' << vti << '\t' << r.v[i]
            << '\t' << vti - r.v[i] << '\n';
    }
    std::cout << "---------------------------------------------\n"
        << "max|vti-vi|: " << r.maxError << '\n';
}

void printSummary(const std::vector<SweepResult>& results) {
    std::cout << "\n-- for the report table --\n";
    for (const auto& r : results) {
        std::cout << "version: " << r.name
            << ", max|vti-vi|: " << r.maxError
            << ", time: " << r.timeUs << " us\n";
    }
}