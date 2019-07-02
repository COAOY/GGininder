#include "atpg.h"
using namespace std;
#define U  2
#define D  3
#define B  4
void ATPG::secondary_fault(string& vec) {
   forward_list<wptr> path;       // path_tree (a LIFO stack)
   int inserted_num = 0;
   string ori = vec;
   char tail = vec[cktin.size()-1];

   // apply the pattern for primary fault
   // V1
   for(unsigned i=0; i<cktin.size(); i++) {
      cktin[i]->value = ctoi(vec[i]);
      cktin[i]->flag |= CHANGED;
   }
   // cout << "V1:  ";
   // cout << "in: " << scan_ckt();
   sim();
   // cout << "    out: " << scan_ckt_out() << endl;
   for(wptr wire: sort_wlist) {
      wire->wire_value1 = wire->value;    // use value1 to temperarily store the value for V1
   }
   // V2
   cktin[0]->value = ctoi(vec.back());
   for(unsigned i=1; i<cktin.size(); i++) {
      cktin[i]->value = ctoi(vec[i-1]);
      cktin[i]->flag |= CHANGED;
   }
   // cout << "V2:  ";
   // cout << "in: " << scan_ckt();
   sim();
   // cout << "    out: " << scan_ckt_out() << endl;
    
   // clear the tried flag and set new triversal flag number
   for(fptr f_ele: flist_undetect)
      f_ele->second_tried = false;
   triversal_flag++;

   fptr f = nullptr;
   // cout << scan_ckt_out() << endl;

   for(wptr po: cktout) {
      if(po->value == U) {      // find an unknown output
         no_of_backtracks = 0;
         find_test = false;
         no_test = false;

         path.clear();
         wptr w;
         path.push_front(po);
         // cout << "Tracing back from " << po->name << endl;

         while(!find_test && !no_test && no_of_backtracks < backtrack_limit) {
            while(f == nullptr && !path.empty()) {
               w = path.front();
               f = select_fault(path, w);
               // cout <<"hi1!" << endl;
               // cout <<"hi2!" << endl;
            }
            // cout << endl;
            if(f != nullptr) {
               // cout << "trying fault on " << f->node->name << endl;
               switch(second_podem(f, no_of_backtracks)) {
                  case TRUE:
                     inserted_num++;
                     break;
                  case FALSE:
                     no_of_redundant_faults++;
                     break;
                  case MAYBE:
                     break;
               }
               w = path.front();
               path.pop_front();
            }
            else break;
         } 
         total_no_of_backtracks += no_of_backtracks;
      }  // end of this PO
   }

   if(inserted_num > 0) {
      string new_vec = scan_ckt();
      vec = new_vec.substr(1, cktin.size()-1) + tail + ' ' + new_vec[0];
   }
   cout << "# (" << inserted_num << " of faults inserted)   ";
   // cout << "original: " << ori << "   new: " << vec << endl;
   cout << endl;
}

// Trace back from an unknown output and return a fault
// Using DFS
ATPG::fptr ATPG::select_fault(forward_list<wptr>& path, wptr w) {
   fptr f = nullptr;
   // cout << "  -> " << w->name << endl;

   if(w->flag & INPUT) {
      // cout << "   is a input" << endl;
      for(fptr f_ele: w->faults) {
         if(f_ele->detect == FALSE && !f_ele->second_tried)
            if(pi_activated(f_ele, w))  return f_ele;
      }
      // cout <<"    no fault to activate" << endl;
   }
   else {
      nptr n = w->inode.front();
      // cout << "      checking input wires..." << endl;
      for(wptr wi: n->iwire) {
         if(wi->value == U && wi->triv != triversal_flag) {
            // cout << "      move to the next level" << endl;
            path.push_front(wi);
            f = select_fault(path, wi);   // recursively trace back
            if(f != nullptr)  break;
         }
      }
      if(f != nullptr)  return f;
      // cout << "      no available input wire." << endl;

      // try the faults on this wire
      // cout << "      check faults on this wire" << endl;
      for(fptr f_ele: w->faults) {
         if(is_activated(f_ele) != FALSE)
            if(f_ele->detect == FALSE && !f_ele->second_tried)
               return f_ele;
      }
   }
   
   // cout << "      nothing available on this wire. Leave." << endl << endl;
   w->triv = triversal_flag;
   path.pop_front();
   return nullptr;
}

short ATPG::is_activated(fptr f) {
   wptr w = sort_wlist[f->to_swlist];
   if(f->fault_type == STR) {
      if(w->wire_value1 == 0) {
         if(w->value == 1)       return TRUE;
         else if(w->value == U)  return MAYBE;
         else                    return FALSE;
      }
      else return FALSE;
   }
   else {
      if(w->wire_value1 == 1) {
         if(w->value == 0)       return TRUE;
         else if(w->value == U)  return MAYBE;
         else                    return FALSE;
      }
      else return FALSE;
   }
}

bool ATPG::pi_activated(fptr f, wptr w) {
   bool activate = false;
   int act_value;
   if(f->fault_type == STR) {
      if(w->wire_value1 == 1)    return false;
      else {
         if(w->value == 0)       return false;
         else if(w->value == U) {
            w->value = 1;
            forward_imply(w);
         }

         if(w->wire_value1 == U) {
            activate = true;
            act_value = 0;
         }
      }
   }
   else {
      if(w->wire_value1 == 0)    return false;
      else {
         if(w->value == 1)       return false;
         else if(w->value == U) {
            w->value = 0;
            forward_imply(w);
         }

         if(w->wire_value1 == U) {
            activate = true;
            act_value = 1;
         }
      }
   }

   if(activate) {
      if(w == cktin.back())  return false;  // not yet support assigning last input bit
      for(unsigned i=0; i<cktin.size(); i++) {
         if(cktin[i] == w) {
            cktin[i+1]->value = act_value;
            break;
         }
      }
      w->wire_value1 = act_value;
   }
   return f;
}

int ATPG::second_podem(const fptr fault, int& current_backtracks) {
  int i,ncktwire,ncktin;
  wptr wpi; // points to the PI currently being assigned
  forward_list<wptr> decision_tree; // design_tree (a LIFO stack)
  wptr wfault;
  bool attempt = false;

  find_test = false;
  no_test = false;
  
  mark_propagate_tree(fault->node);

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
  while ((no_of_backtracks < backtrack_limit) && !no_test &&
    !(find_test && (attempt))) {
    
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
    } // no test possible

/* this again loop is to generate multiple patterns for a single fault 
 * this part is NOT in the original PODEM paper  */
    if (wpi) {
      sim();
      if (wfault = fault_evaluate(fault)) forward_imply(wfault);
      if (check_test()) {
        find_test = true;
        attempt = true; 
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
  
  fault->second_tried = true;
  if (find_test) {
    return(TRUE);
  }
  else if (no_test) {
    /*fprintf(stdout,"redundant fault...\n");*/
    fault->detect = REDUNDANT;
    fault->test_tried = true;
    return(FALSE);
  }
  else {
    /*fprintf(stdout,"test aborted due to backtrack limit...\n");*/
    return(MAYBE);
  }
}/* end of podem */