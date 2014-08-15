#include <emscripten.h>
#include <iostream>
#include <string>
#include <vector>

#include "../tools/functions.h"
#include "../tools/Callbacks.h"
#include "../tools/Random.h"
#include "../libs/Kinetic.h"

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
    const emk::Image & front;
    const emk::Image & back;
    emk::Rect card_rect;
    const int animal;
    const int color;
    const int rank;
    int id;

    emk::Callback * callback_mouseover;
    emk::Callback * callback_mouseout;
    emk::Callback * callback_mousedown;
    emk::Callback * callback_mouseup;
    emk::Callback * callback_mousemove;
    emk::Callback * callback_dragstart;
    emk::Callback * callback_dragend;
  public:
    Card(const emk::Image & _front, emk::Image & _back, int _a, int _c, int _r, int _id)
      : front(_front)
      , back(_back)
      , card_rect(0, 0, 150, 210, "white", "black", 1, true)
      , animal(_a), color(_c), rank(_r), id(_id)
      , callback_mouseover(NULL)
      , callback_mouseout(NULL)
      , callback_mousedown(NULL)
      , callback_mouseup(NULL)
      , callback_mousemove(NULL)
      , callback_dragstart(NULL)
      , callback_dragend(NULL)
    {
      card_rect.SetFillPatternImage(front); 
      card_rect.SetFillPatternScale(0.5);
      card_rect.SetLineJoin("round");
      card_rect.SetCornerRadius(10);
      card_rect.On("mouseover", this, &Card::MouseOver);
      card_rect.On("mouseout", this, &Card::MouseOut);
      card_rect.On("mousedown", this, &Card::MouseDown);
      card_rect.On("mouseup", this, &Card::MouseUp);
      card_rect.On("mousemove", this, &Card::MouseMove);
      card_rect.On("dragstart", this, &Card::DragStart);
      card_rect.On("dragend", this, &Card::DragEnd);
    }
    ~Card() { ; }

    emk::Rect & Rect() { return card_rect; }
    int GetAnimal() const { return animal; }
    int GetColor() const { return color; }
    int GetRank() const { return rank; }
    int GetID() const { return id; }

    Card & SetID(int _id) { id = _id; return *this; }

    template <class T> Card & SetMouseoverCallback(T * obj, void (T::*method_ptr)(int)) {
      callback_mouseover = new emk::MethodCallbackArg<T,int>(obj, method_ptr);
      return *this;
    }
    
    template <class T> Card & SetMouseoutCallback(T * obj, void (T::*method_ptr)(int)) {
      callback_mouseout = new emk::MethodCallbackArg<T,int>(obj, method_ptr);
      return *this;
    }
    
    template <class T> Card & SetMousedownCallback(T * obj, void (T::*method_ptr)(int)) {
      callback_mousedown = new emk::MethodCallbackArg<T,int>(obj, method_ptr);
      return *this;
    }
    
    template <class T> Card & SetMouseupCallback(T * obj, void (T::*method_ptr)(int)) {
      callback_mouseup = new emk::MethodCallbackArg<T,int>(obj, method_ptr);
      return *this;
    }
    
    template <class T> Card & SetMousemoveCallback(T * obj, void (T::*method_ptr)(int)) {
      callback_mousemove = new emk::MethodCallbackArg<T,int>(obj, method_ptr);
      return *this;
    }
    
    template <class T> Card & SetDragstartCallback(T * obj, void (T::*method_ptr)(int)) {
      callback_dragstart = new emk::MethodCallbackArg<T,int>(obj, method_ptr);
      return *this;
    }
    
    template <class T> Card & SetDragendCallback(T * obj, void (T::*method_ptr)(int)) {
      callback_dragend = new emk::MethodCallbackArg<T,int>(obj, method_ptr);
      return *this;
    }
    
    void MouseOver() {
      card_rect.SetStroke("red");
      card_rect.DrawLayer();
      emk::SetCursor("pointer");
      if (callback_mouseover) callback_mouseover->DoCallback((int*)id);
    }
  
    void MouseOut() {
      card_rect.SetStroke("black");
      card_rect.DrawLayer();
      emk::SetCursor("default");
      if (callback_mouseout) callback_mouseout->DoCallback((int*)id);
    }
  
    void MouseDown() {
      // card_rect.MoveToTop();
      if (callback_mousedown) callback_mousedown->DoCallback((int*)id);
    }

    void MouseUp() {
      // card_rect.MoveToTop();
      if (callback_mouseup) callback_mouseup->DoCallback((int*)id);
    }

    void MouseMove() {
      // card_rect.MoveToTop();
      if (callback_mousemove) callback_mousemove->DoCallback((int*)id);
    }

    void DragStart() {
      // card_rect.MoveToTop();
      if (callback_dragstart) callback_dragstart->DoCallback((int*)id);
    }

    void DragEnd() {
      // card_rect.MoveToTop();
      if (callback_dragend) callback_dragend->DoCallback((int*)id);
    }

    void Rescale(double scale) {
      card_rect.SetWidth(150 * scale);
      card_rect.SetHeight(210 * scale);
      card_rect.SetFillPatternScale(0.5 * scale);
      card_rect.SetCornerRadius(10 * scale);
    }
  };
  
  class Deck {
    string path;
    emk::Image back_image;
    vector<emk::Image *> front_images;
    vector<Card *> card_array;
    int next_id;
    emk::Random & random;
  public:
    Deck(string _path, emk::Random & _random, bool include_dragons=false) 
      : path(_path), back_image(path + "Back.png"), next_id(0), random(_random)
    {
      for (int animal_id = 0; animal_id < num_animals; animal_id++) {
        for (int rank_id = 0; rank_id < num_base_ranks; rank_id++) {
          // Load in the associated image.
          emk::Image * cur_front = new emk::Image(path + ranks[rank_id] + "_" + animals[animal_id] + ".png");
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
          emk::Image * cur_front = new emk::Image(path + "D_" + colors[color_id] + ".png");
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

    void Shuffle() {
      Card * temp_card;
      const int num_cards = (int) card_array.size();
      for (int i = 0; i < num_cards; i++) {
        int found_card = random.GetInt(num_cards - i) + i;
        if (found_card != i) {
          temp_card = card_array[i];
          card_array[i] = card_array[found_card];
          card_array[found_card] = temp_card;
          card_array[i]->SetID(i);
        }
      }
    }

  };

}

class DenOfThieves {
private:
  emk::Stage stage;
  emk::Layer layer;
  emk::Random random;
  //  emk::Animation<DenOfThieves> anim;

  GII::Deck deck;
  const int hand_size;
  std::vector<GII::Card *> hand;
  std::vector<emk::Tween *> hand_move;

public:
  DenOfThieves()
    : stage(1200, 600, "container")
    , random(-1)
    , deck("/Users/charles/Sites/Puzzle Engine/GII/", random, false)
    , hand_size(7)
    , hand(hand_size)
    , hand_move(hand_size)
  {
    // Shuffle the deck.
    deck.Shuffle();

    // Build the hand.
    for (int i = 0; i < hand_size; i++) {
      hand[i] = &(deck[i]);
      hand[i]->SetDragendCallback(this, &DenOfThieves::Card_DragEnd);
      hand_move[i] = new emk::Tween(hand[i]->Rect(), 1.0);
    }

    int next_card = hand_size;
    
    // Draw the hand to the screen.
    for (int i = 0; i < hand_size; i++) {
      emk::Rect & rect = hand[i]->Rect();
      rect.SetXY(i*100+10, 100);
      // deck[i].Rescale(0.2 + 0.01 * (double) i);
      layer.Add(rect);
    }


    stage.Add(layer);

    // rect.On("mousemove", this, &DenOfThieves::DoRectRotation);
    // rect.On("click", this, &DenOfThieves::DoClick);
    // rect2.On("click", this, &DenOfThieves::DoClick2);
  }

  void Card_DragEnd(int id) {
    hand_move[id]->SetXY(id*100+10,100).SetEasing(emk::Tween::StrongEaseOut);
    //    hand[id]->Rect().SetXY(id*100+10,100);
    //    hand[id]->Rect().DrawLayer();
    hand_move[id]->Play();
  }
};




extern "C" int emkMain()
{
  DenOfThieves * example = new DenOfThieves();
  return (int) example;
}
