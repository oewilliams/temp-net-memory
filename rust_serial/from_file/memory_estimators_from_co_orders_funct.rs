
//Get the K-L distance between two binary series (this plays the same roll as the 
// conditional entropy did for the memory of general sequences).
pub fn get_kl_dist(target_series: &[i8], driver_series: &[i8], series_len: i32, order: i32) -> f64 {

    //Make indexes, most recent value has lowest contribution.
    let mut sub_index = (0..order as usize).fold(0,|acc, x| (acc << 1) | (driver_series[(order as usize) - x - 1] as usize));
    let mut h_sub_index = (sub_index << 1) | (target_series[order as usize] as usize);
    
    let h_size: i32 = 1<<(order+1);
    let s_size: i32 = 1<<order;
    let mut state_count_h: Vec<i32> = vec![0;h_size as usize];
    let mut state_count: Vec<i32> = vec![0;s_size as usize];
    state_count_h[h_sub_index] += 1;
    state_count[sub_index] += 1;

    //Shift sub_index back one, add the new series_2 entry, then remove the old one.
    //Then append series_1 entry for h_sub_index.
    for i in order as usize..(series_len-1) as usize {
        sub_index = (sub_index << 1) | (driver_series[i] as usize);
		sub_index &= !(1 << order);
	    h_sub_index = (sub_index << 1) | (target_series[i+1] as usize); 

	    state_count_h[h_sub_index] += 1;
	    state_count[sub_index] += 1;
  }

    //Sum to get entropy value.
    (0..h_size as usize).filter(|i| state_count_h[*i] != 0)
                        .fold(0.0,|acc,x| acc + 
                        (f64::from(state_count_h[x]) * f64::ln(f64::from(state_count_h[x])/f64::from(state_count[x >> 1]))))
                        as f64

}

//Get the co-order estimate of two series. Note that this is not symmetric.
pub fn get_co_order_estimate(target_series: &[i8], driver_series: &[i8], max_order: i32, min_order: i32, series_len: i32) -> i32 {

    let num_obs: i32 = series_len - 1;

    (min_order..max_order).map(|i| (f64::from(1<<(i+1))*(f64::ln(f64::ln(f64::from(num_obs))))) 
                                    - get_kl_dist(target_series, driver_series, series_len, i))
                          .enumerate()
                          .min_by(|(_,a),(_,b)| a.partial_cmp(b).unwrap())
                          .map(|(index,_)| index)
                          .unwrap() as i32
}

//Get the shape of the memory in a network.
//Return is a vector of vectors.
//Each subvector in the return represents a sorce link, a target link and the co-order.
pub fn get_network_memory_shape(edge_series: Vec<Vec<i8>>, max_order: i32, min_order: i32) -> Vec<Vec<i32>> {

    let series_len = edge_series[0].len() as i32;
    let num_links = edge_series.len();

    let link_pairs = num_links * num_links;

    let mut order_table = vec![vec![0;3];link_pairs];

    for i in 0..num_links {
         let order_est: Vec<[i32;2]> = edge_series.iter().enumerate()
            .map(|(j,v)| [j as i32, get_co_order_estimate(&edge_series[i], &v, max_order, min_order, series_len)])
            .collect::<Vec<[i32;2]>>();

        for entry in order_est {
            let dum_j = entry[0] as usize;
            order_table[i*num_links + dum_j] = [i as i32, dum_j as i32, entry[1]].to_vec();
        }    
    }
    order_table
}

//Get the effective memory of a network.
pub fn get_network_effective_memory(edge_series: Vec<Vec<i8>>, max_order: i32, min_order: i32) -> i32 {

    let series_len = edge_series[0].len() as i32;
    let num_lnks = edge_series.len();


    (0..num_lnks).map(|i| edge_series.iter()
            .map(|v| get_co_order_estimate(&edge_series[i], &v, max_order, min_order, series_len))
            .max()
            .unwrap())
            .max()
            .unwrap()
}
