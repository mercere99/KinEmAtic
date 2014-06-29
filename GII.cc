#include <emscripten.h>
#include <iostream>
#include <string>
#include <vector>

#include "Kinetic.h"

using namespace std;

namespace GII
{
  const string animals[] = {"hare", "wren", "horse", "owl", "wolf", "hawk"};
  const string colors[] = {"blue", "green", "red"};
  const string ranks[] = {"W", "L", "B", "M", "R"};

  const int num_animals = 6;
  const int num_colors = 3;
  const int num_ranks = 5;

  class Card {
  private:
    const emkImage & front;
    const emkImage & back;
    int x;
    int y;
    const int animal;
    const int color;
    const int rank;
    const int id;
  public:
    Card(const emkImage & _front, emkImage & _back, int _a, int _c, int _r, int _id)
      : front(_front), back(_back), animal(_a), color(_c), rank(_r), id(_id) { ; }
    ~Card() { ; }

    int GetAnimal() const { return animal; }
    int GetColor() const { return color; }
    int GetRank() const { return rank; }
    int GetID() const { return id; }
  };
  
  class Deck {
    string path;
    emkImage back_image;
    vector<emkImage *> front_images;
    vector<Card *> card_array;
    int next_id;
  public:
    Deck(string _path, bool include_dragons=true) 
      : path(_path), back_image(path + "Back.png"), next_id(0)
    {
      for (int animal_id = 0; animal_id < num_animals; animal_id++) {
        for (int rank_id = 0; rank_id < num_ranks; rank_id++) {
          // Load in the associated image.
          emkImage * cur_front = new emkImage(path + ranks[rank_id] + "_" + animals[animal_id] + ".png");
          for (int card_count = 0; card_count <= rank_id; card_count++) {
            Card * new_card = new Card(*cur_front, back_image, animal_id, animal_id/2, rank_id, next_id++);
            card_array.push_back(new_card);
          }
          front_images.push_back(cur_front);
        }
      }

      if (include_dragons) {
        for (int color_id = 0; color_id < num_colors; color_id++) {
          // Load in the associated image.
          emkImage * cur_front = new emkImage(path + "D_" + colors[color_id] + ".png");
          Card * new_card = new Card(*cur_front, back_image, animal_id, animal_id/2, rank_id, next_id++);
          card_array.push_back(new_card);
          front_images.push_back(cur_front);          
        }
      }
    }
    ~Deck() { 
      for (int i = 0; i < (int) card_array.size(); i++) delete card_array[i];
      for (int i = 0; i < (int) front_images.size(); i++) delete front_images[i];
    }
  };
}

class KineticExample {
private:
  emkStage stage;
  emkLayer layer;
  //  emkAnimation<KineticExample> anim;


public:
  KineticExample()
    : stage(600, 600, "container")
  {
    rect.SetOffset(100, 10);
    rect.SetFillPatternImage(image_test);
    rect2.SetFillPatternImage(image_test);
    layer.Add(rect);
    layer.Add(rect2);
    layer.Add(poly);
    layer.Add(text);
    stage.Add(layer);

    rect.On("mousemove", this, &KineticExample::DoRectRotation);
    rect.On("click", this, &KineticExample::DoClick);
    rect2.On("click", this, &KineticExample::DoClick2);
  }

  void DoClick() {
    poly.SetScale(1.0,1.0);
    layer.BatchDraw();
  }

  void DoClick2() {
    poly.SetScale(2.0,2.0);
    layer.BatchDraw();
  }

  void DoRectRotation() {
    rect.DoRotate(2);
    layer.BatchDraw();
  }

  void DoRectScale() {
    rect.SetScale(0.5, 0.5);
  }
};




extern "C" int emkMain()
{
  string path("/Users/charles/Sites/Puzzle Engine/GII/");

  KineticExample * example = new KineticExample();
  return (int) example;
}
