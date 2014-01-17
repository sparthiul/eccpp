#include "ec_decision_tree.h"

#include <vector>
#include <cassert>
#include <utility>

#include "ec_utils.h"
#include "ec_ml_basics.h"

using namespace std;

static const double c_min_probability_factor = 1.0;
static const double c_min_entropy = 0.001;
static const double c_min_information_gain = 0.001;

/*
Notes:
1. internal tree representation: array to store full n-d tree, use dynamic link tree for future extensions, select the latter now

*/

DecisionTree::DecisionTree() :
    _initialized(false),
    _n(0),
    _m(0),
    _class_count(0),
    _tree(NULL),
    _dimensions()
{
}

DecisionTree::~DecisionTree()
{
    this->reset();
}

bool DecisionTree::train(const std::vector<std::vector<double> >& data, const std::vector<int>& dimensions, int class_count)
{
    this->reset();

    if (!this->prepare_data(data, dimensions, class_count))
    {
        this->clear_data();
        return false;
    }

    vector<pair<int, int> > base_path;
    vector<bool> selected_features(this->_n, false);
    double initial_entropy = MLBasics::entropy(this->_class_probabilities);
    this->_tree = this->create_tree(base_path, selected_features, initial_entropy);
    this->clear_data();
    this->_initialized = true;
    return true;
}


int DecisionTree::classify(const std::vector<double>& feature_vector) const
{
    if (!this->_initialized)
    {
        return -1;
    }

    if (feature_vector.size() != static_cast<size_t>(this->_n))
    {
        return -2;
    }

    TreeNode* node = this->_tree;
    while (true)
    {
        double feature_value = feature_vector[node->feature_id];
        TreeNode* next_node = NULL;
        for (TreeNode* child = node->first_child; child != NULL; child = child->next_sibling)
        {
            if (child->feature_value == static_cast<int>(feature_value))
            {
                next_node = child;
                break;
            }
        }

        if (next_node == NULL)
        {
            return node->class_id;
        }
        else
        {
            node = next_node;
        }
    }
}

bool DecisionTree::save_model(const std::string& file_name) const
{
    if (!this->_initialized)
    {
        return false;
    }

    stringstream stream;

    stream << this->_n << " " << this->_class_count << " ";

    for (size_t i = 0; i < this->_dimensions.size(); ++i)
    {
        stream << this->_dimensions[i] << " ";
    }

    stream << endl;

    int tree_id = 0;
    DecisionTree::print_tree(stream, this->_tree, -1, tree_id);
    SUCCESS(write_file(file_name.c_str(), stream.str()), false);
    return true;
}

void DecisionTree::print_tree(ostream& stream, TreeNode* tree, int parent_tree_id, int& tree_id)
{
    int curr_tree_id = tree_id;
    stream << curr_tree_id << " " << parent_tree_id << " " << tree->feature_id << " " << tree->feature_value << " " << tree->class_id << " " << tree->class_confidence << endl;
    for (TreeNode* child = tree->first_child; child != NULL; child = child->next_sibling)
    {
        DecisionTree::print_tree(stream, child, curr_tree_id, ++tree_id);
    }
}

bool DecisionTree::load_model(const std::string& file_name)
{
    this->reset();
    char delimeter;
    stringstream line_stream;

    vector<string> node_lines;
    SUCCESS(read_lines(file_name.c_str(), node_lines) && node_lines.size() > 1, false);

    string line = node_lines[0];
    line_stream << line;
    line_stream >> this->_n >> delimeter >> this->_class_count >> delimeter;
    this->_dimensions.reserve(this->_n);
    for (int i = 0; i < this->_n; ++i)
    {
        line_stream >> this->_dimensions[i] >> delimeter;
    }

    line_stream >> delimeter;

    int curr_tree_id, parent_tree_id;

    std::map<int, TreeNode**> node_map;
    //Note: optimization approach: just save the tree path from the current node to root.
    for (size_t i = 1; i < node_lines.size(); ++i)
    {
        TreeNode* node = new TreeNode();
        node->first_child = NULL;
        node->next_sibling = NULL;

        line_stream << node_lines[i];
        line_stream >> curr_tree_id >> delimeter >> parent_tree_id >> delimeter >> node->feature_id >> delimeter >> node->feature_value >> delimeter >> node->class_id >> delimeter >> node->class_confidence >> delimeter;

        if (parent_tree_id == -1)
        {
            this->_tree = node;
        }
        else
        {
            map<int, TreeNode**>::iterator iter = node_map.find(parent_tree_id);
            assert(iter != node_map.end());
            *(iter->second) = node;
            iter->second = &(node->next_sibling);
        }

        node_map[curr_tree_id] = &(node->first_child);
    }

    this->_m = -1;
    this->_initialized = true;
    return true;
}

