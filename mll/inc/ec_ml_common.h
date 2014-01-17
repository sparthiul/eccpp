#ifndef _EC_ML_COMMON_H_
#define _EC_ML_COMMON_H_

#include <vector>

class IClassifier
{
public:
    virtual int classify(const std::vector<double>& feature_vector) const = 0;
    virtual ~IClassifier()
    {
    }
};

#endif
