#ifndef EMK_PROB_SCHEDULE_H
#define EMK_PROB_SCHEDULE_H

#include <vector>

#include "Random.h"
#include "Schedule.h"

//
// The probability for an item to be scheduled is perfectly proportional to its priority.
//

namespace emk {

  class ProbSchedule : public Schedule
  {
  private:
    Random m_rng;
    
    ProbSchedule(const ProbSchedule&); // @not_implemented
    ProbSchedule& operator=(const ProbSchedule&); // @not_implemented
    
    struct PSEntry {
      int id;
      double weight;
      double tree_weight;
    };
    
    std::vector<PSEntry> weight_set;
    
    int CalcID(double rand_pos, int cur_id) {
      const double cur_weight = weight_set[cur_id].weight;
      if (rand_pos < cur_weight) return cur_id;
      rand_pos -= cur_weight;
      const int left_id = cur_id*2 + 1;
      const double left_weight = weight_set[left_id].tree_weight;
      return (rand_pos < left_weight) ? CalcID(left_id, rand_pos) : CalcID(left_id+1, rand_pos-left_weight);
    }
    
  public:
    ProbSchedule(int _item_count, int seed=-1) : Schedule(_item_count), m_rng(seed), weight_set(_item_count+1) {
      for (int i = 0; i < item_count; i++) {
        weight_set[i].id = i;
        weight_set[i].weight = 0.0;
        weight_set[i].tree_weight = 0.0;
      }
    }
    ~ProbSchedule() { ; }
    
    void Adjust(int id, const double in_weight) {
      const double old_weight = weight_set[id].weight;
      weight_set[id].weight = in_weight;
      const int subtree1 = 2*id + 1;
      const int subtree2 = 2*id + 2;
      const int st1_weight = (subtree1 < item_count) ? weight_set[subtree1].tree_weight : 0.0;
      const int st2_weight = (subtree2 < item_count) ? weight_set[subtree2].tree_weight : 0.0;
      weight_set[id].tree_weight = in_weight + st1_weight + st2_weight;
      
      // Cascade the change up the tree.
      while (id > 0) {
        id = (id-1) / 2;
        weight_set[id].tree_weight = weight_set[id].weight
          + weight_set[id*2+1].tree_weight + weight_set[id*2+2].tree_weight;
      }
    }
    
    int NextID() {
      const int total_weight = weight_set[0].tree_weight;
      
      // Make sure it's possible to schedule...
      if (total_weight == 0.0) return -1;
      
      // If so, choose a random number to use for the scheduling.
      double rand_pos = m_rng.GetDouble(total_weight);
      return CalcID(0, rand_pos);
    }
  };
};

#endif