void DecisionTree::clear_data()
{
    this->_fv_map.clear();
    this->_fvc_map.clear();
    this->_class_probabilities.clear();
}

void DecisionTree::reset()
{
    if (this->_tree != NULL)
    {
        DecisionTree::delete_tree(this->_tree);
    }

    this->_tree = NULL;

    this->clear_data();
    this->_initialized = false;
}

void DecisionTree::delete_tree(TreeNode* tree)
{
    for (TreeNode* child = tree->first_child; child != NULL; )
    {
        TreeNode* next_sibling = child->next_sibling;
        DecisionTree::delete_tree(child);
        child = next_sibling;
    }

    delete tree;
}

bool DecisionTree::prepare_data(const std::vector<std::vector<double> >& data, const std::vector<int>& dimensions, int class_count)
{
    if (data.size() < 1 || data[0].size() < 2 || data[0].size() - 1 != dimensions.size() || class_count < 1)
    {
        return false;
    }

    this->_class_probabilities.resize(class_count, 0);

    this->_dimensions = dimensions;
    this->_class_count = class_count;
    this->_n = data[0].size() - 1;
    this->_m = data.size();

    stringstream dimension_stream;
    vector_to_stream(dimensions, dimension_stream);
    cout << "dimensions" << dimension_stream.str() << endl;

    for (size_t i = 0; i < data.size(); ++i)
    {
        if (data[i].size() - 1 != static_cast<size_t>(this->_n))
        {
            size_t xyz = data[i].size();
            cout << "prepare data invalid feature count" << xyz << endl;
            return false;
        }

        int c = static_cast<int>(data[i][0]);
        if (c < 0 || c >= class_count)
        {
            cout << "prepare data invalid class " << c << endl;
            return false;
        }

        for (int j = 1; j < this->_n; ++j)
        {
            int feature_value = static_cast<int>(data[i][j]);
            if (feature_value < 0 || feature_value >= dimensions[j - 1])
            {
                cout << "prepare data invalid feature_value " << i << " " << j - 1 << " " << feature_value << " " << dimensions[j - 1] << endl;
                return false;
            }

            const string& fv_key = DecisionTree::gen_feature_value_key(j, feature_value);
            this->_fv_map[fv_key] += 1;
            this->_fvc_map[make_pair(fv_key, c)] += 1;
        }

        this->_class_probabilities[c] += 1;
    }

    for (map<string, double>::iterator iter = this->_fv_map.begin(); iter != this->_fv_map.end(); ++iter)
    {
        iter->second /= this->_m;
    }

    for (map<pair<string, int>, double>::iterator iter = this->_fvc_map.begin(); iter != this->_fvc_map.end(); ++iter)
    {
        int c = iter->first.second;
        iter->second /= this->_class_probabilities[c];// currently, this value is class count, not class probability
    }

    for (int i = 0; i < class_count; ++i)
    {
        this->_class_probabilities[i] /= this->_m;
        if (this->_class_probabilities[i] == 0.0)
        {
            this->_class_probabilities[i] = c_min_probability_factor / this->_m;
        }
    }

    return true;
}

DecisionTree::TreeNode* DecisionTree::create_tree(std::vector<std::pair<int, int> >& base_path, std::vector<bool>& selected_features, double base_entropy) const
{
    TreeNode* node = new TreeNode();
    node->first_child = NULL;
    node->next_sibling = NULL;
    node->class_id = -1;
    node->class_confidence = -1;
    if (base_path.size() > 0)
    {
        node->feature_value = base_path[base_path.size() - 1].second; //feature value of this tree node is passed from base path
    }
    else
    {
        node->feature_value = -1;
    }

    double min_entropy;
    int new_feature_id = this->select_feature(base_path, selected_features, base_entropy, min_entropy);
    node->feature_id = new_feature_id;
    if (new_feature_id < 0) //leaf node
    {
        node->class_id = this->determine_node_class(base_path, node->class_confidence);
        return node;
    }

    //assert(dimensions[new_feature_id] > 0); // duplicate op

    selected_features[new_feature_id] = true;
    base_path.push_back(make_pair(new_feature_id, -1));
    TreeNode* current_child = NULL;
    for (int i = 0; i < this->_dimensions[new_feature_id]; ++i)
    {
        base_path[base_path.size() - 1].second = i;
        TreeNode* child = this->create_tree(base_path, selected_features, min_entropy); //since the recursive depth is the feature count, if the feature count is huge, stack overflow may raise
        if (current_child == NULL)
        {
            node->first_child = child;
            current_child = child;
        }
        else
        {
            current_child->next_sibling = child;
        }
    }

    base_path.pop_back();
    selected_features[new_feature_id] = false;
    return node;
}

