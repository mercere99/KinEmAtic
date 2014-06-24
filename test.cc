#include <emscripten.h>
#include <iostream>
#include <cmath>
#include <unistd.h>

#include "Kinetic.h"

using namespace std;

class KineticExample {
private:
  emkStage stage;
  emkLayer layer;
  emkRect rect;
  emkRect rect2;
  emkImage image_test;

  emkAnimation<KineticExample> anim;


public:
  KineticExample()
    : stage(600, 600, "container")
    , rect(250, 250, 200, 200, "green", "black", 4)
    , rect2(350, 250, 200, 200, "red", "black", 4)
    , image_test("http://www.html5canvastutorials.com/demos/assets/darth-vader.jpg")
  {
    rect.SetOffset(100, 10);
    while (EMK_Image_AllLoaded() == false) sleep(200);
    rect.SetFillPatternImage(image_test);
    layer.Add(rect);
    stage.Add(layer);

    stage.On("contentMousemove", this, &KineticExample::DoRectRotation);
    stage.On("contentClick", this, &KineticExample::DoClick);
  }

  void DoRectRotation() {
    rect.DoRotate(2);
    layer.BatchDraw();
  }

  void DoClick() {
    
    layer.Add(rect2);
    layer.BatchDraw();
  }
};




extern "C" int emkMain()
{
  KineticExample * example = new KineticExample();
  return (int) example;
}
