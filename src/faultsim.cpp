/**********************************************************************/
/*           Parallel-Fault Event-Driven Fault Simulator              */
/*                                                                    */
/*           Author: Bing-Chen (Benson) Wu                            */
/*           last update : 01/21/2018                                 */
/**********************************************************************/

#include "atpg.h"

/* pack 16 faults into one packet.  simulate 16 faults togeter. 
 * the following variable name is somewhat misleading */
#define num_of_pattern 16

/* The faulty_wire contains a list of wires that 
 * change their values in the fault simulation for a particular packet.
 * (that is wire_value1 != wire_value2) 
 * Note that the wire themselve are not necessarily a fault site.
 * The list is linked by the pnext pointers */
 
 /* fault simulate a set of test vectors */
void ATPG::fault_simulate_vectors(int& total_detect_num) {
  int i;
  int current_detect_num = 0;
 
  /* for every vector */
  for (i = vectors.size()-1; i >= 0; i--) {
    fault_sim_a_vector(vectors[i], current_detect_num, NULL, i);
    total_detect_num += current_detect_num;
    fprintf(stdout,"vector[%d] detects %d faults (%d)\n",i,current_detect_num,total_detect_num);
  }
}// fault_simulate_vectors

/* fault simulate a single test vector */
void ATPG::fault_sim_a_vector(const string& vec, int& num_of_current_detect, vector<int>** pattern_list, const int& pattern_index) {
  wptr w,faulty_wire;
  /* array of 16 fptrs, which points to the 16 faults in a simulation packet  */
  fptr simulated_fault_list[num_of_pattern];
  fptr f;
  int fault_type;
  int i,start_wire_index, nckt;
  int num_of_fault;

  forward_list<fptr>* fault_ls_ptr = &flist_stc;
  if (pattern_list == NULL) fault_ls_ptr = &flist_undetect;
  
  num_of_fault = 0; // counts the number of faults in a packet

  /* num_of_current_detect is used to keep track of the number of undetected
   * faults detected by this vector, initialize it to zero */
  num_of_current_detect = 0;

  /* Keep track of the minimum wire index of 16 faults in a packet.
   * the start_wire_index is used to keep track of the
   * first gate that needs to be evaluated.
   * This reduces unnecessary check of scheduled events.*/
  start_wire_index = 10000;

  // store and reset the detect flag
  // for the use of non-fault-dropping mode
  for(auto f: flist_undetect) {
    f->detected = f->detect;
    f->detect = FALSE;
  }
  
  /* for every input, set its value to the current vector value */
  for(i = 0; i < cktin.size(); i++) {
    cktin[i]->value = ctoi(vec[i]);
  }

  /* initialize the circuit - mark all inputs as changed and all other
   * nodes as unknown (2) */
  nckt = sort_wlist.size();
  for (i = 0; i < nckt; i++) {
    if (i < cktin.size()) {
      sort_wlist[i]->flag |= CHANGED;
    }
    else {
      sort_wlist[i]->value = U;
    }
  }

  sim(); /* do a fault-free simulation, see sim.c */
  if (0) { display_io(); }

  /* expand the fault-free 0,1,2 value into 32 bits (2 = unknown)  
   * and store it in wire_value1 (good value) and wire_value2 (faulty value)*/
  for (i = 0; i < nckt; i++) {
    switch (sort_wlist[i]->value) {
    case 1: sort_wlist[i]->wire_value1 = ALL_ONE;  // 11 represents logic one
            sort_wlist[i]->wire_value2 = ALL_ONE; break;
    case 2: sort_wlist[i]->wire_value1 = 0x55555555; // 01 represents unknown
            sort_wlist[i]->wire_value2 = 0x55555555; break;
    case 0: sort_wlist[i]->wire_value1 = ALL_ZERO; // 00 represents logic zero
            sort_wlist[i]->wire_value2 = ALL_ZERO; break;
    }
  } // for i

  /* walk through every undetected fault 
   * the undetected fault list is linked by pnext_undetect */
  for (auto pos = fault_ls_ptr-> cbegin(); pos != fault_ls_ptr-> cend(); ++pos) {
    f = *pos;
    if (f->detect == REDUNDANT) { continue;} /* ignore redundant faults */
    /* consider only active (aka. excited) fault
     * (sa1 with correct output of 0 or sa0 with correct output of 1) */
    if (f->activate == 1 && f->fault_type != sort_wlist[f->to_swlist]->value) { 
    //if (f->fault_type != sort_wlist[f->to_swlist]->value) {

	    /* if f is a primary output or is directly connected to an primary output
	     * the fault is detected */
      if ((f->node->type == OUTPUT) ||(f->io == GO && sort_wlist[f->to_swlist]->flag & OUTPUT)) {
        f->detect = TRUE;
        if (pattern_list != NULL)
            pattern_list[f->fault_no]->push_back(pattern_index);
      }
      else {
      
        /* if f is an gate output fault */
        if (f->io == GO) {
	      
			/* if this wire is not yet marked as faulty, mark the wire as faulty
			 * and insert the corresponding wire to the list of faulty wires. */
			if (!(sort_wlist[f->to_swlist]->flag & FAULTY)) {
			  sort_wlist[f->to_swlist]->flag |= FAULTY;
		    wlist_faulty.push_front(sort_wlist[f->to_swlist]);
			}
	      
		  /* add the fault to the simulated fault list and inject the fault */
		  simulated_fault_list[num_of_fault] = f;
		  inject_fault_value(sort_wlist[f->to_swlist], num_of_fault, f->fault_type); 
		    
			/* mark the wire as having a fault injected 
			 * and schedule the outputs of this gate */
		  sort_wlist[f->to_swlist]->flag |= FAULT_INJECTED;
		  for (auto pos_n = sort_wlist[f->to_swlist]->onode.cbegin(), end_n = sort_wlist[f->to_swlist]->onode.cend(); pos_n != end_n; ++pos_n) {
		    (*pos_n)->owire.front()->flag |= SCHEDULED;
		  }
		    
		  /* increment the number of simulated faults in this packet */
		  num_of_fault++;
		  /* start_wire_index keeps track of the smallest level of fault in this packet.
		   * this saves simulation time.  */
		  start_wire_index = min(start_wire_index, f->to_swlist);
		}  // if gate output fault
		    
		/* the fault is a gate input fault */
		else {
		    
			/* if the fault is propagated, set faulty_wire equal to the faulty wire.
			 * faulty_wire is the gate output of f.  */
		  faulty_wire = get_faulty_wire(f, fault_type);
			if (faulty_wire != nullptr) {
		      
		    /* if the faulty_wire is a primary output, it is detected */
		    if (faulty_wire->flag & OUTPUT) {
		      f->detect = TRUE;
              if (pattern_list != NULL)
                  pattern_list[f->fault_no]->push_back(pattern_index);
            }
            else {
		          /* if faulty_wire is not already marked as faulty, mark it as faulty
		           * and add the wire to the list of faulty wires. */
		          if (!(faulty_wire->flag & FAULTY)) {
		            faulty_wire->flag |= FAULTY;
                    wlist_faulty.push_front(faulty_wire);
		          }
		  
              /* add the fault to the simulated list and inject it */
              simulated_fault_list[num_of_fault] = f;
              inject_fault_value(faulty_wire, num_of_fault, fault_type);
		  
		          /* mark the faulty_wire as having a fault injected 
		           *  and schedule the outputs of this gate */
		          faulty_wire->flag |= FAULT_INJECTED;
              for (auto pos_n = faulty_wire->onode.cbegin(), end_n = faulty_wire->onode.cend(); pos_n != end_n; ++pos_n) {
                (*pos_n)->owire.front()->flag |= SCHEDULED;
              }
		   
              num_of_fault++;
              start_wire_index = min(start_wire_index, f->to_swlist);
              }
          }
        }
      } // if  gate input fault
    } // if fault is active


    /*
     * fault simulation of a packet 
     */
    
    /* if this packet is full (16 faults)
     * or there is no more undetected faults remaining (pos points to the final element of flist_undetect),
     * do the fault simulation */
    if ((num_of_fault == num_of_pattern) || (next(pos,1) == fault_ls_ptr-> cend())) {
	  
	    /* starting with start_wire_index, evaulate all scheduled wires
	     * start_wire_index helps to save time. */
	    for (i = start_wire_index; i < nckt; i++) {
	      if (sort_wlist[i]->flag & SCHEDULED) {
	      sort_wlist[i]->flag &= ~SCHEDULED;
	      fault_sim_evaluate(sort_wlist[i]);
	      }
	    } /* event evaluations end here */
	  
	   /* pop out all faulty wires from the wlist_faulty
		 * if PO's value is different from good PO's value, and it is not unknown
		 * then the fault is detected.
		 * 
		 * IMPORTANT! remember to reset the wires' faulty values back to fault-free values.
	   */
      while(!wlist_faulty.empty()) {
        w = wlist_faulty.front();
        wlist_faulty.pop_front();
	      //printf("before : %d\n", w->flag);
	      w->flag &= ~FAULTY;
	      w->flag &= ~FAULT_INJECTED;
	      w->fault_flag &= ALL_ZERO;
	      //printf("after  : %d\n", w->flag);
        /*TODO*/
        //Hint:Use mask to get the value of faulty wire and check every fault in packet
	      if (w->flag & OUTPUT) { // if primary output 
          for (i = 0; i < num_of_fault; i++) { // check every undetected fault
            if (!(simulated_fault_list[i]->detect)) {
              if ((w->wire_value2 & Mask[i]) ^    // if value1 != value2
                  (w->wire_value1 & Mask[i])) {
                if (((w->wire_value2 & Mask[i]) ^ Unknown[i])&&  // and not unknowns
                    ((w->wire_value1 & Mask[i]) ^ Unknown[i])){
                      simulated_fault_list[i]->detect = TRUE;  // then the fault is detected    
                      if (pattern_list != NULL)
                          pattern_list[simulated_fault_list[i]->fault_no]->push_back(pattern_index);
                }
              }
            }
          }
	      }
	      w->wire_value2 = w->wire_value1;  // reset to fault-free values
        /*TODO*/
	    } // pop out all faulty wires
    num_of_fault = 0;  // reset the counter of faults in a packet
    start_wire_index = 10000;  //reset this index to a very large value.
    } // end fault sim of a packet
  } // end loop. for f = flist

  /* fault dropping  */
  if(tdfsim_only && pattern_list == NULL) {
      flist_undetect.remove_if(
          [&](const fptr fptr_ele){
          if (fptr_ele->detect == TRUE) 
          {
              fptr_ele->ndet++;
              //cout << "fault # " <<  fptr_ele->fault_no << " " <<  fptr_ele->node->name << endl;
              if(fptr_ele->ndet >= ndet) {
                  num_of_current_detect += fptr_ele->eqv_fault_num;
                  return true;
              }
              else {
                  fptr_ele->detect = FALSE;
                  return false;
              }
          }
          else
          {
              return false;
          }
          });
  }
  else if(pattern_list == NULL)
  {
      fault_ls_ptr->remove_if(
          [&](const fptr fptr_ele){
          if (fptr_ele->detect == TRUE) 
          {
              fptr_ele->ndet++;
              num_of_current_detect += fptr_ele->eqv_fault_num;
              //cout << "fault # " <<  fptr_ele->fault_no << " " <<  fptr_ele->node->name << endl;
              if(fptr_ele->ndet >= ndet) {
                  return faultdrop;
              }
              else {
                  fptr_ele->detect = FALSE;
                  return false;
              }
          }
          else
          {
              return false;
          }
          });
  }
  else
  {
      //do something to pattern_list
      for (auto pos = fault_ls_ptr -> cbegin(); pos != fault_ls_ptr ->cend(); ++pos) 
      {
          (*pos )->detect = FALSE;
      }
  }

  // restore the detect flag
  for(auto f: flist_undetect) {
    f->detect = f->detected | f->detect;
  }

}/* end of fault_sim_a_vector */

