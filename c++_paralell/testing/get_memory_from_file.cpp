#include <iostream>
#include <cmath>
#include <string.h> 
#include <vector>

#include "temp_net_parser_head.h"
#include "memory_estimators_with_co_order_testing_head.h"

using namespace std;
int main(int argc, char *argv[]) {
	
//	string file_location = "test_data.txt";
	
		
	string file_locaton;
	char seperator;
	int has_header;
	int min_order;
	int max_order;
	
	if(argc > 1) {
		file_locaton = argv[1];
		seperator = *argv[2];
		sscanf(argv[3],"%d",&has_header);
		sscanf(argv[4],"%d",&min_order);
		sscanf(argv[5],"%d",&max_order);
	} else {
		cout << "No arguments provided, testing case..." << endl;
		file_locaton = "/Users/oliverwilliams/Documents/qmul/Network_memory_project/release_builds/manufacturing_elist_comp_res_60_links_100.txt";
		has_header = 0;
		min_order = 0;
		max_order = 10;
	}
		
	
	vector<vector<bool> > edge_series = parse_edge_file(file_locaton, '\t', bool(has_header));
	
	cout << "got edge list from file" << endl;
	
	cout << "edges: " << edge_series.size() << endl;
	cout << "series length: " << edge_series[0].size() << endl;
	
	int memory_estimate = get_network_effective_memory(edge_series, max_order, min_order);
	
	cout << "The estimated memory is: " << memory_estimate << endl;
	

	
	return 0;
}