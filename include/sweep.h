#pragma once
#include <cstddef>
#include <string>
#include <vector>

// Трёхдиагональная система (обозначения из лекции):
//   A[i]*y[i-1] - C[i]*y[i] + B[i]*y[i+1] = -phi[i],   i = 1..n-1
//   y[0] = kp1*y[1] + mu1,   y[n] = kp2*y[n-1] + mu2
// удобная структура для передачи в методы sweep. один раз создал структуру - везде ее передаешь

struct TridiagonalSystem {
    std::size_t n = 0;

    // kp - это каппа из лекции
    double kp1 = 0.0, mu1 = 0.0;
    double kp2 = 0.0, mu2 = 0.0;

    // false: A, B, C хранятся полностью, размер n+1.
    // true:  коэффициенты постоянные, A, B, C имеют размер 2 и значение лежит в [1].
    // phi в обоих случаях имеет размер n+1.
    bool constantCoeffs = false;

    std::vector<double> A, C, B, phi;

    // конструктор
    explicit TridiagonalSystem(std::size_t nodes = 0) { resize(nodes); }

    // инициализация | переопределение структуры (полные массивы)
    void resize(std::size_t nodes) {
        n = nodes;
        constantCoeffs = false;
        A.assign(n + 1, 0.0);
        C.assign(n + 1, 0.0);
        B.assign(n + 1, 0.0);
        phi.assign(n + 1, 0.0);
    }

    // система с постоянными коэффициентами - для оптимизированной прогонки.
    // n и размеры массивов согласованы, флаг явно говорит, как хранятся A/B/C
    static TridiagonalSystem withConstantCoeffs(std::size_t nodes, double a, double b, double c) {
        TridiagonalSystem s;
        s.n = nodes;
        s.constantCoeffs = true;
        s.A = { 0.0, a };
        s.B = { 0.0, b };
        s.C = { 0.0, c };
        s.phi.assign(nodes + 1, 0.0);
        return s;
    }

    // коэффициенты i-й строки независимо от способа хранения (для невязки, вывода и т.п.)
    double a(std::size_t i) const { return constantCoeffs ? A[1] : A[i]; }
    double b(std::size_t i) const { return constantCoeffs ? B[1] : B[i]; }
    double c(std::size_t i) const { return constantCoeffs ? C[1] : C[i]; }
};

// рабочие массивы прогонки. выделяются один раз и переиспользуются,
// чтобы выделение памяти не попадало в замер времени
struct SweepWorkspace {
    std::vector<double> alpha, beta;
};


//класс интерфейс для наследников. I означает, что класс не может иметь своей реализации, а является только типом для своих наследников
class ISweepStrategy {
public:
    virtual ~ISweepStrategy() = default;

    // основной метод: решение пишется в y. если ws и y уже нужного размера, память не выделяется
    virtual void solve(const TridiagonalSystem& s, SweepWorkspace& ws, std::vector<double>& y) const = 0;

    // удобная обертка: сама выделяет память и возвращает решение
    std::vector<double> sweep(const TridiagonalSystem& s) const {
        SweepWorkspace ws;
        std::vector<double> y;
        solve(s, ws, y);
        return y;
    }

    virtual std::string name() const = 0;
};


//конкретная реализация - класс реализующий стандартную прогонку (любые A[i], B[i], C[i])
class GeneralSweepStrategy : public ISweepStrategy {
public:
    void solve(const TridiagonalSystem& s, SweepWorkspace& ws, std::vector<double>& y) const override;
    std::string name() const override { return "general"; }
};

//конкретная реализация - класс реализующий оптимизированную прогонку (постоянные A, B, C)
class OptimizedSweepStrategy : public ISweepStrategy {
public:
    void solve(const TridiagonalSystem& s, SweepWorkspace& ws, std::vector<double>& y) const override;
    std::string name() const override { return "optimized"; }
};


//зачем это все надо? в каком нибудь методе где будет использоваться прогонка в аргументах можно
// будет написать ISweepStrategy& strategy, но передавать можно в этот метод уже объекты любых двух наследников.
// и в зависимости от того, что мы передадим метод будет вести себя по разному.
// в нашем случае если передадим оптимизированную, то, например, скорость работы будет выше, если же мы передавали общий то меньше