/* evaluate wire w 
 * 1. update w->wire_value2 
 * 2. schedule new events if value2 != value1 */
void ATPG::fault_sim_evaluate(const wptr w) {
  unsigned int new_value;
  nptr n;
  int i, nin, nout;

  /*if (w->flag & INPUT) return;*/
  n = w->inode.front();
  nin = n->iwire.size();
  switch(n->type) {
     /*break a multiple-input gate into multiple two-input gates */
    case AND:
    case BUF:
    case NAND:
      new_value = ALL_ONE;
      for (i = 0; i < nin; i++) {
        new_value &= n->iwire[i]->wire_value2;
      }
      if (n->type == NAND) {
	    new_value = PINV(new_value);  // PINV is for three-valued inversion
      }
      break;
	  /*  */
    case OR:
    case NOR:
      new_value = ALL_ZERO;
      for (i = 0; i < nin; i++) {
        new_value |= n->iwire[i]->wire_value2;
      }
      if (n->type == NOR) {
        new_value = PINV(new_value);
      }
      break;

    case NOT:
      new_value = PINV(n->iwire.front()->wire_value2);
      break;

    case XOR:
      new_value = PEXOR(n->iwire[0]->wire_value2, n->iwire[1]->wire_value2);
      break;

    case EQV:
      new_value = PEQUIV(n->iwire[0]->wire_value2, n->iwire[1]->wire_value2);
      break;
  }

  /* if the new_value is different than the wire_value1 (the good value),
   * save it */
  if (w->wire_value1 != new_value) {

    /* if this wire is faulty, make sure the fault remains injected */
    if (w->flag & FAULT_INJECTED) {
      combine(w, new_value);
    }

    /* update wire_value2 */ 
    w->wire_value2 = new_value;

    /* insert wire w into the faulty_wire list */
    if (!(w->flag & FAULTY)) {
      w->flag |= FAULTY;
      wlist_faulty.push_front(w);
    }

    /* schedule new events */
    for (i = 0, nout = w->onode.size(); i < nout; i++) {
      if (w->onode[i]->type != OUTPUT) {
        w->onode[i]->owire.front()->flag |= SCHEDULED;
      }
    }
  } // if new_value is differnt
  // if new_value is the same as the good value, do not schedule any new event
}/* end of fault_sim_evaluate */

