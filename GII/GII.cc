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
  const string base_path = "img/GII/";
  //const string base_path = "/Users/charles/Sites/Puzzle Engine/GII/";
  const int card_width = 150;
  const int card_height = 210;
  const int stage_width = 1000;
  const int stage_height = 700;

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
    emk::Tween card_tween;
    const int global_id;
    int id;

    emk::Callback * callback_mouseover;
    emk::Callback * callback_mouseout;
    emk::Callback * callback_mousedown;
    emk::Callback * callback_mouseup;
    emk::Callback * callback_mousemove;
    emk::Callback * callback_dragstart;
    emk::Callback * callback_dragend;
    emk::Callback * callback_click;
  public:
    Card(const emk::Image & _front, const emk::Image & _back, int _id)
      : front(_front)
      , back(_back)
      , card_rect(0, 0, card_width, card_height, "white", "black", 1, true)
      , card_tween(card_rect, 1.0)
      , global_id(_id), id(_id)
      , callback_mouseover(NULL)
      , callback_mouseout(NULL)
      , callback_mousedown(NULL)
      , callback_mouseup(NULL)
      , callback_mousemove(NULL)
      , callback_dragstart(NULL)
      , callback_dragend(NULL)
      , callback_click(NULL)
    {
      card_rect.SetVisible(0);
      card_rect.SetFillPatternImage(back); 
      card_rect.SetFillPatternScale(0.5);
      card_rect.SetLineJoin("round");
      card_rect.SetCornerRadius(10);
      card_rect.SetDraggable(0);
      card_rect.On("mouseover", this, &Card::MouseOver);
      card_rect.On("mouseout", this, &Card::MouseOut);
      card_rect.On("mousedown", this, &Card::MouseDown);
      card_rect.On("mouseup", this, &Card::MouseUp);
      card_rect.On("mousemove", this, &Card::MouseMove);
      card_rect.On("dragstart", this, &Card::DragStart);
      card_rect.On("dragend", this, &Card::DragEnd);
      card_rect.On("click", this, &Card::Click);
    }
    ~Card() { ; }

    emk::Rect & Rect() { return card_rect; }
    emk::Tween & Tween() { return card_tween; }
    int GetGlobalId() const { return global_id; }
    int GetID() const { return id; }
    Card & SetID(int _id) { id = _id; return *this; }

    Card & ShowFaceUp(bool _face_up) {
      if (_face_up) {
        card_rect.SetFillPatternImage(front);
      } else {
        card_rect.SetFillPatternImage(back);
      }
      return *this;
    }

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
    
    template <class T> Card & SetClickCallback(T * obj, void (T::*method_ptr)(int)) {
      callback_click = new emk::MethodCallbackArg<T,int>(obj, method_ptr);
      return *this;
    }
    
    Card & ClearDragendCallback() { callback_dragend = NULL; return *this; }
    Card & ClearClickCallback() { callback_click = NULL; return *this; }
    
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

    void Click() {
      // card_rect.MoveToTop();
      if (callback_click) callback_click->DoCallback((int*)id);
    }

    void Rescale(double scale) {
      card_rect.SetWidth(card_width * scale);
      card_rect.SetHeight(card_height * scale);
      card_rect.SetFillPatternScale(0.5 * scale);
      card_rect.SetCornerRadius(10 * scale);
    }
  };

  class SuitedCard : public Card {
  private:
    const int animal;
    const int color;
    const int rank;

  public:
    SuitedCard(const emk::Image & _front, const emk::Image & _back, int _a, int _c, int _r, int _id)
      : Card(_front, _back, _id)
      , animal(_a), color(_c), rank(_r)
    { ; }

    ~SuitedCard() { ; }

    int GetAnimal() const { return animal; }
    int GetColor() const { return color; }
    int GetRank() const { return rank; }
  };

  template <class T> class CardSet {
    emk::Layer & layer;
    emk::Random & random;
    double x, y, spread;
    int visible;
    bool face_up;
    Card placeholder;
    vector<T *> card_array;

  public:
    CardSet(emk::Layer & _layer, emk::Random & _random, const emk::Image & placeholder_image)
      : layer(_layer)
      , random(_random)
      , x(0), y(0), spread(0), visible(0), face_up(false)
      , placeholder(placeholder_image, placeholder_image, -1)
    {
      placeholder.Rect().SetDraggable(0);
      placeholder.Rect().SetZIndex(1);
      layer.Add(placeholder.Rect());
    }

    ~CardSet() {
      card_array.clear();
    }

    CardSet & SetX(double _x) { x = _x; return *this; }
    CardSet & SetY(double _y) { y = _y; return *this; }
    CardSet & SetSpread(double _spread) { spread = _spread; return *this; }
    CardSet & SetVisible(int _visible) { visible = _visible; return *this; }
    CardSet & ShowFaceUp(bool _face_up) { face_up = _face_up; return *this; }

    int GetNumCards() const { return (int) card_array.size(); }
    T & operator[](int index) { return *(card_array[index]); }
    const T & operator[](int index) const { return *(card_array[index]); }

    void AddTop(T * card) {
      int id = (int) card_array.size();
      card_array.push_back(card);
      card->SetID(id);
      card->Rect().SetVisible(visible);
    }

    T * TakeTop() {
      T * back = card_array.back();
      card_array.pop_back();
      back->Rect().SetVisible(0);
      return back;
    }

    void Shuffle() {
      T * temp_card;
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

    void Update() {
      //placeholder.Rescale(0.2);
      placeholder.Rect().SetVisible(visible);
      placeholder.Rect().SetXY(x, y);
      placeholder.Rect().SetZIndex(0);
      for (int i = 0; i < (int) card_array.size(); i++) {
        auto card = card_array[i];
        card->ShowFaceUp(face_up);
        emk::Rect &rect = card->Rect();
        rect.SetVisible(visible);
        rect.SetXY(i*spread+x, y);
        rect.SetZIndex(i+1);
      }
    }
  };

  template <class T> class Hand : public CardSet<T> {
  private:
    int max_cards;

  public:
    Hand(emk::Layer & _layer,
         emk::Random & _random,
         const emk::Image & placeholder_image,
         int _max_cards)
      : CardSet<T>(_layer, _random, placeholder_image)
      , max_cards(_max_cards)
    { ; }

    ~Hand() { ; }
  };
}

