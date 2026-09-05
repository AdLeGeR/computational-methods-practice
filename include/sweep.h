#include <iostream>
#include <string>

class ISweepStrategy {
public:
    virtual ~ISweepStrategy() = default;
    virtual void sweep() const = 0;
};



class GenetalSweepStrategy : public ISweepStrategy {
public:
    void sweep() const override {

    }
};

class OptimizedSweepStrategy : public ISweepStrategy {
public:
    void sweep() const override {

    }
};