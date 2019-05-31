#include "atpg.h"

////////////////////////////////////
//Author:     Joe
//Date    :    2018/11/29 
//puepos: Generate the pattern from the input circuit 
///////////////////////////////////
#define CONFLICT 2


vector<int> string_pattern_to_int(string _s)
{
	vector<int> _fi;
    for(auto &x:_s){
  		switch (x) {
      		case '0': _fi.push_back(0); break;
      		case '1': _fi.push_back(1); break;
      		case 'x': _fi.push_back(2); break;
    	}
    }
    return _fi;
};

string ATPG::scan_ckt(void)
{
	string s="";
	for (int i = 0; i < cktin.size(); i++) {
    	switch (cktin[i]->value) {
      		case 0: s+="0"; break;
      		case 1: s+="1"; break;
      		case U: s+="x"; break;
      		case D: s+="1"; break;
      		case B: s+="0"; break;
    	}
  	}
  	return s;
};

string ATPG::scan_ckt_out(void)
{
	string s="";
	for (int i = 0; i < cktout.size(); i++) {
    	switch (cktout[i]->value) {
      		case 0: s+="0"; break;
      		case 1: s+="1"; break;
      		case U: s+="x"; break;
      		case D: s+="1"; break;
      		case B: s+="0"; break;
    	}
  	}
  	return s;
};
 
int ATPG::dual_tdfpodem(const fptr x, int& nbt)
{
	FAULT _F1=*x;
	// int nbt=0,nim=0;
	// _F1.fault_type ^= 1;

	vector<string> _regpats;
  cout <<"Dual: " << _F1.fault_type <<endl;
	switch(tdfpodem(&_F1,nbt,2,_regpats)){
    case TRUE:
      // cout << "Current fault:  ";
      // display_fault(&_F1);
      for(auto &x:_regpats)
      {
        cout << x << endl;
        patterns.push_back(x);
      }
      return(TRUE);
    case FALSE:
      return(FALSE);
    case(MAYBE):
      return(MAYBE);
	}
}

void ATPG::tdfatpg()
{
    // total_attempt_num = 8;
  	// backtrack_limit   = 500;
    set_backtrack_limit(500);
    int nbt = 0;

    for(auto &x:flist_undetect)
    {
        dual_tdfpodem(x, nbt);
    }
    cout << "number of patterns" <<patterns.size()<<endl;
//COAOY this part is random fill 
// if you don't want to use R fill command this.
///////////////////////////////////////////////////////////////
    // vector<string> _v2;
    // for(auto &x:patterns){
    // 	_v2.push_back(x);
    // 	_v2.push_back(x);
    // }
    // patterns=_v2;
    output_patterns();
}

void ATPG::random_fill(string& vec) {
   	for(auto &y: vec){

   		switch (y) 
   		{
         		case '0': y='0'; break;
         		case '1': y='1'; break;
         		case 'x': y=(rand()%2)?'0':'1'; break;
            // case 'x': y='1'; break;
         		case 'D': y='0'; break;
         		case 'B': y='1'; break;
         		case ' ': y=y  ; break;
   		}    
   	}
}

void ATPG::output_patterns() {

   for(auto &x:patterns){
   	  cout << "T'" << x<< "'" << endl;
   }
//////////////////////////////////////////////////////////////////
}

// int podem(fptr, int&);

