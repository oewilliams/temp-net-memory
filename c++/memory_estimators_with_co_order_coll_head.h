#ifndef memory_estimators_with_co_order_coll_head
#define memory_estimators_with_co_order_coll_head

std::vector<int> combine_series(std::vector<bool> &edge_1, std::vector<bool> &edge_2, int series_len);
void convert_series(std::vector<bool> &edge, int *conv_series, int series_len);
double conditional_entropy(std::vector<int> &series, int series_len, int order, int state_size);
int EDC_est(std::vector<int> series, int max_order, int min_order, int series_length, int state_size);
double get_kl_dist(int *series_1, int *series_2, int series_len, int order);
int get_co_order_estimate(int *target_series, int *driver_series, int max_order, int min_order, int series_length);
std::vector< std::vector<int> > get_network_memory_shape(std::vector<std::vector<bool> > &edge_series, int max_order, int min_order);
int get_network_effective_memory(std::vector<std::vector<bool> > &edge_series, int max_order, int min_order);
int get_network_pair_memory(std::vector<std::vector<bool> > &edge_series, int max_order, int min_order);

#endif