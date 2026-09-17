#pragma once
#include <string>
#include <vector>
#include "sweep.h"

// точное решение v(x)
using ExactFn = double (*)(double);

struct SweepResult {
    std::string name;
    std::vector<double> v;
    double maxError = 0.0;
    double timeUs = 0.0;
};

SweepResult runSweep(const ISweepStrategy& strategy, const TridiagonalSystem& s, ExactFn exact);
void printTable(const SweepResult& r, ExactFn exact);
void printSummary(const std::vector<SweepResult>& results);