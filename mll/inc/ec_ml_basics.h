#ifndef _EC_ML_BASICS_H_
#define  EC_ML_BASICS_H_

#include <vector>

class MLBasics
{
public:
    static double entropy(const std::vector<double>& probabilities);
    static double linear_combine(const std::vector<double>& array, const std::vector<double>& weights);

    //P(a|b) = P(b|a) * P(a) / P(b)
    static double bayesian(double pa, double pb, double pba);
};

#endif
