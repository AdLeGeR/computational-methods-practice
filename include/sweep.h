#include <iostream>
#include <string>
#include <vector>

struct TridiagonalSystem {
    std::size_t n = 0;

    double kp1 = 0.0, mu1 = 0.0;
    double kp2 = 0.0, mu2 = 0.0;

    std::vector<double> A, C, B, phi;

    explicit TridiagonalSystem(std::size_t nodes = 0) { resize(nodes); }

    void resize(std::size_t nodes) {
        n = nodes;
        A.assign(n + 1, 0.0);
        C.assign(n + 1, 0.0);
        B.assign(n + 1, 0.0);
        phi.assign(n + 1, 0.0);
    }
};


class ISweepStrategy {
public:
    virtual ~ISweepStrategy() = default;
    virtual std::vector<double> sweep(const TridiagonalSystem& system) const = 0;
protected:
    double eps = 1e-300;
};



class GenetalSweepStrategy : public ISweepStrategy {
public:
    std::vector<double> sweep(const TridiagonalSystem& system) const override;
};

class OptimizedSweepStrategy : public ISweepStrategy {
public:
    std::vector<double> sweep(const TridiagonalSystem& system) const override {

    }
};

