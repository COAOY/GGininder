#include "atpg.h"

#include "dlib/bayes_utils.h"
#include "dlib/graph_utils.h"
#include "dlib/graph.h"
#include "dlib/directed_graph.h"
#include "iostream"
#include <unordered_set>
#include <unordered_map>
using namespace dlib;
// using namespace std;
using std::cout;
using std::endl;
using namespace bayes_node_utils;
// using std::vector;

float return_gate_true_probability(int type,std::vector<bool> input);
std::vector<std::vector<bool> > enurmate_true_tables(int input_num);
void set_parent_argument(directed_graph<bayes_node>::kernel_1a_c&,int,int,assignment&,std::unordered_map<size_t,std::vector<size_t> >&);


void ATPG::run_bayesian()
{
	
	directed_graph<bayes_node>::kernel_1a_c bn;
	size_t nodes_num=0;
	// hash_nlist
	// vvnlist
	// set node(gate) list, ignore dummy_gate
	for(auto &x:hash_nlist){
		for(auto &y:x){
			y->id=nodes_num++;
			if(y->name[5]!='_'){
				vnlist.push_back(&y);
				node_dic[y->name]=y->id;
			}

		}
		
	}
	bn.set_number_of_nodes(nodes_num);

// connect the circuit and make it to a graph, ignore dummy_gate

	std::unordered_map<size_t,std::vector<size_t> > gate_input_dic;
	std::unordered_map<size_t,std::string> gate_name_dic;

cout << "[bayesian]..create edge"<<endl;
	for(auto &x:vnlist){
		std::unordered_set<size_t> ex_dic;
		std::vector<size_t> iwire_dic;
		// cout << (*x)->id <<" " << (*x)->name <<endl;
		for(auto &y:(*x)->iwire){

			for(auto &z:y->inode){
				if(!ex_dic.count(z->id)&& (z->name[5]!='_')){
					ex_dic.insert(z->id);
					(*x)->inode.push_back(z);
					bn.add_edge(z->id,(*x)->id);
					iwire_dic.push_back(z->id);
					// cout <<z->name<< " -> " << (*x)->name <<endl;
				}
			}
		}
		gate_name_dic[(*x)->id]=(*x)->name;
		gate_input_dic[(*x)->id]=iwire_dic;
	}


// set each node(gate) artiburate is bool
cout << "[bayesian]..set gate value(1,0)" <<endl;
	for(int i=0;i<nodes_num;i++){
		set_node_num_values(bn,i,2);
	}

	assignment parent_state;

// truth table example
	// int tm_size=4;
	// std::vector<std::vector<bool> > tm=enurmate_true_tables(tm_size);
	// for(int i=0;i<tm.size();i++){
	// 	for(int j=0;j<tm_size;j++)
	// 		cout << tm[i][tm_size-j-1]<<" ";
	// 	cout <<" result of type XNOR is " << return_gate_true_probability(0,tm[i]); 
	// 	cout<<endl;
	// }
cout << "[bayesian]..set probability of value" <<endl;
	for(auto &x:vnlist){
		// cout << (*x)->name <<" ";
		// for(auto &y:gate_input_dic.find((*x)->id)->second)
			// cout << gate_name_dic.find(y)->second <<" ";cout<<endl;
		set_parent_argument(bn,(*x)->id,(*x)->type,parent_state,gate_input_dic);
	}

	typedef dlib::set<unsigned long>::compare_1b_c set_type;
	typedef graph<set_type, set_type>::kernel_1a_c join_tree_type;
	join_tree_type join_tree;
	create_moral_graph(bn,join_tree);
	create_join_tree(join_tree,join_tree);
	bayesian_network_join_tree solution(bn,join_tree);


cout << "[bayesian]get the probability to table" <<endl;
	for(auto &x:hash_nlist){
		for(auto &y:x){
			if(y->name[5]!='_'){
				truth_prob_by_id[y->id]=solution.probability(y->id)(1);
			}

		}
		
	}

	


	return;
}

std::vector<std::vector<bool> > enurmate_true_tables(int input_num)
{
	bool xor_triger=true;
	std::vector<std::vector<bool> > truth_table;
	std::vector<bool> state(input_num,false);
	truth_table.push_back(state);
	for(int i=0;i<std::pow(2,input_num)-1;i++){
		for(int j=input_num;j>0;j--){
			bool result=true;
			for(int k=0;k<j;k++)
				result=result&state[k];
			state[j]=state[j]^result;
		}
		state[0]=state[0]^xor_triger;
		truth_table.push_back(state);
	}
	return truth_table;
}

void set_parent_argument(
	directed_graph<bayes_node>::kernel_1a_c& bn,
	int gate_id,
	int gate_type,
	assignment& parent_state,
	std::unordered_map<size_t,std::vector<size_t> >& gate_input_dic
	)
{
	parent_state.clear();
	std::vector<size_t> gate_inp=gate_input_dic.find(gate_id)->second;
	std::vector<std::vector<bool> > tb=enurmate_true_tables((int)gate_inp.size());
	bool set_target=false;

	for(auto&x:tb){
		for(int i=0;i<x.size();i++){
			// cout << gate_id <<endl;
			// cout << i << " " <<gate_inp[i]<<" " <<x[i]<<endl;
			if(!set_target){
				parent_state.add(gate_inp[i],x[i]);
				
			}
			else{
				parent_state[gate_inp[i]] = x[i];
			}
		}
		set_target=true;
		// cout << "set_default\n";
		set_node_probability(bn,gate_id,1,parent_state,return_gate_true_probability(gate_type,x));
		set_node_probability(bn,gate_id,0,parent_state,return_gate_true_probability(gate_type,x));

		// cout << "set_success\n";
	}

	parent_state.clear();

	
}




float return_gate_true_probability(int type,std::vector<bool> input)
{
	// #define NOT       1
	// #define NAND      2
	// #define AND       3
	// #define INPUT     4
	// #define NOR       5
	// #define OR        6
	// #define OUTPUT    8
	// #define XOR      11
	// #define BUF      17
	// #define EQV	      0	/* XNOR gate */
	if(input.size()==0){
		switch(type){
			case(1):
				return 0.5;
			break;
			case(2):
				return 0.75;
			break;
			case(3):
				return 0.25;
			break;
			case(5):
				return 0.25;
			break;
			case(6):
				return 0.75;
			break;
			case(11):
				return 0.5;
			break;
			case(0):
				return 0.5;
			break;
			default:
				return 0.5;
			break;
		}
	}
	else if(type==1){
		return !input[0];
	}
	else if(type==2){
		bool r=input[0];
		for(int i=1;i<input.size();i++)
			r=r&input[i];
		return !r;
	}
	else if(type==3){
		bool r=input[0];
		for(int i=1;i<input.size();i++)
			r=r&input[i];
		return r;
	}
	else if(type==5){
		bool r=input[0];
		for(int i=1;i<input.size();i++)
			r=r|input[i];
		return !r;
	}
	else if(type==6){
		bool r=input[0];
		for(int i=1;i<input.size();i++)
			r=r|input[i];
		return r;
	}
	else if(type==11){
		bool r=input[0];
		for(int i=1;i<input.size();i++)
			r=r^input[i];
		return r;
	}
	else if(type==0){
		bool r=input[0];
		for(int i=1;i<input.size();i++)
			r=r^input[i];
		return !r;
	}
	else{
		std::cout<< "The surprise gate "<< type << " was be found in our atpg!!\n";
		assert(0);
	}

	cout << "No case suit gate selection\n";
	assert(0);

}








