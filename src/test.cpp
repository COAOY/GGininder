#include "atpg.h"
#define U  2
#define D  3
#define B  4
void ATPG::bubble_sort(vector<wptr>& vec)
{
  int bound=vec.size();
  for(int i=0;i<bound;i++)
  {
    for(int j=0;j<bound-i-1;j++){
      if( true_prob.find(vec[j]->name)->second < true_prob.find(vec[j+1]->name)->second  ){
        swap(vec[j],vec[j+1]);
      }
    }
  }
}
void ATPG::test(void) {
  string vec;
  int current_detect_num = 0;
  int total_detect_num = 0;
  int current_backtracks = 0;
  int no_of_aborted_faults = 0;
  no_of_redundant_faults = 0;
  no_of_calls = 0;
  total_no_of_backtracks = 0;  // accumulative number of backtracks

  fptr fault_under_test = flist_undetect.front();

  /* Fsim only mode */
  if(fsim_only)
  {
    fault_simulate_vectors(total_detect_num);
    in_vector_no+=vectors.size();
    display_undetect();
    fprintf(stdout,"\n");
    return;
  }// if fsim only
  
  if(tdfsim_only)
  {
    transition_delay_fault_simulation(NULL);
    
    return;
  }// if tdfsim only
set_backtrack_limit(32);
printf(" All filed excest!!\n");
if(bayesian==true){
  ;
}
if(cpdag==true){
      // change the sequence from true prob height to low
      // cout <<".........................................hihihihihihih:"<<endl;
      
      for(auto &y: hash_nlist){
        for(auto &x:y){
          cout << "True brobability of wire  " << x->name << ": ";
          for(auto &z:x->iwire){
            cout<< true_prob.find(z->name)->second << " ";
          }cout<<endl;
          cout << "False brobability of wire  " << x->name << ": ";
          for(auto &z:x->iwire){
            cout<< false_prob.find(z->name)->second << " ";
          }cout<<endl;
          // cout <<".........................................hihihihihihih:"<< x->name <<endl;
          bubble_sort(x->iwire);
          bubble_sort(x->owire);
        }
      }
    }

  /* ATPG mode */
  /* Figure 5 in the PODEM paper */

if(ssfatpg==true){
  while(fault_under_test != nullptr) {
    // cout << "Hi\n";
    unsigned old_size = patterns.size();
    // switch(podem(fault_under_test,current_backtracks)) {
    int _p=podem(fault_under_test,current_backtracks);
    // cout << "_p: " << _p <<endl;
    switch(_p) {
      case 1:
      cout << old_size<<endl;
        for(unsigned i=old_size; i<patterns.size(); i++) {

          // if(compression)   secondary_fault(patterns[i]);
          // cout << patterns[i] << endl;
          random_fill(patterns[i]);
          cout << "# SAF sim\n";
          fault_sim_a_vector(patterns[i], current_detect_num);
          // fault_sim_a_vector(const string&, int&, vector<int>**, const int&);  
          total_detect_num += current_detect_num;
          if(faultdrop) {
            cout << "# vector[" << i << "] detects " << current_detect_num << " faults ("
                << total_detect_num << ")" << endl;
          }
          else {
            cout << "# vector[" << i << "] detects " << current_detect_num << " faults" << endl;
          }
        }
        /* form a vector */
        // vec.clear();
        // for (wptr w: cktin) {
          // vec.push_back(itoc(w->value));
        // }
        /*by defect, we want only one pattern per fault */
        /*run a fault simulation, drop ALL detected faults */
        // if (total_attempt_num == 1) {
          // int dummy = 0;
          /* fault_sim_a_vector(vec, current_detect_num,NULL, dummy); */
          // tdf_sim_a_vector(vec, current_detect_num, NULL, dummy);
          // total_detect_num += current_detect_num;
        // }
        /* If we want mutiple petterns per fault, 
         * NO fault simulation.  drop ONLY the fault under test */ 
        // else {
          // fault_under_test->ndet++;
          // if(fault_under_test->ndet >= ndet) {
            // fault_under_test->detect = TRUE;
            /* drop fault_under_test */
            // flist_undetect.remove(fault_under_test);
          // }
        // }
        // in_vector_no++;
        break;
    case 0:
        fault_under_test->detect = REDUNDANT;
        no_of_redundant_faults++;
        break;
  
    case MAYBE:
        no_of_aborted_faults++;
        break;
    }
    fault_under_test->test_tried = true;
    fault_under_test = nullptr;
    for (fptr fptr_ele: flist_undetect) {
      if (!fptr_ele->test_tried) {
        fault_under_test = fptr_ele;
        break;
      }
    }
    total_no_of_backtracks += current_backtracks; // accumulate number of backtracks
    no_of_calls++;
  }
}


if(atpg==true){
  while(fault_under_test != nullptr) {
    unsigned old_size = vectors.size();
    // switch(podem(fault_under_test,current_backtracks)) {
    switch(dual_tdfpodem(fault_under_test,current_backtracks)) {
      case TRUE:
        for(unsigned i=old_size; i<vectors.size(); i++) {
          if(compression)   secondary_fault(vectors[i]);
          // cout << patterns[i] << endl;
          random_fill(vectors[i]);
          cout << "# tdf sim\n";
          tdf_sim_a_vector(vectors[i], current_detect_num, NULL, i);
          total_detect_num += current_detect_num;
          if(faultdrop) {
            cout << "# vector[" << i << "] detects " << current_detect_num << " faults ("
                << total_detect_num << ")" << endl;
          }
          else {
            cout << "# vector[" << i << "] detects " << current_detect_num << " faults" << endl;
          }
        }
        /* form a vector */
        // vec.clear();
        // for (wptr w: cktin) {
          // vec.push_back(itoc(w->value));
        // }
        /*by defect, we want only one pattern per fault */
        /*run a fault simulation, drop ALL detected faults */
        // if (total_attempt_num == 1) {
          // int dummy = 0;
          /* fault_sim_a_vector(vec, current_detect_num,NULL, dummy); */
          // tdf_sim_a_vector(vec, current_detect_num, NULL, dummy);
          // total_detect_num += current_detect_num;
        // }
        /* If we want mutiple petterns per fault, 
         * NO fault simulation.  drop ONLY the fault under test */ 
        // else {
          // fault_under_test->ndet++;
          // if(fault_under_test->ndet >= ndet) {
            // fault_under_test->detect = TRUE;
            /* drop fault_under_test */
            // flist_undetect.remove(fault_under_test);
          // }
        // }
        // in_vector_no++;
        break;
	  case FALSE:
        fault_under_test->detect = REDUNDANT;
        no_of_redundant_faults++;
        break;
  
	  case MAYBE:
        no_of_aborted_faults++;
        break;
    }
    fault_under_test->test_tried = true;
    fault_under_test = nullptr;
    for (fptr fptr_ele: flist_undetect) {
      if (!fptr_ele->test_tried) {
        fault_under_test = fptr_ele;
        break;
      }
    }
    total_no_of_backtracks += current_backtracks; // accumulate number of backtracks
    no_of_calls++;
  }
}
  in_vector_no = patterns.size();

  // display_undetect();
  fprintf(stdout,"# number of detected faults = %d\n", total_detect_num);
  fprintf(stdout,"#STATS\n");
  fprintf(stdout,"# number of aborted faults = %d\n",no_of_aborted_faults);
  // fprintf(stdout,"\n");
  fprintf(stdout,"# number of redundant faults = %d\n",no_of_redundant_faults);
  // fprintf(stdout,"\n");
  fprintf(stdout,"# number of calling podem1 = %d\n",no_of_calls);
  // fprintf(stdout,"\n");
  fprintf(stdout,"# total number of backtracks = %d\n",total_no_of_backtracks);
  // end of atpg
  if(!get_tdfsim_only())compute_fault_coverage(); //init_flist.cpp
  fprintf(stdout,"#STATS END\n");

  // compression
  if(compression)   
  {
      bool do_enhanced_RO = true;
      //bool do_enhanced_RO = false;
      if (do_enhanced_RO)
      {
         enhanced_RO_mem_aware();
         //enhanced_RO();
      }
      else
      {
          if (ndet == 1) RO();
          else
          {
              write_pattern_to_file();
          }
      }
      
  }
  in_vector_no = patterns.size();

  output_patterns();

}/* end of test */
