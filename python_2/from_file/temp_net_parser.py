#!/usr/bin/python
import numpy as np

def main():
	
	print("Program build correctly!")
	print("Note, this program is not intended to run on its own...")
	print("This is the only output.")
	
	return


"""
Function to get time stamped edge list from a file.
Requires the path to the file containing the edge list data and the seperator for each column in the file.
Optional parameter: flag for header line, if true first line of file will be ignored.
"""
def parse_edge_file(file_path, seperator, has_header=False):
	
	f = open(file_path,'r')
	
	if has_header==True:
		f.readline()		#If there is a header line, then skip it.
	
	
	edge_list = []
	
	for line in f:
		edge_list.append(parse_edge_string(line, seperator))	#Parse the edge and put it in a list.
	
	formatted_edge_list = convert_edge_list_format(edge_list)
	
	return formatted_edge_list
	
	
"""
Take the string representing an edge in the edge list file and return an appropriate format.
Needs the seperator character used (given by parse_edge_file function).
"""
def parse_edge_string(edge_string, seperator):
	
	#To ensure that any seperator can be used, work with the raw representation of the string.
	#This is a bit of a hack.
	split_string = repr(edge_string)[1:-1].split(seperator)
		
	#Get the nodes and the time stamp, ignoring the end of line character.
	source_node = int(split_string[0])
	target_node = int(split_string[1])
	try:
		time_stamp = int(split_string[2][0:-2])	
	except ValueError:
		time_stamp = int(split_string[2])	
	
	return [source_node,target_node,time_stamp]
	
	
"""
Convert a time stamped edge list to a vector of time series for each edge.
//Each time series represents a single edge.
This method is in no way optimal, but should hopefully be clear.
"""
def convert_edge_list_format(edge_list):
	
	end_time = edge_list[-1][2]		#Get the last time stamp value in the edge list.
	
	#First get the number of unique edges.
	#NOTE: we do not assume the network is undirected.
	seen_edges = {}			#Dict of all unique edges and their numeric label.
	edge_count = 0
	for t_edge in edge_list:
		edge = (t_edge[0], t_edge[1])			#Just take the node labels.
		if edge not in seen_edges.keys():
			seen_edges[edge] = edge_count
			edge_count +=1					
			
	num_edges = edge_count		#Get number of edges.
	
	#Initialise array for holding time series.
	#edge_series = np.zeros((num_edges, end_time+1), dtype=bool)
	edge_series = [[0]*(end_time+1)]*num_edges
	
	#Fill array.
	for t_edge in edge_list:
		edge = (t_edge[0], t_edge[1])
		edge_series[seen_edges[edge]][t_edge[2]] = 1			#Update the edge series array.
	
	return edge_series



if __name__ == "__main__":
	main()