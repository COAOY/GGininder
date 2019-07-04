#include "atpg.h"

#include "dlib/bayes_utils.h"
#include "dlib/graph_utils.h"
#include "dlib/graph.h"
#include "dlib/directed_graph.h"
#include "iostream"
using namespace dlib;
using namespace std;


void ATPG::run_bayesian()
{
	using namespace bayes_node_utils;
	directed_graph<bayes_node>::kernel_1a_c bn;
	// size_t nodes_num=0;
	// // hash_nlist
	// // vvnlist
	// for(auto &x:hash_nlist){
	// 	vector<nptr_s> vn
	// 	for(auto &y:x){
	// 		vn.push_back(y);
	// 		nodes_num++;
	// 	}
	// 	vvnlist.push_back(vn);
	// }



	return;
}