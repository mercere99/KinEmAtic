#ifndef EM_KINETIC_H
#define EM_KINETIC_H

#include <emscripten.h>

#include <assert.h>
#include <list>
#include <string>
#include <vector>

#include "tools/Colors.h"

extern "C" {
  extern void EMK_Alert(const char * in_msg);
  extern void EMK_Setup_OnEvent(int obj_id, const char * trigger, int callback_ptr);      
  extern void EMK_Setup_OnEvent_Info(int obj_id, const char * trigger, int callback_ptr); // Pass back event

  extern void EMK_Object_Destroy(int obj_id);
  extern int EMK_Object_GetX(int obj_id);
  extern int EMK_Object_GetY(int obj_id);
  extern int EMK_Object_GetWidth(int obj_id);
  extern int EMK_Object_GetHeight(int obj_id);
  extern bool EMK_Object_GetVisible(int obj_id);
  extern double EMK_Object_GetOpacity(int obj_id);
  extern bool EMK_Object_GetListening(int obj_id);
  extern double EMK_Object_GetScaleX(int obj_id);
  extern double EMK_Object_GetScaleY(int obj_id);
  extern int EMK_Object_GetOffsetX(int obj_id);
  extern int EMK_Object_GetOffsetY(int obj_id);
  extern int EMK_Object_GetRotation(int obj_id);
  extern int EMK_Object_GetDraggable(int obj_id);

  extern void EMK_Object_SetX(int obj_id, int x);
  extern void EMK_Object_SetY(int obj_id, int y);
  extern void EMK_Object_SetXY(int obj_id, int x, int y);
  extern void EMK_Object_SetWidth(int obj_id, int w);
  extern void EMK_Object_SetHeight(int obj_id, int h);
  extern void EMK_Object_SetSize(int obj_id, int w, int h);

  extern void EMK_Object_Draw(int obj_id);
  extern void EMK_Object_DrawLayer(int obj_id);
  extern void EMK_Object_MoveToTop(int obj_id);

  extern void EMK_Canvas_SetFillStyle(const char * fs);
  extern void EMK_Canvas_SetLineJoin(const char * lj);
  extern void EMK_Canvas_SetLineWidth(int lw);
  extern void EMK_Canvas_SetFont(const char * font);
  extern void EMK_Canvas_SetTextAlign(const char * ta);
  extern void EMK_Canvas_SetShadowColor(const char * sc);
  extern void EMK_Canvas_SetShadowBlur(int blur);
  extern void EMK_Canvas_SetShadowOffsetX(int offset_x);
  extern void EMK_Canvas_SetShadowOffsetY(int offset_y);
  extern void EMK_Canvas_FillText(const char * msg, int x, int y);
  extern void EMK_Canvas_StrokeText(const char * msg, int x, int y);
  extern void EMK_Canvas_FillRect(int x, int y, int width, int height);
  extern void EMK_Canvas_StrokeRect(int x, int y, int width, int height);
  extern void EMK_Canvas_Arc(int x, int y, int radius, double start, double end, int cclockwise);
  extern void EMK_Canvas_BeginPath();
  extern void EMK_Canvas_ClosePath();
  extern void EMK_Canvas_Fill();
  extern void EMK_Canvas_LineTo(int x, int y);
  extern void EMK_Canvas_MoveTo(int x, int y);
  extern void EMK_Canvas_Restore();
  extern void EMK_Canvas_Save();
  extern void EMK_Canvas_Scale(double x, double y);
  extern void EMK_Canvas_Translate(int x, int y);
  extern void EMK_Canvas_Stroke();
  extern void EMK_Canvas_SetupTarget(int obj_id);

  extern int EMK_Image_Load(const char * file, int callback_ptr);
  extern int EMK_Image_AllLoaded();

  extern int EMK_Stage_Build(int in_w, int in_h, const char * in_name);
  extern int EMK_Stage_AddLayer(int stage_obj_id, int layer_obj_id);
  extern int EMK_Stage_ResizeMax(int stage_obj_id, int min_x, int min_y);

  extern int EMK_Layer_Build();
  extern int EMK_Layer_AddObject(int layer_obj_id, int add_obj_id);
  extern void EMK_Layer_BatchDraw(int layer_obj_id);

  extern int EMK_Image_Build(int in_x, int in_y, int img_id, int in_w, int in_h);

  extern int EMK_Text_Build(int in_x, int in_y, const char * in_text, const char * in_font_size, const char * in_font_family, const char * in_fill);
  extern int EMK_Rect_Build(int in_x, int in_y, int in_w, int in_h, const char * in_fill, const char * in_stroke, int in_stroke_width, int in_draggable);
  extern int EMK_RegularPolygon_Build(int in_x, int in_y, int in_sides, int in_radius,
                                      const char * in_fill, const char * in_stroke, int in_stroke_width, int in_draggable);

  extern int EMK_Animation_Build(int callback_ptr, int layer_id);
  extern int EMK_Animation_Build_NoFrame(int callback_ptr, int layer_id);
  extern void EMK_Animation_Start(int obj_id);

  extern void EMK_Shape_SetCornerRadius(int obj_id, int radius);
  extern void EMK_Shape_SetFillPatternImage(int obj_id, int img_id);
  extern void EMK_Shape_SetFillPatternScale(int obj_id, double scale);
  extern void EMK_Shape_SetLineJoin(int obj_id, const char * join_type);
  extern void EMK_Shape_SetOffset(int obj_id, int x_offset, int y_offset);
  extern void EMK_Shape_SetScale(int obj_id, double x_scale, double y_scale);
  extern void EMK_Shape_SetStroke(int obj_id, const char * color);
  extern void EMK_Shape_DoRotate(int obj_id, double rot);
  extern void EMK_Shape_SetDrawFunction(int obj_id, int new_callback);

  extern int EMK_Custom_Shape_Build(int x, int y, int w, int h, int draw_callback);

  // These may already be in HTML5
  extern void EMK_Cursor_Set(const char * type);
}

