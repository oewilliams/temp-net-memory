class memory_estimators_with_co_order {
	
	public memory_estimators_with_co_order() {
		
	}
	
	public static int get_network_memory_EDC(boolean[][] edge_series, int max_order, int min_order) {
		
		//Get the link and network memories then take the max.
		int link_memory = EDC_link_memory_est(edge_series, max_order, min_order);
		int net_memory = EDC_network_memory_est(edge_series, max_order, min_order);
		
		if(link_memory > net_memory) {
			return link_memory;
		}
		else {
			return net_memory;
		}
	}
	
	
	public static int EDC_network_memory_est(boolean[][] edge_series, int max_order, int min_order) {
		
		int links = edge_series.length;
		int time_len = edge_series[0].length;
		
		int[] series = new int[time_len];
		
		int net_mem_est=0;
		int new_est;
		
		for (int i=0 ; i<links ; i++) {
			for (int j=0 ; j<i ; j++) {
				series = combine_series(edge_series[i], edge_series[j], time_len);
				new_est = EDC_est(series, max_order, min_order, time_len, 4);
				
				if (new_est > net_mem_est) {
					net_mem_est = new_est;
					/*	
					If you know that you have enough data to ensure that 
					the estimator function will be convex uncomment the following...
					*/
					//min_order = net_mem_est;
				}
			}
		}
	return net_mem_est;
	}
	
	
	public static int EDC_link_memory_est(boolean[][] edge_series, int max_order, int min_order) {
		
		int links = edge_series.length;
		int time_len = edge_series[0].length;
		
		int[] series = new int[time_len];
		
		int link_mem_est = 0;
		int new_est;
		
		for (int i=0 ; i<links ; i++) {
			for (int t=0 ; t<time_len ; t++) {
				series[t] = (edge_series[i][t])? 1 : 0;
			}
			new_est = EDC_est(series, max_order, min_order, time_len, 2);
			
			if (new_est > link_mem_est) {
				link_mem_est = new_est;
				/*	
				If you know that you have enough data to ensure that 
				the estimator function will be convex uncomment the following...
				*/
				//min_order = link_mem_est;
			}
		}
		return link_mem_est;
	}
	
	
	public static int EDC_est(int[] series, int max_order, int min_order, int series_len, int state_size) {
		
		int i;
		int order;
		double conditional_likelihood;
		int num_obs;
		int min_index=0;
		double EDC_val;
		double EDC_min = Double.POSITIVE_INFINITY;
		int[] order_shift = new int[max_order+2];
		
		order_shift[0] = 1;
		for (i=1 ; i<max_order+2 ; i++) {
			order_shift[i] = 1<<((state_size/2)*i);
		}
		
		for (order=min_order ; order<=max_order ; order++) {
			conditional_likelihood = conditional_entropy(series, order, series_len, state_size, order_shift);
			num_obs = series_len - order;
			EDC_val = (-2*conditional_likelihood) + (Math.pow(state_size,order+1)*(Math.log(Math.log(num_obs)))*2);
			if(EDC_val < EDC_min) {
				EDC_min = EDC_val;
				min_index = order;
			}
			/*	
			If you know that you have enough data to ensure that 
			the estimator function will be convex uncomment the following...
			*/
			/*
			else {
				return min_index;
			}
			*/
		}
		return min_index;
	}
	
	
	public static double conditional_entropy(int[] series, int order, int series_len, int state_size, int[] order_shift) {
		
		int h_size = order_shift[order+1];
		int s_size = order_shift[order];
		int[] state_count_h = new int[h_size];
		int[] state_count = new int[s_size];
		
		int i;
		int h_sub_index = series[order];
		int sub_index = 0;
		
		for(i=0 ; i<order ; i++) {
			h_sub_index += series[order - i - 1] * order_shift[i+1]; 
			sub_index += series[order - i -1] * order_shift[i]; 
		}

		state_count_h[h_sub_index] += 1;
		state_count[sub_index] += 1;
		
		//	Here we shift the index.
		//	first remove the oldest value, then multiply by state size to add new element, then add new value.
		for(i=order+1 ; i<series_len-order-1 ; i++) {
			h_sub_index = (state_size * (h_sub_index - (series[i-order-1] * order_shift[order]))) + series[i];
			sub_index = (int)Math.floor(h_sub_index/state_size);		//Use relationship between two indexes.
			state_count_h[h_sub_index] += 1;
			state_count[sub_index] += 1;
		}

		//Add up all values.
		double sum_val = 0.0;
		int s_term;
		for(i=0 ; i< h_size ; i++) {
			if (state_count_h[i] !=0) {
			s_term = (int)(Math.floor(i/state_size));
			sum_val += (double)(state_count_h[i]) * Math.log((double)(state_count_h[i])/(double)(state_count[s_term]));
			}
		}
		return sum_val;
	}
	
	
	private static int[] combine_series(boolean[] series_1, boolean[] series_2, int series_len) {
		
		int[] comb_series = new int[series_len];
		
		int entry_1, entry_2;
		
		for (int i=0 ; i<series_len ; i++) {
			entry_1 = series_1[i]? 1 : 0;
			entry_2 = series_2[i]? 1 : 0;
			comb_series[i] = entry_1 + (2 * entry_2);
		}
		return comb_series;
	}


    public static int get_network_memory_from_co_order(boolean[][] edge_series, int max_order, int min_order) {
        
        int i,j;
	
	    int series_len = edge_series[0].length;
	    int num_links = edge_series.length;
	
	    int order_est = 0;
        int max_observed_order = 0;
        
        int[] target_series = new int[series_len];
        int[] driver_series = new int[series_len];
	
    	for(i=0 ; i<num_links ; i++) {
            target_series = convert_series(edge_series[i], series_len);
	    	for(j=0 ; j<num_links ; j++) {
                driver_series = convert_series(edge_series[j], series_len);
		    	order_est = get_co_order_estimate(target_series, driver_series, max_order, min_order, series_len);
			    if (order_est > max_observed_order) {
				    max_observed_order = order_est;
				/*	
				If you know that you have enough data to ensure that 
				the estimator function will be convex uncomment the following...
				*/
    			//	min_order = max_observed_order;
	    		}
		    }
	    }
	    return order_est;
    }

    public static int get_co_order_estimate(int[] target_series, int[] driver_series, int max_order, int min_order, int series_len) {
        double conditional_likelihood = 0.0;
        double EDC_min = Double.POSITIVE_INFINITY;
	    int min_index = 0;
	    double EDC_val = 0.0;
	    int num_obs;
	

	    for(int i=min_order ; i<=max_order ; i++) {
		    conditional_likelihood = get_kl_dist(target_series, driver_series, series_len, i);
    		num_obs = series_len - 1;
	    	EDC_val = -conditional_likelihood + ((1<<(i+1))*(Math.log(Math.log(num_obs))));
		    if(EDC_val < EDC_min) {
			    EDC_min = EDC_val;
    			min_index = i;
	    	}
			/*	
			If you know that you have enough data to ensure that 
			the estimator function will be convex uncomment the following...
			*/
		    /*
			else {
			    return min_index;
		    }
			*/
	    }
	    return min_index;
    }

    public static double get_kl_dist(int[] target_series, int[] driver_series, int series_len, int order) {
        
        int h_size = 1 << (order+1);
        int s_size = 1 << order;
        int[] state_count_h = new int[h_size];
		int[] state_count = new int[s_size];
        
        int i;
        int h_sub_index = 0;	//Index for counting sequence with present state of series_1.
        int sub_index = 0;		//Index for counting sequence with only entries of series_2.
        
        //Make indexes, most recent value has lowest contribution.
        for(i=0 ; i<order ; i++) {
            sub_index = (sub_index << 1) | driver_series[order-i-1];
        }
        h_sub_index = (sub_index << 1) | target_series[order];
        
        state_count_h[h_sub_index] += 1;
        state_count[sub_index] += 1;
        
        
        //Shift sub_index back one, add the new series_2 entry, then remove the old one.
        //Then append series_1 entry for h_sub_index.
        for(i=order ; i<series_len-1 ; i++) {
            sub_index = ((sub_index << 1) | driver_series[i]);
            sub_index &= ~(1 << order);
            h_sub_index = (sub_index << 1) | target_series[i+1]; 
            
            state_count_h[h_sub_index] += 1;
            state_count[sub_index] += 1;
        }
        
        //Add up all values.
        double sum_val = 0.0;
        int s_term;
        for(i=0 ; i< h_size ; i++) {
            if (state_count_h[i] !=0) {
            s_term = i >> 1;
            sum_val += (double)(state_count_h[i]) * Math.log((double)(state_count_h[i])/(double)(state_count[s_term]));
            }
        }
        return sum_val;
    }
    
    private static int[] convert_series(boolean[] series, int series_len) {
        int[] conv_series = new int[series_len];
        for(int i=0 ; i<series_len ; i++) {
            conv_series[i] = series[i] ? 1 : 0;
        }
        return conv_series;
    }
	
	public static void main(String[] args) {
			
	}
}