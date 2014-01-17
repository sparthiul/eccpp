#ifndef _EC_MLL_DECISION_TREE_H_
#define _EC_MLL_DECISION_TREE_H_

#include <vector>
#include <string>
#include <map>

#include "ec_ml_common.h"

class DecisionTree : public IClassifier
{
public:
    DecisionTree();
    virtual ~DecisionTree();
    // the first column is the labels, each row has the same feature values, dimensions.size() == data[0].size()
    bool train(const std::vector<std::vector<double> >& data, const std::vector<int>& dimensions, int class_count);
    virtual int classify(const std::vector<double>& feature_vector) const;
    void reset();
    bool load_model(const std::string& model_file);
    bool save_model(const std::string& model_file) const;

private:
    struct TreeNode
    {
    public:
        TreeNode* first_child;
        TreeNode* next_sibling;
        int feature_id;
        int feature_value;
        int class_id;
        double class_confidence;
    };

    void clear_data();
    bool prepare_data(const std::vector<std::vector<double> >& data, const std::vector<int>& dimensions, int class_count);
    TreeNode* create_tree(std::vector<std::pair<int, int> >& base_path, std::vector<bool>& selected_features, double base_entropy) const;
    int determine_node_class(const std::vector<std::pair<int, int> >& path, double& confidence) const;
    int select_feature(std::vector<std::pair<int, int> >& base_path, const std::vector<bool>& selected_features, double base_entropy, double& min_entropy) const;
    double calc_feature_entropy(std::vector<std::pair<int, int> >& base_path, int feature_id) const;
    double calc_path_entropy(const std::vector<std::pair<int, int> >& path) const;
    double calc_path_probability_for_class(const std::vector<std::pair<int, int> >& path, int c) const;
    double calc_path_probability_by_class(const std::vector<std::pair<int, int> >& path, int c) const;
    double calc_path_probability(const std::vector<std::pair<int, int> >& path) const;
    double get_feature_value_probability_by_class(int feature_id, int feature_value, int c) const;
    double get_feature_value_probability(int feature_id, int feature_value) const;
    double get_class_probability(int c) const;

    static std::string gen_feature_value_class_key(int feature_id, int feature_value, int c);
    static std::string gen_feature_value_key(int feature_id, int feature_value);
    static void print_tree(std::ostream& stream, TreeNode* tree, int parent_tree_id, int& tree_id);
    static void delete_tree(TreeNode* tree);

    bool _initialized;
    int _n;
    int _m;//just be used in train process, if the model is loaded, it's -1.
    int _class_count;
    TreeNode* _tree;
    std::vector<int> _dimensions;

    // below fields are just used in tree build process
    std::map<std::string, double> _fv_map;
    std::map<std::pair<std::string, int>, double> _fvc_map;
    std::vector<double> _class_probabilities;
};

#endif
