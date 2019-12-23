#!/usr/bin/python
from __future__ import division
import math
import itertools
import collections
"""
Contains functions for getting the memory of a temporal network assuming it is in the correct format.
"""


def main():
	
	print("Program build correctly!")
	print("Note, this program is not intended to run on its own...")
	print("This is the only output.")
	
	return
	

#Utility functions.	

"""
Function takes two series and combines them into a single series in an expanded state space.
Currently only work for binary sequences.
"""
def combine_series(series_1, series_2, series_len):
	
	comb_series = [0]*series_len 
	for i in range(series_len):
		comb_series[i] = int(series_1[i] + (2 * series_2[i]))
	
	return comb_series



"""
Given a series, find the number of unique symbols in that series.
"""
def get_state_size(series):
	
	state_size = 0
	elem_set = set([])
	
	for x in series:
		if x not in elem_set:
			elem_set.add(x)
			state_size += 1
	
	return state_size


#Model functions.

"""
Gets the nth order conditional entropy of a time series, taking parameters for optimisation.
Takes the time series, the value of n.
Optionally takes the length of the time series, the "shift" associated with the order,
and the number of states the series can take.
"""
def get_conditional_entropy(series, order, series_len):
	
	state_count_h = collections.Counter([tuple(series[i:i+order+1]) for i in range(series_len-order-1)])
	state_count = collections.Counter([tuple(series[i:i+order]) for i in range(series_len-order)])
	
	sum_val = 0.0
	
	for state in state_count_h.keys():
		sum_val += float(state_count_h[state]) * math.log(float(state_count_h[state])/float(state_count[state[0:-1]]))
	
	return sum_val
	

"""
Gets the EDC estimate of the order or a time series - general implimentation
Takes the series and the maximum order.
Optionally takes the minimum order, length of the time series and the number of states.
"""
def EDC_est(series, max_order, min_order=0, series_len=-1, state_size=-1):
	
	#Get optional parameters if they are not specified.
	if series_len==-1:
		series_len = len(series)	
	if state_size==-1:
		state_size = get_state_size(series)
		
	memory_est = 0
	EDC_min = float('inf')
		
	for order in range(min_order,max_order+1):
		conditional_likelihood = get_conditional_entropy(series, order, series_len)
		num_obs = series_len - order;
		#Next line is the ECD implimentation.
		EDC_val = (-2*conditional_likelihood) + ((state_size**order+1)*(math.log(math.log(num_obs)))*2);
		if EDC_val < EDC_min:
			EDC_min = EDC_val
			memory_est = order
	
	return memory_est


"""
Get the pair memory estimate for the network.
"""
def get_network_pair_memory_est(edge_series, max_order, min_order=0):
	
	links = len(edge_series)
	time_len = len(edge_series[0])
	
	net_mem_est = 0
	
	#For each possible pair of links, combine the link series then get the memory estimate.
	for l_1 in range(links):
		for l_2 in range(l_1):
			comb_series = combine_series(edge_series[l_1], edge_series[l_2], time_len)
			new_est = EDC_est(comb_series, max_order, min_order, time_len, 4)
			
			if new_est > net_mem_est:
				net_mem_est = new_est

	return net_mem_est


"""
Get the KL-distance between the target and driver series at given order.
"""
def get_kl_dist(target_series, driver_series, series_len, order):
	
	h_size = 1 << (order+1)
	s_size = 1 << order
	state_count_h = [0]*h_size
	state_count = [0]*s_size
	
	h_sub_index = 0	#Index for counting sequence with present state of series_1.
	sub_index = 0		#Index for counting sequence with only entries of series_2.
	
	#Make indexes, most recent value has lowest contribution.
	for i in range(order):
		sub_index = (sub_index << 1) | driver_series[order-i-1]
		
	h_sub_index = (sub_index << 1) | target_series[order]
	
	state_count_h[h_sub_index] += 1
	state_count[sub_index] += 1
	
	
	#Shift sub_index back one, add the new series_2 entry, then remove the old one.
	#Then append series_1 entry for h_sub_index.
	for i in range(series_len-1): 
		sub_index = ((sub_index << 1) | driver_series[i])
		sub_index &= ~(1 << order)
		h_sub_index = (sub_index << 1) | target_series[i+1] 
		
		state_count_h[h_sub_index] += 1
		state_count[sub_index] += 1
	
	
	#Add up all values.
	sum_val = 0.0
	for i in range(h_size):
		if state_count_h[i] !=0 :
			s_term = i >> 1
			sum_val += float(state_count_h[i]) * math.log(float(state_count_h[i])/float(state_count[s_term]))
	
	return sum_val



"""
Get the co order estimate for a target/driver series pair, given a maximum and minimum order and their lenght.
"""
def get_co_order_est(target_series, driver_series, max_order, min_order, series_len):
	
	EDC_min = float('inf')
	min_index = 0
	
	for i in range(min_order,max_order+1):
		conditional_likelihood = get_kl_dist(target_series, driver_series, series_len, i)
		num_obs = series_len - 1
		EDC_val = -conditional_likelihood + ((1<<(i+1))*(math.log(math.log(num_obs))));
		if EDC_val < EDC_min :
			EDC_min = EDC_val
			min_index = i
		
	return min_index

"""
Get the shape of the memory in the network.
This is returned as a list of lists. Each sublist containing the target link, driver link, and co-order.
"""
def get_network_memory_shape(edge_series, max_order, min_order):
	
	num_links = len(edge_series)
	series_len = len(edge_series[0])
	
	memory_shape = [[0,0,0]]*(num_links*num_links)
	counter = 0
	for i in range(len(edge_series)):
		for j in range(len(edge_series)):
			order_est = get_co_order_est(edge_series[i], edge_series[j], max_order, min_order, series_len)
			memory_shape[counter] = [i,j,order_est]
			counter += 1

	return memory_shape


"""
Get the effective memory of the network.
"""
def get_network_effective_memory(edge_series, max_order, min_order=0):
	
	series_len = len(edge_series[0])
	max_observed_order = 0
	
	for edge_1 in edge_series:
		for edge_2 in edge_series:
			co_order_est = get_co_order_est(edge_1, edge_2, max_order, min_order, series_len)
			if co_order_est > max_observed_order:
				max_observed_order = co_order_est
	
	return max_observed_order



if __name__ == "__main__":
	main()