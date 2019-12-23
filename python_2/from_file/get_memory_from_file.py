#!/usr/bin/python
import sys
import os
import memory_estimators
import temp_net_parser

"""
	Master file for getting memory of temporal network from a file containing a time stamped edge list.
	File location should be given as a command line argument, along with the seperator for the edges and whether there
	is a header line or not. Other optional values can be included.
	Required input in order: 
		File location,
		seperator(usually tab or comma),
		header line flag (1/True or 0/False).
	Optional values: 
		minimum order to check, 
		maximum order to check.
"""


def main():
	cmd_args = sys.argv
	
	header_check_dict = {'0':False,'False':False,'1':True,'True':True}
	
	if len(cmd_args) == 1:
		[file_location,seperator,has_header,min_order,max_order] = prompt_params(header_check_dict)
	else:
		file_location = str(cmd_args[1])	#Get the file location.
		seperator = str(cmd_args[2])
		has_header = str(cmd_args[3])
		if has_header in header_check_dict.keys():
			has_header = header_check_dict[has_header]
		else:
			print("Error! bad has_header flag, please use one of 0,1,True,False")
	
		if len(cmd_args) == 5:
			min_order = int(cmd_args[4])	#Get optional min order.
		else:
			min_order = 0
	
		if len(cmd_args) == 6:
			max_order = int(cmd_args[5])	#Get optional max order.
		else:
			max_order = 10
	
	edge_series = get_edge_series(file_location, seperator, has_header)		#Function adds checks on file loc.
	
	print("Got edge list from file, now getting memory...")
	
	memory_est = memory_estimators.get_network_effective_memory(edge_series, max_order, min_order)
	
	print("Estimated memory of temporal network:")
	print(memory_est)


def prompt_params(header_check_dict):
	
	print("No parameters in cmd arguments, please enter them now.")
	file_loc = raw_input("Enter the location of the edge list file: ")
	seperator = raw_input(r"Enter the seperator character for the edge list (tab is \t without ''): ")
	has_header = raw_input("Does the file have a header line? (True,1,False,0) ")
	
	if has_header in header_check_dict.keys():
		has_header = header_check_dict[has_header]
	else:
		print("Error! bad has_header flag, please use one of 0,1,True,False")
	
	min_order = raw_input("Enter minimum order to check: ")
	max_order = raw_input("Enter the maximum order to check: ")
	
	file_loc = str(file_loc)
	seperator = str(seperator)
	has_header = str(has_header)
	min_order = int(min_order)
	max_order = int(max_order)
	
	return [file_loc,seperator,has_header,min_order,max_order]
	

def get_edge_series(file_location, seperator, has_header):
	
	check_file = os.path.isfile(file_location)
	
	if check_file == True:
		edge_series = temp_net_parser.parse_edge_file(file_location, seperator, has_header)
	else:
		print("Error, file not found.")
	
	return edge_series


if __name__ == "__main__":
	main()