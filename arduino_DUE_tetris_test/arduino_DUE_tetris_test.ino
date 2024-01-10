#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000


const int XP=6,XM=A2,YP=A1,YM=7; //320x480 ID=0x9486
const int TS_LEFT=857,TS_RT=210,TS_TOP=892,TS_BOT=111;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button left_btn, right_btn, turn_btn, down_btn;

int pixel_x, pixel_y;     //Touch_getXY() updates global vars
int clear_line =0;
const int BOARD_WIDTH = 10;
const int BOARD_HEIGHT = 20;
int board[BOARD_HEIGHT][BOARD_WIDTH]={
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0}
};


int phase=0;
const int BLOCK_TYPES = 7; 
int BLOCK_SIZE=20;
int currentX = BOARD_WIDTH / 2;
int currentY = 0;
int currentBlock = 0;
int rotation = 0;

unsigned long previousMillis = 0;
const long interval = 500;

unsigned long lastRotationTime = 0;
const unsigned long rotationDelay = 200;

int nextblock[3]={random(0, BLOCK_TYPES),random(0, BLOCK_TYPES),random(0, BLOCK_TYPES)};

bool Touch_getXY(void){
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    }
    return pressed;
}


int blocks[BLOCK_TYPES][4][4][4]={
  {{{0,1,1,1},{0,0,1,0},{0,0,0,0},{0,0,0,0}},
   {{0,0,1,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}},
   {{0,0,1,0},{0,1,1,1},{0,0,0,0},{0,0,0,0}}, 
   {{0,0,1,0},{0,0,1,1},{0,0,1,0},{0,0,0,0}}},

   {{{0,2,2,2},{0,0,0,2},{0,0,0,0},{0,0,0,0}},
   {{0,0,2,0},{0,0,2,0},{0,2,2,0},{0,0,0,0}},
   {{0,2,0,0},{0,2,2,2},{0,0,0,0},{0,0,0,0}}, 
   {{0,2,2,0},{0,2,0,0},{0,2,0,0},{0,0,0,0}}},

   {{{0,0,0,3},{0,3,3,3},{0,0,0,0},{0,0,0,0}},
   {{0,3,0,0},{0,3,0,0},{0,3,3,0},{0,0,0,0}},
   {{0,3,3,3},{0,3,0,0},{0,0,0,0},{0,0,0,0}}, 
   {{0,3,3,0},{0,0,3,0},{0,0,3,0},{0,0,0,0}}},

   {{{0,0,0,0},{0,4,4,0},{0,4,4,0},{0,0,0,0}},
   {{0,0,0,0},{0,4,4,0},{0,4,4,0},{0,0,0,0}},
   {{0,0,0,0},{0,4,4,0},{0,4,4,0},{0,0,0,0}}, 
   {{0,0,0,0},{0,4,4,0},{0,4,4,0},{0,0,0,0}}},

   {{{0,5,5,0},{0,0,5,5},{0,0,0,0},{0,0,0,0}},
   {{0,0,5,0},{0,5,5,0},{0,5,0,0},{0,0,0,0}},
   {{0,5,5,0},{0,0,5,5},{0,0,0,0},{0,0,0,0}}, 
   {{0,0,5,0},{0,5,5,0},{0,5,0,0},{0,0,0,0}}},

   {{{0,0,6,6},{0,6,6,0},{0,0,0,0},{0,0,0,0}},
   {{0,6,0,0},{0,6,6,0},{0,0,6,0},{0,0,0,0}},
   {{0,0,6,6},{0,6,6,0},{0,0,0,0},{0,0,0,0}}, 
   {{0,6,0,0},{0,6,6,0},{0,0,6,0},{0,0,0,0}}},

   {{{7,7,7,7},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
   {{0,0,0,7},{0,0,0,7},{0,0,0,7},{0,0,0,7}},
   {{7,7,7,7},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
   {{0,0,0,7},{0,0,0,7},{0,0,0,7},{0,0,0,7}}}
};


#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ORANGE  0xFD20

int block_color[8]={BLACK,BLUE,RED,GREEN,CYAN,MAGENTA,YELLOW,ORANGE};


void printboard(int clear_board_empty){  
  for (int i=0;i<BOARD_HEIGHT;i++){
    for(int j=0;j<BOARD_WIDTH;j++){
      if (clear_board_empty){
        if(board[i][j]==0) tft.fillRect(10+BLOCK_SIZE*j,10+BLOCK_SIZE*i,BLOCK_SIZE,BLOCK_SIZE,block_color[board[i][j]]);
      }
      else{
        if(board[i][j]==0) tft.fillRect(10+BLOCK_SIZE*j,10+BLOCK_SIZE*i,BLOCK_SIZE,BLOCK_SIZE,block_color[board[i][j]]);
        else{
          tft.fillRect(10+BLOCK_SIZE*j,10+BLOCK_SIZE*i,BLOCK_SIZE,BLOCK_SIZE,WHITE);
          tft.fillRect(10+BLOCK_SIZE*j+1,10+BLOCK_SIZE*i+1,BLOCK_SIZE-2,BLOCK_SIZE-2,block_color[board[i][j]]);
        }
      }      
    }
  }
}


void setup() {
  Serial.begin(9600);
  uint16_t ID = tft.readID();
  Serial.print("TFT ID = 0x");
  Serial.println(ID, HEX);
  if (ID == 0xD3D3) ID = 0x9486;
  tft.begin(ID);
  tft.setRotation(0);
  tft.fillScreen(MAGENTA);
  left_btn.initButton(&tft,40,450,70,40,WHITE,CYAN,BLACK,"LEFT",1);
  right_btn.initButton(&tft,120,450,70,40,WHITE,CYAN,BLACK,"RIGHT",1);
  turn_btn.initButton(&tft,200,450,70,40,WHITE,CYAN,BLACK,"TURN",1);
  down_btn.initButton(&tft,280,450,70,40,WHITE,CYAN,BLACK,"DOWN",1);
  left_btn.drawButton(false);
  right_btn.drawButton(false);
  turn_btn.drawButton(false);
  down_btn.drawButton(false);
  tft.fillRect(10,10,200,400,BLACK);
  tft.fillRect(220,10,90,270,BLACK);
}


void handleInput(bool touch){
  left_btn.press(touch&& left_btn.contains(pixel_x, pixel_y));
  right_btn.press(touch&& right_btn.contains(pixel_x, pixel_y));
  turn_btn.press(touch&& turn_btn.contains(pixel_x, pixel_y));
  down_btn.press(touch&& down_btn.contains(pixel_x, pixel_y));

  if (left_btn.justPressed()){
    left_btn.drawButton(true);
    move(-1,0);
  }
  if (left_btn.justReleased()) {
    left_btn.drawButton();
    }

  if (right_btn.justPressed()){
    right_btn.drawButton(true);
    move(1,0);
  }
  if (right_btn.justReleased()) {
      right_btn.drawButton();
    }

  if (turn_btn.justPressed()){
    turn_btn.drawButton(true);
    rotateBlock();
  }
  if (turn_btn.justReleased()) {
    turn_btn.drawButton();
    }

    if (down_btn.justPressed()){
    down_btn.drawButton(true);
    if (!move(0, 1)) {
      drop();
    }
  }
  if (down_btn.justReleased()) {
    down_btn.drawButton();
    }
}

void printPreview(){
  tft.fillRect(220,10,90,270,BLACK);
  for(int i=0;i<3;i++){
    for (int row = 0; row < 4; row++) {
      for (int col = 0; col < 4; col++) {
        if (blocks[nextblock[i]][0][row][col]) {
          tft.fillRect(225+(col) * BLOCK_SIZE,20+90*i+(row) * BLOCK_SIZE,BLOCK_SIZE,BLOCK_SIZE,WHITE);
          tft.fillRect(225+(col) * BLOCK_SIZE+1,20+90*i+(row) * BLOCK_SIZE+1,BLOCK_SIZE-2,BLOCK_SIZE-2,block_color[nextblock[i]+1]);
        }
      }
    }
  }
}

void new_block(){
  printPreview();
  currentX = BOARD_WIDTH / 2 -2;
  currentY = 0;
  currentBlock = nextblock[0];
  nextblock[0]=nextblock[1];
  nextblock[1]=nextblock[2];
  nextblock[2]=random(0, BLOCK_TYPES);
  printPreview();

  rotation = 0;
  if (checkCollision(currentX, currentY, rotation)) {
    for (int row = 0; row < BOARD_HEIGHT; row++) {
      for (int col = 0; col < BOARD_WIDTH; col++) board[row][col] = 0; //end game
    }
    Serial.print("GAME OVER");
    tft.fillRect(10,200,300,80,WHITE);
    tft.setCursor(30,230);
    tft.setTextSize(5);
    tft.setTextColor(RED);
    tft.println("GAME OVER");
    for(;;);
  }
}

bool checkCollision(int x, int y, int rot){
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (blocks[currentBlock][rotation][row][col] &&
          (board[y + row][x + col] != 0 || x + col < 0 || x + col >= BOARD_WIDTH || y + row >= BOARD_HEIGHT)) {
        return true;
      }
    }
  }
  return false;
}

