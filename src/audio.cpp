#include "audio.hpp"

void loadAudio()
{
   clickSound.loadFromFile("assets/click.wav");
   player1ScoreSound.loadFromFile("assets/player1Score.wav");
   player2ScoreSound.loadFromFile("assets/player2Score.wav");
   winSound.loadFromFile("assets/win.wav");
   hitSound.loadFromFile("assets/hit.wav");
}

void playAudio(const sf::SoundBuffer& buffer, bool audioOn)
{
   if (!audioOn) return;

   std::thread([&buffer]()
   {
      sf::Sound sound;
      sound.setBuffer(buffer);
      sound.play();

      while (sound.getStatus() == sf::Sound::Status::Playing)
         sf::sleep(sf::milliseconds(100));
   }).detach();
}