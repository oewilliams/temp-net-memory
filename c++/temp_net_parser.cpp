#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <fstream>
#include <map>

//Collection of functions for parseing time stamped edge list and converting to format for memory estimation.


std::vector<int> parse_edge_string(std::string string_edge, char seperator) {
	
	int string_index=0;
	int seperator_index=0;
	std::vector<std::string> split_string_edge(3);	//Only take three strings.
	std::vector<int> edge_vector(3);
	
	//Loop over each element of the string_edge.
	while(string_index < string_edge.length()) {
		//If the element is the seperator character, then move to the next element in the split vector.
		//Do not include seperator in the stored strings.
		//Else add the element to the current string in split vector.
		if(string_edge[string_index] == seperator) {
			seperator_index++;
		}
		else {
			split_string_edge[seperator_index] += string_edge[string_index];
		}
		string_index++;
	}
	
	//Convert strings for each edge/time stamp to integers for output vector.
	for(seperator_index=0 ; seperator_index<3 ; seperator_index++) {
		edge_vector[seperator_index] = stoi(split_string_edge[seperator_index]);
	}
	
	return edge_vector;
}


//Convert a time stamped edge list to a vector of time series for each edge.
//Each time series represents a single edge.
//Each element in a time series is a bool, 1 for link present, 0 else. 
std::vector<std::vector<bool> > convert_edge_list_format(std::vector<std::vector<int> > int_edge_list) {
	
	int edge_index;
	int unique_counter = 0;							//Count the number of unique edges.
	int end_time = int_edge_list.back()[2];		//Get the final time stamp for the edge list.
	std::pair<int,int> edge;
	
	std::vector<bool> dum_edge_series(end_time,0);
	
	std::vector<std::vector<bool> > bool_edge_list;			//Output format.
	std::map<std::pair<int, int>, int> edge_ind_map;		//Label each unique edge.
	
	for(edge_index=0 ; edge_index<int_edge_list.size() ; edge_index++) {
		edge.first = int_edge_list[edge_index][0];
		edge.second = int_edge_list[edge_index][1]; 
		
		//See if we can find the current edge in the list of previously seen edges.
		std::map<std::pair<int,int>, int>::const_iterator e_map_iter = edge_ind_map.find(edge);
		if (e_map_iter != edge_ind_map.end()) {
			//If we have seen this edge before, update the output.
			bool_edge_list[e_map_iter->second][int_edge_list[edge_index][2]] = 1;
		}
		else {
			//If this edge is new to us, then make a new element in the output.
			edge_ind_map[edge] = unique_counter;
			bool_edge_list.push_back(dum_edge_series);
			bool_edge_list[unique_counter][int_edge_list[edge_index][2]] = 1;
			unique_counter++;
		}
	}
	return bool_edge_list;
}


//Function to get time stamped edge list from a file.
//Requires the path to the file containing the edge list data and the seperator for each column in the file.
//Optional parameter: flag for header line, if true first line of file will be ignored.
std::vector<std::vector<bool> > parse_edge_file(std::string file_path, char seperator, bool has_headline){
	
	std::ifstream inFile;
	inFile.open(file_path);
	
	if (!inFile) {
		std::cerr << "Unable to open file.";
		exit(1);   // call system to stop
	}
	
	//If the function is told that the file will have a header line, then ignore it.
	if(has_headline) {
		inFile.ignore(1,'\n');
	}
	
	std::vector<std::vector<int> > parsed_edge_list;
	std::vector<int> dum_edge(3);						//Vector to hold each time stamped edge that appears.
	std::string line = "";
	while(getline(inFile,line)) {
		dum_edge = parse_edge_string(line, seperator);		//Parse the string for this edge as a vector of ints.
		parsed_edge_list.push_back(dum_edge);		//Add the parsed edge to the edge list.
	}
	
	std::cout << "Got edge list, now formatting..." << std::endl;
		
	//Convert the parsed edge list into the vec<vec<bool>> format used for finding memory.
	std::vector<std::vector<bool> > formatted_edge_list = convert_edge_list_format(parsed_edge_list);
	
	return formatted_edge_list;
}

