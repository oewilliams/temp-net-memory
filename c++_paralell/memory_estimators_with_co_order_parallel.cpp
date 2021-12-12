#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include <future>
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




std::vector<int> convert_to_int_vec(const std::vector<bool> &edge_series, const int series_len) {

	std::vector<int> conv_series;
	conv_series.reserve(series_len);
	for(const auto &x : edge_series) {
		conv_series.push_back(int(x));
	} 
	return conv_series;
}
/*****************************************************************************
Here we provide an implimentation of the EDC for co-orders of links.
These links will be assumed to be binary (on or off).
******************************************************************************/

//Get the K-L distance between two binary series (this plays the same roll as the 
// conditional entropy did for the memory of general sequences.
double get_kl_dist(
        const std::vector<int> &series_1,
        const std::vector<int> &series_2,
        const int series_len,
        const int order) {

	//Arrays to hold counts of orders.
	int * state_count_h = new int[1<<(order+1)]();
	int * state_count = new int[1<<order]();
	
	uint sub_index = 0;		//Index for counting sequence with only entries of series_2.
	//Make indexes, most recent value has lowest contribution.
	for(int i=0 ; i<order ; i++) {
		sub_index = (sub_index << 1) | series_2[order-i-1];
	}
	state_count[sub_index] += 1;

    uint h_sub_index = (sub_index << 1) | series_1[order];
	state_count_h[h_sub_index] += 1;
	
	
	//Shift sub_index back one, add the new series_2 entry, then remove the old one.
	//Then append series_1 entry for h_sub_index.
    int shift_s = ~(1UL << order);
    for(int i=order ; i<series_len-1 ; i++) {
		sub_index = ((sub_index << 1) | series_2[i]) & shift_s;
		state_count[sub_index] += 1;

		h_sub_index = (sub_index << 1) | series_1[i+1]; 
		state_count_h[h_sub_index] += 1;
	}
	
	//Add up all values.
	double sum_val = 0.0;
	for(int i=0 ; i< 1<<(order+1) ; i++) {
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
int get_co_order_estimate(
        const std::vector<int> &target_series,
        const std::vector<int> &driver_series,
        const int max_order,
        const int min_order,
        const int series_length) {
	
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


std::vector<int> shape_funct(
        const std::vector<bool> &target_series,
        const std::vector< std::vector<bool> > &edge_series,
        const int max_order,
        const int min_order) {

    int series_len = target_series.size();
	std::vector<int> target_conv = convert_to_int_vec(target_series, series_len);

	int num_links = edge_series.size();
	std::vector<int> shape_line;
	shape_line.reserve(num_links);

	for(int i=0 ; i<num_links ; i++) {
		std::vector<int> driver_conv = convert_to_int_vec(edge_series[i], series_len);
		int co_order = get_co_order_estimate(target_conv, driver_conv, max_order, min_order, series_len);
		shape_line.push_back(co_order);
	}
    
	return shape_line;
}


//Get the shape of the memory in a network.
//Return is a vector of vectors.
//Each subvector in the return represents a sorce link, a target link and the co-order.
std::vector< std::vector<int> > get_network_memory_shape(
        const std::vector<std::vector<bool> > &edge_series,
        const int max_order,
        const int min_order) {
	
	int num_links = edge_series.size();			//Number of links in the network.
	
	//Initialise the output.
	
	std::vector< std::future<std::vector<int> > > fut_shape;
	fut_shape.reserve(num_links);

	for(int i=0 ; i<num_links ; i++) {
		fut_shape.push_back(std::async(
			std::launch::async, 
			shape_funct,
			std::ref(edge_series[i]), std::ref(edge_series), max_order, min_order
		));
	}

	//Now assign values.
	std::vector<std::vector<int> > shape_matrix;
	shape_matrix.reserve(num_links);
	for(auto &x : fut_shape) {
		shape_matrix.push_back(x.get());
	}

    //Print for checking
    /*
    std::cout << "Post async matrix is:" << std::endl;
    for(int i=0 ; i< shape_matrix.size() ; i++) {
        for(int j=0 ; j< shape_matrix.size() ; j++) {
            std::cout << shape_matrix[i][j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
    */

	//Now we assign it to the correct format.
	int link_pairs = num_links * num_links;
	std::vector<std::vector<int> > memory_shape;
	memory_shape.reserve(link_pairs);
	for(int i=0 ; i<num_links ; i++) {
		for(int j=0 ; j<num_links ; j++) {
			memory_shape.push_back({i, j, shape_matrix[i][j]});
		}
	}

	
	return memory_shape;
}


int eff_mem_funct(
        const std::vector<bool> &target_series,
        const std::vector< std::vector<bool> > &edge_series,
        const int max_order,
        const int min_order) {

    int series_len = target_series.size();
    std::vector<int> target_conv = convert_to_int_vec(target_series, series_len);
    
    int max_observed_order = 0;

    int num_links = edge_series.size();
    for(int i=0 ; i<num_links ; i++) {
        std::vector<int> driver_conv = convert_to_int_vec(edge_series[i], series_len);
        int co_order = get_co_order_estimate(target_conv, driver_conv, max_order, min_order, series_len);
        
        max_observed_order = (co_order > max_observed_order) ? co_order : max_observed_order;
    }
    
    return max_observed_order;
}



//Get the effective memory of a network.
int get_network_effective_memory(
        const std::vector<std::vector<bool> > &edge_series,
        const int max_order,
        const int min_order) {
	
    int num_links = edge_series.size();         //Number of links in the network.
    
    //Initialise the output.
    
    std::vector< std::future<int> > fut_eff_mem;
    fut_eff_mem.reserve(num_links);

    for(int i=0 ; i<num_links ; i++) {
        fut_eff_mem.push_back(std::async(
            std::launch::async, 
            eff_mem_funct,
            std::ref(edge_series[i]), std::ref(edge_series), max_order, min_order
        ));
    }
    
    int max_observed_order = 0;
    for(auto &x: fut_eff_mem) {
        int test_mem = x.get();
        max_observed_order = (test_mem > max_observed_order) ? test_mem : max_observed_order;
    }

	return max_observed_order;
}


