#pragma once
#include <SFML/Graphics.hpp>

#include <cmath>
#include <string>
#include <vector>

using std::string;
using std::vector;

// Normalize a vector.
sf::Vector2f normalize(const sf::Vector2f& vector2f);

// Convert a C-string to a string.
string toString(const string& prompt);

// Create a new text object at the given position.
sf::Text createText(const sf::Vector2f& pos, const sf::Font& font, const string& prompt, unsigned size);