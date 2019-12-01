import java.util.*;

class DARNp_generator {
	
	public DARNp_generator() {
		
	}
	
	//Generate Bernouli random variable.
	private static boolean bernoulli(Double p, Random rand) {
		if (rand.nextDouble() < p) {
			return true;
		}
		else {
			return false;
		}
	}
	
	//Generate DARNp edge series.
	public static boolean[][] generate_DARNp_net(int nodes, int mem_leng, double mem_str, double avg_prob, int time_steps) {
		
		Random rand = new Random();
		int i,t;
		int past_val;
		boolean mem_check;
		
		int num_links = (nodes * (nodes-1))/2;
		
		//Initialise array of bools for the edge series.
		boolean[][] edge_series = new boolean[num_links][time_steps+mem_leng];
		
		//Make edge series. Note that since links are independent we do this link by link.
		for (i=0 ; i<num_links ; i++) {
			//Initialise the memory of the network
			for (t=0 ; t<mem_leng ; t++) {
				edge_series[i][t] = bernoulli(avg_prob, rand);
			}
			
			//Now fill the edge list.
			for (t=mem_leng ; t<time_steps+mem_leng ; t++) {
				mem_check = bernoulli(mem_str, rand);
				
				if (mem_check) {
					past_val = rand.nextInt(mem_leng)+1;
					edge_series[i][t] = edge_series[i][t-past_val];
				}
				else {
					edge_series[i][t] = bernoulli(avg_prob, rand);
				}
			}
		}
		
		return edge_series;
	}
	
	public static void main(String[] args) {
		
	}
}