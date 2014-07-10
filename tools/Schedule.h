#ifndef EMK_SCHEDULE_H
#define EMK_SCHEDULE_H

// 
// This class is the base object to handle event scheduling based on priorities.
// All other schedulers are derived from this class.  This is an abstract base class.
// 

namespace emk {
  class Schedule {
  protected:
    int item_count;
        
    Schedule(); // @not_implemented
    Schedule(const Schedule&); // @not_implemented
    Schedule& operator=(const Schedule&); // @not_implemented
    
  public:
    Schedule(int _item_count) : item_count(_item_count) { ; }
    virtual ~Schedule() { ; }
    
    virtual bool OK() { return true; }
    virtual void Adjust(int item_id, double priority) = 0;
    virtual int NextID() = 0;
    virtual double GetStatus(int id) { return 0.0; }
    
    void SetSize(int _item_count) { item_count = _item_count; }
  };
}  

#endif
