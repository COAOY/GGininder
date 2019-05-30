/**********************************************************************/
/*  Parallel-Fault Event-Driven Transition Delay Fault Simulator      */
/*                                                                    */
/**********************************************************************/

#include "atpg.h"
#include <cassert>
/* pack 16 faults into one packet.  simulate 16 faults togeter. 
 * the following variable name is somewhat misleading */
#define num_of_pattern 16


void ATPG::transition_delay_fault_simulation(vector<int>** pattern_list) {
  int total_detect_num = 0;
  int total_fault_num = 0;
  int current_detect_num = 0;

  // calculate the total fault number

  // run through every pattern
  for (int i = vectors.size()-1; i >= 0;i--)
  // for (int i=0; i < vectors.size();i++)
  {
    tdf_sim_a_vector(vectors[i], current_detect_num, pattern_list, i);
    total_detect_num += current_detect_num;
    if (pattern_list==NULL)
    {
        cout << "# vector[" << i << "] detects " << current_detect_num << " faults ("
            << total_detect_num << ")" << endl;
    }
  }

  if (pattern_list==NULL)
  {
      for (auto pos = flist_undetect.cbegin(); pos != flist_undetect.cend(); ++pos) 
        total_fault_num += (*pos)->eqv_fault_num ;
      cout<<"# Result:" << endl
          <<"-----------------------"<<endl
          <<"# total transition delay faults: "<<total_fault_num<<endl
          <<"# total detected faults: "<<total_detect_num <<endl;
      cout.precision(8);
      cout <<"# fault coverage: "<<100*( double( total_detect_num )/double( total_fault_num ) )<<" %" << endl;
  }
}

void ATPG::tdf_sim_a_vector(const string& vector_i, int& current_detect_num, vector<int>** pattern_list, const int& i) {
      //assign values to the PI
      forward_list<fptr>* fault_ls_ptr = &flist_stc;
      if (pattern_list == NULL) fault_ls_ptr = &flist_undetect;

      for(int j = 0; j < cktin.size(); j++) {
        cktin[j]->value = ctoi(vector_i[j]);
      }
      for (int j = 0; j < sort_wlist.size(); j++) {
        if (j < cktin.size()) {
          sort_wlist[j]->flag |= CHANGED;
        }
        else {
          sort_wlist[j]->value = U;
        }
      }

      sim();
      //for (auto pos = flist_undetect.cbegin(); pos != flist_undetect.cend(); ++pos) 
      for (auto pos = fault_ls_ptr->cbegin(); pos != fault_ls_ptr->cend(); ++pos) 
      {
        fptr f = *pos;
        if (f->fault_type == sort_wlist[f->to_swlist]->value) f->activate = 1;
        else f->activate = 0;
      }
      string vec = "";
      vec+=( vector_i.back() );
      for (int j =1; j < cktin.size(); j++){
          vec+=( vector_i[j-1] );
      }
    
      current_detect_num = 0;
      fault_sim_a_vector(vec, current_detect_num, pattern_list, i);
}

void ATPG::transition_delay_fault_simulation(vector<string>& T_compact) {
  int total_detect_num = 0;
  int total_fault_num = 0;
  int current_detect_num = 0;

  // calculate the total fault number

  // run through every pattern
  flist_undetect = flist_stc;
  for (int i = vectors.size()-1; i >= 0;i--)
  {
    tdf_sim_a_vector(vectors[i], current_detect_num, NULL, i);
    total_detect_num += current_detect_num;
    if (current_detect_num!=0)
          T_compact.push_back(vectors.at(i));
  }

}
void ATPG::transition_delay_fault_simulation(vector<int>** pattern_list, vector<int>& indices) {
  int total_detect_num = 0;
  int total_fault_num = 0;
  int current_detect_num = 0;

  // calculate the total fault number

  // run through every pattern
  assert(indices.size() == vectors.size());
  for (int i = vectors.size()-1; i >= 0;i--)
  {
    tdf_sim_a_vector(vectors[i], current_detect_num, pattern_list, indices[i]);
    total_detect_num += current_detect_num;
    if (pattern_list==NULL)
    {
        cout << "# vector[" << i << "] detects " << current_detect_num << " faults ("
            << total_detect_num << ")" << endl;
    }
  }

  if (pattern_list==NULL)
  {
      for (auto pos = flist_undetect.cbegin(); pos != flist_undetect.cend(); ++pos) 
        total_fault_num += (*pos)->eqv_fault_num ;
      cout<<"# Result:" << endl
          <<"-----------------------"<<endl
          <<"# total transition delay faults: "<<total_fault_num<<endl
          <<"# total detected faults: "<<total_detect_num <<endl;
      cout.precision(8);
      cout <<"# fault coverage: "<<100*( double( total_detect_num )/double( total_fault_num ) )<<" %" << endl;
  }
}
