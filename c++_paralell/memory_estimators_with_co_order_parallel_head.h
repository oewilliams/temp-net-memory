#ifndef memory_estimators_with_co_order_paralell_head
#define memory_estimators_with_co_order_paralell_head

std::vector<int> convert_to_int_vec(
        const std::vector<bool> &edge_series,
        const int series_len);

double get_kl_dist(
        const std::vector<int> &series_1,
        const std::vector<int> &series_2,
        const int series_len,
        const int order);

int get_co_order_estimate(
        const std::vector<int> &target_series,
        const std::vector<int> &driver_series,
        const int max_order,
        const int min_order,
        const int series_length);

std::vector<int> shape_funct(
        const std::vector<bool> &target_series,
        const std::vector< std::vector<bool> > &edge_series,
        const int max_order,
        const int min_order); 

std::vector< std::vector<int> > get_network_memory_shape(
        const std::vector<std::vector<bool> > &edge_series,
        const int max_order,
        const int min_order);

int eff_mem_funct(
        const std::vector<bool> &target_series,
        const std::vector< std::vector<bool> > &edge_series,
        const int max_order,
        const int min_order);

int get_network_effective_memory(
        const std::vector<std::vector<bool> > &edge_series,
        const int max_order,
        const int min_order);

#endif