class DenOfThieves {
private:
  emk::Stage stage;
  emk::Layer layer;
  emk::Random random;
  //  emk::Animation<DenOfThieves> anim;

  string path;
  int next_id;
  emk::Image back_image;
  emk::Image discard_image;
  emk::Image empty_image;
  const int hand_size;
  const int num_hands;
  const bool include_dragons;
  const double default_spread;
  vector<emk::Image *> front_images;
  vector<GII::SuitedCard *> card_array;
  GII::CardSet<GII::SuitedCard> deck;
  GII::CardSet<GII::SuitedCard> discard;
  GII::CardSet<GII::SuitedCard> dragons;
  vector<GII::Hand<GII::SuitedCard> *> hands;
  int total_turns;
  int turn;
  int player;

  // Info
  emk::Text turn_text;
  emk::Text turn_value;

public:
  DenOfThieves()
    : stage(GII::stage_width, GII::stage_height, "container")
    , random(-1)
    , path(GII::base_path)
    , next_id(0)
    , back_image(path + "Back.png")
    , discard_image(path + "Discard.png")
    , empty_image(path + "Empty.png")
    , hand_size(7)
    , num_hands(2)
    , include_dragons(false)
    , default_spread(0.2)
    , deck(layer, random, empty_image)
    , discard(layer, random, discard_image)
    , dragons(layer, random, empty_image)
    , total_turns(2)
    , turn(0)
    , player(0)
    , turn_text(10, 10, "Turn")
    , turn_value(100, 10, "0")
  {
    // Create all the non-dragon cards and assign to deck.
    for (int animal_id = 0; animal_id < GII::num_animals; animal_id++) {
      for (int rank_id = 0; rank_id < GII::num_base_ranks; rank_id++) {
        // Load in the associated image.
        emk::Image * cur_front = new emk::Image(path + GII::ranks[rank_id] + "_" + GII::animals[animal_id] + ".png");
        front_images.push_back(cur_front);
        for (int card_count = 0; card_count <= rank_id; card_count++) {
          GII::SuitedCard * new_card = new GII::SuitedCard(*cur_front, back_image, animal_id, animal_id/2, rank_id, next_id++);
          new_card->SetClickCallback(this, &DenOfThieves::Card_Click);
          layer.Add(new_card->Rect());
          card_array.push_back(new_card);
          deck.AddTop(new_card);
        }
      }
    }

    // Create all the dragon cards and assign to the dragon deck.  Include them
    // in deck if appropriate flag is set.
    for (int color_id = 0; color_id < GII::num_colors; color_id++) {
      // Load in the associated image.
      emk::Image * cur_front = new emk::Image(path + "D_" + GII::colors[color_id] + ".png");
      front_images.push_back(cur_front);          
      GII::SuitedCard * new_card = new GII::SuitedCard(*cur_front, back_image, color_id*2, color_id, GII::num_base_ranks, next_id++);
      layer.Add(new_card->Rect());
      card_array.push_back(new_card);
      dragons.AddTop(new_card);
      if (include_dragons) {
        deck.AddTop(new_card);
      }
    }

    // Shuffle the deck.
    deck.Shuffle();

    // Create hands
    for (int i = 0; i < num_hands; i++) {
      auto hand = new GII::Hand<GII::SuitedCard>(layer, random, empty_image, hand_size);
      for (int j = 0; j < hand_size; j++) {
        auto top_card = deck.TakeTop();
        hand->AddTop(top_card);
      }
      hands.push_back(hand);
    }

    // Place cards
    deck.SetX(335);
    deck.SetY(245);
    deck.SetSpread(default_spread);
    deck.SetVisible(1);
    deck.Update();

    discard.SetX(335 + 150 + 30); // left + deck width + 30 buffer
    discard.SetY(245);
    discard.SetSpread(default_spread);
    discard.SetVisible(1);
    discard.Update();

    for (int i = 0; i < (int) hands.size(); i++) {
      DrawHand(*hands[i], i, i == player);
    }

    // Add
    layer.Add(turn_text);
    layer.Add(turn_value);

    stage.Add(layer);
    layer.Draw();
  }

