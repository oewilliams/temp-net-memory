#include <iostream>
#include <vector>
#include <math.h>
#include <random>
#include <string>
//Generators for synthetic models with known memory.


class Uniform {
private:
	std::mt19937_64 mt;
	std::uniform_real_distribution<double> u;
public:
	Uniform(const unsigned int s) : mt(s) {}
	double operator()() {
		return u(mt);
	}
};

//Generate a DARN(p) temporal network with N nodes, memory p, memory strength q and link chance y for T steps.
//Model waits for "wait_time" time steps before filling edge series.
std::vector< std::vector<bool> > DARNp_generator(int N, int p, double q, double y, int T, int wait_time, const unsigned int seed) {
	
	Uniform well(seed);		//Seed RNG.
	
	int i,t;
	int ref_time;
	int num_links = N*(N-1)/2;
	
	std::vector< std::vector<bool> > edge_series(num_links, std::vector<bool>(T,0));
	std::vector< std::vector<bool> > init_series(num_links, std::vector<bool>(p + wait_time,0));
	
	//Random initialisation.
	for(t=0 ; t<p ; t++) {
		for(i=0 ; i<num_links ; i++) {
			init_series[i][t] = (well() < y) ? 1 : 0;
		}
	}
	//DARN(p) warm-up.
	for (t=p; t<wait_time ; t++) {
		for(i=0 ; i<num_links ; i++) {
			if(well() < q){
				ref_time = int(floor(well()*p))+1;					//Get time in past.
				init_series[i][t] = init_series[i][t-ref_time];	//Get point in past.
			}
			else {
				init_series[i][t] = (well() < y) ? 1 : 0;
			}
		}
	}

	//Fill first p steps of edge series.
	for (t=0; t<p ; t++) {
		for(i=0 ; i<num_links ; i++) {
			if(well() < q){
				ref_time = int(floor(well()*p))+1;								//Get time in past.
				if(ref_time > t) {
					edge_series[i][t] = init_series[i][p+wait_time-ref_time+t];	//Get point in past with shift from init series.
				}
				else {
					edge_series[i][t] = edge_series[i][t-ref_time];				//Get point in past from edge series.
				}
			}
			else {
				edge_series[i][t] = (well() < y) ? 1 : 0;
			}
		}
	}
	
	//Fill rest of edge series.
	for (t=p; t<T ; t++) {
		for(i=0 ; i<num_links ; i++) {
			if(well() < q){
				ref_time = int(floor(well()*p))+1;					//Get time in past.
				edge_series[i][t] = edge_series[i][t-ref_time];	//Get point in past.
			}
			else {
				edge_series[i][t] = (well() < y) ? 1 : 0;
			}
		}
	}
	return edge_series;
}


