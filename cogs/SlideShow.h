#ifndef EMK_SLIDESHOW_H
#define EMK_SLIDESHOW_H

#include <vector>

#include <html5.h>

#include "Control.h"
#include "../cogs/ProgressBar.h"

namespace emk {
  
  EM_BOOL slideshow_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData);

  class SlideShow;

  class SlideAction {
  private:
  public:
    SlideAction() { ; }
    virtual ~SlideAction() { ; }

    virtual void Trigger(SlideShow * show) = 0;
    virtual void Reverse(SlideShow * show) = 0;
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
    void Reverse(SlideShow * show);
  };
    
  class SlideAction_Appear_Image : public SlideAction {
  private:
    Image * image;
  public:
    SlideAction_Appear_Image(Image * _image) : image(_image) { ; }
    ~SlideAction_Appear_Image() { ; }
    
    void Trigger(SlideShow * show);
    void Reverse(SlideShow * show);
  };
    
  class SlideAction_NextSlide : public SlideAction {
  private:
  public:
    SlideAction_NextSlide() { ; }
    ~SlideAction_NextSlide() { ; }
    
    void Trigger(SlideShow * show);
    void Reverse(SlideShow * show);
  };

  class SlideAction_Tween : public SlideAction {
  private:
    emk::Tween * tween;
  public:
    SlideAction_Tween(emk::Tween * _tween) : tween(_tween) { ; }
    ~SlideAction_Tween() { ; }
    
    void Trigger(SlideShow * show);
    void Reverse(SlideShow * show);
  };
  

  class SlideAction_Pause : public SlideAction {
  private:
  public:
    SlideAction_Pause() { ; }
    ~SlideAction_Pause() { ; }
    
    void Trigger(SlideShow * show);
    void Reverse(SlideShow * show);
  };
    
  class SlideAction_Clear : public SlideAction {
  private:
    std::vector<Object *> cleared_set;
  public:
    SlideAction_Clear() { ; }
    ~SlideAction_Clear() { ; }
    
    void Trigger(SlideShow * show);
    void Reverse(SlideShow * show);
  };
    

  class SlideShow : public Control {
  private:
    emk::Layer & layer;
    emk::Animation<SlideShow> run_anim;

    // Variables to track while the show is being created.
    std::vector<SlideAction *> action_list;  // List of all actions to be taken in the show.
    std::vector<Shape *> bg_shapes;          // Shapes to put on the background of each slide. // @CAO should also handle Images
    int slide_id;                            // Current slide being built
    int temp_id;                             // ID to be used next for constructing temporary shapes and images

    // Variable to track while the show is running.
    std::vector<Object *> visible_set;       // Set of all objects currently visible in the show.
    int next_action;                         // What show action needs to be taken next?
    int active_slide_id;                     // What slide are we actively running?
    bool pause;                              // Is the show currently running or paused?

    // Configuration information.
    int display_image_load;  // Should we indicate progress on loading images at show start? 0=no, 1=yes, 2=already in progress.
    emk::ProgressBar progress_image_load;

    emk::Font title_font;   // What font should the title be in?
    int title_y;            // How far down should the title be?  // @CAO assuming cenetered, but should give control over that.

    // Private methods
    std::string GetTempName() { return std::string("temp") + std::to_string(temp_id++); }

  public:
    SlideShow() 
      : layer(BuildLayer("main_layer"))
      , run_anim(this, &SlideShow::Go, layer)
      , slide_id(0)
      , temp_id(0)
      , next_action(0)
      , active_slide_id(0)
      , pause(false)
      , display_image_load(1)
      , progress_image_load(Stage().GetWidth()/2-200, Stage().GetHeight()/2-60, 400, 120)
      , title_font(Stage().GetWidth()/20)
      , title_y(ScaleY(0.08))
    {
      SetAspect(1.778);
      progress_image_load.SetMessage("Images Loaded: ");
      Stage().Add(layer);
      emscripten_set_keypress_callback(0, this, 1, slideshow_callback);
    }
    ~SlideShow() { ; }

    inline double ScaleX(double x_frac) const {
      assert(cur_stage != NULL);
      assert(x_frac >= 0.0 && x_frac <= 1.0);
      return cur_stage->ScaleX(x_frac);
    }
    inline double ScaleY(double y_frac) const {
      assert(cur_stage != NULL);
      assert(y_frac >= 0.0 && y_frac <= 1.0);
      return cur_stage->ScaleY(y_frac);
    }
    inline emk::Point ScaleXY(double x_frac, double y_frac) { return emk::Point( ScaleX(x_frac), ScaleY(y_frac) ); }
    
    // Setup basic parentheses usage for scaling to new positions...
    inline double operator()(double x_frac) { return ScaleX(x_frac); }
    inline emk::Point operator()(double x_frac, double y_frac) { return emk::Point( ScaleX(x_frac), ScaleY(y_frac) ); }

    void Go() {
      // Make sure all images are loaded before we start the show...
      if (emk::Image::AllLoaded() == false) {
        progress_image_load.SetMaxCount(emk::Image::NumImages());
        progress_image_load.SetCurCount(emk::Image::NumLoaded());

        // If this is our first time through, setup image loading progress bar.
        if (display_image_load == 1) {
          Layer().Add(progress_image_load);
          display_image_load = 2;
        }

        // Otherwise just draw the updated progress bar.
        else progress_image_load.DrawLayer();

        return;
      }
      
      else if (display_image_load == 2) {
        progress_image_load.SetMaxCount(emk::Image::NumImages());
        progress_image_load.SetCurCount(emk::Image::NumLoaded());

        static int countdown = 50;
        if (countdown > 0) {
          countdown--;
          return;
        }

        Layer().Remove(progress_image_load);
        display_image_load = 0;
      }

      // Run through actions until we hit a pause.  Unpause occurs by users hitting a key.
      while (!pause && next_action < (int) action_list.size()) {
        action_list[next_action]->Trigger(this);
        next_action++;
      }

      // If we are done with the entire show, stop running this method.
      if (next_action == (int) action_list.size()) {
        emk::Alert("Finished Actions!");
        Stop();
      }
    }

    void Start() { run_anim.Start(); }
    void Stop() { run_anim.Stop(); }

    // Setup pre-programmed actions!  All of the objects used in Appear must be created and managed from elsewhere.
    // See operator<< methods below if you want a copy to be created and managed inside this class.
    void Appear(Shape & shape) { action_list.push_back( new SlideAction_Appear(&shape) ); }
    void Appear(Shape & shape, emk::Image & image) { action_list.push_back( new SlideAction_Appear(&shape, &image) ); }
    void Appear(emk::Image & image) { action_list.push_back( new SlideAction_Appear_Image(&image) ); }
    void Appear(const std::string & name) {
      action_list.push_back( new SlideAction_Appear(shape_map[name]) );
      // @CAO need to make sure shape exists!
    }
    void SetAction(emk::Tween & tween) { action_list.push_back( new SlideAction_Tween(&tween) ); }
    void Pause() { action_list.push_back( new SlideAction_Pause() ); }
    void Clear() { action_list.push_back( new SlideAction_Clear() ); }
 
    // What should we do when we create a new slide?
    void NewSlide(const std::string & title="", bool include_bg=true) {
      if (slide_id++ > 0) Pause();                           // Pause between slides, but only after the first.
      action_list.push_back( new SlideAction_NextSlide() );  // Track that this is where the new slide officially starts.
      Clear();                                               // Start each new slide with a clear screen.
      
      // Unless it is being surpressed, we should include all background objects on this slide.
      if (include_bg) {
        for (int i = 0; i < (int) bg_shapes.size(); i++) Appear(*(bg_shapes[i]));
      }

      // If a title is provided, include it centered on the screen. // @CAO eventually include proper alignment cues.
      if (title.size() > 0) {
        std::string title_name = std::string("slide") + std::to_string(slide_id) + std::string("_title");
        emk::Text & title_text = BuildText(title_name, emk::Point(0,0), title, title_font);
        int title_x = (Stage().GetWidth() - title_text.GetWidth()) / 2;
        title_text.SetXY(title_x, title_y);
        Appear(title_text);
      }
    }

    void AddBackground(Shape & new_bg) {
      bg_shapes.push_back(&new_bg);
    }

    //  ---- Setup building of slides using << operators  ----

    SlideShow & operator<<(const std::string & msg) {
      // Build a text message on the screen using the default information.
      emk::Text & temp_text = BuildText(GetTempName(), default_point, msg, default_font);
      Appear( temp_text );                          // Schedule the text to appear on the screen.
      default_point.TransX(temp_text.GetWidth());   // Start the next text after this.
      return *this;
    }

    SlideShow & operator<<(const char * msg) { return this->operator<<(std::string(msg)); }

    SlideShow & operator<<(const emk::Tween & tween) {
      cur_tween = new emk::Tween(tween);                // Build a copy of the input tween.
      if (tween.GetName() != "") {                      // If the tween has a name, save it to that name.
        const std::string & name = tween.GetName();
        if (tween_map.find(name) != tween_map.end()) {
          delete tween_map[name];                       // If we a replacing a tween, delete the old one.
        }
        tween_map[name] = cur_tween;
      } else {                                          // Otherwise treat it as temporary.
        ManageTemp(cur_tween);                           // Keep track of the copy (to eventually delete)
      }
      SetAction(*cur_tween);                           // Schedule the tween to activate.
      return *this;
    }

    SlideShow & operator<<(const emk::Image & image) {
      cur_image = new emk::Image(image);                // Build a copy of the input image.
      if (image.GetName() != "") {                      // If the image has a name, save it to that name.
        const std::string & name = image.GetName();
        if (image_map.find(name) != image_map.end()) {
          delete image_map[name];                       // If we a replacing a image, delete the old one.
        }
        image_map[name] = cur_image;
      } else {                                          // Otherwise treat it as temporary.
        ManageTemp(cur_image);                         // Keep track of the copy (to eventually delete)
      }
      Appear(*cur_image);                              // Schedule the image to activate.
      return *this;
    }


    /* CAO CLEANUP! */ SlideShow & operator<<(emk::Shape & shape) { Appear(shape); return *this; }
    /* CAO CLEANUP! */ SlideShow & operator<<(emk::Image & image) { Appear(image); return *this; }

    SlideShow & operator<<(const emk::Font & font) {
      // Change the default font.
      default_font = font;
      return *this;
    }

    SlideShow & operator<<(const emk::Point & point) {
      // Change the default position.
      default_point = point;
      return *this;
    }

    SlideShow & operator<<(const emk::Color & color) {
      // Change the default color
      default_color = color;
      default_font.SetColor(color);
      return *this;
    }
    

    // And make the pre-programed actions happen!
    void DoPause() { pause = true; }
    void DoAdvance() { pause = false; }

    void DoRewind() {
      pause = false;
      next_action--;
      while (!pause && next_action-- > 0) {
        action_list[next_action]->Reverse(this);
      }
      next_action++;
      pause = true;
    }

    void DoAppear(Shape & shape)
    {
      shape.SetVisible(true);
      if (shape.GetLayer() != &layer) layer.Add(shape);
      layer.Draw();
      visible_set.push_back(&shape);
    }

    void DoDisappear(Shape & shape)
    {
      shape.SetVisible(false);
      // layer.Remove(shape);   // @CAO Make work!!
      layer.Draw();
      // cur_stage->Draw();
      assert(visible_set.back() == &shape);
      visible_set.pop_back();
    }

    void DoAppear(emk::Image & image)
    {
      image.SetVisible(true);
      if (image.GetLayer() != &layer) layer.Add(image);
      layer.Draw();
      visible_set.push_back(&image);
    }

    void DoDisappear(emk::Image & image)
    {
      image.SetVisible(false);
      // layer.Remove(image);  // @CAO Make work!!!
      layer.Draw();
      assert(visible_set.back() == &image);
      visible_set.pop_back();
    }

    void DoNextSlide() { active_slide_id++; }
    void DoPrevSlide() { active_slide_id--; }

    void DoTween(emk::Tween & tween) { tween.Play(); }
    void DoReverseTween(emk::Tween & tween) { tween.Reverse(); }

    void DoClear(std::vector<Object *> & cleared_set) {
      // @CAO Remove all shapes from layer properly?
      for (int i = 0; i < (int) visible_set.size(); i++) {
        visible_set[i]->SetVisible(false);
      }
      cleared_set = visible_set;
      visible_set.resize(0);
    }

    void DoRestore(std::vector<Object *> & cleared_set) {
      for (int i = 0; i < (int) cleared_set.size(); i++) {
        visible_set[i]->SetVisible(true);
      }
      visible_set = cleared_set;
      cleared_set.resize(0);
    }
  };


  EM_BOOL slideshow_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData)
  {                                                                                     
    // Handle Keypresses...
    if (eventType == EMSCRIPTEN_EVENT_KEYPRESS) {
      if (!strcmp(e->key, " ")) {
        ((SlideShow *) userData)->DoAdvance(); // Unpause the slide show.
        return 1;
      }                                                                                   
    
      if (!strcmp(e->key, "p")) {
        ((SlideShow *) userData)->DoRewind(); // Rewind the slide show.
        return 1;
      }

      // emk::Alert(std::string("::") + std::string(e->key) + std::string("::"));
    }                                                                       
    
    return 0;                                                                           
  }


  void SlideAction_Appear::Trigger(SlideShow * show) { show->DoAppear(*target); }
  void SlideAction_Appear::Reverse(SlideShow * show) { show->DoDisappear(*target); }

  void SlideAction_Appear_Image::Trigger(SlideShow * show) { show->DoAppear(*image); }
  void SlideAction_Appear_Image::Reverse(SlideShow * show) { show->DoDisappear(*image); }

  void SlideAction_NextSlide::Trigger(SlideShow * show) { show->DoNextSlide(); }
  void SlideAction_NextSlide::Reverse(SlideShow * show) { show->DoPrevSlide(); }

  void SlideAction_Tween::Trigger(SlideShow * show) { show->DoTween(*tween); }
  void SlideAction_Tween::Reverse(SlideShow * show) { show->DoReverseTween(*tween); }

  void SlideAction_Pause::Trigger(SlideShow * show) { show->DoPause(); }
  void SlideAction_Pause::Reverse(SlideShow * show) { show->DoPause(); }

  void SlideAction_Clear::Trigger(SlideShow * show) { show->DoClear(cleared_set); }
  void SlideAction_Clear::Reverse(SlideShow * show) { show->DoRestore(cleared_set); }
};

#endif