  ~DenOfThieves() {
    for (int i = 0; i < (int) hands.size(); i++) delete hands[i];
    for (int i = 0; i < (int) card_array.size(); i++) delete card_array[i];
    for (int i = 0; i < (int) front_images.size(); i++) delete front_images[i];
  }

  void Card_DragEnd(int id) {
    //hand[id].Tween().SetXY(id*100+10,100).SetEasing(emk::Tween::StrongEaseOut);
    //hand[id].Tween().Play();
  }

  void Card_Click(int id) {
    /*GII::SuitedCard * top_card = deck.TakeTop();
    top_card->ShowFaceUp(true);
    top_card->Rect().SetDraggable(0);
    top_card->ClearClickCallback();
    discard.AddTop(top_card);
    discard.Update();
    layer.Draw();*/
  }

  void DrawHand(GII::Hand<GII::SuitedCard> & hand, int player_index, bool active) {
    if (player_index == 0) {
      hand.SetX(320);
      hand.SetY(480);
    } else if (player_index == 1) {
      hand.SetX(320);
      hand.SetY(10);
    }

    if (active) {
      hand.ShowFaceUp(false);
    } else {
      hand.ShowFaceUp(true);
    }
    hand.SetSpread(35.0);
    hand.SetVisible(1);
    hand.Update();
  }
};




extern "C" int emkMain()
{
  DenOfThieves * example = new DenOfThieves();
  return (int) example;
}
