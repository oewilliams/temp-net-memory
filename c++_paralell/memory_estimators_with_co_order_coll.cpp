#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include <algorithm>
#include <iterator>
#include <numeric>
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

//Given a pair of edge series and a pointer to an initialised char array, combine the edge series.
//See SI for details in pair memory estimation.
std::vector<int> combine_series(std::vector<bool> &edge_1, std::vector<bool> &edge_2, int series_len) {
	std::vector<int> comb_series(series_len, 0);
	for(int i=0 ; i<series_len ; i++) {
		comb_series[i] = edge_1[i] + (2 * edge_2[i]);
	}
	return comb_series;
}

//Convert a vector<bool> into an integer array. 
void convert_series(std::vector<bool> &edge, int *conv_series, int series_len) {
	for(int i =0 ; i<series_len ; i++) {
		conv_series[i] = int(edge[i]);
	}
	return;
}

std::vector<int> convert_series_vector(std::vector<bool> &edge, int series_len) {
	std::vector<int> conv_series(series_len, 0);
	for(int i=0 ; i<series_len ; i++) {
		conv_series[i] = int(edge[i]);
	}
	return conv_series;
}

//Model functions. 


/****************************************************************************
						General memory estimators.
Here we provide an implimentation of the EDC for general time series.
****************************************************************************/

//Conditional entropy of a sequence in a general state space.
double conditional_entropy(std::vector<int> &series, int series_len, int order, int state_size) {
	
	//Number of possible combinations of length order+1 and order.	
	int h_size = pow(state_size,order+1);
	int s_size = pow(state_size,order);
	
	//Arrays for counting the occurences of combinations.
	int * state_count_h = new int[h_size]();
	int * state_count = new int[s_size]();

	int h_sub_index = series[order];
	int sub_index = 0;
	
	for(int i=0 ; i<order ; i++) {
		h_sub_index += series[order - i - 1] * pow(state_size,i+1); 
		sub_index += series[order - i -1] * pow(state_size,i); 
	}
	state_count_h[h_sub_index] += 1;
	state_count[sub_index] += 1;
			
	//	Here we shift the index.
	//	first remove the oldest value, then multiply by state size to add new element, then add new value.
	for(int i=order+1 ; i<series_len-order-1 ; i++) {
		h_sub_index = (state_size * (h_sub_index - (series[i-order-1] * pow(state_size,order)))) + series[i];
		sub_index = int(floor(h_sub_index/state_size));		//Use relationship between two indexes.
		state_count_h[h_sub_index] += 1;
		state_count[sub_index] += 1;
	}
		
	//Add up all values.
	double sum_val = 0.0;
	for(int i=0 ; i< h_size ; i++) {
		if (state_count_h[i] !=0) {
			int s_term = int(floor(i/state_size));
			sum_val += double(state_count_h[i]) * log(double(state_count_h[i])/double(state_count[s_term]));
		}
	}
	
	delete [] state_count_h;
	delete [] state_count;
	
	return sum_val;
}

//Efficient determination criterion.
int EDC_est(std::vector<int> series, int max_order, int min_order, int series_length, int state_size) {
	
	double EDC_min = INFINITY;	//Want minimum, so start at infinity.
	int min_index = 0;			//

	for(int i=min_order ; i<=max_order ; i++) {
		double conditional_likelihood = conditional_entropy(series, series_length, i, state_size);
		int num_obs = series_length - i;
		double EDC_val = (-2.0*conditional_likelihood) + (pow(state_size,i+1)*(log(log(num_obs)))*2.0);
		
		if(EDC_val < EDC_min) {
			EDC_min = EDC_val;
			min_index = i;
		}
	}
	return min_index;
}



/****************************************************************************
						Co order estimators.
Here we provide an implimentation of the EDC for co-orders of links.
These links will be assumed to be binary (on or off).
****************************************************************************/

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
		if (state_count_h[i] !=0) {
			int s_term = i >> 1;
			sum_val += double(state_count_h[i]) * log(double(state_count_h[i])/double(state_count[s_term]));
		}
	}
	
	delete [] state_count_h;
	delete [] state_count;
	
	return sum_val;
	
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
		/*	
		If you know that you have enough data to ensure that 
		the estimator function will be convex uncomment the following...
		*/
		/*
		else {
			return min_index;
		}
		//*/
	}
	
	return min_index;
}



