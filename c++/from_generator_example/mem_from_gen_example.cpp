#include <iostream>
#include <vector>
#include "../memory_estimators_with_co_order_coll_head.h"
#include "../Synthetic_generators_head.h"
//Generate and test memory for a single DARN(p) network.

using namespace std;


int main(int argc, char *argv[]) {
	
	int nodes = 20;
	int mem_len = 4;
	double mem_strength = 0.9;
	double avg_prob = 0.1;
	int time_steps = 100000;
	int wait_time = 100;			//Time to wait for the model to reach a steady state.
	int max_order = 10;
	int gen_seed = 12353423;		//For seeding the random number generator.
	
	vector<vector<bool> > edge_series = DARNp_generator(nodes, mem_len, mem_strength, avg_prob, time_steps, wait_time, gen_seed);
	int memory_estimate = get_network_effective_memory(edge_series, max_order, 0);
	
	cout << "The estimated memory is: " << memory_estimate << endl;
	
	return 0;
}
