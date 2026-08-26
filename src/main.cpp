#include "util.hpp"
#include "audio.hpp"

#include <cstdlib>

using std::vector;

constexpr unsigned targetFrames {60};
const sf::VideoMode screen (960, 540);

constexpr unsigned maxPoints {5U};
constexpr float dotSize {25.f};
constexpr float ballSize {35.f};
constexpr float ballSpeed {5.f};
constexpr float ballTimeout {2.f};
constexpr float winTimeout {5.f};
constexpr float playerSpeed {3.f};
const sf::Vector2f playerSize (25.f, 100.f);

enum class gameState {MAIN_MENU, IN_GAME, PAUSED, WIN_SCREEN};
enum class lastHit {PLAYER2 = -1, NOBODY, PLAYER1};

// Main program entry point.
int main()
{
   srand(static_cast<unsigned>(time(nullptr)));
   loadAudio();

   sf::RenderWindow window (screen, "Pong", sf::Style::Titlebar | sf::Style::Close);
   window.setFramerateLimit(targetFrames);

   sf::Event event;
   gameState state {gameState::MAIN_MENU};

   // Load the DejaVuSans font.
   sf::Font font;
   font.loadFromFile("assets/DejaVuSans.ttf");

   sf::Text mainMenuHeader {createText(sf::Vector2f(screen.width / 2.f, 100.f), font, "PONG", 110U)};
   sf::Text mainMenuPlay {createText(sf::Vector2f(screen.width / 2.f, screen.height / 2.f), font, "Play", 50U)};
   sf::Text mainMenuAudio {createText(sf::Vector2f(screen.width / 2.f, 330.f), font, "Audio: On", 50U)};

   sf::RectangleShape ball (sf::Vector2f(ballSize, ballSize));
   sf::RectangleShape player1 (playerSize);
   sf::RectangleShape player2 (playerSize);

   sf::Text player1Score {createText(sf::Vector2f(screen.width / 4.f, 100.f), font, "0", 75U)};
   sf::Text player2Score {createText(sf::Vector2f(screen.width / 4.f * 3.f, 100.f), font, "0", 75U)};
   player1Score.setFillColor(sf::Color(128U, 128U, 128U));
   player2Score.setFillColor(sf::Color(128U, 128U, 128U));

   sf::Text pausedText {createText(sf::Vector2f(screen.width / 2.f, 230.f), font, "PAUSED", 110U)};
   sf::Text pausedMainMenu {createText(sf::Vector2f(screen.width / 2.f, 310.f), font, "Main Menu", 30U)};

   sf::Text winScreenText {createText(sf::Vector2f(screen.width / 2.f, screen.height / 2.f), font, "Player0 won with " + std::to_string(maxPoints) + " points!", 60U)};

   // Variables for keeping track of score.
   sf::Vector2u score (0U, 0U);
   sf::Vector2f ballDir {0.f, 0.f};
   lastHit last {lastHit::NOBODY};
   float ballSpeedMult {1.f};
   bool audioOn {true};

   sf::Clock ballTimer;
   sf::Clock pauseTimer;
   sf::Clock winTimer;

   // Create the dotted line in the middle.
   vector<sf::RectangleShape> line;

   for (float y {dotSize}; y < screen.height - dotSize * 2; y += dotSize * 2)
   {
      sf::RectangleShape dot (sf::Vector2f(dotSize, dotSize));
      dot.setPosition(sf::Vector2f(screen.width / 2.f - dotSize / 2.f, y));
      dot.setFillColor(sf::Color(128U, 128U, 128U));
      line.push_back(dot);
   }

   // Game loop.
   while (window.isOpen())
   {
      while (window.pollEvent(event))
      {
         if (event.type == sf::Event::Closed)
            window.close();
      }

      // Check if the user has pressed the "play" or "audio" button in the main menu.
      if (state == gameState::MAIN_MENU && sf::Mouse::isButtonPressed(sf::Mouse::Left))
      {
         sf::Vector2f mouse {sf::Mouse::getPosition(window)};

         if (mainMenuPlay.getGlobalBounds().contains(mouse))
         {
            playAudio(clickSound, audioOn);
            state = gameState::IN_GAME;
            player1.setPosition(0.f, screen.height / 2.f - playerSize.y / 2.f);
            player2.setPosition(screen.width - playerSize.x, screen.height / 2.f - playerSize.y / 2.f);
            ball.setPosition(screen.width / 2.f - ballSize / 2.f, screen.height / 2.f - ballSize / 2.f);

            player1Score.setString("0");
            player2Score.setString("0");

            score = sf::Vector2u(0U, 0U);
            last = lastHit::NOBODY;
            ballDir = sf::Vector2f(0.f, 0.f);
            ballTimer.restart();
            ballSpeedMult = 1.f;
         }
         else if (mainMenuAudio.getGlobalBounds().contains(mouse) && pauseTimer.getElapsedTime().asSeconds() >= .25f)
         {
            audioOn = !audioOn;
            playAudio(clickSound, audioOn);
            mainMenuAudio.setString("Audio: " + toString((audioOn ? "On" : "Off")));
            pauseTimer.restart();
         }
      }

      // Pause the game.
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)
         && pauseTimer.getElapsedTime().asSeconds() >= .25f
         && (state == gameState::IN_GAME || state == gameState::PAUSED))
      {
         playAudio(clickSound, audioOn);
         state = (state == gameState::IN_GAME ? gameState::PAUSED : gameState::IN_GAME);
         pauseTimer.restart();
      }

      if (state == gameState::PAUSED && sf::Mouse::isButtonPressed(sf::Mouse::Left))
      {
         sf::Vector2f mouse {sf::Mouse::getPosition(window)};

         if (pausedMainMenu.getGlobalBounds().contains(mouse))
         {
            playAudio(clickSound, audioOn);
            state = gameState::MAIN_MENU;
         }
      }

      // Control both of the players and the ball.
      if (state == gameState::IN_GAME)
      {
         float player1Dir = sf::Keyboard::isKeyPressed(sf::Keyboard::S) - sf::Keyboard::isKeyPressed(sf::Keyboard::W);
         float player2Dir = sf::Keyboard::isKeyPressed(sf::Keyboard::Down) - sf::Keyboard::isKeyPressed(sf::Keyboard::Up);

         float player1Target {player1.getPosition().y + player1Dir * playerSpeed};
         float player2Target {player2.getPosition().y + player2Dir * playerSpeed};

         if (player1Target >= 0.f && player1Target <= screen.height - playerSize.y)
            player1.move(0.f, player1Dir * playerSpeed);

         if (player2Target >= 0.f && player2Target <= screen.height - playerSize.y)
            player2.move(0.f, player2Dir * playerSpeed);
         
         // Control the ball.
         if (last == lastHit::NOBODY && ballDir.x == 0 && ballTimer.getElapsedTime().asSeconds() >= ballTimeout)
            ballDir = sf::Vector2f((rand() % 2 == 0 ? -1.f : 1.f), (rand() % 2 == 0 ? -1.f : 1.f));
         else if (ballDir.x == 0 && ballTimer.getElapsedTime().asSeconds() >= ballTimeout)
            ballDir = sf::Vector2f(static_cast<float>(last), (rand() % 2 == 0 ? -1.f : 1.f));
         
         sf::Vector2f ballTarget (ball.getPosition() + normalize(ballDir) * ballSpeed * ballSpeedMult);
         
         if (ballTarget.y < 0 || ballTarget.y + ballSize > screen.height)
         {
            playAudio(hitSound, audioOn);
            ballDir.y = -ballDir.y;
         }

         if (last != lastHit::PLAYER1 && player1.getGlobalBounds().intersects(ball.getGlobalBounds()))
         {
            playAudio(hitSound, audioOn);
            ballDir.x = abs(ballDir.x);
            ballDir.y = rand() % 3 - 1;
            ballSpeedMult = std::min(ballSpeedMult * 1.05f, 3.f);
            last = lastHit::PLAYER1;
         }
         else if (last != lastHit::PLAYER2 && player2.getGlobalBounds().intersects(ball.getGlobalBounds()))
         {
            playAudio(hitSound, audioOn);
            ballDir.x = -abs(ballDir.x);
            ballDir.y = rand() % 3 - 1;
            ballSpeedMult = std::min(ballSpeedMult * 1.05f, 3.f);
            last = lastHit::PLAYER2;
         }
         
         if (ballTarget.x < 0)
         {
            playAudio(player2ScoreSound, audioOn);
            score.y++;
            player2Score.setString(std::to_string(score.y));
            last = lastHit::PLAYER2;

            ball.setPosition(screen.width / 2.f - ballSize / 2.f, screen.height / 2.f - ballSize / 2.f);
            ballDir = sf::Vector2f(0.f, 0.f);
            ballTimer.restart();
            ballSpeedMult = 1.f;
         }
         else if (ballTarget.x + ballSize > screen.width)
         {
            playAudio(player1ScoreSound, audioOn);
            score.x++;
            player1Score.setString(std::to_string(score.x));
            last = lastHit::PLAYER1;

            ball.setPosition(screen.width / 2.f - ballSize / 2.f, screen.height / 2.f - ballSize / 2.f);
            ballDir = sf::Vector2f(0.f, 0.f);
            ballTimer.restart();
            ballSpeedMult = 1.f;
         }
         else
            ball.move(normalize(ballDir) * ballSpeed * ballSpeedMult);
         
         // One of the players have won the game.
         if (score.x >= maxPoints || score.y >= maxPoints)
         {
            playAudio(winSound, audioOn);
            winScreenText.setString("Player" + toString((score.x >= maxPoints ? "1" : "2")) + " won with " + std::to_string(maxPoints) + " points!");
            state = gameState::WIN_SCREEN;
            winTimer.restart();
         }
      }

      // Handle win screen timeout.
      if (state == gameState::WIN_SCREEN && winTimer.getElapsedTime().asSeconds() >= winTimeout)
         state = gameState::MAIN_MENU;

      window.clear(sf::Color::Black);

      // Draw the main menu.
      if (state == gameState::MAIN_MENU)
      {
         window.draw(mainMenuHeader);
         window.draw(mainMenuPlay);
         window.draw(mainMenuAudio);
      }

      // Draw the pong table.
      if (state != gameState::MAIN_MENU)
      {
         for (const auto& dot : line)
            window.draw(dot);
         
         window.draw(player1Score);
         window.draw(player2Score);

         window.draw(player1);
         window.draw(player2);

         if (state != gameState::WIN_SCREEN)
            window.draw(ball);

         if (state == gameState::PAUSED)
         {
            window.draw(pausedText);
            window.draw(pausedMainMenu);
         }
         else if (state == gameState::WIN_SCREEN)
            window.draw(winScreenText);
      }

      window.display();
   }
}