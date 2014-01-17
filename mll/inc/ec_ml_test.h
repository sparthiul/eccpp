#ifndef _EC_ML_TEST_H_
#define _EC_ML_TEST_H_

class MLTest
{
public:
    static double cross_validation(const IClassifier* classifier, const std::vector<std::vector<double> >& data);
    static double get_classifier_results(const IClassifier* classifier, const std::vector<std::vector<double> >& data, std::vector<double>& results);
};

#endif
