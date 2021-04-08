#include "deck.h"

#include <algorithm>
#include <sstream>

Deck::Deck(int size) {
  cardback = door::ANSIColor(door::COLOR::RED);
  card_height = size;
  init();
}

Deck::Deck(door::ANSIColor backcolor, int size) : cardback{backcolor} {
  card_height = size;
  init();
}

void Deck::init(void) {
  if (card_height != 3) {
    if (card_height != 5) {
      card_height = 3;
    }
  }
  for (int i = 0; i < 52; ++i) {
    cards.push_back(card_of(i));
  }
  // 0 = BLANK, 1-4 levels
  for (int i = 0; i < 5; ++i) {
    backs.push_back(back_of(i));
  }
  mark.push_back(mark_of(0));
  mark.push_back(mark_of(1));
}

Deck::~Deck() {
  for (auto c : cards) {
    delete c;
  }
  cards.clear();
  for (auto b : backs) {
    delete b;
  }
  backs.clear();
  for (auto m : mark) {
    delete m;
  }
  mark.clear();
}

int Deck::is_deck(int c) { return c / 52; }
int Deck::is_suit(int c) { return (c % 52) / 13; }
int Deck::is_rank(int c) { return (c % 52) % 13; }

char Deck::rank_symbol(int c) {
  const char symbols[] = "A23456789TJQK";
  return symbols[c];
}

std::string Deck::suit_symbol(int c) {
  // unicode
  if (door::unicode) {
    switch (c) {
    case 0:
      return std::string("\u2665");
    case 1:
      return std::string("\u2666");
    case 2:
      return std::string("\u2663");
    case 3:
      return std::string("\u2660");
    }
  } else {
    if (door::full_cp437) {
      switch (c) {
      case 0:
        return std::string(1, '\x03');
      case 1:
        return std::string(1, '\x04');
      case 2:
        return std::string(1, '\x05');
      case 3:
        return std::string(1, '\x06');
      }
    } else {
      // These look horrible!
      switch (c) {
      case 0:
        return std::string(1, '*'); // H
      case 1:
        return std::string(1, '^'); // D
      case 2:
        return std::string(1, '%'); // C
      case 3:
        return std::string(1, '$'); // S
      }
    }
  }
  return std::string("!", 1);
}

door::Panel *Deck::card_of(int c) {
  int suit = is_suit(c);
  int rank = is_rank(c);
  bool is_red = (suit < 2);
  door::ANSIColor color;

  if (is_red) {
    color = door::ANSIColor(door::COLOR::RED, door::COLOR::WHITE);
  } else {
    color = door::ANSIColor(door::COLOR::BLACK, door::COLOR::WHITE);
  }
  door::Panel *p = new door::Panel(0, 0, 5);
  // setColor sets border_color.  NOT WHAT I WANT.
  // p->setColor(color);
  char r = rank_symbol(rank);
  std::string s = suit_symbol(suit);

  // build lines
  std::ostringstream oss;
  oss << r << s << "   ";
  std::string str = oss.str();
  p->addLine(std::make_unique<door::Line>(str, 5, color));
  oss.str(std::string());
  oss.clear();

  if (card_height == 5)
    p->addLine(std::make_unique<door::Line>("     ", 5, color));

  oss << "  " << s << "  ";
  str = oss.str();
  p->addLine(std::make_unique<door::Line>(str, 5, color));
  oss.str(std::string());
  oss.clear();

  if (card_height == 5)
    p->addLine(std::make_unique<door::Line>("     ", 5, color));

  oss << "   " << s << r;
  str = oss.str();
  p->addLine(std::make_unique<door::Line>(str, 5, color));
  oss.str(std::string());
  oss.clear();

  return p;
}

std::string Deck::back_char(int level) {
  std::string c;
  if (level == 0) {
    c = ' ';
    return c;
  }
  if (door::unicode) {
    switch (level) {
    case 1:
      c = "\u2591";
      break;
    case 2:
      c = "\u2592";
      break;
    case 3:
      c = "\u2593";
      break;
    case 4:
      c = "\u2588";
      break;
    }
  } else {
    switch (level) {
    case 1:
      c = "\xb0";
      break;
    case 2:
      c = "\xb1";
      break;
    case 3:
      c = "\xb2";
      break;
    case 4:
      c = "\xdb";
      break;
    }
  }
  return c;
}

door::Panel *Deck::back_of(int level) {
  // using: \xb0, 0xb1, 0xb2, 0xdb
  // OR:    \u2591, \u2592, \u2593, \u2588

  // door::ANSIColor color(door::COLOR::RED, door::COLOR::BLACK);
  door::Panel *p = new door::Panel(0, 0, 5);
  std::string c = back_char(level);
  std::string l = c + c + c + c + c;
  for (int x = 0; x < card_height; ++x) {
    p->addLine(std::make_unique<door::Line>(l, 5, cardback));
  };
  // p->addLine(std::make_unique<door::Line>(l, 5, cardback));
  // p->addLine(std::make_unique<door::Line>(l, 5, cardback));
  return p;
}

