#include "ec_decision_tree.h"
#include "ec_utils.h"

#include <algorithm>
#include <vector>
#include <cstring>
#include <iostream>

using namespace std;

int train(const string& train_file, const string& model_file)
{
    stringstream train_stream;
    SUCCESS(read_file(train_file.c_str(), train_stream), -2);

    string comment_header;
    std::getline(train_stream, comment_header);

    vector<int> header_fields;
    // feature count, class_count, f1_dimen, f2_dimen, ...
    SUCCESS(read_stream_line(train_stream, -1, " ", header_fields) || header_fields.size() < 3, -3);

    int n = header_fields[0];
    int class_count = header_fields[1];

    SUCCESS(n > 0 && class_count > 0 && header_fields.size() == static_cast<size_t>(n) + 2, -4);
    SUCCESS(std::find_if(header_fields.begin() + 2, header_fields.end(), less_equal_to_zero<int>) == header_fields.end(), -5);
    vector<int> dimensions(n);
    std::copy(header_fields.begin() + 2, header_fields.end(), dimensions.begin());

    vector<vector<double> > feature_matrix;
    SUCCESS(read_stream_matrix(train_stream, n + 1, " ", feature_matrix) == 0, -6);

    DecisionTree tree;
    SUCCESS(tree.train(feature_matrix, dimensions, class_count), -7);

    SUCCESS(tree.save_model(model_file), -8);

    return 0;
}

int classify(const string& model_file, const vector<double>& feature_vector)
{
    DecisionTree tree;
    SUCCESS(tree.load_model(model_file), -2);
    return tree.classify(feature_vector);
}

int main(int argc, char* argv[])
{
    const char* usage = 
        "parameters: train train_file model_file\n"
        "            classify model_file f1, f2, ...";

    SUCCESS(validate_args(argc, argv, usage, 2, -2), -1);
    if (strncmp(argv[1], "train", strlen("train")) == 0)
    {
        if (argc < 4)
        {
            cout << usage;
            return -1;
        }

        return train(argv[2], argv[3]);
    }
    else
    {
        if (argc < 3)
        {
            cout << usage;
            return -1;
        }

        vector<double> feature_vector;
        double feature;
        for (int i = 3; i < argc; ++i)
        {
            SUCCESS(string_to_double(argv[i], strlen(argv[i]), feature), -1);
            feature_vector.push_back(feature);
        }

        return classify(argv[2], feature_vector);
    }
}
