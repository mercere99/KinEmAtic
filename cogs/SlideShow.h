#ifndef EMK_SLIDESHOW_H
#define EMK_SLIDESHOW_H

#include <vector>

#include <html5.h>

#include "Control.h"

namespace emk {
  
  EM_BOOL slideshow_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData);

  class SlideShow;

  class SlideAction {
  private:
  public:
    SlideAction() { ; }
    virtual ~SlideAction() { ; }

    virtual void Trigger(SlideShow * show) = 0;
  };
  
  class SlideAction_Appear : public SlideAction {
  private:
    Shape * target;
    Image * image;
  public:
    SlideAction_Appear(Shape * _in) : target(_in), image(NULL) { ; }
    SlideAction_Appear(Shape * _in, Image * _image) : target(_in), image(_image) { ; }
    ~SlideAction_Appear() { ; }
    
    void Trigger(SlideShow * show);
  };
    
  class SlideAction_Appear_Image : public SlideAction {
  private:
    Image * image;
  public:
    SlideAction_Appear_Image(Image * _image) : image(_image) { ; }
    ~SlideAction_Appear_Image() { ; }
    
    void Trigger(SlideShow * show);
  };
    
  class SlideAction_Pause : public SlideAction {
  private:
  public:
    SlideAction_Pause() { ; }
    ~SlideAction_Pause() { ; }
    
    void Trigger(SlideShow * show);
  };
    
  class SlideAction_Clear : public SlideAction {
  private:
  public:
    SlideAction_Clear() { ; }
    ~SlideAction_Clear() { ; }
    
    void Trigger(SlideShow * show);
  };
    

  class SlideShow : public Control {
  private:
    emk::Layer & layer;
    emk::Animation<SlideShow> run_anim;

    std::vector<Object *> visible_set;

    std::vector<SlideAction *> action_list;

    int next_action;
    bool pause;

    int temp_id;

    std::string GetTempName() { return std::string("temp") + std::to_string(temp_id++); }
  public:
    SlideShow() 
      : layer(AddLayer("main_layer"))
      , run_anim(this, &SlideShow::Go, layer)
      , next_action(0)
      , pause(false)
      , temp_id(0)
    {
      Stage().Add(layer);
      emscripten_set_keypress_callback(0, this, 1, slideshow_callback);
    }
    ~SlideShow() { ; }

    inline int ScaleX(double x_frac) const { assert(cur_stage != NULL); return cur_stage->ScaleX(x_frac); }
    inline int ScaleY(double y_frac) const { assert(cur_stage != NULL); return cur_stage->ScaleY(y_frac); }
    
    void Go() {
      while (!pause && next_action < (int) action_list.size()) {
        action_list[next_action]->Trigger(this);
        next_action++;
      }

      if (next_action == (int) action_list.size()) {
        emk::Alert("Finished Actions!");
        Stop();
      }
    }

    void Start() { run_anim.Start(); }
    void Stop() { run_anim.Stop(); }

    // Setup pre-programmed actions!
    void Appear(Shape & shape) { action_list.push_back( new SlideAction_Appear(&shape) ); }
    void Appear(Shape & shape, emk::Image & image) { action_list.push_back( new SlideAction_Appear(&shape, &image) ); }
    void Appear(emk::Image & image) { action_list.push_back( new SlideAction_Appear_Image(&image) ); }
    void Appear(const std::string & name) {
      action_list.push_back( new SlideAction_Appear(shape_map[name]) );
      // @CAO need to make sure shape exists!
    }
    void Pause() { action_list.push_back( new SlideAction_Pause() ); }
    void Clear() { action_list.push_back( new SlideAction_Clear() ); }
    void NewSlide() { Pause(); Clear(); }

    SlideShow & operator<<(const std::string & msg) {
      // Build a text message on the screen using the default information.
      emk::Text & temp_text = AddText(GetTempName(), cur_point->GetX(), cur_point->GetY(), msg, *cur_font);
      Appear( temp_text );
      cur_point->TransX(temp_text.GetWidth());
      return *this;
    }

    SlideShow & operator<<(const emk::Font & font) {
      // Change the default font.
      AddFont( GetTempName(), font );
      return *this;
    }

    // And make the pre-programed actions happen!
    void DoPause() { pause = true; }
    void DoAdvance() { pause = false; }

    void DoAppear(Shape & shape)
    {
      shape.SetVisible(true);
      layer.Add(shape);
      layer.Draw();
      visible_set.push_back(&shape);
    }

    void DoAppear(emk::Image & image)
    {
      image.SetVisible(true);
      layer.Add(image);
      layer.Draw();
      visible_set.push_back(&image);
    }

    void DoClear() {
      // @CAO Remove all shapes from layer properly?
      for (int i = 0; i < (int) visible_set.size(); i++) {
        visible_set[i]->SetVisible(false);
      }
      visible_set.resize(0);
    }

  };


  EM_BOOL slideshow_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData)
  {                                                                                     
    if (eventType == EMSCRIPTEN_EVENT_KEYPRESS && (!strcmp(e->key, " "))) {
      ((SlideShow *) userData)->DoAdvance(); // Unpause the slide show.
    }                                                                                   
    
    return 0;                                                                           
  }


  void SlideAction_Appear::Trigger(SlideShow * show) {
    if (image) { target->SetFillPatternImage(*image); emk::Alert(image->GetWidth()); }
    show->DoAppear(*target);
  }
  void SlideAction_Appear_Image::Trigger(SlideShow * show) {
    show->DoAppear(*image);
  }
  void SlideAction_Pause::Trigger(SlideShow * show) { show->DoPause(); }
  void SlideAction_Clear::Trigger(SlideShow * show) { show->DoClear(); }
};

#endif
