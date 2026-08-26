#pragma once
#include <SFML/Audio.hpp>
#include <thread>

inline sf::SoundBuffer clickSound;
inline sf::SoundBuffer player1ScoreSound;
inline sf::SoundBuffer player2ScoreSound;
inline sf::SoundBuffer winSound;
inline sf::SoundBuffer hitSound;

// Load all of the audio files.
void loadAudio();

// Play an audio file.
void playAudio(const sf::SoundBuffer& buffer, bool audioOn);