/****************************************************************************
						Memory measure functions
Here we provide functions to retrive the shape of a networks memory,
the effective memory of a network, and the pair memory of the network. 
****************************************************************************/



//Get the shape of the memory in a network.
//Return is a vector of vectors.
//Each subvector in the return represents a sorce link, a target link and the co-order.
std::vector< std::vector<int> > get_network_memory_shape(std::vector<std::vector<bool> > &edge_series, int max_order, int min_order) {
	
	int series_len = edge_series[0].size();		//Number of time sereis the network exists for.
	int num_links = edge_series.size();			//Number of links in the network.
	
	//Initialise the output.
	int link_pairs = num_links * num_links;
	std::vector< std::vector<int> > memory_shape(link_pairs, std::vector<int>(3,0));
	
	int * target_series = new int[series_len]();
	int * driver_series = new int[series_len]();
	
	int counter = 0;
	for(int i = 0 ; i<num_links ; i++) {
		convert_series(edge_series[i],target_series,series_len);
		for(int j=0 ; j<num_links ; j++) {
			convert_series(edge_series[j],driver_series,series_len);
			int co_order = get_co_order_estimate(target_series, driver_series, max_order, min_order, series_len);
			memory_shape[counter] = {i, j, co_order};
			counter++;
		}
	}
	
	//Clean up.
	delete [] target_series;
	delete [] driver_series;
	
	return memory_shape;
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
				/*	
				If you know that you have enough data to ensure that 
				the estimator function will be convex uncomment the following...
				*/
				/*
				min_order = max_observed_order;		 
				//*/		
			}
		}
	}
	
	delete [] target_series;
	delete [] driver_series;
	
	return max_observed_order;
}

//Get the effective memory of a network.
int get_network_effective_memory_paralell(std::vector<std::vector<bool> > &edge_series, int max_order, int min_order) {
		
	int series_len = edge_series[0].size();
	int num_links = edge_series.size();
	
	int max_observed_order = 0;
	
	for(int i=0 ; i<num_links ; i++) {
		std::vector<int> target_series = convert_series_vector(edge_series[i],series_len);
		
		std::vector<int> co_orders;                   // no default init
		co_orders.reserve(series_len);                        // allocate ints
		std::iota(co_orders.begin(), co_orders.end(), 0); // fill them
		
		// Itterate over links in paralell.
		std::for_each(
			std::execution::par_unseq,
			co_orders.begin(),
			co_orders.end(),
			[](auto&& dum_index)
			{
				int j = dum_index;
				std::vector<int> driver_series = convert_series_vector(edge_series[j],series_len);
				int order_est = get_co_order_estimate(target_series, driver_series, max_order, min_order, series_len);
				co_orders[j] = order_est;
			});
			
		for(int j=0 ; j<num_links ; j++) {
			if (co_orders[j] > max_observed_order) {
				max_observed_order = co_orders[j];		
			}
		}
	}
	
	return max_observed_order;
}


int get_network_pair_memory(std::vector<std::vector<bool> > &edge_series, int max_order, int min_order) {

	int series_len = edge_series[0].size();
	int num_links = edge_series.size();
	
	int max_observed_order = 0;
		
	for(int i=0 ; i<num_links ; i++) {
		for(int j=0 ; j<i ; j++) {
			std::vector<int> comb_series = combine_series(edge_series[i], edge_series[j], series_len);
			int order_est = EDC_est(comb_series, max_order, min_order, series_len, 4); //$ possible states in combined series.
			if (order_est > max_observed_order) {
				max_observed_order = order_est;
				/*	
				If you know that you have enough data to ensure that 
				the estimator function will be convex uncomment the following...
				*/
				/*
				min_order = max_observed_order;		 
				//*/		
			}
		}
	}	
	return max_observed_order;
}