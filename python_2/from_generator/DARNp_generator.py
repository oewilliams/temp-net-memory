#!/usr/bin/python
import random

"""
This program contains functions to generate a simple DARN(p) temporal network.
This network has memory precicely p.
"""


def main():
	
	print("Program build correctly!")
	print("Note, this program is not intended to run on its own...")
	print("This is the only output.")
	
	return


"""
Function to generate DARN(p) network.
Takes (in order):
	- Number of nodes.
	- Memory length.
	- Memory strength (reliance on memory).
	- Link probability.
	- Timesteps to run model for.
returns edge sequence in format required for memory estimator.
"""
def generate_DARNp_net(nodes, mem_leng, mem_str, avg_prob, time_steps):
	
	num_links = (nodes * (nodes-1))/2		#Number of links in this model.
	
	#Initialise edge sequence, add extra time steps for initialising memory.
	edge_sequence = [[0]*(time_steps+mem_leng)]*num_links
	
	#Initialise memory.
	for t in xrange(mem_leng):
		for l in xrange(num_links):
			edge_sequence[l][t] = bool(bernoulli(avg_prob))
		
	
	#Now run model.
	for t in range(mem_leng,time_steps):
		for l in xrange(num_links):
			mem_check = bernoulli(mem_str)
			
			if mem_check == 0:
				edge_sequence[l][t] = bool(bernoulli(avg_prob))
			else:
				past_val = random.randint(1,mem_leng)
				edge_sequence[l][t] = edge_sequence[l][t-past_val]
	
	return edge_sequence


"""
Function generates Bernoulli random variables with probability p.
"""
def bernoulli(p):

	r_num = random.random()
	if r_num < p:
		return 1
	else:
		return 0

if __name__ == "__main__":
	main()