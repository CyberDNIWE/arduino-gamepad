# Arduino PC/PS3 compatible gamepad

This is Arduino code for making a gamepad using an **ATmega32U4**- based board.

Tested with an [Arduino Pro Micro clone](https://www.makershop.de/plattformen/arduino/pro-micro-32u4/), works out of the box with at least PC (and supposedly PS3).

This fork is made primarily for [This not-so-slimbox 3d print project](https://www.thingiverse.com/thing:5349287), which you should check out ;)


## Why this one instead of [jfedor2's](https://github.com/jfedor2/gamepad/)?
 - Blazing fast!
   - On `Arduino Pro Micro 8Mhz @3.3V` entire update loop takes only **`230 μs`** ie **`0.230 ms`**.
   - _Note: you will most likely be bottlenecked by USB update rate (max. 1000 updates a second, ie fastest update time possible is **`1ms`**)_
 - **OOP** style: reliable and trivial to extend
   - Like the functionality but not the imlementation? Just inherit and override!
 - **Per-button debouncing** with possibility for **per-button custom timings**
   - Just edit respective `DEBOUNCE_BUT_XXX` definitions with whatever timings you want!
 - **S**.**O**.**C**.**D.** (**S**imultanious **O**pposing **C**ordinal **D**irections) cleaning strategies:
   - **`TournamentLegal`** (default): 
     - left + right = neutral
     - up + down = up
   - **`AllNeutral`**: 
     - left + right = neutral
     - up + down = neutral
   - **`LastInputPriority`**
     - left + right = right
     - right + left = left
     - up + down = down
     - down + up = up
   - **`None`**:
     - No cleaning
   - Possibility to **add your own** as  simple or as complex as you'd like!
## What is yet lacking
  - D-pad mode switch to make it act as left stick
  - ~~Switching of **S**.**O**.**C**.**D.** strategy on the fly~~

## Requirements
 - **ATmega32U4**- based board (mine was a [Arduino Pro Micro clone](https://www.makershop.de/plattformen/arduino/pro-micro-32u4/))
 - [Arduino IDE](https://www.arduino.cc/en/software) or [Platform.io](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide) plugin for [VSCode](https://code.visualstudio.com/)

## Installation
 - For `Arduino IDE`:
   - rename `main.cpp` into `main.ino` and flash
 - For `Platform.io`:
   - Open directory using Platform.io plugin and flash
> Should compile in about 2 seconds with no errors
## SOCD type selection
 - By default is **`TournamentLegal`** (resets on every reconnection)
 - To cycle between types press buttons defined by **`SOCD_STRATEGY_CYCLE_BUTTONS`**
   - **`PS`** + **`Start`** by default
   - Order of key presses doesn't matter
 - To change button combination for SOCD type cycling:
   - Navigate to line **`69`** that says **`#define SOCD_STRATEGY_CYCLE_BUTTONS &PS, &Start`**
   - List the buttons you'd like instead in similar fasion (see comment in code for more info)
     - Example new line **`69`**: **`#define SOCD_STRATEGY_CYCLE_BUTTONS &Select, &Start`**

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## Acknowledgment
> Originally forked from [jfedor2/gamepad repo](https://github.com/jfedor2/gamepad/), but departed heavliy from it.