class get_memory_from_file {

    public get_memory_from_file() {

    }

    public static void main(String[] args) {
        
        //* Pre set values here, comment out to use command line arguments.
        String file_location = "/Users/oliverwilliams/Documents/qmul/Network_memory_project/release_builds/manufacturing_elist_comp_res_60_links_100.txt";
        String separator = "\t";
        boolean has_header = false;
        int min_order = 0;
        int max_order = 10;
        //*/

        /* Comment out to use pre set values.
        String file_location = args[1];
        String separator = args[2];
        boolean has_header = Boolean.parseBoolean(args[3]);
        int min_order = Integer.parseInt(args[4]);
        int max_order = Integer.parseInt(args[5]);
        //*/

        boolean[][] edge_series = temp_net_parser.parse_edge_file(file_location, separator, has_header);
        int order_est = memory_estimators_with_co_order_coll.get_network_effective_memory(edge_series, max_order, min_order);

        System.out.println(order_est);
    }
}