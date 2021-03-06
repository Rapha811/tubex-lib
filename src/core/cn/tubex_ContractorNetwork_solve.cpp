/** 
 *  ContractorNetwork class : solver
 * ----------------------------------------------------------------------------
 *  \date       2020
 *  \author     Simon Rohou
 *  \copyright  Copyright 2020 Simon Rohou
 *  \license    This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 */

#include "tubex_ContractorNetwork.h"

using namespace std;
using namespace ibex;

namespace tubex
{
  double ContractorNetwork::contract(bool verbose)
  {
    clock_t t_start = clock();

    if(verbose)
    {
      cout << "Contractor network has " << m_v_ctc.size()
           << " contractors and " << m_v_domains.size() << " domains" << endl;
      cout << "Computing, " << nb_ctc_in_stack() << " contractors in m_deque";
      if(!isinf(m_contraction_duration_max))
        cout << " during " << m_contraction_duration_max << "s";
      cout << endl;
    }

    while(!m_deque.empty()
      && (double)(clock() - t_start)/CLOCKS_PER_SEC < m_contraction_duration_max)
    {
      Contractor *ctc = m_deque.front();
      m_deque.pop_front();

      ctc->contract();
      ctc->set_active(false);

      for(auto& ctc_dom : ctc->domains()) // for each domain related to this contractor
      {
        // If the domain has "changed" after the contraction
        propag_active_ctc_from_dom(ctc_dom, ctc);
      }
    }

    if(verbose)
      cout << endl
           << "  computation time: " << (double)(clock() - t_start)/CLOCKS_PER_SEC << "s" << endl;

    // Emptiness test
    // todo: test only contracted domains?
    //for(const auto& ctc : m_v_ctc)
      for(const auto& dom : m_v_domains)
        if(dom->is_empty())
        {
          cout << "  warning: empty set" << endl;
          exit(1);
        }

    return (double)(clock() - t_start)/CLOCKS_PER_SEC;
  }

  double ContractorNetwork::contract_during(double dt, bool verbose)
  {
    double prev_dt = m_contraction_duration_max;
    m_contraction_duration_max = dt;
    double contraction_time = contract(verbose);
    m_contraction_duration_max = prev_dt;
    return contraction_time;
  }

  void ContractorNetwork::propag_active_ctc_from_dom(Domain *dom, Contractor *ctc_to_avoid)
  {
    double current_volume = dom->compute_volume(); // new volume after contraction

    if(current_volume/dom->get_saved_volume() < 1.-m_fixedpoint_ratio)
    {
      // We activate each contractor related to these domains, according to graph orientation

      // Local deque, for specific order related to this domain
      deque<Contractor*> ctc_deque;

      for(auto& ctc_of_dom : dom->contractors()) 
        if(ctc_of_dom != ctc_to_avoid && !ctc_of_dom->is_active())
        {
          ctc_of_dom->set_active(true);
          add_ctc_to_queue(ctc_of_dom, ctc_deque);
        }

      // Merging this local deque in the CN one
      for(auto& c : ctc_deque)
        m_deque.push_front(c);
    }
    
    dom->set_volume(current_volume); // updating old volume
  }
}