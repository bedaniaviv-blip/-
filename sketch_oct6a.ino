
#include &lt;LedControl.h&gt;

const int DIN_PIN = 11;
const int CLK_PIN = 13;
const int CS_PIN = 10;
LedControl matrix = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1);

const int UP_BUTTON = 2;
const int DOWN_BUTTON = 3;
const int RIGHT_BUTTON = 4;
const int LEFT_BUTTON = 5;

const int WIDTH = 8;
const int HEIGHT = 8;
const int MAX_LENGTH = 64;

int snakeX[MAX_LENGTH];
int snakeY[MAX_LENGTH];
int snakeLength;
int foodX;
int foodY;

enum Direction { UP, DOWN, LEFT, RIGHT };
Direction direction;
Direction nextDirection;

unsigned long lastMove = 0;
int moveDelay = 700;
const int MIN_SPEED = 300;
const int SPEED_INCREASE = 30;

const unsigned long debounceDelay = 50;
bool upLast = HIGH;
bool downLast = HIGH;
bool rightLast = HIGH;
bool leftLast = HIGH;
bool upStable = HIGH;
bool downStable = HIGH;
bool rightStable = HIGH;
bool leftStable = HIGH;
unsigned long upTime = 0;
unsigned long downTime = 0;
unsigned long rightTime = 0;
unsigned long leftTime = 0;

unsigned long lastBlink = 0;
bool foodVisible = true;
const unsigned long blinkDelay = 300;

const bool MIRROR_X = true;

void drawPixel(int x, int y, bool state) {
int displayX = x;
if (MIRROR_X) displayX = 7 - x;

matrix.setLed(0, y, displayX, state);
}

void clearMatrix() {
matrix.clearDisplay(0);
}

void drawGame() {
clearMatrix();

for (int i = 0; i &lt; snakeLength; i++)
drawPixel(snakeX[i], snakeY[i], true);

if (foodVisible)
drawPixel(foodX, foodY, true);
}

void createFood() {
bool valid = false;

while (!valid) {
foodX = random(0, WIDTH);
foodY = random(0, HEIGHT);
valid = true;

for (int i = 0; i &lt; snakeLength; i++) {
if (snakeX[i] == foodX &amp;&amp; snakeY[i] == foodY) {
valid = false;
break;

}
}
}

foodVisible = true;
}

void newGame() {
clearMatrix();

snakeLength = 2;
snakeX[0] = 4;
snakeY[0] = 4;
snakeX[1] = 3;
snakeY[1] = 4;

direction = RIGHT;
nextDirection = RIGHT;
moveDelay = 700;

createFood();
lastMove = millis();
lastBlink = millis();
}

void readButtons() {
unsigned long now = millis();
bool reading;

reading = digitalRead(UP_BUTTON);
if (reading != upLast) {
upTime = now;
upLast = reading;
}
if (now - upTime &gt;= debounceDelay) {
if (reading != upStable) {
upStable = reading;
if (upStable == LOW &amp;&amp; direction != LEFT)
nextDirection = RIGHT;
}
}

reading = digitalRead(DOWN_BUTTON);
if (reading != downLast) {
downTime = now;
downLast = reading;
}
if (now - downTime &gt;= debounceDelay) {
if (reading != downStable) {
downStable = reading;
if (downStable == LOW &amp;&amp; direction != RIGHT)
nextDirection = LEFT;
}
}

reading = digitalRead(RIGHT_BUTTON);
if (reading != rightLast) {
rightTime = now;

rightLast = reading;
}
if (now - rightTime &gt;= debounceDelay) {
if (reading != rightStable) {
rightStable = reading;
if (rightStable == LOW &amp;&amp; direction != DOWN)
nextDirection = UP;
}
}

reading = digitalRead(LEFT_BUTTON);
if (reading != leftLast) {
leftTime = now;
leftLast = reading;
}
if (now - leftTime &gt;= debounceDelay) {
if (reading != leftStable) {
leftStable = reading;
if (leftStable == LOW &amp;&amp; direction != UP)
nextDirection = DOWN;
}
}
}

bool hitsSnake(int x, int y) {
for (int i = 0; i &lt; snakeLength; i++) {
if (snakeX[i] == x &amp;&amp; snakeY[i] == y)
return true;
}

return false;
}

bool moveSnake() {
direction = nextDirection;

int newX = snakeX[0];
int newY = snakeY[0];

if (direction == UP) newY--;
if (direction == DOWN) newY++;
if (direction == RIGHT) newX++;
if (direction == LEFT) newX--;

if (newX &lt; 0 || newX &gt;= WIDTH || newY &lt; 0 || newY &gt;= HEIGHT)
return false;

if (hitsSnake(newX, newY))
return false;

bool ateFood = (newX == foodX &amp;&amp; newY == foodY);

if (ateFood &amp;&amp; snakeLength &lt; MAX_LENGTH)
snakeLength++;

for (int i = snakeLength - 1; i &gt; 0; i--) {
snakeX[i] = snakeX[i - 1];
snakeY[i] = snakeY[i - 1];
}

snakeX[0] = newX;
snakeY[0] = newY;

if (ateFood) {
createFood();

if (moveDelay &gt; MIN_SPEED) {
moveDelay -= SPEED_INCREASE;

if (moveDelay &lt; MIN_SPEED)
moveDelay = MIN_SPEED;
}
}

return true;
}

void gameOverAnimation() {
clearMatrix();

for (int i = 0; i &lt; 8; i++) {
drawPixel(i, i, true);
drawPixel(7 - i, i, true);
}

delay(1000);
clearMatrix();
delay(300);

for (int i = 0; i &lt; 8; i++) {
drawPixel(i, i, true);
drawPixel(7 - i, i, true);
}

delay(1000);
clearMatrix();
}

void setup() {
matrix.shutdown(0, false);
matrix.setIntensity(0, 5);
matrix.clearDisplay(0);

pinMode(UP_BUTTON, INPUT_PULLUP);
pinMode(DOWN_BUTTON, INPUT_PULLUP);
pinMode(RIGHT_BUTTON, INPUT_PULLUP);
pinMode(LEFT_BUTTON, INPUT_PULLUP);

randomSeed(analogRead(A0));
newGame();
}

void loop() {
unsigned long now = millis();

readButtons();

if (now - lastBlink &gt;= blinkDelay) {
lastBlink = now;
foodVisible = !foodVisible;
}

if (now - lastMove &gt;= moveDelay) {
lastMove = now;

bool alive = moveSnake();

if (!alive) {
gameOverAnimation();
newGame();
return;
}
}

drawGame();
}
