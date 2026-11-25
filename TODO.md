# Fixes before Thursday's presentation

## Must fix (these will break the demo)

- [x] **Vent servo and grow light are both on D3.** Grow light moved to D6 in code. Still to do: rewire it to D6 and update the pin map in the Run Procedure.
  `VENT_SERVO_PIN = 3` in `vent.ino:5` and `ledPin = 3` in `light_system_sketch_v2.ino:4`.
  The light code fights the servo signal. Move one to a free pin (D5, D6 or D8), rewire it, and update the pin map.
- [x] **The logger connects to the wrong IP address.**
  The board uses `172.20.10.9` (`network.ino:19`), but the logger connects to `172.20.10.2` (`logger/wifi_test.cpp:72`).
  Change the logger to `172.20.10.9`.
- [ ] **The alarm timings don't match the Run Procedure.**
  `ALARM_DELAY_MS = 900000` (15 min) and `MUTE_MS = 600000` (10 min) in `vent.ino:9-10`.
  The procedure says 60 s and 5 min. Change them to `60000` and `300000`, or the buzzer never sounds during the demo.
- [x] **If the hotspot isn't up, the whole greenhouse is dead.** Fixed: gives up after 15 s and runs without the logger.
  `netSetup()` waits forever for WiFi (`network.ino:25`), so the fan, display and everything else never start.
  Add a timeout of about 15 s so the greenhouse still runs without WiFi.
- [x] **The hotspot name and password are still placeholders.**
  `network.ino:7-8`. Set them on the demo laptop, and don't commit the real password to GitHub.
- [ ] **The Servo library isn't in the setup steps.**
  Every laptop that uploads the sketch needs it installed: Arduino IDE → Library Manager → "Servo".

## Should fix

- [ ] **The grow light may flicker on and off.**
  If the LED shines on the light sensor, it reads "bright" and switches itself off, then back on 5 s later (`light_system_sketch_v2.ino:32-43`).
  Shield the sensor from the LED, or add a margin above the threshold before turning the light off.
- [ ] **"TOO COLD 0.0C" flashes at startup.**
  `climateSetup()` never copies its first reading into `sysStatus.tempVal` (`climate.ino:49`).
  Add `sysStatus.tempVal = climateTempC;` after the first reading.
- [ ] **Check that an unplugged temperature sensor is detected.**
  `climate.ino:24` only flags an error when the reading is exactly 0 or 1023. An unconnected pin may read random values instead.
  Unplug it and confirm SENSOR ERROR appears. If it doesn't, widen the check.
- [ ] **The home screen doesn't show fan, vent or grow-light status,** and the light number has no label (`display.ino:115-126`).
- [ ] **Remove leftover code:**
  - `sunlightSeconds` and `displayLight()` in `light_system_sketch_v2.ino` are never used.
  - `settings.profile` is never used; `currentProfile` in `display.ino` is used instead.
  - `lighttime` is counted but never shown or logged. Either show it or remove it.

## Docs and housekeeping

- [ ] **Update the Run Procedure:**
  - [ ] Remove `fake_board.cpp`. It isn't in the repo.
  - [ ] Add the vent servo to the pin map, on its new pin.
  - [ ] Describe the vent warning: the servo opens, as well as the orange backlight.
  - [ ] Add installing the Servo library to the software steps.
  - [ ] The menu opens on any press, not only a long press. Fix the text or the code so they agree.
  - [ ] Check that the alarm timings match the code once they're changed.
- [ ] **Make the plant profiles match.**
  `tests/climate_control` uses Seedlings/Herbs/Cactus, and the main sketch uses Basil/Lettuce/Tomato. The slides and proposal should match whichever one you keep.
- [ ] **Write a proper README** with what the project is, the pin map, and how to run it. Right now it's one unfinished sentence.
- [ ] **Remove `.DS_Store` from the repo** and add a `.gitignore`.
- [ ] **`logger/com.serial.cpp` hard-codes `COM4`.** Note this in the procedure, or remove the file if you no longer need it.
