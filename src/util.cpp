#include "util.hpp"

sf::Vector2f normalize(const sf::Vector2f& vector2f)
{
   const float magnitude {sqrtf(vector2f.x * vector2f.x + vector2f.y * vector2f.y)};

   if (magnitude == 0.f)
      return sf::Vector2f(0.f, 0.f);
   
   return sf::Vector2f(vector2f.x / magnitude, vector2f.y / magnitude);
}

string toString(const string& prompt)
{
   return prompt;
}

sf::Text createText(const sf::Vector2f& pos, const sf::Font& font, const string& prompt, unsigned size)
{
   sf::Text text (prompt, font, size);
   sf::FloatRect textBounds {text.getGlobalBounds()};
   
   text.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
   text.setPosition(pos);
   return text;
}