//Generate am extended DARN(p) temporal network with L nodes, memory p, memory strength q and link chance y for T steps.
//Model waits for "wait_time" time steps before filling edge series.
//Takes number of links insted of nodes and a vector of link memories.
std::vector< std::vector<bool> > DARNp_ext_generator(int L, double q, double y, int T, std::vector<int> &link_mems, int wait_time, const unsigned int seed) {
	
	Uniform well(seed);		//Seed RNG.
	int i,t;
	int ref_time;
	int num_links = L;
	int p;
	
	//Get maximum memory length used.
	int p_max = 0;
	for(int x : link_mems) {
		p_max = (p_max > x) ? p_max : x;
	}
	
	std::vector< std::vector<bool> > edge_series(num_links, std::vector<bool>(T,0));
	std::vector< std::vector<bool> > init_series(num_links, std::vector<bool>(p_max + wait_time,0));
	
	//Random initialisation.
	for(t=0 ; t<p_max ; t++) {
		for(i=0 ; i<num_links ; i++) {
			init_series[i][t] = (well() < y) ? 1 : 0;
		}
	}
	//DARN(p) warm-up.
	for (t=p_max; t<wait_time ; t++) {
		for(i=0 ; i<num_links ; i++) {
			p = link_mems[i];
			if(well() < q){
				ref_time = int(floor(well()*p))+1;					//Get time in past.
				init_series[i][t] = init_series[i][t-ref_time];		//Get point in past.
			}
			else {
				init_series[i][t] = (well() < y) ? 1 : 0;
			}
		}
	}

	//Fill first p steps of edge series.
	for (t=0; t<p_max ; t++) {
		for(i=0 ; i<num_links ; i++) {
			p = link_mems[i];
			if(well() < q){
				ref_time = int(floor(well()*p))+1;								//Get time in past.
				if(ref_time > t) {
					edge_series[i][t] = init_series[i][p+wait_time-ref_time+t];	//Get point in past with shift from init series.
				}
				else {
					edge_series[i][t] = edge_series[i][t-ref_time];				//Get point in past from edge series.
				}
			}
			else {
				edge_series[i][t] = (well() < y) ? 1 : 0;
			}
		}
	}
	
	//Fill rest of edge series.
	for (t=p_max; t<T ; t++) {
		for(i=0 ; i<num_links ; i++) {
			p = link_mems[i];
			if(well() < q){
				ref_time = int(floor(well()*p))+1;					//Get time in past.
				edge_series[i][t] = edge_series[i][t-ref_time];		//Get point in past.
			}
			else {
				edge_series[i][t] = (well() < y) ? 1 : 0;
			}
		}
	}
	return edge_series;
}



//Generate a CDARN(p) temporal network with N nodes, memory p, memory strength q and link chance y for T steps.
//Model has symmetric coupling with bias c.
//Model waits for "wait_time" time steps before filling edge series.
std::vector< std::vector<bool> > CDARNp_symm_generator(int N, int p, double q, double y, double c, int T, int wait_time, const unsigned int seed) {
	
	Uniform well(seed);		//Seed RNG.
	
	int i,t;
	int ref_link;
	int ref_time;
	int num_links = N*(N-1)/2;
	
	std::vector< std::vector<bool> > edge_series(num_links, std::vector<bool>(T,0));
	std::vector< std::vector<bool> > init_series(num_links, std::vector<bool>(p + wait_time,0));
	
	//Random initialisation.
	for(t=0 ; t<p ; t++) {
		for(i=0 ; i<num_links ; i++) {
			init_series[i][t] = (well() < y) ? 1 : 0;
		}
	}
		
	//CDARN(p) warm-up.
	for (t=p; t<wait_time ; t++) {
		for(i=0 ; i<num_links ; i++) {
			if(well() < q){
				ref_time = int(floor(well()*p))+1;									//Get time in past.
				ref_link = (well() < c) ? i : int(floor(well()*num_links));			//Get link to refer to.
				init_series[i][t] = init_series[ref_link][t-ref_time];				//Get point in past.
			}
			else {
				init_series[i][t] = (well() < y) ? 1 : 0;
			}
		}
	}
	
	//Fill first p steps of edge series.
	for (t=0; t<p ; t++) {
		for(i=0 ; i<num_links ; i++) {
			if(well() < q){
				ref_time = int(floor(well()*p))+1;								//Get time in past.
				ref_link = (well() < c) ? i : int(floor(well()*num_links));			//Get link to refer to.
				if(ref_time > t) {
					edge_series[i][t] = init_series[ref_link][p+wait_time-ref_time+t];	//Get point in past with shift from init series.
				}
				else {
					edge_series[i][t] = edge_series[ref_link][t-ref_time];				//Get point in past from edge series.
				}
			}
			else {
				edge_series[i][t] = (well() < y) ? 1 : 0;
			}
		}
	}
		
	//Fill rest of edge series.
	for (t=p; t<T ; t++) {
		for(i=0 ; i<num_links ; i++) {
			if(well() < q){
				ref_time = int(floor(well()*p))+1;									//Get time in past.
				ref_link = (well() < c) ? i : int(floor(well()*num_links));			//Get link to refer to.
				edge_series[i][t] = edge_series[ref_link][t-ref_time];						//Get point in past.
			}
			else {
				edge_series[i][t] = (well() < y) ? 1 : 0;
			}
		}
	}
	
	return edge_series;
}