// Pre-declarations of some classes...
class emkEventInfo;
class emkLayer;
class emkStage;


void emkAlert(const std::string & msg) { EMK_Alert(msg.c_str()); }
void emkAlert(int val) { EMK_Alert(std::to_string(val).c_str()); }
void emkAlert(double val) { EMK_Alert(std::to_string(val).c_str()); }

#define emkAlertVar(VAR) emkAlert(std::string(#VAR) + std::string("=") + std::to_string(VAR))


// All emscripten-wrapped Kinetic objects should be derived from this base class.
class emkObject {
protected:
  int obj_id;
  emkObject * layer;

  emkObject() : obj_id(-1) {;}  // Protected so that you can't make a direct emkObject.
public:
  ~emkObject() {
    EMK_Object_Destroy(obj_id); // Cleanup this object from Kinetic.
  }
  int GetID() const { return obj_id; }

  virtual std::string GetType() { return "emkObject"; }

  // Retrieve info from JS Kinetic::Node objects
  int GetX() const { return EMK_Object_GetX(obj_id); }
  int GetY() const { return EMK_Object_GetY(obj_id); }
  int GetWidth() const { return EMK_Object_GetWidth(obj_id); }
  int GetHeight() const { return EMK_Object_GetHeight(obj_id); }
  bool GetVisible() const { return EMK_Object_GetVisible(obj_id); }
  double GetOpacity() const { return EMK_Object_GetOpacity(obj_id); }
  bool GetListening() const { return EMK_Object_GetListening(obj_id); }
  double GetScaleX() const { return EMK_Object_GetScaleX(obj_id); }
  double GetScaleY() const { return EMK_Object_GetScaleY(obj_id); }
  int GetOffsetX() const { return EMK_Object_GetOffsetX(obj_id); }
  int GetOffsetY() const { return EMK_Object_GetOffsetY(obj_id); }
  int GetRotation() const { return EMK_Object_GetRotation(obj_id); }
  int GetDraggable() const { return EMK_Object_GetDraggable(obj_id); }

  inline void SetX(int x) { EMK_Object_SetX(obj_id, x); }
  inline void SetY(int y) { EMK_Object_SetY(obj_id, y); }
  inline void SetXY(int x, int y) { EMK_Object_SetXY(obj_id, x, y); }
  inline void SetWidth(int w) { EMK_Object_SetWidth(obj_id, w); }
  inline void SetHeight(int h) { EMK_Object_SetHeight(obj_id, h); }
  inline void SetSize(int w, int h) { EMK_Object_SetSize(obj_id, w, h); }
  inline void SetLayout(int x, int y, int w, int h) {
    EMK_Object_SetXY(obj_id, x, y);
    EMK_Object_SetSize(obj_id, w, h);
  }

