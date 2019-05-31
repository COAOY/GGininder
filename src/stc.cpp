/***********************************
* created by Tung-Wei, Lin
* purpose: implementation of various STC technques
**********************************/
#include "atpg.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <iomanip>
using namespace std;
void ATPG::RO()
{
    int old_fault_size = 0;
	for(auto &x:flist_stc)
    {
        old_fault_size+=x->eqv_fault_num;
        x->detect = FALSE;
    }
    cout << "#STATS\n";
    cout << "# original test set length: " << patterns.size() << endl;
    vector<string> T_compact;
    vectors = patterns; 
    assert(!vectors.empty());

    transition_delay_fault_simulation(T_compact);
    
    patterns = T_compact;
    
    write_pattern_to_file(); 
    cout << "# after RO, test set length: " << patterns.size() << endl;
    cout << "#STATS END\n";
    
    vectors.clear();
    assert(vectors.empty());


}
void ATPG::enhanced_RO_mem_aware()
{
    int fault_size = 0;
	for(auto &x:flist_stc)
    {
        fault_size+=x->eqv_fault_num;
        x->detect = FALSE;
    }
    cout << "#STATS\n";
    cout << "# original test set length: " << patterns.size() << endl;
    vector<int>* pattern_list[fault_size];//to record which and how many patterns can detect one fault
    vector<int>* tmp_pattern_list[fault_size];
    vector<int>* fault_list[patterns.size()];//to record which faults the pattern detects
    vector<string> T_compact;
    int faults_covered[patterns.size()];
    int detected_times[fault_size];
    //////////////////////////////////////////////////////////////////////////
    int ability[patterns.size()];
    int thresh_index[fault_size];
    ///////////////////////init pattern_list and vectors//////////////////////
    vectors = patterns; 
    for (int i = 0; i < fault_size; i++)
    {
        detected_times[i] = 0;
        thresh_index[i] = -1;
        vector<int>* tmp_vec = new vector<int>;
        pattern_list[i] = tmp_vec;
        
        vector<int>* tmp_tmp_vec = new vector<int>;
        tmp_pattern_list[i] = tmp_tmp_vec;

    }
    for (int i = 0; i < patterns.size(); i++)
    {
        ability[i] =0;
        faults_covered[i]=0;
        vector<int>* tmp_vec = new vector<int>;
        fault_list[i]=tmp_vec;
    }
    /////////////////////////////////////////////////////////////////////////
    assert(!vectors.empty());

    //transition_delay_fault_simulation(pattern_list);

    vector<string> tmp_patterns;
    vector<int> indices;
    tmp_patterns = patterns;
    while(!tmp_patterns.empty())
    {
        int sentinel = LIMIT < tmp_patterns.size() ? LIMIT : tmp_patterns.size();
        vectors.clear();
        indices.clear();
        for (int i = 0; i < sentinel; i++)
        {
            vectors.push_back(tmp_patterns.back());
            indices.push_back(tmp_patterns.size()-1);
            tmp_patterns.pop_back();
        }
        for (int i = 0; i < fault_size; i++)
            tmp_pattern_list[i]->clear();
        transition_delay_fault_simulation(tmp_pattern_list, indices);
        for (int i = 0; i < fault_size; i++)
        {
            for (int j = 0; j < tmp_pattern_list[i]->size(); j++)
            {
                ability[tmp_pattern_list[i]->at(j)] ++;
            }
        }
        for (int i = 0; i < fault_size; i++)
        {
            for (int j = 0; j < tmp_pattern_list[i]->size(); j++)
            {
                if(pattern_list[i]->size() < LIMIT)
                {
                    if (thresh_index[i] == -1) 
                    {
                        thresh_index[i] = pattern_list[i]->size();
                    }
                    else//replace the thresh if we have a lower thresh
                    {
                        if ( ability[pattern_list[i]->at(thresh_index[i])] > ability[tmp_pattern_list[i]->at(j)])
                        {
                            thresh_index[i] = pattern_list[i]->size();
                        }
                    }
                    pattern_list[i]->push_back(tmp_pattern_list[i]->at(j));
                }
                else
                {
                    if (ability[pattern_list[i]->at(thresh_index[i])] < ability[tmp_pattern_list[i]->at(j)])
                    {
                        pattern_list[i]->at(thresh_index[i]) = tmp_pattern_list[i]->at(j);
                        for (int k = 0; k < pattern_list[i]->size(); k++)
                        {
                            thresh_index[i] = ability[pattern_list[i]->at(thresh_index[i])] < ability[pattern_list[i]->at(k)] ? thresh_index[i] : k;
                        }
                    }
                }
            }
        }

    }

    //enable this to visualize
/*    for (int i = 0; i < fault_size; i++)
    {
        if (!pattern_list[i]->empty())
        {
            cout << "fault # "<<i<<": ";
            for (int j = 0; j < pattern_list[i]->size(); j++)
            {
                cout << pattern_list[i]->at(j) << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
*/
    for (int i = 0; i < fault_size; i++)
    {
        for (int j = 0; j < pattern_list[i]->size();j++)
            fault_list[pattern_list[i]->at(j)]->push_back(i);
    }
    if (ndet >1)
    {
        duplicate_patterns(T_compact, pattern_list, fault_list, fault_size, detected_times);
        for (int i = 0; i < fault_size; i ++)
        {
            if (detected_times[i]>=ndet)
            {
                pattern_list[i]->clear();
            }
            else 
            {
                for (int j = 0; j < pattern_list[i]->size();j++)
                {
                    if (faults_covered[pattern_list[i]->at(j)]==0)
                    faults_covered[pattern_list[i]->at(j)] = fault_list[pattern_list[i]->at(j)]->size();
                }
            }
        }
    }
    else
    {
        vector<int> essential_tests;
        for (int i = 0; i < fault_size; i++)
        {
            if(pattern_list[i]->size() == ndet)
            {
                if (detected_times[i]<ndet)
                {
                    for (int j = 0; j < pattern_list[i]->size();j++)
                    {
                        int pattern_index = pattern_list[i]->at(j);
                        T_compact.push_back(patterns.at(pattern_index));
                        for (int x = 0; x < fault_list[pattern_index]->size(); x ++ )
                        {
                            detected_times[fault_list[pattern_index]->at(x)]++;
                        }
                        essential_tests.push_back(pattern_index);
                    }
                }
                pattern_list[i]->clear();//is this line necessary?
            }
            else if (pattern_list[i]->size() > ndet)
            {
                for (int j = 0; j < pattern_list[i]->size();j++)
                {
                    faults_covered[pattern_list[i]->at(j)]++;
                }
            }
        }
        for (int i = 0; i < essential_tests.size(); i++)
            faults_covered[essential_tests.at(i)] = 0;

    }
    bool someone_is_not_detected = false;
    do 
    {
        someone_is_not_detected = false;
        for (int i = fault_size-1; i >= 0 ; i--)
        {
            if (!pattern_list[i]->empty())
            {
                someone_is_not_detected = true;
                int max_covering_index = 0;
                for(int j = 0; j < pattern_list[i]->size(); j++)
                {
                    if (faults_covered[pattern_list[i]->at(max_covering_index)]<faults_covered[pattern_list[i]->at(j)])
                        max_covering_index = j;
                }
                if (detected_times[i] < ndet)//still not detected, must chooose one test vector 
                {
                    faults_covered[pattern_list[i]->at( max_covering_index )] = 0;
                    
                    T_compact.push_back(patterns.at(pattern_list[i]->at(max_covering_index)));
                    for (int x = 0; x <fault_list[pattern_list[i]->at(max_covering_index)]->size();x++ )
                    {
                        detected_times[fault_list[pattern_list[i]->at(max_covering_index)]->at(x)] ++;
                    }
                }
                else
                {
                    for (int j = 0; j < pattern_list[i]->size(); j++)
                    {
                        faults_covered[pattern_list[i]->at(j)] --;
                    }
                    pattern_list[i]->clear();
                }
            }


        }
    }while(someone_is_not_detected);
    for (int i = 0; i < fault_size; i++)
    {
        delete pattern_list[i];
        delete tmp_pattern_list[i];
    }
    for (int i = 0; i < patterns.size(); i++)
    {
        delete fault_list[i];
    }
    patterns = T_compact;
    
    write_pattern_to_file(); 
    ////////////////////////////////////////////////////////////////////////////
    cout << "# after enhanced_RO, test set length: " << patterns.size() << endl;
    cout << "#STATS END\n";
    ////////////////////////////////////////////////////////////////////////////
    vectors.clear();
    assert(vectors.empty());

}
void ATPG::enhanced_RO()
{
    int fault_size = 0;
	for(auto &x:flist_stc)
    {
        fault_size+=x->eqv_fault_num;
        x->detect = FALSE;
    }
    cout << "#STATS\n";
    cout << "# original test set length: " << patterns.size() << endl;
    vector<int>* pattern_list[fault_size];//to record which and how many patterns can detect one fault
    vector<int>* fault_list[patterns.size()];//to record which faults the pattern detects
    vector<string> T_compact;
    int faults_covered[patterns.size()];
    int detected_times[fault_size];
    ///////////////////////init pattern_list and vectors//////////////////////
    vectors = patterns; 
    for (int i = 0; i < fault_size; i++)
    {
        detected_times[i] = 0;
        vector<int>* tmp_vec = new vector<int>;
        pattern_list[i] = tmp_vec;
    }
    for (int i = 0; i < patterns.size(); i++)
    {
        faults_covered[i]=0;
        vector<int>* tmp_vec = new vector<int>;
        fault_list[i]=tmp_vec;
    }
    /////////////////////////////////////////////////////////////////////////
    assert(!vectors.empty());

    transition_delay_fault_simulation(pattern_list);

    //enable this to visualize
/*    for (int i = 0; i < fault_size; i++)
    {
        //if (!pattern_list[i]->empty())
        //{
            cout << "fault # "<<i<<": ";
            for (int j = 0; j < pattern_list[i]->size(); j++)
            {
                cout << pattern_list[i]->at(j) << " ";
            }
            cout << endl;
        //}
        //cout << endl;
    }*/
    for (int i = 0; i < fault_size; i++)
    {
        for (int j = 0; j < pattern_list[i]->size();j++)
            fault_list[pattern_list[i]->at(j)]->push_back(i);
    }
    if (ndet >1)
    {
        duplicate_patterns(T_compact, pattern_list, fault_list, fault_size, detected_times);
        for (int i = 0; i < fault_size; i ++)
        {
            if (detected_times[i]>=ndet)
            {
                pattern_list[i]->clear();
            }
            else 
            {
                for (int j = 0; j < pattern_list[i]->size();j++)
                {
                    if (faults_covered[pattern_list[i]->at(j)]==0)
                    faults_covered[pattern_list[i]->at(j)] = fault_list[pattern_list[i]->at(j)]->size();
                }
            }
        }
    }
    else
    {
        vector<int> essential_tests;
        for (int i = 0; i < fault_size; i++)
        {
            if(pattern_list[i]->size() == ndet)
            {
                if (detected_times[i]<ndet)
                {
                    for (int j = 0; j < pattern_list[i]->size();j++)
                    {
                        int pattern_index = pattern_list[i]->at(j);
                        T_compact.push_back(patterns.at(pattern_index));
                        for (int x = 0; x < fault_list[pattern_index]->size(); x ++ )
                        {
                            detected_times[fault_list[pattern_index]->at(x)]++;
                        }
                        essential_tests.push_back(pattern_index);
                    }
                }
                pattern_list[i]->clear();//is this line necessary?
            }
            else if (pattern_list[i]->size() > ndet)
            {
                for (int j = 0; j < pattern_list[i]->size();j++)
                {
                    faults_covered[pattern_list[i]->at(j)]++;
                }
            }
        }
        for (int i = 0; i < essential_tests.size(); i++)
            faults_covered[essential_tests.at(i)] = 0;

    }
    bool someone_is_not_detected = false;
    do 
    {
        someone_is_not_detected = false;
        for (int i = fault_size-1; i >= 0 ; i--)
        {
            if (!pattern_list[i]->empty())
            {
                someone_is_not_detected = true;
                int max_covering_index = 0;
                for(int j = 0; j < pattern_list[i]->size(); j++)
                {
                    if (faults_covered[pattern_list[i]->at(max_covering_index)]<faults_covered[pattern_list[i]->at(j)])
                        max_covering_index = j;
                }
                if (detected_times[i] < ndet)//still not detected, must chooose one test vector 
                {
                    faults_covered[pattern_list[i]->at( max_covering_index )] = 0;
                    
                    T_compact.push_back(patterns.at(pattern_list[i]->at(max_covering_index)));
                    for (int x = 0; x <fault_list[pattern_list[i]->at(max_covering_index)]->size();x++ )
                    {
                        detected_times[fault_list[pattern_list[i]->at(max_covering_index)]->at(x)] ++;
                    }
                }
                else
                {
                    for (int j = 0; j < pattern_list[i]->size(); j++)
                    {
                        faults_covered[pattern_list[i]->at(j)] --;
                    }
                    pattern_list[i]->clear();
                }
            }


        }
    }while(someone_is_not_detected);
    for (int i = 0; i < fault_size; i++)
    {
        delete pattern_list[i];
    }
    for (int i = 0; i < patterns.size(); i++)
    {
        delete fault_list[i];
    }
    patterns = T_compact;
    
    write_pattern_to_file(); 
    ////////////////////////////////////////////////////////////////////////////
    cout << "# after enhanced_RO, test set length: " << patterns.size() << endl;
    cout << "#STATS END\n";
    ////////////////////////////////////////////////////////////////////////////
    vectors.clear();
    assert(vectors.empty());

}
void ATPG::write_pattern_to_file()
{
    fstream file;
    file.open("patterns", ios::out|ios::trunc);
    for (int i = 0; i < patterns.size(); i++)
    {
        file << "T\'" <<patterns.at(i) <<"\'" <<endl;
    }
    file.close(); 
}
void ATPG::duplicate_patterns(vector<string>& T_compact,vector<int>**pattern_list, vector<int>**fault_list,const int& fault_size, int* detected_times)
{
    vector<int> T_compact_pattern_index;
    vector<vector<int> > under_ndet_faults;
    int max_list[fault_size];
    int orig_length[fault_size];
    for (int i = 0; i < ndet; i++)
    {
        vector<int> tmp;
        under_ndet_faults.push_back(tmp);
    }
    for (int i = 0; i < fault_size; i++)
    {
        if (pattern_list[i]->size() < ndet)
        {
            under_ndet_faults.at(pattern_list[i]->size()).push_back(i);
        }
        orig_length[i] = pattern_list[i]->size();
    }
    for (int i = 0; i < fault_size; i ++)
    {
        if (pattern_list[i]->empty())continue;
        int max_index = 0;
        for (int j = 1; j < pattern_list[i]->size(); j++)
        {
            if (fault_list[pattern_list[i]->at(max_index)]->size() <fault_list[pattern_list[i]->at(j)]->size() )
                max_index = j;
        }
        max_list[i] = pattern_list[i]->at(max_index);
    }
    for (int i = 1; i < ndet; i++)
    {
        int j = 0;
        while (!under_ndet_faults.at(i).empty())
        {
            int fault_index =under_ndet_faults.at(i).at(j);
            assert (pattern_list[fault_index]->size() == i );
            int counter = 0;
            int pattern_index = max_list[fault_index];//use random index temporarily
            do 
            {
                counter ++; 
                T_compact.push_back(patterns.at(pattern_index));
                T_compact_pattern_index.push_back(pattern_index);
                for (int h = 0; h<fault_list[pattern_index]->size(); h++)
                {
                    pattern_list[fault_list[pattern_index]->at(h)]->push_back(pattern_index);
                }
            }while(pattern_list[fault_index]->size() < ndet);
            for (int h = 0; h< fault_list[pattern_index]->size();h++ )
            {
                int orig_under_ndet_index = pattern_list[fault_list[pattern_index]->at(h)]->size() - counter;
                if (orig_under_ndet_index >= ndet) continue; //this fault takes care of itself
                int new_under_ndet_index = pattern_list[fault_list[pattern_index]->at(h)]->size();
                int target_fault_index = fault_list[pattern_index]->at(h);
                vector<int>::iterator it = find(under_ndet_faults.at(orig_under_ndet_index).begin(), under_ndet_faults.at(orig_under_ndet_index).end(), target_fault_index);
                assert (it!=under_ndet_faults.at(orig_under_ndet_index).end()) ;
                under_ndet_faults.at(orig_under_ndet_index).erase(it);
                if (new_under_ndet_index < ndet)
                    under_ndet_faults.at(new_under_ndet_index).push_back(*it);
                

            }
            T_compact.push_back(patterns.at(pattern_index));
            T_compact_pattern_index.push_back(pattern_index);
            for (int h = 0; h < pattern_list[fault_index]->size();h++)
            {
                if (pattern_list[fault_index]->at(h)!=pattern_index)
                {
                    T_compact.push_back(patterns.at(pattern_list[fault_index]->at(h)));
                    T_compact_pattern_index.push_back(pattern_list[fault_index]->at(h));
                }
            }
        }
    }

    for (int i = 0; i < T_compact_pattern_index.size(); i++)
    {
        for (int j =0; j < fault_list[T_compact_pattern_index.at(i)]->size(); j++)
            detected_times[fault_list[T_compact_pattern_index.at(i)]->at(j)] ++;
    } 
    
   cout << "# duplicated tests: " << T_compact.size() << endl; 
 
    for (int i = 0; i < fault_size; i++)
    {
        pattern_list[i]->erase(pattern_list[i]->begin()+orig_length[i], pattern_list[i]->end());
    }
    
}

//first duplicate the faults that has only one detecting test
//then update and see if others are detected more times already
//then see the faults that has only two detecting tests
