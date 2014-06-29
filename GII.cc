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
  const string ranks[] = {"W", "L", "B", "M", "R", "D"};

  const int num_animals = 6;
  const int num_colors = 3;
  // const int num_base_ranks = 0;
  const int num_base_ranks = 5;
  const int num_ranks = 6;

  class Card {
  private:
    const emkImage & front;
    const emkImage & back;
    emkRect card_rect;
    const int animal;
    const int color;
    const int rank;
    const int id;
  public:
    Card(const emkImage & _front, emkImage & _back, int _a, int _c, int _r, int _id)
      : front(_front)
      , back(_back)
      , card_rect(0, 0, 150, 210, "white", "black", 1, true)
      , animal(_a), color(_c), rank(_r), id(_id)
    {
      card_rect.SetFillPatternImage(front); 
      card_rect.SetFillPatternScale(0.5);
      card_rect.SetLineJoin("round");
      card_rect.SetCornerRadius(10);
    }
    ~Card() { ; }

    emkRect & Rect() { return card_rect; }
    int GetAnimal() const { return animal; }
    int GetColor() const { return color; }
    int GetRank() const { return rank; }
    int GetID() const { return id; }
  
    void Rescale(double scale) {
      card_rect.SetWidth(150 * scale);
      card_rect.SetHeight(210 * scale);
      card_rect.SetFillPatternScale(0.5 * scale);
      card_rect.SetCornerRadius(10 * scale);
    }
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
        for (int rank_id = 0; rank_id < num_base_ranks; rank_id++) {
          // Load in the associated image.
          emkImage * cur_front = new emkImage(path + ranks[rank_id] + "_" + animals[animal_id] + ".png");
          for (int card_count = 0; card_count <= rank_id*2; card_count++) {
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
          Card * new_card = new Card(*cur_front, back_image, color_id*2, color_id, num_base_ranks, next_id++);
          card_array.push_back(new_card);
          front_images.push_back(cur_front);          
        }
      }
    }
    ~Deck() { 
      for (int i = 0; i < (int) card_array.size(); i++) delete card_array[i];
      for (int i = 0; i < (int) front_images.size(); i++) delete front_images[i];
    }

    int GetNumCards() const { return (int) card_array.size(); }
    Card & operator[](int index) { return *(card_array[index]); }
    const Card & operator[](int index) const { return *(card_array[index]); }
  };
}

class KineticExample {
private:
  emkStage stage;
  emkLayer layer;
  //  emkAnimation<KineticExample> anim;

  GII::Deck deck;

public:
  KineticExample()
    : stage(1200, 600, "container")
    , deck("/Users/charles/Sites/Puzzle Engine/GII/")
  {
    
    for (int i = 0; i < deck.GetNumCards(); i++) {
      emkRect & rect = deck[i].Rect();
      rect.SetXY(i*5+10, 100);
      deck[i].Rescale(0.2 + 0.01 * (double) i);
      layer.Add(rect);
    }


    stage.Add(layer);

    // rect.On("mousemove", this, &KineticExample::DoRectRotation);
    // rect.On("click", this, &KineticExample::DoClick);
    // rect2.On("click", this, &KineticExample::DoClick2);
  }

};




extern "C" int emkMain()
{
  KineticExample * example = new KineticExample();
  return (int) example;
}