/* Given a gate-input fault f, check if f is propagated to the gate output.
   If so, returns the gate output wire as the faulty_wire.
   Also returns the gate output fault type. 
*/
ATPG::wptr ATPG::get_faulty_wire(const fptr f, int& fault_type) {
  int i, nin;
  bool is_faulty;

  is_faulty = true;
  nin = f->node->iwire.size();
  switch(f->node->type) {

       /* this case should not occur,
        * because we do not create fault in the NOT BUF gate input */
    case NOT:
      fault_type = f->fault_type? STUCK0:STUCK1;
    case BUF:
      //fprintf(stdout,"something is fishy(get_faulty_net)...\n");
      fault_type = f->fault_type? STUCK1:STUCK0;
      break;

	  /*check every gate input of AND 
	   if any input is zero or unknown, then fault f is not propagated */ 
    case AND:
      for (i = 0; i < nin; i++) {
        if (f->node->iwire[i] != sort_wlist[f->to_swlist]) {
          if (f->node->iwire[i]->value != 1) {
            is_faulty = false;  // not propagated
          }
        }
      }
	  /* AND gate input stuck-at one fault is propagated to 
	     AND gate output stuck-at one fault */
      //fault_type = STUCK1;  
      fault_type = f->fault_type? STUCK1:STUCK0;
      break;

    case NAND:
      for (i = 0; i < nin; i++) {
        if (f->node->iwire[i] != sort_wlist[f->to_swlist]) {
          if (f->node->iwire[i]->value != 1) {
            is_faulty = false;
          }
        }
      }
//      fault_type = STUCK0;
      fault_type = f->fault_type? STUCK0:STUCK1;
      break;
    case OR:
      for (i = 0; i < nin; i++) {
        if (f->node->iwire[i] != sort_wlist[f->to_swlist]) {
          if (f->node->iwire[i]->value != 0) {
            is_faulty = false;
          }
        }
      }
      //fault_type = STUCK0;
      fault_type = f->fault_type? STUCK1:STUCK0;
      break;
    case  NOR:
      for (i = 0; i < nin; i++) {
        if (f->node->iwire[i] != sort_wlist[f->to_swlist]) {
          if (f->node->iwire[i]->value != 0) {
            is_faulty = false;
          }
        }
      }
      //fault_type = STUCK1;
      fault_type = f->fault_type? STUCK0:STUCK1;
      break;
    case XOR:
      for (i = 0; i < nin; i++) {
        if (f->node->iwire[i] != sort_wlist[f->to_swlist]) {
          if (f->node->iwire[i]->value == 0) {
            fault_type = f->fault_type;
          }
          else {
            fault_type = f->fault_type ^ 1;
          }
        }
      }
      break;
    case EQV:
      for (i = 0; i < nin; i++) {
        if (f->node->iwire[i] != sort_wlist[f->to_swlist]) {
          if (f->node->iwire[i]->value == 0) {
            fault_type = f->fault_type ^ 1;
          }
          else {
            fault_type = f->fault_type;
          }
        }
      }
      break;
  }
  if (is_faulty) {
    return(f->node->owire.front());
  }
  return(nullptr);
}/* end of get_faulty_wire */

