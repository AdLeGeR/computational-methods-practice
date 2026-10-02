#pragma once
#include <string>
#include <vector>
#include "sweep.h"

// точное решение v(x)
using ExactFn = double (*)(double);

struct SweepResult {
    std::string name;
    std::size_t n = 0;
    std::vector<double> v;   // решение (пустое, если keepSolution == false)
    double maxError = 0.0;
    double timeUs = 0.0;     // медиана времени одного вызова, без выделения памяти
};

// что запускать: стратегия + система для нее
struct SweepCase {
    const ISweepStrategy* strategy;
    const TridiagonalSystem* system;
};

// замер нескольких стратегий в одинаковых условиях:
// прогрев, буферы выделены заранее, порядок запуска чередуется, берется медиана
std::vector<SweepResult> benchmark(const std::vector<SweepCase>& cases, ExactFn exact,
    bool keepSolution, int samples = 11);

double maxError(const std::vector<double>& v, ExactFn exact);

// невязка r_i = A*y[i-1] - C*y[i] + B*y[i+1] + phi[i], r_0 = r_n = 0
std::vector<double> residual(const TridiagonalSystem& s, const std::vector<double>& y);

void printTable(const SweepResult& r, const TridiagonalSystem& s, ExactFn exact);
void printSummary(const std::vector<SweepResult>& results);
