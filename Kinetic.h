#ifndef EM_KINETIC_H
#define EM_KINETIC_H

#include <emscripten.h>

#include <assert.h>
#include <string>
#include <vector>


extern "C" {
  extern void EMK_Alert(const char * in_msg);
  extern void EMK_Setup_OnEvent(int obj_id, const char * trigger, int callback_id);

  extern int EMK_Stage_Build(int in_w, int in_h, const char * in_name);
  extern int EMK_Stage_AddLayer(int stage_obj_id, int layer_obj_id);

  extern int EMK_Layer_Build(int stage_id);
  extern int EMK_Layer_AddObject(int layer_obj_id, int add_obj_id);
  extern void EMK_Layer_BatchDraw(int layer_obj_id);

  extern int EMK_Text_Build(int in_x, int in_y, const char * in_text, const char * in_font_size, const char * in_font_family, const char * in_fill);
  extern int EMK_Rect_Build(int in_x, int in_y, int in_w, int in_h, const char * in_fill, const char * in_stroke, int in_stroke_width, int in_draggable);
  extern int EMK_RegularPolygon_Build(int in_x, int in_y, int in_sides, int in_radius,
                                      const char * in_fill, const char * in_stroke, int in_stroke_width, int in_draggable);

  extern int EMK_Animation_Build(int callback_id, int layer_id);
  extern int EMK_Animation_Start(int obj_id);
  extern int EMK_AnimationFrame_GetTime();
  extern int EMK_AnimationFrame_GetLastTime();
  extern int EMK_AnimationFrame_GetTimeDiff();
  extern int EMK_AnimationFrame_GetFrameRate();

  extern void EMK_Shape_SetScale(int obj_id, double x_scale, double y_scale);
  extern void EMK_Shape_SetOffset(int obj_id, int x_offset, int y_offset);
  extern void EMK_Shape_DoRotate(int obj_id, double rot);
}

// Pre-declarations of some classes...
class emkLayer;
class emkStage;


// All emscripten-wrapped Kinetic objects should be derived from this base class.
class emkObject {
protected:
  int obj_id;

  emkObject() : obj_id(-1) {;}  // Protected so that you can't make a direct emkObject.
public:
  int GetID() { return obj_id; }

  template<class T> void On(std::string in_trigger, T * in_target, void (T::*in_method_ptr)());
};


// Mediator for handling callbacks from JS.
class emkJSCallback : public emkObject {  // @CAO Should this really be an emkObject??
public:
  emkJSCallback() { ; }
  virtual ~emkJSCallback() { ; }

  virtual void DoCallback() = 0;
};


// Global info for this Kinetic Wrapper
// @CAO This is currently ugly callback management (too many steps...)
class emkInfo {
private:
  std::vector<emkJSCallback *> callback_info;  // Keep track of all of the callbacks that we are waiting for.

  emkInfo() { ; }                    // Private to prevent multiple instances.
  emkInfo(const emkInfo &);          // Don't allow copies to be made.
  void operator=(const emkInfo &);   // Don't allow.

public:
  static emkInfo & Info() { static emkInfo info; return info; }  // Singleton

  int RegisterCallback(emkJSCallback * _cb) {
    int cb_id = (int) callback_info.size();
    callback_info.push_back(_cb);
    return cb_id;
  }

  void DoCallback(int cb_id) {
    callback_info[cb_id]->DoCallback();
  }
};


template <class T> class emkSimpleCallback : public emkJSCallback {
private:
  T * target;
  void (T::*method_ptr)();
public:
  emkSimpleCallback(T * in_target, void (T::*in_method_ptr)())
    : target(in_target)
    , method_ptr(in_method_ptr)
  { ; }

  ~emkSimpleCallback() { ; }

  void DoCallback() { (target->*(method_ptr))(); }
};


template<class T> void emkObject::On(std::string trigger, T * target, void (T::*method_ptr)())
{
  emkSimpleCallback<T> * new_callback = new emkSimpleCallback<T>(target, method_ptr);
  int cb_id = emkInfo::Info().RegisterCallback(new_callback);
  EMK_Setup_OnEvent(obj_id, trigger.c_str(), cb_id);
}

extern "C" void emkJSDoCallback(int cb_id)
{ 
  emkInfo::Info().DoCallback(cb_id);
}


// The subclass of object that may be placed in a layer.
class emkShape : public emkObject {
private:
public:
  emkShape() { ; }
  virtual ~emkShape() { ; }

