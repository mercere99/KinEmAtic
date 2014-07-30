#ifndef EM_KINETIC_H
#define EM_KINETIC_H

#include <emscripten.h>

#include <list>
#include <string>
#include <vector>

#include "../tools/Callbacks.h"
#include "../tools/Colors.h"

#include "../tools/assert.h"

extern "C" {
  extern int EMK_Tween_Build(int target_id, double seconds);

  extern int EMK_Image_Load(const char * file, int callback_ptr);

  extern int EMK_Stage_Build(int _w, int _h, const char * _name);
  extern int EMK_Stage_ResizeMax(int stage_obj_id, int min_x, int min_y);

  extern int EMK_Layer_Build();

  extern int EMK_Image_Build(int _x, int _y, int img_id, int _w, int _h);

  extern int EMK_Rect_Build(int _x, int _y, int _w, int _h, const char * _fill, const char * _stroke, int _stroke_width, int _draggable);
  extern int EMK_RegularPolygon_Build(int _x, int _y, int _sides, int _radius,
                                      const char * _fill, const char * _stroke, int _stroke_width, int _draggable);

  extern int EMK_Animation_Build(int callback_ptr, int layer_id);
  extern int EMK_Animation_Build_NoFrame(int callback_ptr, int layer_id);

  extern void EMK_Shape_SetDrawFunction(int obj_id, int new_callback);

  extern int EMK_Custom_Shape_Build(int x, int y, int w, int h, int draw_callback);
}


namespace emk {

  // Pre-declarations of some classes...
  class Layer;
  class Stage;


  inline void Alert(const std::string & msg) { EM_ASM_ARGS({ msg = Pointer_stringify($0); alert(msg); }, msg.c_str()); }
  void Alert(int val) { Alert(std::to_string(val)); }
  void Alert(double val) { Alert(std::to_string(val)); }

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

    void SetX(int _in) { EM_ASM_ARGS({emk_info.objs[$0].x($1);}, obj_id, _in); }
    void SetY(int _in) { EM_ASM_ARGS({emk_info.objs[$0].y($1);}, obj_id, _in); }
    void SetWidth(int _in) { EM_ASM_ARGS({emk_info.objs[$0].width($1);}, obj_id, _in); }
    void SetHeight(int _in) { EM_ASM_ARGS({emk_info.objs[$0].height($1);}, obj_id, _in); }
    void SetVisible(int _in) { EM_ASM_ARGS({emk_info.objs[$0].visible($1);}, obj_id, _in); }
    void SetOpacity(double _in) { EM_ASM_ARGS({emk_info.objs[$0].opacity($1);}, obj_id, _in); }
    void SetListening(int _in) { EM_ASM_ARGS({emk_info.objs[$0].listening($1);}, obj_id, _in); }
    void SetScaleX(double _in) { EM_ASM_ARGS({emk_info.objs[$0].scaleX($1);}, obj_id, _in); }
    void SetScaleY(double _in) { EM_ASM_ARGS({emk_info.objs[$0].scaleY($1);}, obj_id, _in); }    
    void SetOffsetX(int _in) { EM_ASM_ARGS({emk_info.objs[$0].offsetX($1);}, obj_id, _in); }
    void SetOffsetY(int _in) { EM_ASM_ARGS({emk_info.objs[$0].offsetY($1);}, obj_id, _in); }
    void SetRotation(int _in) { EM_ASM_ARGS({emk_info.objs[$0].rotation($1);}, obj_id, _in); }
    void SetDraggable(int _in) { EM_ASM_ARGS({emk_info.objs[$0].draggable($1);}, obj_id, _in); }

    inline void SetXY(int x, int y) { SetX(x); SetY(y); }
    inline void SetSize(int w, int h) { SetWidth(w); SetHeight(h); }
    inline void SetLayout(int x, int y, int w, int h) { SetX(x); SetY(y); SetWidth(w); SetHeight(h); }
    inline void SetScale(double _x, double _y) { SetScaleX(_x); SetScaleY(_y); }
    inline void SetScale(double _in) { SetScaleX(_in); SetScaleY(_in); }
    inline void SetOffset(int _x, int _y) { SetOffsetX(_x); SetOffsetY(_y); }

    void SetLayer(Object * _layer) { layer = _layer; }

    // Draw either this object or objects in contains.
    void Draw() { EM_ASM_ARGS({emk_info.objs[$0].draw();}, obj_id); }

