#ifndef Synthetic_generators_head
#define Synthetic_generators_head

std::vector< std::vector<bool> > DARNp_generator(int N, int p, double q, double y, int T, int wait_time, const unsigned int seed);
std::vector< std::vector<bool> > DARNp_ext_generator(int L, double q, double y, int T, std::vector<int> &link_mems, int wait_time, const unsigned int seed);
std::vector< std::vector<bool> > CDARNp_symm_generator(int N, int p, double q, double y, double c, int T, int wait_time, const unsigned int seed);
std::vector< std::vector<bool> > CDARNp_ext_symm_generator(int L, double q, double y, double c, int T, std::vector<std::pair<int,int> > &link_mems, int wait_time, const unsigned int seed);

#endif