  void SetScale(double _x, double _y) {    
    EMK_Shape_SetScale(obj_id, _x, _y);
  }

  void SetOffset(int _x, int _y) {    
    EMK_Shape_SetOffset(obj_id, _x, _y);
  }

  void DoRotate(double rot) {
    EMK_Shape_DoRotate(obj_id, rot);
  }
};


// Manager for stage layers
class emkLayer : public emkObject {
public:
public:
  emkLayer() { obj_id = EMK_Layer_Build(obj_id); }
  ~emkLayer() { ; }

  // Add other types of stage objects; always place them in the current layer.
  emkLayer & Add(emkShape & in_obj) {
    EMK_Layer_AddObject(obj_id, in_obj.GetID());
    return *this;
  }

  void BatchDraw() { EMK_Layer_BatchDraw(obj_id); }
};


// The main Stage object from Kinetic
class emkStage : public emkObject {
private:
  std::string m_container;      // Name of this stage.
  int m_width;
  int m_height;

public:
  emkStage(int _w, int _h, std::string name="container") 
    : m_container(name)
    , m_width(_w)
    , m_height(_h)
  {
    obj_id = EMK_Stage_Build(m_width, m_height, m_container.c_str());
  }
  ~emkStage() { ; }

  // Accessors
  int GetWidth() { return m_width; }
  int GetHeight() { return m_height; }

  // Add a layer and return this stage itself (so adding can be chained...)
  emkStage & Add(emkLayer & in_layer) {
    EMK_Stage_AddLayer(obj_id, in_layer.GetID());
    return *this;
  }
};


// The text object from Kinetic...
class emkText : public emkShape {
public:
  emkText(int x=0, int y=0, std::string text="", std::string font_size="30", std::string font_family="Calibri", std::string fill="black")
  {
    obj_id = EMK_Text_Build(x, y, text.c_str(), font_size.c_str(), font_family.c_str(), fill.c_str());
  }
};

// The rectangle object from Kinetic...
class emkRect : public emkShape {
public:
  emkRect(int x=0, int y=0, int w=10, int h=10, std::string fill="white", std::string stroke="black", int stroke_width=1, int draggable=0)
  {
    obj_id = EMK_Rect_Build(x, y, w, h, fill.c_str(), stroke.c_str(), stroke_width, draggable);
  }
};

// The regular polygon object from Kinetic...
class emkRegularPolygon : public emkShape {
public:
  emkRegularPolygon(int x=0, int y=0, int sides=4, int radius=10, std::string fill="white", std::string stroke="black", int stroke_width=1, int draggable=0)
  {
    obj_id = EMK_RegularPolygon_Build(x, y, sides, radius, fill.c_str(), stroke.c_str(), stroke_width, draggable);
  }
};


// Frame information for animations...
class emkAnimationFrame {
public:
  int time_diff;  // Miliseconds since last frame. 
  int last_time;  // Miliseconds from start to last frame.
  int time;       // Miliseconds from start to current frame.
  int frame_rate; // Current frames per second.
public:
  emkAnimationFrame() {
    time_diff  = EMK_AnimationFrame_GetTimeDiff();
    last_time  = EMK_AnimationFrame_GetLastTime();
    time       = EMK_AnimationFrame_GetTime();
    frame_rate = EMK_AnimationFrame_GetFrameRate();
  }
  ~emkAnimationFrame() { ; }
};


template <class T> class emkAnimation : public emkJSCallback {
private:
  T * target;
  void (T::*method_ptr)(const emkAnimationFrame &);
public:
  emkAnimation() : target(NULL), method_ptr(NULL) { ; }
  ~emkAnimation() { ; }

  // Setup this animation object to know what class it will be working with, which update method it should use,
  // and what Stage layer it is in.
  void Setup(T * in_target, void (T::*in_method_ptr)(const emkAnimationFrame &), emkLayer & layer) {
    target = in_target;
    method_ptr = in_method_ptr;
    int cb_id = emkInfo::Info().RegisterCallback(this);
    obj_id = EMK_Animation_Build(cb_id, layer.GetID());
  }

  void DoCallback() {
    emkAnimationFrame frame;
    (target->*(method_ptr))(frame);
  }
    
  void Start() {
    assert(obj_id >= 0); // Make sure we've setup this animation before starting it.
    EMK_Animation_Start(obj_id);
  }
};

#endif
