#include <exception>
#include <iostream>
#include <vector>
#include "sweep.h"
#include "sweep_runner.h"

static double exactSolution(double x) {
    return 10.0 + 90.0 * x * x;
}
\
static double phiAt(double x) {
    return -2110.0 + 450.0 * x * x;
}

static TridiagonalSystem makeSystem(std::size_t n) {
    const double h = 1.0 / static_cast<double>(n);
    const double k = 12.0 / (h * h);

    TridiagonalSystem s(n);
    for (std::size_t i = 1; i < n; ++i) {
        s.A[i] = k;
        s.B[i] = k;
        s.C[i] = 2.0 * k + 5.0;
        s.phi[i] = phiAt(static_cast<double>(i) * h);
    }
    s.mu1 = 10.0;
    s.mu2 = 100.0;
    return s;
}

static TridiagonalSystem makeOptimizedSystem(std::size_t n) {
    const double h = 1.0 / static_cast<double>(n);
    const double k = 12.0 / (h * h);

    // n и размеры массивов согласованы, A/B/C хранятся один раз
    TridiagonalSystem s = TridiagonalSystem::withConstantCoeffs(n, k, k, 2.0 * k + 5.0);
    for (std::size_t i = 1; i < n; ++i) {
        s.phi[i] = phiAt(static_cast<double>(i) * h);
    }
    s.mu1 = 10.0;
    s.mu2 = 100.0;
    return s;
}

int main() {
#ifndef NDEBUG
    std::cout << "WARNING: it's debug mod. "
        "build in Release.\n";
#endif

    int n;
    std::cout << "enter n: ";
    if (!(std::cin >> n) || n < 2) {
        std::cerr << "n must be an integer >= 2\n";
        return 1;
    }

    GeneralSweepStrategy general;
    OptimizedSweepStrategy optimized;

    // таблицу целиком печатаем только для небольших n, иначе вывод на миллионы строк
    const int maxTableN = 100;

    try {
        // 1) решение для введенного n
        {
            const TridiagonalSystem system = makeSystem(static_cast<std::size_t>(n));
            const TridiagonalSystem osystem = makeOptimizedSystem(static_cast<std::size_t>(n));

            const std::vector<SweepResult> results =
                benchmark({ { &general, &system }, { &optimized, &osystem } }, exactSolution, true);

            if (n <= maxTableN) {
                printTable(results[0], system, exactSolution);
                printTable(results[1], osystem, exactSolution);
            }
            printSummary(results);
        }

        // 2) зависимость времени и погрешности от n.
        //    m - отдельная переменная, введенное n не перекрывается
        std::vector<SweepResult> all;
        for (std::size_t m = 10; m <= 10'000'000; m *= 10) {
            const TridiagonalSystem system = makeSystem(m);
            const TridiagonalSystem osystem = makeOptimizedSystem(m);

            // решения не храним (keepSolution = false), иначе память копится
            const std::vector<SweepResult> results =
                benchmark({ { &general, &system }, { &optimized, &osystem } }, exactSolution, false);
            all.insert(all.end(), results.begin(), results.end());
        }   // system и osystem освобождаются на каждой итерации
        printSummary(all);
    }
    catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