/* This function injects a fault 
 * 32 bits(16 faults) in a word, 
 * mask[bit position]=11 is the bit position of the fault
 */
void ATPG::inject_fault_value(const wptr faulty_wire, const int& bit_position, const int& fault_type) {
  /*TODO*/
  //Hint use mask to inject fault to the right position
  if (fault_type == STUCK1) faulty_wire->wire_value2 |= Mask[bit_position];// SA1 fault
  if (fault_type == STUCK0) faulty_wire->wire_value2 &= ~Mask[bit_position]; // SA0 fault
  faulty_wire->fault_flag |= Mask[bit_position];// bit position of the fault 
  /*TODO*/
}/* end of inject_fault_value */

/* For each fault in this packet, check
 * if wire w itself is the fault site, do not change its wire_value2.
 * (because the wire_value2 was already decided by the inject_fault_value fucntion) */
void ATPG::combine(const wptr w, unsigned int& new_value) {
  int i;
  
  for (i = 0; i < num_of_pattern; i++) {
    if (w->fault_flag & Mask[i]) {
      new_value &= ~Mask[i];
      new_value |= (w->wire_value2 & Mask[i]);
    }
  }
  return;
} /* end of combine */

/* for three-valued logic inversion
 * Swap the odd bits and the even bits,
 * then do a inversion.
 * The purpose of swaping is to keep the unknown u=01 unchanged after inversion */  
unsigned int ATPG::PINV(const unsigned int& value) {
    return((((value & 0x55555555) << 1) ^ 0xaaaaaaaa) | 
           (((value & 0xaaaaaaaa) >> 1) ^ 0x55555555));
}/* end of PINV */


unsigned int ATPG::PEXOR(const unsigned int& value1, const unsigned int& value2) {
    return((value1 & PINV(value2)) | (PINV(value1) & value2));
}/* end of PEXOR */


unsigned int ATPG::PEQUIV(const unsigned int& value1, const unsigned int& value2) {
    return((value1 | PINV(value2)) & (PINV(value1) | value2));
}/* end of PEQUIV */
