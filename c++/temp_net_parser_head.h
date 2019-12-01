#ifndef temp_net_parser_head
#define temp_net_parser_head

std::vector<int> parse_edge_string(std::string string_edge, char seperator);
std::vector<std::vector<bool> > convert_edge_list_format(std::vector<std::vector<int> > int_edge_list);
std::vector<std::vector<bool> > parse_edge_file(std::string file_path, char seperator, bool has_headline=false);


#endif