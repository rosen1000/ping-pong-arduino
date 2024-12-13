#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SDA_PIN 21
#define SCL_PIN 22
#define POTENTIOMETER_PIN 4
#define DIN 23
#define CS 5
#define CLK 18
#define BEEPER_PIN 19
#define DELAY_FRAME 0

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
MD_Parola pl = MD_Parola(MD_MAX72XX::FC16_HW, DIN, CLK, CS, 1);

int paddleHeight = 16;
int paddleWidth = 4;
int ballSize = 4;
int playerY = SCREEN_HEIGHT / 2 - paddleHeight / 2;
int aiY = SCREEN_HEIGHT / 2 - paddleHeight / 2;
int ballX = SCREEN_WIDTH / 2, ballY = SCREEN_HEIGHT / 2;
int ballSpeedX = 2, ballSpeedY = 2;
int playerScore = 0, aiScore = 0;

void resetBall()
{
  ballX = SCREEN_WIDTH / 2;
  ballY = SCREEN_HEIGHT / 2;
  ballSpeedX = random(0, 2) == 0 ? 2 : -2;
  ballSpeedY = random(0, 2) == 0 ? 2 : -2;
}

void renderGame()
{
  display.clearDisplay();
  display.fillRect(0, playerY, paddleWidth, paddleHeight, WHITE);
  display.fillRect(SCREEN_WIDTH - paddleWidth, aiY, paddleWidth, paddleHeight, WHITE);
  display.fillRect(ballX, ballY, ballSize, ballSize, WHITE);

  for (int i = 0; i < SCREEN_HEIGHT; i += 4)
  {
    display.drawFastVLine(SCREEN_WIDTH / 2, i, 2, WHITE);
  }

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(8, 0);
  display.print("Player: ");
  display.print(playerScore);
  display.setCursor(SCREEN_WIDTH - 60, 0);
  display.print("AI: ");
  display.print(aiScore);

  display.display();
}

void updateScore()
{
  pl.displayClear();
  if (playerScore == 10)
  {
    playerScore = 0;
    aiScore = 0;
  }
  pl.write((char)(playerScore + '0'));

  tone(BEEPER_PIN, 1047, 200);
  delay(200);
  tone(BEEPER_PIN, 1568, 100);
}

void setup()
{
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    for (;;)
      ;
  }
  display.clearDisplay();

  pinMode(4, INPUT);
  pinMode(19, OUTPUT);

  pl.begin();
  pl.setIntensity(0);
  pl.displayClear();
  pl.write('0');

  randomSeed(analogRead(0));
  ballSpeedX = random(0, 2) == 0 ? 2 : -2;
}

void loop()
{
  int potValue = analogRead(POTENTIOMETER_PIN);
  playerY = map(potValue, 0, 1023, 0, SCREEN_HEIGHT - paddleHeight);

  ballX += ballSpeedX;
  ballY += ballSpeedY;

  if (ballY <= 0 || ballY >= SCREEN_HEIGHT - ballSize)
    ballSpeedY *= -1;

  if (ballX <= paddleWidth && ballY + ballSize >= playerY && ballY <= playerY + paddleHeight)
    ballSpeedX *= -1;

  if (ballX >= SCREEN_WIDTH - paddleWidth - ballSize &&
      ballY + ballSize >= aiY && ballY <= aiY + paddleHeight)
    ballSpeedX *= -1;

  if (ballY < aiY && aiY > 0)
    aiY -= 2;

  if (ballY > aiY + paddleHeight && aiY < SCREEN_HEIGHT - paddleHeight)
    aiY += 2;

  if (ballX <= 0)
  {
    aiScore++;
    updateScore();
    resetBall();
  }

  if (ballX >= SCREEN_WIDTH)
  {
    playerScore++;
    updateScore();
    resetBall();
  }

  renderGame();
#if DELAY_FRAME > 0
  delay(DELAY_FRAME);
#endif
}