int ATPG::tdfpodem(fptr fault, int& current_backtracks, int fram, vector<string>& regpat) {

  int i,ncktwire,ncktin;
  wptr wpi; // points to the PI currently being assigned
  forward_list<wptr> decision_tree; // design_tree (a LIFO stack)
  wptr wfault;
  int attempt_num = 0;  // counts the number of pattern generated so far for the given fault

  /* initialize all circuit wires to unknown */
  ncktwire = sort_wlist.size();
  ncktin = cktin.size();


  for (i = 0; i < ncktwire; i++) {
    sort_wlist[i]->value = U;
  }

  no_of_backtracks = 0;
  find_test = false;
  no_test = false;
  bool find_tdf_test = false;
  
  mark_propagate_tree(fault->node);

// cout << "hi4\n";
  /* Fig 7 starts here */
  /* set the initial goal, assign the first PI.  Fig 7.P1 */
  switch (set_uniquely_implied_value(fault)) {
    case TRUE: // if a  PI is assigned 
      sim();  // Fig 7.3
      wfault = fault_evaluate(fault);
      if (wfault != nullptr) forward_imply(wfault);// propagate fault effect
      if (check_test()) find_test = true; // if fault effect reaches PO, done. Fig 7.10
      break;
    case CONFLICT:
      no_test = true; // cannot achieve initial objective, no test
      break;
    case FALSE: 
      break;  //if no PI is reached, keep on backtracing. Fig 7.A 
  }

  /* loop in Fig 7.ABC 
   * quit the loop when either one of the three conditions is met: 
   * 1. number of backtracks is equal to or larger than limit
   * 2. no_test
   * 3. already find a test pattern AND no_of_patterns meets required total_attempt_num */
// cout << "hi5\n";
int count=0;
  while ((no_of_backtracks < backtrack_limit) && !no_test &&
    !(find_tdf_test&& find_test && (attempt_num == total_attempt_num))) {
    /* check if test possible.   Fig. 7.1 */
    if (wpi = test_possible(fault)) {
      wpi->flag |= CHANGED;
      /* insert a new PI into decision_tree */
      decision_tree.push_front(wpi);
    }
    else { // no test possible using this assignment, backtrack. 

      while (!decision_tree.empty() && (wpi == nullptr)) {
        /* if both 01 already tried, backtrack. Fig.7.7 */
        if (decision_tree.front()->flag & ALL_ASSIGNED) {
          decision_tree.front()->flag &= ~ALL_ASSIGNED;  // clear the ALL_ASSIGNED flag
          decision_tree.front()->value = U; // do not assign 0 or 1
          decision_tree.front()->flag |= CHANGED; // this PI has been changed
          /* remove this PI in decision tree.  see dashed nodes in Fig 6 */
          decision_tree.pop_front();
        }  
        /* else, flip last decision, flag ALL_ASSIGNED. Fig. 7.8 */
        else {
          decision_tree.front()->value = decision_tree.front()->value ^ 1; // flip last decision
          decision_tree.front()->flag |= CHANGED; // this PI has been changed
          decision_tree.front()->flag |= ALL_ASSIGNED;
          no_of_backtracks++;
          wpi = decision_tree.front(); 
        }
      } // while decision tree && ! wpi
      if (wpi == nullptr) no_test = true; //decision tree empty,  Fig 7.9
      // cout << "no end\n";
    } // no test possible
    // display_io();
/* this again loop is to generate multiple patterns for a single fault 
 * this part is NOT in the original PODEM paper  */
again:  if (wpi) {
      sim();
      if (wfault = fault_evaluate(fault)) forward_imply(wfault);
      if (check_test()) {
        find_test = true;
        /* if multiple patterns per fault, print out every test cube */
//////////////////////////////////////////////////////////////////////////////////
        string _ans=scan_ckt();
        FAULT _F2 = *fault;
        _F2.fault_type ^=1;
        if(MayIassignYou(&_F2,_ans)){
          find_tdf_test = true;
          regpat.push_back(_ans);
          attempt_num++;
        }
        /*
        else
        {
          decision_tree.front()->value = decision_tree.front()->value ^ 1;
              decision_tree.front()->flag |= CHANGED;
              decision_tree.front()->flag |= ALL_ASSIGNED;
              no_of_backtracks++;
              wpi = decision_tree.front();
        }
        */
//////////////////////////////////////////////////////////////////////////////
        // increase pattern count for this fault

            /* keep trying more PI assignments if we want multiple patterns per fault
             * this is not in the original PODEM paper*/
        if (total_attempt_num > attempt_num) {
          wpi = nullptr;
          while (!decision_tree.empty() && (wpi == nullptr)) {
            /* backtrack */
            if (decision_tree.front()->flag & ALL_ASSIGNED) {
              decision_tree.front()->flag &= ~ALL_ASSIGNED;
              decision_tree.front()->value = U;
              decision_tree.front()->flag |= CHANGED;
              decision_tree.pop_front();
            }
            /* flip last decision */
            else {
              decision_tree.front()->value = decision_tree.front()->value ^ 1;
              decision_tree.front()->flag |= CHANGED;
              decision_tree.front()->flag |= ALL_ASSIGNED;
              no_of_backtracks++;
              wpi = decision_tree.front();
            }
          }
          if (!wpi) no_test = true;
          goto again;  // if we want multiple patterns per fault
        } // if total_attempt_num > attempt_num
      }  // if check_test()
    } // again
  } // while (three conditions)

  /* clear everthing */
  for (wptr wptr_ele: decision_tree) {
    wptr_ele->flag &= ~ALL_ASSIGNED;
  }
  decision_tree.clear();
  
  current_backtracks = no_of_backtracks;
  unmark_propagate_tree(fault->node);

  if (find_tdf_test) {
  	// cout << fault->node->name << " gate fault have "<< attempt_num<<" patterns" <<endl;
    /* normally, we want one pattern per fault */
    // cout << "detected fault:  ";
    // display_fault(fault);
    return(TRUE);
  }
  else if (no_test) {
    /*fprintf(stdout,"redundant fault...\n");*/
    return(FALSE);
  }
  else {
    /*fprintf(stdout,"test aborted due to backtrack limit...\n");*/
    return(MAYBE);
  }
}/* end of podem */


bool ATPG::MayIassignYou(const fptr fault, string& _s)
{

  	int ncktwire = sort_wlist.size();
  	int ncktin = cktin.size();
  	bool test_success = false;

// first scan
  	string _s2=scan_ckt();
  //reset value
  vector<int> _ov;
  	for (int i = 0; i < ncktwire; i++) {
      _ov.push_back(sort_wlist[i]->value);
      sort_wlist[i]->value = U;
  	}

  //change pattern from string to int array
  	vector<int> _fi=string_pattern_to_int(_s);
  	
  // set cktin value
	for(int _i=0;_i<(ncktin-1);_i++){
  		cktin[_i]->value = _fi[_i+1];
  	}
    string _s2_1=scan_ckt();
  
	int pi_is_reach = set_uniquely_implied_value(fault);

  wptr ret;
	if(pi_is_reach == TRUE)
	{
		ret = find_pi_assignment(sort_wlist[fault->to_swlist], fault->fault_type) ;
	}	

// twice scan
	string _s1=scan_ckt();

//reset flag
  for (int i = 0; i < ncktwire; i++) {
   		sort_wlist[i]->value = _ov[i];
  }

	
// print out
	if(pi_is_reach == TRUE && _s1!=_s2 )
	{
    
		// cout <<"Fault name: " << fault->node->name <<endl;
  //   cout <<"Fault typed:" << fault->fault_type <<endl;
		// cout <<"Or:"<<_s<<endl;
  //   cout <<"v2_1:"  << _s2_1 << endl;
		// cout <<"v2:"  << _s2 << endl;
		// cout <<"v1: " <<_s1 <<" " << _s[0]<<endl;
    
		_s=_s1+" "+_s[0];
		return true;
	}

	return false;

}
