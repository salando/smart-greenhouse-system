#include "settings.h"

void setup() {
  displaySetup();
  tempSetup();
}

void loop() {
  displayUpdate();
  readTemp();

}
