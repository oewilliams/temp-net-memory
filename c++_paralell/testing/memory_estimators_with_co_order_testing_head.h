#ifndef memory_estimators_with_co_order_testing_head
#define memory_estimators_with_co_order_testing_head

void convert_series(std::vector<bool> &edge, int *conv_series, int series_len);
void convert_series_binary(std::vector<bool> &edge, int *conv_series, int series_len, int order);
int * make_markov_series(int *target_series, int *driver_series, int series_len, int order);
double get_kl_dist_new(int *series, int *sub_series, int series_len, int order);
double get_kl_dist(int *series_1, int *series_2, int series_len, int order);
int get_co_order_estimate_new(int *target_series, int *driver_series, int max_order, int min_order, int series_length);
int get_co_order_estimate(int *target_series, int *driver_series, int max_order, int min_order, int series_length);
int get_network_effective_memory_new(std::vector< std::vector<bool> > &edge_series, int max_order, int min_order);
int get_network_effective_memory(std::vector<std::vector<bool> > &edge_series, int max_order, int min_order);

#endif