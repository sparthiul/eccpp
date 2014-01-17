#include "ec_ml_basics.h"

#include <cmath>
#include <cassert>

using namespace std;

double MLBasics::entropy(const std::vector<double>& probabilities)
{
    double result = 0.0;
    for (size_t i = 0; i < probabilities.size(); ++i)
    {
        double p = probabilities[i];
        assert(p > 0.0);
        result += -1.0 * p * log(p); //ln(p)
    }

    return result;
}

double MLBasics::linear_combine(const std::vector<double>& values, const std::vector<double>& weights)
{
    assert(values.size() == weights.size());
    double result;
    for (size_t i = 0; i < values.size(); ++i)
    {
        result += values[i] * weights[i];
    }

    return result;
}

double MLBasics::bayesian(double pa, double pb, double pba)
{
    assert(pb != 0.0);
    return pba * pa / pb;
}