  void SetLayer(emkObject * _layer) { layer = _layer; }

  // Draw either this object or objects in contains.
  void Draw() { EMK_Object_Draw(obj_id); }

  // Draw all objects in this layer.
  void DrawLayer() { if (layer) EMK_Object_Draw(layer->GetID()); }

  // Move this object to the top of the current layer.
  void MoveToTop() { EMK_Object_MoveToTop(obj_id); }


  template<class T> void On(const std::string & in_trigger, T * in_target, void (T::*in_method_ptr)());
  template<class T> void On(const std::string & in_trigger, T * in_target, void (T::*in_method_ptr)(const emkEventInfo &));
};


// Manual control over the canvas...  For the moment, we're going to keep the canvas info on the JS side of things.
class emkCanvas {
public:
  // Setting values
  inline static void SetFillStyle(const emk::Color & color) { EMK_Canvas_SetFillStyle(color.AsString().c_str()); }
  inline static void SetLineJoin(const std::string & lj) { EMK_Canvas_SetLineJoin(lj.c_str()); }
  inline static void SetLineWidth(int width) { EMK_Canvas_SetLineWidth(width); }

  inline static void SetFont(const std::string & font) { EMK_Canvas_SetFont(font.c_str()); }
  inline static void SetTextAlign(const std::string & align) { EMK_Canvas_SetTextAlign(align.c_str()); }

  inline static void SetShadowColor(const emk::Color & color) { EMK_Canvas_SetShadowColor(color.AsString().c_str()); }
  inline static void SetShadowBlur(int blur) { EMK_Canvas_SetShadowBlur(blur); }
  inline static void SetShadowOffsetX(int offset_x) { EMK_Canvas_SetShadowOffsetX(offset_x); }
  inline static void SetShadowOffsetY(int offset_y) { EMK_Canvas_SetShadowOffsetY(offset_y); }

  // Shapes and Text
  inline static void Text(const std::string & msg, int x, int y, bool fill=true) {
    if (fill) EMK_Canvas_FillText(msg.c_str(), x, y);
    else EMK_Canvas_StrokeText(msg.c_str(), x, y);
  }

  inline static void Rect(int x, int y, int width, int height, bool fill=false) {
    if (fill) EMK_Canvas_FillRect(x, y, width, height);
    else EMK_Canvas_StrokeRect(x, y, width, height);
  }

  inline static void Arc(int x, int y, int radius, double start, double end, bool cclockwise=false) {
    EMK_Canvas_Arc(x, y, radius, start, end, cclockwise);
  }

  // Paths
  inline static void BeginPath() { EMK_Canvas_BeginPath(); }
  inline static void ClosePath() { EMK_Canvas_ClosePath(); }
  inline static void Fill() { EMK_Canvas_Fill(); }
  inline static void LineTo(int x, int y) { EMK_Canvas_LineTo(x, y); }
  inline static void MoveTo(int x, int y) { EMK_Canvas_MoveTo(x, y); }

  // Transformations
  inline static void Restore() { EMK_Canvas_Restore(); }
  inline static void Save() { EMK_Canvas_Save(); }
  inline static void Scale(double x, double y) { EMK_Canvas_Scale(x, y);  }
  inline static void Translate(int x, int y) { EMK_Canvas_Translate(x, y);  } 
 

  // Finsihing
  inline static void Stroke() { EMK_Canvas_Stroke(); }
  inline static void SetupTarget(const emkObject & obj) { EMK_Canvas_SetupTarget(obj.GetID()); }
};

// Mediator for handling callbacks from JS.
class emkCallback : public emkObject {
private:
  bool is_disposible;
public:
  emkCallback() : is_disposible(false) { obj_id = -2; }
  virtual ~emkCallback() { ; }

  virtual std::string GetType() { return "emkCallback"; }

  virtual void DoCallback(int * arg_ptr=NULL) = 0;

  bool IsDisposible() const { return is_disposible; }