door::Panel *Deck::mark_of(int c) {
  door::Panel *p = new door::Panel(1);
  door::ANSIColor color = door::ANSIColor(
      door::COLOR::BLUE, door::COLOR::WHITE); // , door::ATTR::BOLD);
  std::string m;
  if (c == 0)
    m = " ";
  else {
    if (door::unicode) {
      m = "\u25a0";
    } else {
      m = "\xfe";
    }
  }
  p->addLine(std::make_unique<door::Line>(m, 1, color));
  return p;
}

void Deck::part(int x, int y, door::Door &d, int level, bool left) {
  // Render part of the back of a card.
  y += 2;
  if (!left) {
    x += 2;
  }
  std::string c = back_char(level);
  std::string l = c + c + c;
  door::Goto g(x, y);
  d << g << cardback << l;
}

door::Panel *Deck::card(int c) { return cards[c]; }

door::Panel *Deck::back(int level) { return backs[level]; }

const std::array<std::pair<int, int>, 18> Deck::blocks = {
    make_pair(3, 4),   make_pair(5, 6),   make_pair(7, 8), // end row 1
    make_pair(9, 10),  make_pair(10, 11), make_pair(12, 13),
    make_pair(13, 14), make_pair(15, 16), make_pair(16, 17),
    make_pair(18, 19), // end row 2
    make_pair(19, 20), make_pair(20, 21), make_pair(21, 22),
    make_pair(22, 23), make_pair(23, 24), make_pair(24, 25),
    make_pair(25, 26), make_pair(26, 27) // 27
};

/**
 * @brief Which card (if any) is unblocked by this card
 *
 * @param c
 * @return * int
 */
std::vector<int> Deck::unblocks(int c) {
  std::vector<int> result;
  for (size_t i = 0; i < blocks.size(); ++i) {
    if ((blocks.at(i).first == c) || (blocks.at(i).second == c)) {
      result.push_back(i);
    }
  }
  return result;
}

bool Deck::can_play(int c1, int c2) {
  int s1, s2;
  s1 = is_rank(c1);
  s2 = is_rank(c2);

  // this works %13 handles wrap-around for us.
  if ((s1 + 1) % 13 == s2)
    return true;

  if (s1 == 0) {
    s1 += 13;
  }
  if (s1 - 1 == s2)
    return true;
  return false;
}

door::Panel *Deck::marker(int c) { return mark[c]; }

void Deck::remove_card(door::Door &door, int c, int off_x, int off_y, bool left,
                       bool right) {
  int cx, cy, level;
  cardgo(c, cx, cy, level);
  if (level > 1)
    --level;
  std::string cstr = back_char(level);
  door::Goto g(cx + off_x, cy + off_y);
  door << g << cardback;
  if (left)
    door << cstr;
  else
    door << " ";
  door << "   ";
  if (right)
    door << cstr;
  else
    door << " ";
  g.set(cx + off_x, cy + off_y + 1);
  door << g << "     ";
  g.set(cx + off_x, cy + off_y + 2);
  door << g << "     ";
}

/*
Layout spacing 1:

         1         2         3         4         5         6
123456789012345678901234567890123456789012345678901234567890
         ░░░░░             ░░░░░             ░░░░░
         ░░░░░             ░░░░░             ░░░░░
      ▒▒▒▒▒░▒▒▒▒▒       #####░#####       #####░#####
      ▒▒▒▒▒ ▒▒▒▒▒       ##### #####       ##### #####
   ▓▓▓▓▓▒▓▓▓▓▓▒▓▓▓▓▓ #####=#####=##### #####=#####=#####
   ▓▓▓▓▓ ▓▓▓▓▓ ▓▓▓▓▓ ##### ##### ##### ##### ##### #####
█████▓█████▓█████▓#####=#####=#####=#####=#####=#####=#####
█████ █████ █████ ##### ##### ##### ##### ##### ##### #####
█████ █████ █████ ##### ##### ##### ##### ##### ##### #####

width = 5 * 10 + (1*9) = 59   OK!

Layout with spacing = 2:

            EEEEE
        ZZZZZ
    yyyyyZZZyyyyy
    yyyyy   yyyyy
XXXXXyyyXXXXXyyyXXXXX
XXXXX   XXXXX   XXXXX

width = 5 * 10 + (2 * 9) = 50+18 = 68   !  I could do that!
*/
void cardgo(int pos, int space, int h, int &x, int &y, int &level) {
  // special cases here
  if (pos == 28) {
    // cardgo(23, space, h, x, y, level);
    cardgo(23, x, y, level);
    y += h + 1;
    --level;
    return;
  } else {
    if (pos == 29) {
      // cardgo(22, space, h, x, y, level);
      cardgo(22, x, y, level);
      y += h + 1;
      --level;
      return;
    }
  }

  const int CARD_WIDTH = 5;
  int HALF_WIDTH = 3;
  // space = 1 or 3
  // int space = 1;
  // space = 3;
  HALF_WIDTH += space / 2;

  /*
int levels[4] = {3, 6, 9, 10};

  for (level = 0; level < 4; ++level) {
      if (pos < levels[level]) {
          level++;
          // we're here
          y = (level -1) * 2 + 1;
      } else {
          pos -= levels[level];
      }
  }
*/
  int between = CARD_WIDTH + space;

  if (pos < 3) {
    // top
    level = 1;
    y = (level - 1) * (h - 1) + 1;
    x = pos * (between * 3) + between + HALF_WIDTH + space; // 10
    return;
  } else {
    pos -= 3;
  }
  if (pos < 6) {
    level = 2;
    y = (level - 1) * (h - 1) + 1;
    int group = (pos) / 2;
    x = pos * between + (group * between) + CARD_WIDTH + space * 2;
    return;
  } else {
    pos -= 6;
  }
  if (pos < 9) {
    level = 3;
    y = (level - 1) * (h - 1) + 1;
    x = pos * between + HALF_WIDTH + space;
    return;
  } else {
    pos -= 9;
  }
  if (pos < 10) {
    level = 4;
    y = (level - 1) * (h - 1) + 1;
    x = (pos)*between + space;
    return;
  } else {
    // something is wrong.
    y = -1;
    x = -1;
    level = -1;
  }
}

