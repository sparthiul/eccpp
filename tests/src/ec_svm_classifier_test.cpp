#include "ec_test_common.h"
#include "ec_svm_classifier.h"

#include <vector>

using namespace std;

TEST(SvmClassifier, main)
{
    const double features[][4] = 
    {
        {0.4, 0, 0, 0},
        {0,   1, 0, 0},
        {0.4, 1, 1, 0},
        {0,   1, 0, 0},
        {0.8, 0, 0, 1},
    };

    const double labels[] =
    {
        1, 0, 1, 0, 1,
    };

    SvmClassifier classifier;
    bool success = classifier.init("test_data/list_page_classifier_test.svm");
    EXPECT_TRUE(success);

    for (size_t i = 0; i < sizeof(features) / sizeof(double) / 4; ++i)
    {
        vector<double> feature(features[i], features[i] + 4);
        double label = classifier.classify(feature);
        EXPECT_EQ(labels[i], label);
    }
}

TEST(SvmClassifier, init_by_string)
{
    const double features[][4] = 
    {
        {0.4, 0, 0, 0},
        {0,   1, 0, 0},
        {0.4, 1, 1, 0},
        {0,   1, 0, 0},
        {0.8, 0, 0, 1},
    };

    const double labels[] =
    {
        1, 0, 1, 0, 1,
    };

    SvmClassifier classifier;
    stringstream model_string;
    read_file("test_data/list_page_classifier_test.svm", model_string);

    bool success = classifier.init_by_string(model_string.str());
    EXPECT_TRUE(success);

    for (size_t i = 0; i < sizeof(features) / sizeof(double) / 4; ++i)
    {
        vector<double> feature(features[i], features[i] + 4);
        double label = classifier.classify(feature);
        EXPECT_EQ(labels[i], label);
    }
}