  void SetDisposible(bool _in=true) { is_disposible = _in; }
};


template <class T> class emkMethodCallback : public emkCallback {
private:
  T * target;
  void (T::*method_ptr)();
public:
  emkMethodCallback(T * in_target, void (T::*in_method_ptr)())
    : target(in_target)
    , method_ptr(in_method_ptr)
  { ; }

  ~emkMethodCallback() { ; }

  virtual std::string GetType() { return "emkMethodCallback"; }

  void DoCallback(int * arg_ptr) { (void) arg_ptr; (target->*(method_ptr))(); }
};


class emkEventInfo {
public:
  int layer_x;      // Coordinates of mouse in this layer.
  int layer_y;
  int button;       // Which button was pressed?
  int key_code;     // Which key was pressed?
  bool alt_key;     // Modifier keys
  bool ctrl_key;
  bool meta_key;
  bool shift_key;  

  emkEventInfo(int lx, int ly, int but, int key, int alt, int ctrl, int meta, int shift)
    : layer_x(lx), layer_y(ly), button(but), key_code(key)
    , alt_key(alt), ctrl_key(ctrl), meta_key(meta), shift_key(shift) { ; }
};

template <class T> class emkMethodCallback_Event : public emkCallback {
private:
  T * target;
  void (T::*method_ptr)(const emkEventInfo &);
public:
  emkMethodCallback_Event(T * in_target, void (T::*in_method_ptr)(const emkEventInfo &))
    : target(in_target)
    , method_ptr(in_method_ptr)
  { ; }

  ~emkMethodCallback_Event() { ; }

  virtual std::string GetType() { return "emkCallback_Event"; }

  void DoCallback(int * arg_ptr) {
    emkEventInfo info(arg_ptr[0], arg_ptr[1], arg_ptr[2], arg_ptr[3], arg_ptr[4], arg_ptr[5], arg_ptr[6], arg_ptr[7]);
    (target->*(method_ptr))(info);
    
    // (target->*(method_ptr))( *((emkEventInfo *) arg_ptr) );
  }
};


template <class T> class emkDrawCallback : public emkCallback {
private:
  T * target;
  void (T::*method_ptr)(emkCanvas &);
public:
  emkDrawCallback(T * in_target, void (T::*in_method_ptr)(emkCanvas &))
    : target(in_target)
    , method_ptr(in_method_ptr)
  { ; }

  ~emkDrawCallback() { ; }

  virtual std::string GetType() { return "emkDrawCallback"; }

  void DoCallback(int * arg_ptr) {
    emkCanvas canvas; // @CAO For now, all canvas objects are alike; we should allow them to coexist.
    (target->*(method_ptr))(canvas); }
};


template<class T> void emkObject::On(const std::string & trigger, T * target,
                                     void (T::*method_ptr)())
{
  // @CAO Technically we should track these callbacks to make sure we delete them properly.
  emkMethodCallback<T> * new_callback = new emkMethodCallback<T>(target, method_ptr);
  EMK_Setup_OnEvent(obj_id, trigger.c_str(), (int) new_callback);
}

template<class T> void emkObject::On(const std::string & trigger, T * target,
                                     void (T::*method_ptr)(const emkEventInfo &))
{
  // @CAO Technically we should track these callbacks to make sure we delete them properly.
  emkMethodCallback_Event<T> * new_callback = new emkMethodCallback_Event<T>(target, method_ptr);
  EMK_Setup_OnEvent_Info(obj_id, trigger.c_str(), (int) new_callback);
}

extern "C" void emkJSDoCallback(int cb_ptr, int arg_ptr)
{
  emkCallback * const cb_obj = (emkCallback *) cb_ptr;
  cb_obj->DoCallback((int *) arg_ptr);
  if (cb_obj->IsDisposible()) {
    delete cb_obj;
  }
  // ((emkCallback *) cb_ptr)->DoCallback((int *) arg_ptr);
}


class emkImage : public emkCallback {
private:
  const std::string filename;
  mutable bool has_loaded;
  mutable std::list<emkLayer *> layers_waiting;
public:
  emkImage(const std::string & _filename) : filename(_filename), has_loaded(false) {
    obj_id = EMK_Image_Load(filename.c_str(), (int) this);   // Start loading the image.
  }

