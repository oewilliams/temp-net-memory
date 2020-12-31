#include <iostream>
#include <vector>
#include <math.h>
#include <string>
//Functions for getting the memory of a temporal network.
//EDGE LIST MUST BE HELD IN A vector< vector<bool> > FORMAT!!!

/*
	Functions (not including utilities):
		1 - conditional_entropy - takes a vector of integers, its length, the disired order to test and the number of states.
			Most of these are calculated in program for the estimators and the user need not worry. The series and the state size are key.
			Returns the value of series_len * the conditional entropy.
			
		2 - EDC_est - EDC estimator for memory of a sequence (vector of integers).
			Takes the sequence, the maximum order to check, the minimum order to check, the sequence length 
			and the number of states in the sequence.
			
		3 - get_kl_dist - takes pointers to arrays of a target and driver series, their length, and the order to check,
			then calculates the K-L distance between the two sequences at the order.
			
		4 - get_co_order_estimate - given a target series, a driver series (as pointers to integer arrays),
			the maximum order to check, the minimum order to check, and the length of the series, get 
			the co-order estimate for the pair of series.
			
		5 - get_network_memory_shape - given the edge series of a network as a std::vector<std::vector<bool> >, 
			the maximum order to check and the minimum order to check, get the shape of the networks memory.
			
		6 - get_network_effective_memory - given the edge series of a network as a std::vector<std::vector<bool> >, 
			the maximum order to check and the minimum order to check, get the effective memory of the network.
		
		7 - get_network_pair_memory - given the edge series of a network as a std::vector<std::vector<bool> >, 
			the maximum order to check and the minimum order to check, get the pair memory of the network.

*/



//Utility functions.

//Convert a vector<bool> into an integer array. 
void convert_series(std::vector<bool> &edge, int *conv_series, int series_len) {
	for(int i =0 ; i<series_len ; i++) {
		conv_series[i] = int(edge[i]);
	}
	return;
}

//Convert a vector<bool into an integer array, but with each element being the concatenation of the previous "order" values.
void convert_series_binary(std::vector<bool> &edge, int *conv_series, int series_len, int order) {
	
	uint sub_index = 0;
	
	//Initialise the concatenated series.
	for(int i=0 ; i<order ; i++) {
		sub_index = (sub_index << 1) | edge[i];
	}
	
	//Now fill the converted series.
	conv_series[0] = sub_index;
	
	uint sub_shift = ~(1UL << order);
	for(int i=order ; i<series_len ; i++) {
		sub_index = ((sub_index << 1) | edge[i]) & sub_shift;
		conv_series[i-order+1] = sub_index;
	}
	return;
}

int * make_markov_series(int *target_series, int *driver_series, int series_len, int order) {
	
	int * markov_series = new int[series_len-order] ();
	
	for(int i=0 ; i<series_len-order-1 ; i++) {
		markov_series[i] = (driver_series[i] << 1) | target_series[i+order+1];
	}
	return markov_series;	
}


//Model functions. 

/****************************************************************************
						Co order estimators.
Here we provide an implimentation of the EDC for co-orders of links.
These links will be assumed to be binary (on or off).
****************************************************************************/


double get_kl_dist_new(int *series, int *sub_series, int series_len, int order) {

	//Number of possible sequences of length order+1 and order.
	uint h_size = 1 << (order+1);
	uint s_size = 1 << order;
	//Arrays to hold counts of orders.
	int * state_count_h = new int[h_size]();
	int * state_count = new int[s_size]();
	
	uint sub_shift = (1 << order) - 1;
	uint h_sub_shift = (1 << (order + 1)) - 1;
	for(int i=0 ; i<series_len ; i++) {
		int sub_index = sub_series[i] & sub_shift;
		state_count[sub_index] += 1;
		
		int h_sub_index = series[i] & h_sub_shift;
		state_count_h[h_sub_index] += 1;
	}
	
	//Add up all values.
	double sum_val = 0.0;
	for(int i=0 ; i< h_size ; i++) {
		if (state_count_h[i] !=0) {
			int s_term = i >> 1;
			sum_val += double(state_count_h[i]) * log(double(state_count_h[i])/double(state_count[s_term]));
		}
	}
	
	delete [] state_count_h;
	delete [] state_count;
	
	return sum_val;
	
}