void updateBoard() {
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (blocks[currentBlock][rotation][row][col]) {
        board[currentY + row][currentX + col] = currentBlock + 1;
      }
    }
  }
}

void drawBlock() {
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (blocks[currentBlock][rotation][row][col]) {
        int x = (currentX + col) * BLOCK_SIZE;
        int y = (currentY + row) * BLOCK_SIZE;
        tft.fillRect(10+x,10+y,BLOCK_SIZE,BLOCK_SIZE,WHITE);
        tft.fillRect(10+x+1,10+y+1,BLOCK_SIZE-2,BLOCK_SIZE-2,block_color[currentBlock+1]);
      }
    }
  }
}

void rotateBlock() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastRotationTime >= rotationDelay) {
    lastRotationTime = currentMillis;
    int newRotation = (rotation + 1) % 4;
    if (checkCollision(currentX, currentY, newRotation)) {
      return;
    }
    rotation = newRotation;
  }
}

bool move(int x, int y) {
  if (checkCollision(currentX + x, currentY + y, rotation)) {
    return false;
  }
  currentX += x;
  currentY += y;
  return true;
}

void checkLines() {
  for (int row = BOARD_HEIGHT - 1; row >= 0; row--) {
    bool lineIsFull = true;
    for (int col = 0; col < BOARD_WIDTH; col++) {
      if (board[row][col] == 0) {
        lineIsFull = false;
        break;
      }
    }
    if (lineIsFull) {
      for (int j = row; j > 0; j--) {
        for (int col = 0; col < BOARD_WIDTH; col++) {
          board[j][col] = board[j - 1][col];
        }
      }
      for (int col = 0; col < BOARD_WIDTH; col++) {
        board[0][col] = 0;
      }
      clear_line ++;
    }
  }
}

void drop(){
  updateBoard();
  checkLines();
  new_block();
  tft.fillRect(220,299,90,16,BLACK);
  tft.setCursor(220, 300);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.println("SCORE="+String(clear_line));
}

void loop() {
  bool touch=Touch_getXY();

  handleInput(touch);

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (!move(0, 1)) {
      drop();
    }
  }
  printboard(1);
  drawBlock();
}
