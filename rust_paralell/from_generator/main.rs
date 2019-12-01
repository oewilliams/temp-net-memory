mod network_generator;
mod memory_estimators_from_co_orders_funct;
/*
    Test program for finding the memory of a temporal network given by a generator.
    This is the master program. 
*/

fn main() {
    let nodes = 20;
    let mem_leng = 4;
    let mem_strength = 0.9;
    let avg_prob = 0.1;
    let time_steps = 100_000;
    let wait_time = 100;
    let max_order = 10;
    let min_order = 0;

    let edge_series = network_generator::DARNp_generator(nodes,mem_leng,mem_strength,avg_prob,time_steps,wait_time);
    let memory_est = memory_estimators_from_co_orders_funct::get_network_effective_memory(edge_series, max_order, min_order);

    println!("The estimated memory is: {}\n",memory_est);
}