//Get the K-L distance between two binary series (this plays the same roll as the 
// conditional entropy did for the memory of general sequences.
double get_kl_dist(int *series_1, int *series_2, int series_len, int order) {

	//Number of possible sequences of length order+1 and order.
	uint h_size = 1 << (order+1);
	uint s_size = 1 << order;
	//Arrays to hold counts of orders.
	int * state_count_h = new int[h_size]();
	int * state_count = new int[s_size]();
	
	//uint h_sub_index = 0;	//Index for counting sequence with present state of series_1.
	uint sub_index = 0;		//Index for counting sequence with only entries of series_2.
	
	//Make indexes, most recent value has lowest contribution.
	for(int i=0 ; i<order ; i++) {
		sub_index = (sub_index << 1) | series_2[order-i-1];
	}
	state_count[sub_index] += 1;
	state_count_h[(sub_index << 1) | series_1[order]] += 1;
	
	
	
	//Shift sub_index back one, add the new series_2 entry, then remove the old one.
	//Then append series_1 entry for h_sub_index.
	uint sub_shift = ~(1UL << order);
	for(int i=order ; i<series_len-1 ; i++) {
		sub_index = ((sub_index << 1) | series_2[i]) & sub_shift;
		state_count[sub_index] += 1;

		state_count_h[(sub_index << 1) | series_1[i+1]] += 1;
	}
	
	//Add up all values.
	double sum_val = 0.0;
	for(int i=0 ; i< h_size ; i++) {
		int check_entry = state_count_h[i];
		if (check_entry !=0) {
			int s_term = i >> 1;
			sum_val += double(check_entry) * log(double(check_entry)/double(state_count[s_term]));
		}
	}
	
	delete [] state_count_h;
	delete [] state_count;
	
	return sum_val;
	
}


//Get the co-order estimate of two series. Note that this is not symmetric.
//New version uses pre-converted approach.
//Target series is the non-converted series.
int get_co_order_estimate_new(int *target_series, int *driver_series, int max_order, int min_order, int series_length) {
	
	//First combine the two series to make the maximum order Markov chain. 
	
	int * order_markov_series = make_markov_series(target_series, driver_series, series_length, max_order); 
	
	double EDC_min = INFINITY;
	int min_index = 0;	

	for(int i=min_order ; i<=max_order ; i++) {
		int conditional_likelihood = get_kl_dist_new(order_markov_series, driver_series, series_length-max_order-1, i);
		int num_obs = series_length - 1;
		double EDC_val = -conditional_likelihood + ((1<<(i+1))*(log(log(num_obs))));
		if(EDC_val < EDC_min) {
			EDC_min = EDC_val;
			min_index = i;
		}
	}
	
	return min_index;
}

//Get the co-order estimate of two series. Note that this is not symmetric.
int get_co_order_estimate(int *target_series, int *driver_series, int max_order, int min_order, int series_length) {
	
	double EDC_min = INFINITY;
	int min_index = 0;	

	for(int i=min_order ; i<=max_order ; i++) {
		int conditional_likelihood = get_kl_dist(target_series, driver_series, series_length, i);
		int num_obs = series_length - 1;
		double EDC_val = -conditional_likelihood + ((1<<(i+1))*(log(log(num_obs))));
		if(EDC_val < EDC_min) {
			EDC_min = EDC_val;
			min_index = i;
		}
	}
	
	return min_index;
}



/****************************************************************************
						Memory measure functions
Here we provide functions to retrive the shape of a networks memory,
the effective memory of a network, and the pair memory of the network. 
****************************************************************************/

int get_network_effective_memory_new(std::vector< std::vector<bool> > &edge_series, int max_order, int min_order) {
	
	int series_len = edge_series[0].size();
	int num_links = edge_series.size();
	
	int max_observed_order = 0;
	
	int * target_series = new int[series_len]();
	int * driver_series = new int[series_len]();
	
	for(int i=0 ; i<num_links ; i++) {
		convert_series_binary(edge_series[i], driver_series, series_len, max_order);
		for(int j=0 ; j<num_links ; j++) {
			convert_series(edge_series[j], target_series, series_len);
			int order_est = get_co_order_estimate_new(target_series, driver_series, max_order, min_order, series_len);
			if (order_est > max_observed_order) {
				max_observed_order = order_est;		
			}
		}
	}
	
	delete [] target_series;
	delete [] driver_series;
	
	return max_observed_order;
	
}

//Get the effective memory of a network.
int get_network_effective_memory(std::vector<std::vector<bool> > &edge_series, int max_order, int min_order) {
		
	int series_len = edge_series[0].size();
	int num_links = edge_series.size();
	
	int max_observed_order = 0;
	
	int * target_series = new int[series_len]();
	int * driver_series = new int[series_len]();
	
	for(int i=0 ; i<num_links ; i++) {
		convert_series(edge_series[i],target_series,series_len);
		for(int j=0 ; j<num_links ; j++) {
			convert_series(edge_series[j],driver_series,series_len);
			int order_est = get_co_order_estimate(target_series, driver_series, max_order, min_order, series_len);
			if (order_est > max_observed_order) {
				max_observed_order = order_est;		
			}
		}
	}
	
	delete [] target_series;
	delete [] driver_series;
	
	return max_observed_order;
}