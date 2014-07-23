#ifndef EM_KINETIC_H
#define EM_KINETIC_H

#include <emscripten.h>

#include <list>
#include <string>
#include <vector>

#include "../tools/Callbacks.h"
#include "../tools/Colors.h"

#include "../tools/debug.h"

extern "C" {
  extern void EMK_Alert(const char * msg);

  extern void EMK_Object_Draw(int obj_id);
  extern void EMK_Object_DrawLayer(int obj_id);
  extern void EMK_Object_MoveToTop(int obj_id);

  extern int EMK_Tween_Build(int target_id, double seconds);
  extern void EMK_Tween_Configure(int settings_id, int obj_id);
  extern void EMK_Tween_SetFinishedCallback(int settings_id, int callback_ptr, int info_ptr);
  extern void EMK_Tween_SetX(int settings_id, int _x);
  extern void EMK_Tween_SetY(int settings_id, int _y);
  extern void EMK_Tween_SetXY(int settings_id, int _x, int _y);
  extern void EMK_Tween_SetScaleX(int settings_id, double _x);
  extern void EMK_Tween_SetScaleY(int settings_id, double _y);
  extern void EMK_Tween_SetScaleXY(int settings_id, double _x, double _y);
  extern void EMK_Tween_Play(int obj_id);

  extern void EMK_Canvas_SetFillStyle(const char * fs);
  extern void EMK_Canvas_SetStroke(const char * fs);
  extern void EMK_Canvas_SetLineJoin(const char * lj);
  extern void EMK_Canvas_SetLineWidth(double lw);
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
  extern void EMK_Canvas_DrawImage(int image_id, int x, int y);
  extern void EMK_Canvas_DrawImage_Size(int image_id, int x, int y, int w, int h);
  extern void EMK_Canvas_BeginPath();
  extern void EMK_Canvas_ClosePath();
  extern void EMK_Canvas_Fill();
  extern void EMK_Canvas_LineTo(int x, int y);
  extern void EMK_Canvas_MoveTo(int x, int y);
  extern void EMK_Canvas_Restore();
  extern void EMK_Canvas_Save();
  extern void EMK_Canvas_Scale(double x, double y);
  extern void EMK_Canvas_Translate(int x, int y);
  extern void EMK_Canvas_Rotate(double angle);
  extern void EMK_Canvas_Stroke();
  extern void EMK_Canvas_SetupTarget(int obj_id);

  extern int EMK_Image_Load(const char * file, int callback_ptr);
  extern int EMK_Image_AllLoaded();

  extern int EMK_Stage_Build(int _w, int _h, const char * _name);
  extern int EMK_Stage_AddLayer(int stage_obj_id, int layer_obj_id);
  extern int EMK_Stage_ResizeMax(int stage_obj_id, int min_x, int min_y);

  extern int EMK_Layer_Build();
  extern int EMK_Layer_AddObject(int layer_obj_id, int add_obj_id);
  extern void EMK_Layer_BatchDraw(int layer_obj_id);

  extern int EMK_Image_Build(int _x, int _y, int img_id, int _w, int _h);

  extern int EMK_Rect_Build(int _x, int _y, int _w, int _h, const char * _fill, const char * _stroke, int _stroke_width, int _draggable);
  extern int EMK_RegularPolygon_Build(int _x, int _y, int _sides, int _radius,
                                      const char * _fill, const char * _stroke, int _stroke_width, int _draggable);

  extern int EMK_Animation_Build(int callback_ptr, int layer_id);
  extern int EMK_Animation_Build_NoFrame(int callback_ptr, int layer_id);
  extern void EMK_Animation_Start(int obj_id);
  extern void EMK_Animation_Stop(int obj_id);

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


namespace emk {

  // Pre-declarations of some classes...
  class Layer;
  class Stage;


  void Alert(const std::string & msg) { EMK_Alert(msg.c_str()); }
  void Alert(int val) { EMK_Alert(std::to_string(val).c_str()); }
  void Alert(double val) { EMK_Alert(std::to_string(val).c_str()); }

#define AlertVar(VAR) emk::Alert(std::string(#VAR) + std::string("=") + std::to_string(VAR))


  // All emscripten-wrapped Kinetic objects should be derived from this base class.
  class Object {
  protected:
    int obj_id;
    Object * layer;

    Object() : obj_id(-1) {;}  // Protected so that you can't make a direct Object.
  public:
    ~Object() {
      EM_ASM_ARGS({
          if ($0 >= 0) emk_info.objs[$0].destroy();
        }, obj_id);
    }
    int GetID() const { return obj_id; }

    virtual std::string GetType() { return "emkObject"; }

    // Retrieve info from JS Kinetic::Node objects
    int GetX() const { return EM_ASM_INT({return emk_info.objs[$0].x();}, obj_id); }
    int GetY() const { return EM_ASM_INT({return emk_info.objs[$0].y();}, obj_id); }
    int GetWidth() const { return EM_ASM_INT({return emk_info.objs[$0].width();}, obj_id); }
    int GetHeight() const { return EM_ASM_INT({return emk_info.objs[$0].height();}, obj_id); }
    bool GetVisible() const { return EM_ASM_INT({return emk_info.objs[$0].visible();}, obj_id); }
    double GetOpacity() const { return EM_ASM_DOUBLE({return emk_info.objs[$0].opacity();}, obj_id); }
    bool GetListening() const { return EM_ASM_INT({return emk_info.objs[$0].listening();}, obj_id); }
    double GetScaleX() const { return EM_ASM_DOUBLE({return emk_info.objs[$0].scaleX();}, obj_id); }
    double GetScaleY() const { return EM_ASM_DOUBLE({return emk_info.objs[$0].scaleY();}, obj_id); }
    int GetOffsetX() const { return EM_ASM_INT({return emk_info.objs[$0].offsetX();}, obj_id); }
    int GetOffsetY() const { return EM_ASM_INT({return emk_info.objs[$0].offsetY();}, obj_id); }
    int GetRotation() const { return EM_ASM_INT({return emk_info.objs[$0].rotation();}, obj_id); }
    int GetDraggable() const { return EM_ASM_INT({return emk_info.objs[$0].draggable();}, obj_id); }

    int SetX(int _in) const { return EM_ASM_ARGS({emk_info.objs[$0].x($1);}, obj_id, _in); }
    int SetY(int _in) const { return EM_ASM_ARGS({emk_info.objs[$0].y($1);}, obj_id, _in); }
    int SetWidth(int _in) const { return EM_ASM_ARGS({emk_info.objs[$0].width($1);}, obj_id, _in); }
    int SetHeight(int _in) const { return EM_ASM_ARGS({emk_info.objs[$0].height($1);}, obj_id, _in); }
    bool SetVisible(int _in) const { return EM_ASM_ARGS({emk_info.objs[$0].visible($1);}, obj_id, _in); }
    double SetOpacity(double _in) const { return EM_ASM_ARGS({emk_info.objs[$0].opacity($1);}, obj_id, _in); }
    bool SetListening(int _in) const { return EM_ASM_ARGS({emk_info.objs[$0].listening($1);}, obj_id, _in); }
    double SetScaleX(double _in) const { return EM_ASM_ARGS({emk_info.objs[$0].scaleX($1);}, obj_id, _in); }
    double SetScaleY(double _in) const { return EM_ASM_ARGS({emk_info.objs[$0].scaleY($1);}, obj_id, _in); }
    int SetOffsetX(int _in) const { return EM_ASM_ARGS({emk_info.objs[$0].offsetX($1);}, obj_id, _in); }
    int SetOffsetY(int _in) const { return EM_ASM_ARGS({emk_info.objs[$0].offsetY($1);}, obj_id, _in); }
    int SetRotation(int _in) const { return EM_ASM_ARGS({emk_info.objs[$0].rotation($1);}, obj_id, _in); }
    int SetDraggable(int _in) const { return EM_ASM_ARGS({emk_info.objs[$0].draggable($1);}, obj_id, _in); }

    inline void SetXY(int x, int y) { SetX(x); SetY(y); }
    inline void SetSize(int w, int h) { SetWidth(w); SetHeight(h); }
    inline void SetLayout(int x, int y, int w, int h) { SetX(x); SetY(y); SetWidth(w); SetHeight(h); }

    void SetLayer(Object * _layer) { layer = _layer; }

    // Draw either this object or objects in contains.
    void Draw() { EMK_Object_Draw(obj_id); }

    // Draw all objects in this layer.
    void DrawLayer() { if (layer) EMK_Object_Draw(layer->GetID()); }

    // Move this object to the top of the current layer.
    void MoveToTop() { EMK_Object_MoveToTop(obj_id); }


    template<class T> void On(const std::string & _trigger, T * _target, void (T::*_method_ptr)());
    template<class T> void On(const std::string & _trigger, T * _target, void (T::*_method_ptr)(const EventInfo &));
  };


  class Tween : public Object {
  private:
    Object * target;   // What object is this tween associated with?
    double seconds;    // How long should this tween run for?

    int settings_id;   // JS memory position where the tween settings should go.
    bool needs_config; // Does the Tween need to be reconfigured?

    void BuildTween() {
      EMK_Tween_Configure(settings_id, obj_id);
      needs_config = false;
    }
  public:
    Tween(Object & _target, double _seconds) : target(&_target), seconds(_seconds), needs_config(false) {
      settings_id = EMK_Tween_Build(target->GetID(), seconds);
      obj_id = settings_id + 1;
    }
    ~Tween() { ; }

    // void SetTarget(Object & _target) { target = &_target; needs_config=true; }
    // void SetTime(double _seconds) { seconds = _seconds; needs_config=true; }
    void SetX(int _x) { EMK_Tween_SetX(settings_id, _x); needs_config=true; }
    void SetY(int _y) { EMK_Tween_SetY(settings_id, _y); needs_config=true; }
    void SetXY(int _x, int _y) { EMK_Tween_SetXY(settings_id, _x, _y); needs_config=true; }
    void SetScaleX(double _x) { EMK_Tween_SetScaleX(settings_id, _x); needs_config=true; }
    void SetScaleY(double _y) { EMK_Tween_SetScaleY(settings_id, _y); needs_config=true; }
    void SetScaleXY(double _x, double _y) {
      EMK_Tween_SetScaleXY(settings_id, _x, _y); needs_config=true;
    }

    void SetFinishedCallback(Callback * callback, int * info_ptr) {
      EMK_Tween_SetFinishedCallback(settings_id, (int) (callback), (int) info_ptr);
    }

    void Play() {
      if (needs_config) BuildTween();
      EMK_Tween_Play(obj_id);
    }
  };


  class Image : public Callback, public Object {
  private:
    const std::string filename;
    mutable bool has_loaded;
    mutable std::list<Layer *> layers_waiting;
  public:
    Image(const std::string & _filename) : filename(_filename), has_loaded(false) {
      obj_id = EMK_Image_Load(filename.c_str(), (int) this);   // Start loading the image.
    }

    virtual std::string GetType() { return "emkImage"; }

    bool HasLoaded() const { return has_loaded; }

    void DrawOnLoad(Layer * _layer) const { layers_waiting.push_back(_layer); }

    void DoCallback(int * arg_ptr); // Called back when image is loaded
  };


  // Manual control over the canvas...  For the moment, we're going to keep the canvas info on the JS side of things.
  class Canvas {
  public:
    // Setting values
    inline static void SetFillStyle(const Color & color) { EMK_Canvas_SetFillStyle(color.AsString().c_str()); }
    inline static void SetStroke(const Color & color) { EMK_Canvas_SetStroke(color.AsString().c_str()); }
    inline static void SetLineJoin(const std::string & lj) { EMK_Canvas_SetLineJoin(lj.c_str()); }
    inline static void SetLineWidth(double width) { EMK_Canvas_SetLineWidth(width); }

    inline static void SetFont(const std::string & font) { EMK_Canvas_SetFont(font.c_str()); }
    inline static void SetTextAlign(const std::string & align) { EMK_Canvas_SetTextAlign(align.c_str()); }

    inline static void SetShadowColor(const Color & color) { EMK_Canvas_SetShadowColor(color.AsString().c_str()); }
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

    inline static void DrawImage(const Image & image, int x, int y) {
      EMK_Canvas_DrawImage(image.GetID(), x, y);
    }

    inline static void DrawImage(const Image & image, int x, int y, int w, int h) {
      EMK_Canvas_DrawImage_Size(image.GetID(), x, y, w, h);
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
    inline static void Scale(double new_scale) { EMK_Canvas_Scale(new_scale, new_scale);  }
    inline static void Translate(int x, int y) { EMK_Canvas_Translate(x, y);  } 
    inline static void Rotate(double angle) { EMK_Canvas_Rotate(angle);  } 
 

    // Finsihing
    inline static void Stroke() { EMK_Canvas_Stroke(); }
    inline static void SetupTarget(const Object & obj) { EMK_Canvas_SetupTarget(obj.GetID()); }
  };


  template <class T> class Callback_Canvas : public Callback {
  private:
    T * target;
    void (T::*method_ptr)(Canvas &);
  public:
    Callback_Canvas(T * _target, void (T::*_method_ptr)(Canvas &))
      : target(_target)
      , method_ptr(_method_ptr)
    { ; }

    ~Callback_Canvas() { ; }

    virtual std::string GetType() { return "emkCallback_Canvas"; }

    void DoCallback(int * arg_ptr) {
      Canvas canvas; // @CAO For now, all canvas objects are alike; we should allow them to coexist.
      (target->*(method_ptr))(canvas); }
  };


  template<class T> void Object::On(const std::string & trigger, T * target,
                                       void (T::*method_ptr)())
  {
    // @CAO Technically we should track these callbacks to make sure we delete them properly.
    MethodCallback<T> * new_callback = new MethodCallback<T>(target, method_ptr);
    EM_ASM_ARGS({
        var trigger = Pointer_stringify($1);
        emk_info.objs[$0].on(trigger, function() {
            emkJSDoCallback($2);
        });
    }, obj_id, trigger.c_str(), (int) new_callback);
  }

  template<class T> void Object::On(const std::string & trigger, T * target,
                                       void (T::*method_ptr)(const EventInfo &))
  {
    // @CAO Technically we should track these callbacks to make sure we delete them properly.
    MethodCallback_Event<T> * new_callback = new MethodCallback_Event<T>(target, method_ptr);
    EM_ASM_ARGS({
        var trigger = Pointer_stringify($1);
        emk_info.objs[$0].on(trigger, function(event) {
            var evt = event.evt;
            var ptr = Module._malloc(32); // 8 ints @ 4 bytes each...
            setValue(ptr,    evt.layerX,   'i32');
            setValue(ptr+4,  evt.layerY,   'i32');
            setValue(ptr+8,  evt.button,   'i32');
            setValue(ptr+12, evt.keyCode,  'i32');
            setValue(ptr+16, evt.altKey,   'i32');
            setValue(ptr+20, evt.ctrlKey,  'i32');
            setValue(ptr+24, evt.metaKey,  'i32');
            setValue(ptr+28, evt.shiftKey, 'i32');

            emkJSDoCallback($2, ptr);
            Module._free(ptr);
        });
    },  obj_id, trigger.c_str(), (int) new_callback);
  }


  // The subclass of object that may be placed in a layer.
  class Shape : public Object {
  protected:
    const Image * image;  // If we are drawing an image, keep track of it to make sure it loads.
    Callback * draw_callback; // If we override the draw callback, keep track of it here.

    Shape() : image(NULL), draw_callback(NULL) { obj_id = -3; } // The default Shape constructor should only be called from derived classes.
  public:
    virtual ~Shape() { ; }

    virtual std::string GetType() { return "emkShape"; }

    virtual void SetFillPatternImage(const Image & _image) {
      image = &_image;
      EMK_Shape_SetFillPatternImage(obj_id, image->GetID());
    }

    inline void SetCornerRadius(int radius) { EMK_Shape_SetCornerRadius(obj_id, radius); }
    inline void SetFillPatternScale(double scale) { EMK_Shape_SetFillPatternScale(obj_id, scale); }
    inline void SetLineJoin(const char * join_type) { EMK_Shape_SetLineJoin(obj_id, join_type); }
    inline void SetOffset(int _x, int _y) { EMK_Shape_SetOffset(obj_id, _x, _y); }
    inline void SetScale(double _x, double _y) { EMK_Shape_SetScale(obj_id, _x, _y); }
    inline void SetScale(double scale) { EMK_Shape_SetScale(obj_id, scale, scale); }
    inline void SetStroke(const Color & color) { EMK_Shape_SetStroke(obj_id, color.AsString().c_str()); }


    // Override the drawing of this shape.
    template<class T> void SetDrawFunction(T * target, void (T::*draw_ptr)(Canvas &) ) {
      if (draw_callback != NULL) delete draw_callback;
      draw_callback = new Callback_Canvas<T>(target, draw_ptr);
      EMK_Shape_SetDrawFunction(obj_id, (int) draw_callback);
    }

    void DoRotate(double rot) { EMK_Shape_DoRotate(obj_id, rot); }

    const Image * GetImage() { return image; }
  };


  // Build your own shape!
  class CustomShape : public Shape {
  public:
    template <class T> CustomShape(T * target, void (T::*draw_ptr)(Canvas &))
    {
      draw_callback = new Callback_Canvas<T>(target, draw_ptr);
      obj_id = EMK_Custom_Shape_Build(0, 0, 0, 0, (int) draw_callback);
    }
    template <class T> CustomShape(int _x, int _y, int _w, int _h, T * target, void (T::*draw_ptr)(Canvas &))
    {
      draw_callback = new Callback_Canvas<T>(target, draw_ptr);
      obj_id = EMK_Custom_Shape_Build(_x, _y, _w, _h, (int) draw_callback);
    }
    virtual ~CustomShape() { ; }

    virtual std::string GetType() { return "emkCustomShape"; }
  };



  // Manager for stage layers
  class Layer : public Object {
  public:
  public:
    Layer() { obj_id = EMK_Layer_Build(); }
    ~Layer() { ; }

    virtual std::string GetType() { return "emkLayer"; }

    // Add other types of stage objects; always place them in the current layer.
    Layer & Add(Shape & _obj) {
      _obj.SetLayer(this);

      // If the object we are adding has an image that hasn't been loaded, setup a callback.
      const Image * image = _obj.GetImage();
      if (image && image->HasLoaded() == false) {
        image->DrawOnLoad(this);
      }
      EMK_Layer_AddObject(obj_id, _obj.GetID());
      return *this;
    }

    void BatchDraw() { EMK_Layer_BatchDraw(obj_id); }
  };


  // The main Stage object from Kinetic
  class Stage : public Object {
  private:
    std::string m_container;      // Name of this stage.

  public:
    Stage(int _w, int _h, std::string name="container") 
      : m_container(name)
    {
      obj_id = EMK_Stage_Build(_w, _h, m_container.c_str());
    }
    ~Stage() { ; }

    virtual std::string GetType() { return "emkStage"; }

    // Sizing
    void ResizeMax(int min_width=0, int min_height=0) { EMK_Stage_ResizeMax(obj_id, min_width, min_height); }

    // Add a layer and return this stage itself (so adding can be chained...)
    Stage & Add(Layer & _layer) {
      EMK_Stage_AddLayer(obj_id, _layer.GetID());
      return *this;
    }
  };


  // The text object from Kinetic...
  class Text : public Shape {
  public:
    Text(int x=0, int y=0, std::string text="", std::string font_size="30", std::string font_family="Calibri", std::string fill="black")
    {
      obj_id = EM_ASM_INT( {
          var obj_id = emk_info.objs.length;         // Determine the next free id for a Kinetic object.
          _text = Pointer_stringify($2);             // Make sure string values are properly converted (text)
          _font_size = Pointer_stringify($3);        // Make sure string values are properly converted (size)
          _font_family = Pointer_stringify($4);      // Make sure string values are properly converted (font)
          _fill = Pointer_stringify($5);             // Make sure string values are properly converted (color)
        
          emk_info.objs[obj_id] = new Kinetic.Text({           // Build the new text object!
              x: $0,
              y: $1,
              text: _text,
              fontSize: _font_size,
              fontFamily: _font_family,
              fill: _fill
          });
          return obj_id;                                       // Return the Kinetic object id.
      }, x, y, text.c_str(), font_size.c_str(), font_family.c_str(), fill.c_str());
    }
    ~Text() { ; }

    virtual std::string GetType() { return "emkText"; }

    void SetText(const std::string & _text) {
      EM_ASM_ARGS({var _text = Pointer_stringify($1); emk_info.objs[$0].text(_text);}, obj_id, _text.c_str());
    }
  };

  // The rectangle object from Kinetic...
  class Rect : public Shape {
  public:
    Rect(int x=0, int y=0, int w=10, int h=10, std::string fill="white", std::string stroke="black", int stroke_width=1, int draggable=0)
    {
      obj_id = EMK_Rect_Build(x, y, w, h, fill.c_str(), stroke.c_str(), stroke_width, draggable);
    }

    virtual std::string GetType() { return "emkRect"; }
  };

  // The regular polygon object from Kinetic...
  class RegularPolygon : public Shape {
  public:
    RegularPolygon(int x=0, int y=0, int sides=4, int radius=10, std::string fill="white", std::string stroke="black", int stroke_width=1, int draggable=0)
    {
      obj_id = EMK_RegularPolygon_Build(x, y, sides, radius, fill.c_str(), stroke.c_str(), stroke_width, draggable);
    }

    virtual std::string GetType() { return "emkRegularPolygon"; }
  };


  // Frame information for animations...
  class AnimationFrame {
  public:
    int time_diff;  // Miliseconds since last frame. 
    int last_time;  // Miliseconds from start to last frame.
    int time;       // Miliseconds from start to current frame.
    int frame_rate; // Current frames per second.
  public:
    AnimationFrame(int _td, int _lt, int _t, int _fr)
      : time_diff(_td), last_time(_lt), time(_t), frame_rate(_fr) { ; }
    ~AnimationFrame() { ; }
  };


  template <class T> class Animation : public Callback, public Object {
  private:
    T * target;
    void (T::*method_ptr)(const AnimationFrame &);
    void (T::*method_ptr_nf)();
    bool is_running;
  public:
    Animation() : target(NULL), method_ptr(NULL), method_ptr_nf(NULL), is_running(false) { ; }
    ~Animation() { ; }

    std::string GetType() const { return "emkAnimation"; }

    bool IsRunning() const { return is_running; }

    // Setup this animation object to know what class it will be working with, which update method it should use,
    // and what Stage layer it is in.  The method pointed to may, optionally, take a frame object.
    void Setup(T * _target, void (T::*_method_ptr)(const AnimationFrame &), Layer & layer) {
      target = _target;
      method_ptr = _method_ptr;
      obj_id = EMK_Animation_Build((int) this, layer.GetID());
    }

    void Setup(T * _target, void (T::*_method_ptr)(), Layer & layer) {
      target = _target;
      method_ptr_nf = _method_ptr;
      obj_id = EMK_Animation_Build_NoFrame((int) this, layer.GetID());
    }

    void DoCallback(int * arg_ptr) {
      if (arg_ptr > 0) {
        AnimationFrame frame(arg_ptr[0], arg_ptr[1], arg_ptr[2], arg_ptr[3]);
        (target->*(method_ptr))(frame);
      } else {
        AnimationFrame frame(arg_ptr[0], arg_ptr[1], arg_ptr[2], arg_ptr[3]);
        (target->*(method_ptr_nf))();
      }
    }
    
    void Start() {
      assert(obj_id >= 0); // Make sure we've setup this animation before starting it.
      EMK_Animation_Start(obj_id);
      is_running = true;
    }
    void Stop() {
      assert(obj_id >= 0); // Make sure we've setup this animation before stopping it.
      EMK_Animation_Stop(obj_id);
      is_running = false;
    }
  };


  //////////////////////////////////////////////////////////
  // Methods previously declared

  void Image::DoCallback(int * arg_ptr) { // Called back when image is loaded
    (void) arg_ptr;
    has_loaded = true;
    while (layers_waiting.size()) {
      Layer * cur_layer = layers_waiting.front();
      layers_waiting.pop_front();
      cur_layer->BatchDraw();
    }
  }

};


#endif