//Generate an extended CDARN(p) temporal network with N nodes, memory p, memory strength q and link chance y for T steps.
//Model has symmetric coupling with bias c.
//Model waits for "wait_time" time steps before filling edge series.
//Takes number of links insted of nodes and a vector pairs for link memories, one for self, one for external.
std::vector< std::vector<bool> > CDARNp_ext_symm_generator(int L, double q, double y, double c, int T, std::vector<std::pair<int,int> > &link_mems, int wait_time, const unsigned int seed) {
	
	Uniform well(seed);		//Seed RNG.
	
	int i,t;
	int ref_link;
	int ref_time;
	int num_links = L;
	int p;
	
	//Get maximum memory length used.
	int p_max = 0;
	for(std::pair<int,int> x : link_mems) {
		p_max = (p_max > x.first) ? p_max : x.first;
		p_max = (p_max > x.second) ? p_max : x.second;
	}
	
	std::vector< std::vector<bool> > edge_series(num_links, std::vector<bool>(T,0));
	std::vector< std::vector<bool> > init_series(num_links, std::vector<bool>(p_max + wait_time,0));
	
	//Random initialisation.
	for(t=0 ; t<p_max ; t++) {
		for(i=0 ; i<num_links ; i++) {
			init_series[i][t] = (well() < y) ? 1 : 0;
		}
	}
	
	//CDARN(p) warm-up.
	for (t=p_max; t<wait_time ; t++) {
		for(i=0 ; i<num_links ; i++) {
			if(well() < q){
				if(well() > c) {
					ref_time = int(floor(well()*link_mems[i].second))+1;
					ref_link = int(floor(well()*num_links));
				}
				else {
					ref_time = int(floor(well()*link_mems[i].first))+1;
					ref_link = i;
				}
				init_series[i][t] = init_series[ref_link][t-ref_time];				//Get point in past.
			}
			else {
				init_series[i][t] = (well() < y) ? 1 : 0;
			}
		}
	}
	
	//Fill first p steps of edge series.
	for (t=0; t<p_max ; t++) {
		for(i=0 ; i<num_links ; i++) {
			if(well() < q){
				if (well() > c) {
					p = link_mems[i].second;
					ref_time = int(floor(well()*link_mems[i].second))+1;
					ref_link = int(floor(well()*num_links));
				}
				else {
					p = link_mems[i].first;
					ref_time = int(floor(well()*link_mems[i].first))+1;
					ref_link = i;
				}
				if(ref_time > t) {
					edge_series[i][t] = init_series[ref_link][p+wait_time-ref_time+t];	//Get point in past with shift from init series.
				}
				else {
					edge_series[i][t] = edge_series[ref_link][t-ref_time];				//Get point in past from edge series.
				}
			}
			else {
				edge_series[i][t] = (well() < y) ? 1 : 0;
			}
		}
	}
	//
	//Fill rest of edge series.
	for (t=p_max; t<T ; t++) {
		for(i=0 ; i<num_links ; i++) {
			if(well() < q){
				if(well() > c) {
					ref_time = int(floor(well()*link_mems[i].second))+1;
					ref_link = int(floor(well()*num_links));
				}
				else {
					ref_time = int(floor(well()*link_mems[i].first))+1;
					ref_link = i;
				}
				edge_series[i][t] = edge_series[ref_link][t-ref_time];				//Get point in past.
			}
			else {
				edge_series[i][t] = (well() < y) ? 1 : 0;
			}
		}
	}
	return edge_series;
}

