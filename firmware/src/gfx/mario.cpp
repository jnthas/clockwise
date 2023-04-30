#include "mario.h"

Mario::Mario(int x, int y) {
  _x = x;
  _y = y;
}

void Mario::move(Direction dir, int times) {
  
  if (dir == RIGHT) {
    _x += MARIO_PACE;
  } else if (dir == LEFT) {
    _x -= MARIO_PACE;
  }  

}

void Mario::jump() {
  if (_state != JUMPING && (millis() - lastMillis > 500) ) {
    // Serial.println("Jump - Start");

    _lastState = _state;
    _state = JUMPING;

    Locator::getDisplay()->fillRect(_x, _y, _width, _height, SKY_COLOR);
    
    _width = MARIO_JUMP_SIZE[0];
    _height = MARIO_JUMP_SIZE[1];
    _sprite = MARIO_JUMP;

    direction = UP;

    _lastY = _y;
    _lastX = _x;
  }  
}

void Mario::idle() {
  if (_state != IDLE) {
    // Serial.println("Idle - Start");

    _lastState = _state;
    _state = IDLE;

    Locator::getDisplay()->fillRect(_x, _y, _width, _height, SKY_COLOR);

    _width = MARIO_IDLE_SIZE[0];
    _height = MARIO_IDLE_SIZE[1];
    _sprite = MARIO_IDLE;
  }
}


void Mario::init() {
  Locator::getEventBus()->subscribe(this);
  Locator::getDisplay()->drawRGBBitmap(_x, _y, MARIO_IDLE, MARIO_IDLE_SIZE[0], MARIO_IDLE_SIZE[1]);
}

void Mario::update() {
  

  if (_state == IDLE && _state != _lastState) {
    Locator::getDisplay()->drawRGBBitmap(_x, _y, MARIO_IDLE, MARIO_IDLE_SIZE[0], MARIO_IDLE_SIZE[1]);
  } else if (_state == JUMPING) {
    
    if (millis() - lastMillis >= 50) {

      //Serial.println(_y);
      
      Locator::getDisplay()->fillRect(_x, _y, _width, _height, SKY_COLOR);
      
      _y = _y + (MARIO_PACE * (direction == UP ? -1 : 1));

      Locator::getDisplay()->drawRGBBitmap(_x, _y, _sprite, _width, _height);
      
      Locator::getEventBus()->broadcast(MOVE, this);

     
      if (floor(_lastY - _y) >= MARIO_JUMP_HEIGHT) {
        direction = DOWN;
      }

      if (_y+_height >= 56) {
        idle();
      }

      lastMillis = millis();
    }

  }
}

void Mario::execute(EventType event, Sprite* caller) {
  if (event == EventType::COLLISION) {
    //Serial.println("MARIO - Collision detected");
    direction = DOWN;
  }
}

const char* Mario::name() {
  return "MARIO";
}