#include <iostream>
#include <string>
#include <vector>

// удобная структура для передачи в методы sweep, добавил для удобства. один раз создал структуру - везде ее передаешь

struct TridiagonalSystem {
    std::size_t n = 0;
    
    // kp - это каппа из лекции
    double kp1 = 0.0, mu1 = 0.0;
    double kp2 = 0.0, mu2 = 0.0;

    std::vector<double> A, C, B, phi;
    
    // конструктор 
    explicit TridiagonalSystem(std::size_t nodes = 0) { resize(nodes); }

    // инициализация | переопределение структуры
    void resize(std::size_t nodes) {
        n = nodes;
        A.assign(n + 1, 0.0);
        C.assign(n + 1, 0.0);
        B.assign(n + 1, 0.0);
        phi.assign(n + 1, 0.0);
    }
};


//класс интерфейс для наследников. I означает, что класс не может иметь своей реализации, а является только типом для своих наследников
class ISweepStrategy {
public:
    virtual ~ISweepStrategy() = default;
    virtual std::vector<double> sweep(const TridiagonalSystem& system) const = 0;
protected:
    double eps = 1e-300;
};


//конкретная реализация - класс реализующий стандартную прогонку 
class GenetalSweepStrategy : public ISweepStrategy {
public:
    std::vector<double> sweep(const TridiagonalSystem& system) const override;
};

//конкретная реализация - класс реализующий оптимизированную прогонку
class OptimizedSweepStrategy : public ISweepStrategy {
public:
    std::vector<double> sweep(const TridiagonalSystem& system) const override {

    }
};

//зачем это все надо? в каком нибудь методе где будет использоваться прогонка в аргументах можно будет написать IsweepStrategy& strategy, но передавать можно в этот метод уже объекты любых двух наследников. и в зависимости от того, что мы передадим метод будет вести себя по разному. в нашем случае если передадим оптимизированну, то, например, скорость работы будет выше, если же мы передавали общий то меньше

