mod memory_estimators_from_co_orders_funct;
mod temp_net_parser;

fn main() {
    
    let file_path = "/Users/oliverwilliams/Documents/qmul/Network_memory_project/release_builds/manufacturing_elist_comp_res_60_links_100.txt".to_string();
    let seperator: char = '\t';
    let has_header: bool = true;
    let min_order: i32 = 0;
    let max_order: i32 = 10;

    let edge_series = temp_net_parser::parse_edge_file(file_path,seperator,has_header);
    println!("{}\t{}",edge_series.len(),edge_series[0].len());
    //let order_est = memory_estimators_from_co_orders_funct::get_network_effective_memory(edge_series, max_order, min_order);

    //println!("Memory estimate: {}",order_est);
}
