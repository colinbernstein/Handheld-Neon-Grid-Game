/**
   Author: Colin Bernstein
   Title: Handheld 6x6 Multiplexed Neon Bulb Pong Game
*/


//Define GPIO pins and instance variables
const byte cathode[] = {2, 3, 4, 5, 6, 7}, anode[] = {8, 9, 10, 11, 12, 13};
unsigned long lastBlink, lastTick;
boolean ticking, blinking, holding = false, grid[6][6];

byte leftGuyPos = 3, rightGuyPos = 3, ballX = 1, ballY = 3, x = 1, y = 1;
boolean ballIsInFlight = false, leftGuyHasTheBall = true;
int ballTime = 300;
unsigned long last, timeStamp;

boolean twoPlayer = false;

//Initizalize hard-wired inputs and outputs
void setup() {
  for (byte i = 2; i <= 13; i++)
    pinMode(i, OUTPUT);
  for (byte i = 14; i <= 19; i++)
    pinMode(i, INPUT_PULLUP);
}

//Constantly multiplex, check for button presses, 
//draw the state of the grid, and update the position of the ball.
void loop() {
  multPlex();
  checkButtons();
  drawGuys();
  if (millis() - last >= ballTime && ballIsInFlight) {
    refreshBall();
    if (!twoPlayer)
      AI();
  }
}

//If one-player, have the AI make a move.
//Simply moves to the left or right depending on the
//Position of the ball.
void AI() {
  if (millis() % 4 > 0)
    if (leftGuyPos < 4 && leftGuyPos < ballY)
      leftGuyPos++;
    else if (leftGuyPos > 1 && leftGuyPos > ballY)
      leftGuyPos--;
}

//Multiplex through the columns by flashing a 
//column at a time and outputting the six row elements each time

void multPlex() {
  for (byte col = 0; col < 6; col++) {
    for (byte row = 0; row < 6; row++)
      digitalWrite(anode[row], grid[row][col] ? HIGH : LOW);
    digitalWrite(cathode[col], HIGH);
    //delayMicroseconds(100);
    delay(2);
    digitalWrite(cathode[col], LOW);
  }
}

//Update the position of the ball knowing its current trajectory and positon 
void refreshBall() {
  last = millis();
  grid[ballX][ballY] = false;
  if (ballY == 0 || ballY == 5)
    y *= -1;
  if (leftGuyHasTheBall && ballIsInFlight && ballX == 4 && abs(ballY - rightGuyPos) < 2) {
    x *= -1;
    leftGuyHasTheBall = false;
  }
  if (!leftGuyHasTheBall && ballIsInFlight && ballX == 1 && abs(ballY - leftGuyPos) < 2) {
    x *= -1;
    leftGuyHasTheBall = true;
  }
  ballIsInFlight = true;
  ballX += x;
  ballY += y;
  grid[ballX][ballY] = true;
  if (ballX == 0)
    win(false);
  else if (ballX == 5)
    win(true);
}

//Play a winning animation for the left or right side players
void win(boolean left) {
  for (byte i = 0; i < 3; i++) {
    for (byte a = 0; a < 6; a++)
      for (byte b = 0; b < 6; b++)
        grid[a][b] = true;
    timeStamp = millis();
    while (millis() - timeStamp <= 300) multPlex();
    blank();
    timeStamp = millis();
    while (millis() - timeStamp <= 300) multPlex();
  }
  leftGuyPos = 3;
  rightGuyPos = 3;
  ballX = left ? 1 : 4;
  ballY = 3;
  ballIsInFlight = false;
  leftGuyHasTheBall = left;
  x = left ? 1 : -1;
}

//Blank the grid
void blank() {
  for (byte a = 0; a < 6; a++)
    for (byte b = 0; b < 6; b++)
      grid[a][b] = false;
}

//Check for button presses
//Adjust the position of the players accordingly
//If a button is pressed, a button is now being held
void checkButtons() {
  if (twoPlayer) {
    if (!holding && !digitalRead(17) && !leftGuyHasTheBall)
      ballIsInFlight = true;
    else if (!holding && !digitalRead(19) && leftGuyHasTheBall)
      ballIsInFlight = true;
    else if (!holding && !digitalRead(14) && rightGuyPos <= 3 && rightGuyPos > 0) {
      holding = true;
      rightGuyPos++;
      if (!ballIsInFlight && !leftGuyHasTheBall)
        ballY++;
    }
    else if (!holding && !digitalRead(16) && leftGuyPos <= 3 && leftGuyPos > 0) {
      holding = true;
      leftGuyPos++;
      if (!ballIsInFlight && leftGuyHasTheBall)
        ballY++;
    }

    else if (!holding && !digitalRead(18) && leftGuyPos <= 4 && leftGuyPos > 1) {
      holding = true;
      leftGuyPos--;
      if (!ballIsInFlight && leftGuyHasTheBall)
        ballY--;
    }
  } else {
    if (!digitalRead(17) && !leftGuyHasTheBall)
      ballIsInFlight = true;
    else if (!digitalRead(19) && leftGuyHasTheBall)
      ballIsInFlight = true;
    else if (!holding && !digitalRead(14) && rightGuyPos <= 3 && rightGuyPos > 0) {
      holding = true;
      rightGuyPos++;
      if (!ballIsInFlight && !leftGuyHasTheBall)
        ballY++;
    }
  }
  if (!digitalRead(17) && !digitalRead(19)) {
    twoPlayer = !twoPlayer;
    win(leftGuyHasTheBall);
  }
  else if (!holding && !digitalRead(15) && rightGuyPos <= 4 && rightGuyPos > 1) {
    holding = true;
    rightGuyPos--;
    if (!ballIsInFlight && !leftGuyHasTheBall)
      ballY--;
  }
  else if (digitalRead(14) && digitalRead(15) && digitalRead(16) && digitalRead(17) && digitalRead(18) && digitalRead(19))
    holding = false;
}

//Display the paddles
void drawGuys() {
  blank();
  for (byte y = leftGuyPos - 1; y <= leftGuyPos + 1; y++)
    grid[0][y] = true;
  for (byte y = rightGuyPos - 1; y <= rightGuyPos + 1; y++)
    grid[5][y] = true;
  grid[ballX][ballY] = true;
}