  virtual std::string GetType() { return "emkImage"; }

  bool HasLoaded() const { return has_loaded; }

  void DrawOnLoad(emkLayer * in_layer) const { layers_waiting.push_back(in_layer); }

  void DoCallback(int * arg_ptr); // Called back when image is loaded
};


// The subclass of object that may be placed in a layer.
class emkShape : public emkObject {
protected:
  const emkImage * image;  // If we are drawing an image, keep track of it to make sure it loads.
  emkCallback * draw_callback; // If we override the draw callback, keep track of it here.

  emkShape() : image(NULL), draw_callback(NULL) { obj_id = -3; } // The default Shape constructor should only be called from derived classes.
public:
  virtual ~emkShape() { ; }

  virtual std::string GetType() { return "emkShape"; }

  void SetFillPatternImage(const emkImage & _image) {
    image = &_image;
    EMK_Shape_SetFillPatternImage(obj_id, image->GetID());
  }

  /*
  inline int GetX() const { return EMK_Shape_GetX(obj_id); }
  inline int GetY() const { return EMK_Shape_GetY(obj_id); }
  inline int GetWidth() const { return EMK_Shape_GetWidth(obj_id); }
  inline int GetHeight() const { return EMK_Shape_GetHeight(obj_id); }
  */

  inline void SetCornerRadius(int radius) { EMK_Shape_SetCornerRadius(obj_id, radius); }
  inline void SetFillPatternScale(double scale) { EMK_Shape_SetFillPatternScale(obj_id, scale); }
  inline void SetLineJoin(const char * join_type) { EMK_Shape_SetLineJoin(obj_id, join_type); }
  inline void SetOffset(int _x, int _y) { EMK_Shape_SetOffset(obj_id, _x, _y); }
  inline void SetScale(double _x, double _y) { EMK_Shape_SetScale(obj_id, _x, _y); }
  inline void SetScale(double scale) { EMK_Shape_SetScale(obj_id, scale, scale); }
  inline void SetStroke(const emk::Color & color) { EMK_Shape_SetStroke(obj_id, color.AsString().c_str()); }


  // Override the drawing of this shape.
  template<class T> void SetDrawFunction(T * target, void (T::*draw_ptr)(emkCanvas &) ) {
    if (draw_callback != NULL) delete draw_callback;
    draw_callback = new emkDrawCallback<T>(target, draw_ptr);
    EMK_Shape_SetDrawFunction(obj_id, (int) draw_callback);
  }

  void DoRotate(double rot) { EMK_Shape_DoRotate(obj_id, rot); }

  const emkImage * GetImage() { return image; }
};


// Build your own shape!
class emkCustomShape : public emkShape {
public:
  template <class T> emkCustomShape(T * target, void (T::*draw_ptr)(emkCanvas &))
  {
    draw_callback = new emkDrawCallback<T>(target, draw_ptr);
    obj_id = EMK_Custom_Shape_Build(0, 0, 0, 0, (int) draw_callback);
  }
  template <class T> emkCustomShape(int _x, int _y, int _w, int _h, T * target, void (T::*draw_ptr)(emkCanvas &))
  {
    draw_callback = new emkDrawCallback<T>(target, draw_ptr);
    obj_id = EMK_Custom_Shape_Build(_x, _y, _w, _h, (int) draw_callback);
  }
  virtual ~emkCustomShape() { ; }

  virtual std::string GetType() { return "emkCustomShape"; }
};



// Manager for stage layers
class emkLayer : public emkObject {
public:
public:
  emkLayer() { obj_id = EMK_Layer_Build(); }
  ~emkLayer() { ; }

  virtual std::string GetType() { return "emkLayer"; }

  // Add other types of stage objects; always place them in the current layer.
  emkLayer & Add(emkShape & in_obj) {
    in_obj.SetLayer(this);

    // If the object we are adding has an image that hasn't been loaded, setup a callback.
    const emkImage * image = in_obj.GetImage();
    if (image && image->HasLoaded() == false) {
      image->DrawOnLoad(this);
    }
    EMK_Layer_AddObject(obj_id, in_obj.GetID());
    return *this;
  }