    // Draw all objects in this layer.
    void DrawLayer() { if (layer) layer->Draw(); }

    // Move this object to the top of the current layer.
    void MoveToTop() { EM_ASM_ARGS({emk_info.objs[$0].moveToTop();}, obj_id); }


    template<class T> void On(const std::string & _trigger, T * _target, void (T::*_method_ptr)());
    template<class T> void On(const std::string & _trigger, T * _target, void (T::*_method_ptr)(const EventInfo &));
  };


  // This is a baseclass for other classes that contain a whole set of objects
  // @CAO Should this be an object itself?
  class ObjectSet {
  public:
    virtual void AddToLayer(Layer & layer) = 0;  // Add all objects in set to this layer.
  };


  class Tween : public Object {
  private:
    Object * target;   // What object is this tween associated with?
    double seconds;    // How long should this tween run for?

    int settings_id;   // JS memory position where the tween settings should go.
    bool needs_config; // Does the Tween need to be reconfigured?

    void ConfigureTween() {
      EM_ASM_ARGS({ emk_info.objs[$1] = new Kinetic.Tween( emk_info.objs[$0] ); }, settings_id, obj_id);
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
    Tween & SetX(int _in) { EM_ASM_ARGS({ emk_info.objs[$0].x = $1; }, settings_id, _in); needs_config=true; return *this; }
    Tween & SetY(int _in) { EM_ASM_ARGS({ emk_info.objs[$0].y = $1; }, settings_id, _in); needs_config=true; return *this; }
    Tween & SetScaleX(int _in) { EM_ASM_ARGS({ emk_info.objs[$0].scaleX = $1; }, settings_id, _in); needs_config=true; return *this; }
    Tween & SetScaleY(int _in) { EM_ASM_ARGS({ emk_info.objs[$0].scaleY = $1; }, settings_id, _in); needs_config=true; return *this; }

    Tween & SetXY(int _x, int _y) { SetX(_x); SetY(_y); return *this; }
    Tween & SetScaleXY(double _x, double _y) { SetScaleX(_x); SetScaleY(_y); return *this; }

    Tween & SetFinishedCallback(Callback * callback, int * info_ptr) {
      EM_ASM_ARGS({
          emk_info.objs[$0].onFinish = function() { emkJSDoCallback($1, $2); };
        }, settings_id, (int) (callback), (int) info_ptr);
      return *this;
    }

    void Play() {
      if (needs_config) ConfigureTween();
      EM_ASM_ARGS({ emk_info.objs[$0].play(); }, obj_id);
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

    static bool AllLoaded() { return EM_ASM_INT_V({return (emk_info.images.length == emk_info.image_load_count);}); }
  };


  // Manual control over the canvas...  For the moment, we assume that a context has been placed in emk_info.ctx
  class Canvas {
  public:
    // Setting values
    inline static void SetFillStyle(const Color & color) { 
      EM_ASM_ARGS({var fs = Pointer_stringify($0); emk_info.ctx.fillStyle = fs;}, color.AsString().c_str());
    }

    inline static void SetStroke(const Color & color) {
      EM_ASM_ARGS({var stroke = Pointer_stringify($0); emk_info.ctx.strokeStyle = stroke;}, color.AsString().c_str());
    }

    inline static void SetLineWidth(double width) { EM_ASM_ARGS({emk_info.ctx.lineWidth = $0;}, width); }

    inline static void SetLineJoin(const std::string & lj) {
      EM_ASM_ARGS({var lj = Pointer_stringify($0); emk_info.ctx.lineJoin = lj;}, lj.c_str());
    }

    inline static void SetFont(const std::string & font) {
      EM_ASM_ARGS({var font = Pointer_stringify($0); emk_info.ctx.font = font;}, font.c_str());
    }

    inline static void SetTextAlign(const std::string & align) {
      EM_ASM_ARGS({var align = Pointer_stringify($0); emk_info.ctx.textAlign = align;}, align.c_str());
    }

    inline static void SetShadowColor(const Color & color) {
      EM_ASM_ARGS({var color = Pointer_stringify($0); emk_info.ctx.shadowColor = color;}, color.AsString().c_str());
    }
    inline static void SetShadowBlur(int _in) { EM_ASM_ARGS({emk_info.ctx.shadowBlur = $0;}, _in); }
    inline static void SetShadowOffsetX(int _in) { EM_ASM_ARGS({emk_info.ctx.shadowOffsetX = $0;}, _in); }
    inline static void SetShadowOffsetY(int _in) { EM_ASM_ARGS({emk_info.ctx.shadowOffsetY = $0;}, _in); }

    // Shapes and Text
    inline static void Text(const std::string & msg, int x, int y, bool fill=true) {
      if (fill) EM_ASM_ARGS({var msg = Pointer_stringify($0); emk_info.ctx.fillText(msg, $1, $2);}, msg.c_str(), x, y);
      else EM_ASM_ARGS({var msg = Pointer_stringify($0); emk_info.ctx.strokeText(msg, $1, $2);}, msg.c_str(), x, y);
    }

    inline static void Rect(int x, int y, int width, int height, bool fill=false) {
      if (fill) EM_ASM_ARGS({emk_info.ctx.fillRect($0, $1, $2, $3);}, x, y, width, height);
      else EM_ASM_ARGS({emk_info.ctx.strokeRect($0, $1, $2, $3);}, x, y, width, height);
    }

    inline static void Arc(int x, int y, int radius, double start, double end, bool cclockwise=false) {
      EM_ASM_ARGS({emk_info.ctx.arc($0, $1, $2, $3, $4, $5);}, x, y, radius, start, end, cclockwise);
    }

    inline static void DrawImage(const Image & image, int x, int y) {
      // @CAO Do something different if the image hasn't loaded yet?  Maybe draw a placeholder rectangle?
      EM_ASM_ARGS({emk_info.ctx.drawImage(emk_info.objs[$0], $1, $2);}, image.GetID(), x, y);
    }

    inline static void DrawImage(const Image & image, int x, int y, int w, int h) {
      EM_ASM_ARGS({emk_info.ctx.drawImage(emk_info.objs[$0], $1, $2, $3, $4);}, image.GetID(), x, y, w, h);
    }

    // Paths
    inline static void BeginPath() { EM_ASM( emk_info.ctx.beginPath() ); }
    inline static void ClosePath() { EM_ASM( emk_info.ctx.closePath() ); }
    inline static void Fill() { EM_ASM( emk_info.ctx.fill() ); }
    inline static void LineTo(int x, int y) { EM_ASM_ARGS({ emk_info.ctx.lineTo($0, $1); }, x, y); }
    inline static void MoveTo(int x, int y) { EM_ASM_ARGS({ emk_info.ctx.moveTo($0, $1); }, x, y); }

    // Transformations
    inline static void Restore() { EM_ASM( emk_info.ctx.restore() ); }
    inline static void Save() { EM_ASM( emk_info.ctx.save() ); }
    inline static void Scale(double x, double y) { EM_ASM_ARGS({ emk_info.ctx.scale($0, $1); }, x, y); }
    inline static void Scale(double new_scale) { EM_ASM_ARGS({ emk_info.ctx.scale($0, $0); }, new_scale); }
    inline static void Translate(int x, int y) { EM_ASM_ARGS({ emk_info.ctx.translate($0, $1); }, x, y); }
    inline static void Rotate(double angle) { EM_ASM_ARGS({ emk_info.ctx.rotate($0); }, angle); }
 

    // Finsihing
    inline static void Stroke() { EM_ASM( emk_info.ctx.stroke() ); }
    inline static void SetupTarget(const Object & obj) {
      EM_ASM_ARGS({emk_info.canvas.fillStrokeShape(emk_info.objs[$0])}, obj.GetID());
    }
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
      EM_ASM_ARGS({
        emk_info.objs[$0].setFillPriority('pattern');
        emk_info.objs[$0].setFillPatternImage(emk_info.objs[$1]);
      }, obj_id, image->GetID());
    }


    void SetFill(const Color & color) {
      EM_ASM_ARGS({var fill = Pointer_stringify($1); emk_info.objs[$0].fill(fill);}, obj_id, color.AsString().c_str());
    }

    int GetFillPatternX() const { return EM_ASM_INT({return emk_info.objs[$0].fillPatternX();}, obj_id); }
    void SetFillPatternX(int _in) { EM_ASM_ARGS({emk_info.objs[$0].fillPatternX($1);}, obj_id, _in); }

    int GetFillPatternY() const { return EM_ASM_INT({return emk_info.objs[$0].fillPatternY();}, obj_id); }
    void SetFillPatternY(int _in) { EM_ASM_ARGS({emk_info.objs[$0].fillPatternY($1);}, obj_id, _in); }

    void SetFillPatternXY(int _x, int _y) { SetFillPatternX(_x); SetFillPatternY(_y); }


    int GetFillPatternOffsetX() const { return EM_ASM_INT({return emk_info.objs[$0].fillPatternOffsetX();}, obj_id); }
    void SetFillPatternOffsetX(int _in) { EM_ASM_ARGS({emk_info.objs[$0].fillPatternOffsetX($1);}, obj_id, _in); }

    int GetFillPatternOffsetY() const { return EM_ASM_INT({return emk_info.objs[$0].fillPatternOffsetY();}, obj_id); }
    void SetFillPatternOffsetY(int _in) { EM_ASM_ARGS({emk_info.objs[$0].fillPatternOffsetY($1);}, obj_id, _in); }

    void SetFillPatternOffsetXY(int _x, int _y) { SetFillPatternOffsetX(_x); SetFillPatternOffsetY(_y); }


    double GetFillPatternScaleX() const { return EM_ASM_DOUBLE({return emk_info.objs[$0].fillPatternScaleX();}, obj_id); }
    void SetFillPatternScaleX(double _in) { EM_ASM_ARGS({emk_info.objs[$0].fillPatternScaleX($1);}, obj_id, _in); }

    double GetFillPatternScaleY() const { return EM_ASM_DOUBLE({return emk_info.objs[$0].fillPatternScaleY();}, obj_id); }
    void SetFillPatternScaleY(double _in) { EM_ASM_ARGS({emk_info.objs[$0].fillPatternScaleY($1);}, obj_id, _in); }

    void SetFillPatternScale(double _x, double _y) { SetFillPatternScaleX(_x); SetFillPatternScaleY(_y); }
    void SetFillPatternScale(double scale) { SetFillPatternScaleX(scale); SetFillPatternScaleY(scale); }


    double GetFillPatternRotation() const { return EM_ASM_DOUBLE({return emk_info.objs[$0].fillPatternRotation();}, obj_id); }
    void SetFillPatternRotation(double _in) { EM_ASM_ARGS({emk_info.objs[$0].fillPatternRotation($1);}, obj_id, _in); }


    void SetFillPatternRepeat(int _in) { EM_ASM_ARGS({emk_info.objs[$0].fillPatternRepeat($1);}, obj_id, _in); }


    bool GetFillEnabled() const { return EM_ASM_INT({return emk_info.objs[$0].fillEnabled();}, obj_id); }
    void SetFillEnabled(bool _in) { EM_ASM_ARGS({emk_info.objs[$0].fillEnabled($1);}, obj_id, (int) _in); }


    void SetFillPriority(const std::string & _in) {
      EM_ASM_ARGS({var priority = Pointer_stringify($1); emk_info.objs[$0].fillPriority(priority);}, obj_id, (int) _in.c_str());
    }


    void SetStroke(const std::string & _in) {
      EM_ASM_ARGS({var stroke = Pointer_stringify($1); emk_info.objs[$0].stroke(stroke);}, obj_id, _in.c_str());
    }

    int GetStrokeWidth() const { return EM_ASM_INT({return emk_info.objs[$0].strokeWidth();}, obj_id); }
    void SetStrokeWidth(int _in) { EM_ASM_ARGS({emk_info.objs[$0].strokeWidth($1);}, obj_id, _in); }

    bool GetStrokeScaleEnabled() const { return EM_ASM_INT({return emk_info.objs[$0].strokeScaleEnabled();}, obj_id); }
    void SetStrokeScaleEnabled(bool _in) { EM_ASM_ARGS({emk_info.objs[$0].strokeScaleEnabled($1);}, obj_id, (int) _in); }

    bool GetStrokeEnabled() const { return EM_ASM_INT({return emk_info.objs[$0].strokeEnabled();}, obj_id); }
    void SetStrokeEnabled(bool _in) { EM_ASM_ARGS({emk_info.objs[$0].strokeEnabled($1);}, obj_id, (int) _in); }


    void SetLineJoin(const std::string & _in) {
      EM_ASM_ARGS({var lj = Pointer_stringify($1); emk_info.objs[$0].lineJoin(lj);}, obj_id, _in.c_str());
    }
    void SetLineCap(const std::string & _in) {
      EM_ASM_ARGS({var lc = Pointer_stringify($1); emk_info.objs[$0].lineCap(lc);}, obj_id, _in.c_str());
    }

    void SetShadowColor(const Color & _in) {
      EM_ASM_ARGS({var sc = Pointer_stringify($1); emk_info.objs[$0].shadowColor($1);}, obj_id, _in.AsString().c_str());
    }

    double GetShadowBlur() const { return EM_ASM_DOUBLE({return emk_info.objs[$0].shadowBlur();}, obj_id); }
    void SetShadowBlur(double _in) { EM_ASM_ARGS({emk_info.objs[$0].shadowBlur($1);}, obj_id, _in); }


    int GetShadowOffsetX() const { return EM_ASM_INT({return emk_info.objs[$0].shadowOffsetX();}, obj_id); }
    void SetShadowOffsetX(int _in) { EM_ASM_ARGS({emk_info.objs[$0].shadowOffsetX($1);}, obj_id, _in); }

    int GetShadowOffsetY() const { return EM_ASM_INT({return emk_info.objs[$0].shadowOffsetY();}, obj_id); }
    void SetShadowOffsetY(int _in) { EM_ASM_ARGS({emk_info.objs[$0].shadowOffsetY($1);}, obj_id, _in); }

    void SetShadowOffset(int _x, int _y) { SetShadowOffsetX(_x); SetShadowOffsetY(_y); }


    double GetShadowOpacity() const { return EM_ASM_DOUBLE({return emk_info.objs[$0].shadowOpacity();}, obj_id); }
    void SetShadowOpacity(double _in) { EM_ASM_ARGS({emk_info.objs[$0].shadowOpacity($1);}, obj_id, _in); }

    bool GetShadowEnabled() const { return EM_ASM_INT({return emk_info.objs[$0].shadowEnabled();}, obj_id); }
    void SetShadowEnabled(bool _in) { EM_ASM_ARGS({emk_info.objs[$0].shadowEnabled($1);}, obj_id, (int) _in); }


    // @CAO -- not the proper place for this??
    inline void SetCornerRadius(int radius) { EM_ASM_ARGS({emk_info.objs[$0].cornerRadius($1);}, obj_id, radius); }


    // Override the drawing of this shape.
    template<class T> void SetDrawFunction(T * target, void (T::*draw_ptr)(Canvas &) ) {
      if (draw_callback != NULL) delete draw_callback;
      draw_callback = new Callback_Canvas<T>(target, draw_ptr);
      EMK_Shape_SetDrawFunction(obj_id, (int) draw_callback);
    }

    void DoRotate(double rot) { EM_ASM_ARGS({emk_info.objs[$0].rotate($1);}, obj_id, rot); }

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
      EM_ASM_ARGS({emk_info.objs[$0].add(emk_info.objs[$1]);}, obj_id, _obj.GetID());
      return *this;
    }

    Layer & Add(ObjectSet & set) { set.AddToLayer(*this); return *this; }

    void Draw() { EM_ASM_ARGS({emk_info.objs[$0].draw();}, obj_id); }
    void BatchDraw() { EM_ASM_ARGS({emk_info.objs[$0].batchDraw();}, obj_id); }
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
      EM_ASM_ARGS({emk_info.objs[$0].add(emk_info.objs[$1]);}, obj_id, _layer.GetID());
      return *this;
    }
  };


  // The text object from Kinetic...
  class Text : public Shape {
  public:
    Text(int x=0, int y=0, std::string text="", int font_size=30, std::string font_family="Calibri", std::string fill="black")
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
      }, x, y, text.c_str(), std::to_string(font_size).c_str(), font_family.c_str(), fill.c_str());
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
    Animation(T * _target, void (T::*_method_ptr)(), Layer & layer) : target(_target), method_ptr(NULL), method_ptr_nf(_method_ptr) {
      obj_id = EMK_Animation_Build_NoFrame((int) this, layer.GetID());
    }
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
      EM_ASM_ARGS({emk_info.objs[$0].start();}, obj_id);
      is_running = true;
    }
    void Stop() {
      assert(obj_id >= 0); // Make sure we've setup this animation before stopping it.
      EM_ASM_ARGS({emk_info.objs[$0].stop();}, obj_id);
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
