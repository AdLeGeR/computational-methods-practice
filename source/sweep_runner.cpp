#include "sweep_runner.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>

using Clock = std::chrono::steady_clock;

double maxError(const std::vector<double>& v, ExactFn exact) {
    if (v.size() < 2) return 0.0;
    const double h = 1.0 / static_cast<double>(v.size() - 1);
    double m = 0.0;
    for (std::size_t i = 0; i < v.size(); ++i) {
        m = std::max(m, std::abs(exact(static_cast<double>(i) * h) - v[i]));
    }
    return m;
}

std::vector<double> residual(const TridiagonalSystem& s, const std::vector<double>& y) {
    std::vector<double> r(s.n + 1, 0.0);
    for (std::size_t i = 1; i < s.n; ++i) {
        r[i] = s.a(i) * y[i - 1] - s.c(i) * y[i] + s.b(i) * y[i + 1] + s.phi[i];
    }
    return r;
}

std::vector<SweepResult> benchmark(const std::vector<SweepCase>& cases, ExactFn exact, bool keepSolution, int samples) {
    const std::size_t k = cases.size();
    std::vector<SweepWorkspace> ws(k);
    std::vector<std::vector<double>> y(k);
    std::vector<std::vector<double>> times(k);
    std::vector<std::size_t> repeats(k);

    for (std::size_t i = 0; i < k; ++i) {
        // прогрев: первый вызов выделяет память под ws и y, прогревает кэши. в замер не идет
        cases[i].strategy->solve(*cases[i].system, ws[i], y[i]);

        // при маленьком n один вызов идет доли микросекунды - это сравнимо с точностью таймера.
        // поэтому меряем пачку вызовов и делим на их количество
        const std::size_t n = cases[i].system->n;
        repeats[i] = std::max<std::size_t>(1, 1'000'000 / n);
    }

    for (int sm = 0; sm < samples; ++sm) {
        for (std::size_t j = 0; j < k; ++j) {
            // чередуем порядок, чтобы ни одна стратегия не шла всегда первой или последней
            const std::size_t i = (sm % 2 == 0) ? j : k - 1 - j;

            const auto start = Clock::now();
            for (std::size_t r = 0; r < repeats[i]; ++r) {
                cases[i].strategy->solve(*cases[i].system, ws[i], y[i]);
            }
            const auto end = Clock::now();

            times[i].push_back(std::chrono::duration<double, std::micro>(end - start).count()
                / static_cast<double>(repeats[i]));
        }
    }

    std::vector<SweepResult> results(k);
    for (std::size_t i = 0; i < k; ++i) {
        // медиана устойчива к случайным выбросам (прерывания, другие процессы)
        auto& t = times[i];
        std::nth_element(t.begin(), t.begin() + t.size() / 2, t.end());

        SweepResult& r = results[i];
        r.name = cases[i].strategy->name();
        r.n = cases[i].system->n;
        r.timeUs = t[t.size() / 2];
        r.maxError = maxError(y[i], exact);
        if (keepSolution) r.v = std::move(y[i]);
    }
    return results;
}

void printTable(const SweepResult& r, const TridiagonalSystem& s, ExactFn exact) {
    const double h = 1.0 / static_cast<double>(r.v.size() - 1);
    const std::vector<double> res = residual(s, r.v);

    std::cout << "\n--- " << r.name << " sweep ---\n"
        << "i\txi\tvti\tvi\tvti-vi\tri\n"
        << "---------------------------------------------\n";
    for (std::size_t i = 0; i < r.v.size(); ++i) {
        const double xi = static_cast<double>(i) * h;
        const double vti = exact(xi);
        std::cout << i << '\t' << xi << '\t' << vti << '\t' << r.v[i]
            << '\t' << vti - r.v[i] << '\t' << res[i] << '\n';
    }
    std::cout << "---------------------------------------------\n"
        << "max|vti-vi|: " << r.maxError << '\n';
}

void printSummary(const std::vector<SweepResult>& results) {
    std::cout << "\n-- for the report table --\n"
        << "n\tversion\t\tmax|vti-vi|\ttime, us\n";
    for (const auto& r : results) {
        std::cout << r.n << '\t' << r.name << (r.name.size() < 8 ? "\t\t" : "\t")
            << r.maxError << '\t' << r.timeUs << '\n';
    }
}