  void BatchDraw() { EMK_Layer_BatchDraw(obj_id); }
};


// The main Stage object from Kinetic
class emkStage : public emkObject {
private:
  std::string m_container;      // Name of this stage.

public:
  emkStage(int _w, int _h, std::string name="container") 
    : m_container(name)
  {
    obj_id = EMK_Stage_Build(_w, _h, m_container.c_str());
  }
  ~emkStage() { ; }

  virtual std::string GetType() { return "emkStage"; }

  // Sizing
  void ResizeMax(int min_width=0, int min_height=0) { EMK_Stage_ResizeMax(obj_id, min_width, min_height); }

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
  ~emkText() { ; }

  virtual std::string GetType() { return "emkText"; }
};

// The rectangle object from Kinetic...
class emkRect : public emkShape {
public:
  emkRect(int x=0, int y=0, int w=10, int h=10, std::string fill="white", std::string stroke="black", int stroke_width=1, int draggable=0)
  {
    obj_id = EMK_Rect_Build(x, y, w, h, fill.c_str(), stroke.c_str(), stroke_width, draggable);
  }

  virtual std::string GetType() { return "emkRect"; }
};

// The regular polygon object from Kinetic...
class emkRegularPolygon : public emkShape {
public:
  emkRegularPolygon(int x=0, int y=0, int sides=4, int radius=10, std::string fill="white", std::string stroke="black", int stroke_width=1, int draggable=0)
  {
    obj_id = EMK_RegularPolygon_Build(x, y, sides, radius, fill.c_str(), stroke.c_str(), stroke_width, draggable);
  }

  virtual std::string GetType() { return "emkRegularPolygon"; }
};


// Frame information for animations...
class emkAnimationFrame {
public:
  int time_diff;  // Miliseconds since last frame. 
  int last_time;  // Miliseconds from start to last frame.
  int time;       // Miliseconds from start to current frame.
  int frame_rate; // Current frames per second.
public:
  emkAnimationFrame(int _td, int _lt, int _t, int _fr)
    : time_diff(_td), last_time(_lt), time(_t), frame_rate(_fr) { ; }
  ~emkAnimationFrame() { ; }
};


template <class T> class emkAnimation : public emkCallback {
private:
  T * target;
  void (T::*method_ptr)(const emkAnimationFrame &);
  void (T::*method_ptr_nf)();
public:
  emkAnimation() : target(NULL), method_ptr(NULL), method_ptr_nf(NULL) { ; }
  ~emkAnimation() { ; }

  virtual std::string GetType() { return "emkAnimation"; }

  // Setup this animation object to know what class it will be working with, which update method it should use,
  // and what Stage layer it is in.  The method pointed to may, optionally, take a frame object.
  void Setup(T * in_target, void (T::*in_method_ptr)(const emkAnimationFrame &), emkLayer & layer) {
    target = in_target;
    method_ptr = in_method_ptr;
    obj_id = EMK_Animation_Build((int) this, layer.GetID());
  }

  void Setup(T * in_target, void (T::*in_method_ptr)(), emkLayer & layer) {
    target = in_target;
    method_ptr_nf = in_method_ptr;
    obj_id = EMK_Animation_Build_NoFrame((int) this, layer.GetID());
  }

  void DoCallback(int * arg_ptr) {
    if (arg_ptr > 0) {
      emkAnimationFrame frame(arg_ptr[0], arg_ptr[1], arg_ptr[2], arg_ptr[3]);
      (target->*(method_ptr))(frame);
    } else {
      emkAnimationFrame frame(arg_ptr[0], arg_ptr[1], arg_ptr[2], arg_ptr[3]);
      (target->*(method_ptr_nf))();
    }
  }
    
  void Start() {
    assert(obj_id >= 0); // Make sure we've setup this animation before starting it.
    EMK_Animation_Start(obj_id);
  }
};


//////////////////////////////////////////////////////////
// Methods that refer to other classes defined later...

void emkImage::DoCallback(int * arg_ptr) { // Called back when image is loaded
  (void) arg_ptr;
  has_loaded = true;
  while (layers_waiting.size()) {
    emkLayer * cur_layer = layers_waiting.front();
    layers_waiting.pop_front();
    cur_layer->BatchDraw();
  }
}

#endif
