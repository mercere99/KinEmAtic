#ifndef EMK_EVENTS_H
#define EMK_EVENTS_H

#include "../Kinetic.h"

namespace emk {

  class EventChain;
  
  class Event {
  protected:
    Event * next;       // which event should go next?
    bool next_simul;    // shouild the next event be simultaneous with this one (is possible)?
    
  public:
    Event() : next(NULL), next_simul(false) { ; }
    virtual ~Event() { if (next) delete next; }

    Event * GetNext() const { return next; }
    bool GetNextSimul() const { return next_simul; }

    virtual void Trigger(EventChain * chain) = 0;

    Event * Then(Event * _next) { next = _next; return next; }
    Event * With(Event * _next) { next = _next; next_simul=true; return next; }
  };

  class Event_Tween : public Event { 
  private:
    Tween & tween;

  public:
    Event_Tween(Tween & _tween) : tween(_tween) { ; }
    ~Event_Tween() { ; }

    void Trigger(EventChain * chain) {
      tween.Play();
      // If there is something to be run at the same time, make sure to do so!
      if (next && next_simul) next->Trigger(chain);
      else {
        // Otherwise, setup a callback so the chain knows when to take the next step.
        tween.SetFinishedCallback((Callback *) chain, (int *) next);
      }
    }
  };

  class EventChain : public emk::Callback {
  private:
    Event * first;
    Event * last;
    int length;
    bool is_running;

    Event * BuildEvent(Tween & tween) { return new Event_Tween(tween); }

  public:
    EventChain() : first(NULL), last(NULL), length(0), is_running(false) { ; }
    ~EventChain() { Clear(); }

    int GetSize() { return length; }
    bool IsRunning() const { return is_running; }

    void Clear() { if (first) delete first; }

    void DoCallback(int * next) {
      // The previous event must have fiinshed.  Move on to the next one!
      if (next) ((Event *) next)->Trigger(this);
      else is_running = false;
    }

    template <class T> EventChain & First(T & _in) {
      assert(first == NULL && last == NULL);
      assert(is_running == false);
      first = last = BuildEvent(_in);
      length = 1;
      return *this;
    }

    template <class T> EventChain & Then(T & _in) {
      assert(first == NULL && last == NULL);
      assert(is_running == false);
      last = last->Then( BuildEvent(_in) );
      length++;
      return *this;
    }

    template <class T> EventChain & With(T & _in) {
      assert(first == NULL && last == NULL);
      assert(is_running == false);
      last = last->With( BuildEvent(_in) );
      length++;
      return *this;
    }

    void Trigger() {
      assert(is_running == false);
      is_running = true;
      if (first) first->Trigger(this);
    }
  };

};

#endif
