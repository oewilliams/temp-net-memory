import DARNp_generator
import memory_estimators_paralell

"""
This program serves as an example for how to use the memory estimator with a generator program.
Paralell version.
"""

def main():
	nodes = 3
	mem_len = 4
	mem_strength = 0.9
	avg_prob = 0.1
	time_steps = 100000
	max_order = 10
	
	edge_series = DARNp_generator.generate_DARNp_net(nodes, mem_len, mem_strength, avg_prob, time_steps)
	
	memory_estimate = memory_estimators_paralell.get_network_effective_memory(edge_series, max_order, 0)
	
	print("The estimated memory is: %d" %(memory_estimate))

if __name__ == "__main__":
	main()
