#include "ec_test_common.h"

#include "ec_ml_basics.h"
#include "ec_utils.h"

#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <cmath>

using namespace std;

TEST(MLBasics, entropy)
{
    double input_array[] = {0.5, 0.2};
    std::vector<double> input(input_array, input_array + sizeof(input_array) / sizeof(input_array[0]));
    EXPECT_TRUE(fabs(MLBasics::entropy(input) - 0.668461) <= 0.0001);
}
