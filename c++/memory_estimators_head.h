#ifndef memory_estimators_head
#define memory_estimators_head

void combine_series(std::vector<bool> &edge_1, std::vector<bool> &edge_2, char *comb_series, int series_len);
double get_conditional_entropy(char *series, int series_len, int order, int *order_shift, int state_size);
int EDC_est(char *series, int max_order, int min_order, int series_length, int state_size);
int EDC_link_memory_est(std::vector<std::vector<bool> > &edge_series, int max_order, int min_order);
int EDC_network_memory_est(std::vector<std::vector<bool> > &edge_series, int max_order, int min_order);
int get_network_memory_EDC(std::vector<std::vector<bool> > &edge_series, int max_order, int min_order);

#endif