int DecisionTree::determine_node_class(const std::vector<std::pair<int, int> >& path, double& confidence) const
{
    double max_probability = -1.0;
    int max_class = -1;
    double total_probability = 0.0;
    for (int i = 0; i < this->_class_count; ++i)
    {
        double probability = this->calc_path_probability_for_class(path, i);
        if (probability > max_probability)
        {
            max_probability = probability;
            max_class = i;
        }

        total_probability += probability;
    }

    confidence = max_probability / total_probability;
    return max_class;
}

int DecisionTree::select_feature(std::vector<std::pair<int, int> >& base_path, const std::vector<bool>& selected_features, double base_entropy, double& min_entropy) const
{
    if (base_entropy < c_min_entropy)
    {
        return -1;
    }

    int selected_feature = -1;
    for (size_t i = 0; i < selected_features.size(); ++i)
    {
        if (!selected_features[i]) //for each not selected feature
        {
            double entropy = this->calc_feature_entropy(base_path, i);
            if (selected_feature == -1 || (selected_feature >= 0 && entropy < min_entropy))
            {
                min_entropy = entropy;
                selected_feature = i;
            }
        }
    }

    if (base_entropy - min_entropy <= c_min_information_gain)
    {
        return -1;
    }

    return selected_feature;
}

//E'(F1V1, F2V2, …, Fx) = sum(E(F1V1, F2V2, …, FxVi)*P(same)), Vi in [V1 ~ Vn]
double DecisionTree::calc_feature_entropy(std::vector<std::pair<int, int> >& base_path, int feature_id) const
{
    // assert(this->_dimensions[feature_id] > 0); duplicate op
    base_path.push_back(make_pair(feature_id, -1));
    double total_entropy = 0.0;

    for (int i = 0; i < this->_dimensions[feature_id]; ++i)
    {
        base_path[base_path.size() - 1].second = i;
        double entropy = this->calc_path_entropy(base_path);
        double prob = this->calc_path_probability(base_path);
        total_entropy += entropy * prob;
    }

    return total_entropy;
}

double DecisionTree::calc_path_entropy(const std::vector<std::pair<int, int> >& path) const
{
    vector<double> probabilities;
    for (int i = 0; i < this->_class_count; ++i)
    {
        double p = this->calc_path_probability_for_class(path, i);
        probabilities.push_back(p);
    }

    return MLBasics::entropy(probabilities);
}

// P(c/F1V1, F2V2, ...)
double DecisionTree::calc_path_probability_for_class(const std::vector<std::pair<int, int> >& path, int c) const
{
    double path_probability_by_class = this->calc_path_probability_by_class(path, c);
    double class_probability = this->get_class_probability(c);
    double path_probability = this->calc_path_probability(path);
    return MLBasics::bayesian(path_probability_by_class, class_probability, path_probability);
}

// P(F1V1, F2V2, .../c)
// assume features are independent in this implementation
double DecisionTree::calc_path_probability_by_class(const std::vector<std::pair<int, int> >& path, int c) const
{
    double prob = 1.0;
    for (size_t i = 0; i < path.size(); ++i)
    {
        prob *= this->get_feature_value_probability_by_class(path[i].first, path[i].second, c);
    }

    return prob;
}

// P(F1V1, F2V2, ...)
// assume features are independent in this implementation
double DecisionTree::calc_path_probability(const std::vector<std::pair<int, int> >& path) const
{
    double prob = 1.0;
    for (size_t i = 0; i < path.size(); ++i)
    {
        prob *= this->get_feature_value_probability(path[i].first, path[i].second);
    }

    return prob;
}

double DecisionTree::get_feature_value_probability_by_class(int feature_id, int feature_value, int c) const
{
    const string& key = DecisionTree::gen_feature_value_key(feature_id, feature_value);
    map<pair<string, int>, double>::const_iterator iter = this->_fvc_map.find(make_pair(key, c));
    if (iter != this->_fvc_map.end())
    {
        return iter->second;
    }
    else
    {
        return c_min_probability_factor / this->_m;
    }
}

double DecisionTree::get_feature_value_probability(int feature_id, int feature_value) const
{
    const string& key = DecisionTree::gen_feature_value_key(feature_id, feature_value);
    map<string, double>::const_iterator iter = this->_fv_map.find(key);
    if (iter != this->_fv_map.end())
    {
        return iter->second;
    }
    else
    {
        return c_min_probability_factor / this->_m;
    }
}

double DecisionTree::get_class_probability(int c) const
{
    return this->_class_probabilities[c];
}

string DecisionTree::gen_feature_value_class_key(int feature_id, int feature_value, int c)
{
    stringstream key_stream;
    key_stream << feature_id << "##" << feature_value << "##" << c;
    return key_stream.str();
}

string DecisionTree::gen_feature_value_key(int feature_id, int feature_value)
{
    stringstream key_stream;
    key_stream << feature_id << "##" << feature_value;
    return key_stream.str();
}