/**
 * @brief Given card pos, calculate x, y, and level values.
 *
 * level is used to determine the card background gradient.
 *
 * @param pos
 * @param x
 * @param y
 * @param level
 */
void cardgo(int pos, int &x, int &y, int &level) {
  const int space = 3;
  const int h = 3;

  // special cases here
  if (pos == 28) {
    cardgo(23, x, y, level);
    y += h + 1;
    --level;
    return;
  } else {
    if (pos == 29) {
      cardgo(22, x, y, level);
      y += h + 1;
      --level;
      return;
    }
  }

  const int CARD_WIDTH = 5;
  int HALF_WIDTH = 3;
  HALF_WIDTH += space / 2;

  int between = CARD_WIDTH + space;

  if (pos < 3) {
    // top
    level = 1;
    y = (level - 1) * (h - 1) + 1;
    x = pos * (between * 3) + between + HALF_WIDTH + space; // 10
    return;
  } else {
    pos -= 3;
  }
  if (pos < 6) {
    level = 2;
    y = (level - 1) * (h - 1) + 1;
    int group = (pos) / 2;
    x = pos * between + (group * between) + CARD_WIDTH + space * 2;
    return;
  } else {
    pos -= 6;
  }
  if (pos < 9) {
    level = 3;
    y = (level - 1) * (h - 1) + 1;
    x = pos * between + HALF_WIDTH + space;
    return;
  } else {
    pos -= 9;
  }
  if (pos < 10) {
    level = 4;
    y = (level - 1) * (h - 1) + 1;
    x = (pos)*between + space;
    return;
  } else {
    // something is wrong.
    y = -1;
    x = -1;
    level = -1;
  }
}

cards card_shuffle(std::seed_seq &seed, int decks) {
  std::mt19937 gen;

  // build deck of cards
  int size = decks * 52;
  std::vector<int> deck;
  deck.reserve(size);
  for (int x = 0; x < size; ++x) {
    deck.push_back(x);
  }

  // repeatable, but random
  gen.seed(seed);
  std::shuffle(deck.begin(), deck.end(), gen);
  return deck;
}

cards card_states(int decks) {
  // auto states = std::unique_ptr<std::vector<int>>(); // (decks * 52, 0)>;
  std::vector<int> states;
  states.assign(decks * 52, 0);
  return states;
}

/**
 * @brief Find the next card we can move the marker to.
 *
 * if left, look in the left - direction, otherwise the right + direction.
 * current is the current active card.
 * states is the card states (0 = down, 1 = in play, 2 = removed)
 *
 * @param left
 * @param states
 * @param current
 * @return int
 */
int find_next(bool left, const cards &states, int current) {
  int cx, cy, level;
  int current_x;
  cardgo(current, cx, cy, level);
  current_x = cx;
  int x;
  int pos = -1;
  int pos_x;
  if (left)
    pos_x = 0;
  else
    pos_x = 100;

  for (x = 0; x < 28; x++) {
    if (states.at(x) == 1) {
      // possible location
      if (x == current)
        continue;
      cardgo(x, cx, cy, level);
      if (left) {
        if ((cx < current_x) and (cx > pos_x)) {
          pos_x = cx;
          pos = x;
        }
      } else {
        if ((cx > current_x) and (cx < pos_x)) {
          pos_x = cx;
          pos = x;
        }
      }
    }
  }
  return pos;
}

int find_next_closest(const cards &states, int current) {
  int cx, cy, level;
  int current_x;
  cardgo(current, cx, cy, level);
  current_x = cx;
  int x;
  int pos = -1;
  int pos_x = -1;

  for (x = 0; x < 28; x++) {
    if (states.at(x) == 1) {
      // possible location
      if (x == current)
        continue;
      cardgo(x, cx, cy, level);
      if (pos == -1) {
        pos = x;
        pos_x = cx;
      } else {
        if (abs(current_x - cx) < abs(current_x - pos_x)) {
          pos = x;
          pos_x = cx;
        }
      }
    }
  }
  return pos;
}