const byte cathode[] = {2, 3, 4, 5, 6, 7}, anode[] = {8, 9, 10, 11, 12, 13};
const int tickRate = 300;
unsigned long lastTick;
boolean flip, grid[6][6];

double n = 0.0;

void setup() {
  for (byte i = 2; i <= 13; i++)
    pinMode(i, OUTPUT);
}

void loop() {
  multPlex();
  if (millis() - lastTick >= tickRate) {
    lastTick = millis();
    flip = !flip;
    for (byte row = 0; row < 6; row++)
      for (byte col = 0; col < 6; col++) {
        grid[row][col] = (int) (sqrt((row * row) + (col * col)) + n) % 2 == 0;
      }
    n += 0.01;
  }
}

void multPlex() {
  for (byte col = 0; col < 6; col++) {
    for (byte row = 0; row < 6; row++) {
      digitalWrite(anode[row], grid[row][col] ? HIGH : LOW);
    }
    digitalWrite(cathode[col], HIGH);
    delay(1);
    digitalWrite(cathode[col], LOW);
  }
}

boolean inBounds(byte i, byte j) {
  return i >= 0 && j >= 0 && i < 6 && j < 6;
}

byte neighbors(byte i, byte j) {
  byte ret = 0;
  byte dx[] = {0, 0, 1, -1, 1, 1, -1, -1};
  byte dy[] = {1, -1, 0, 0, 1, -1, 1, -1};
  for (byte a = 0; a < 8; a++)
    if (inBounds(i + dx[a], j + dy[a]))
      if (grid[i + dx[a]][j + dy[a]])
        ret++;
  return ret;
}

/*void blank() {
  for (byte col = 0; col < 6; col++)
    digitalWrite(cathode[col], LOW);
  }*/
