#ap-feature/more_keyboard
This branch is focused on supporting non-ASCII keyboards as well as keyboard that use a different key layout (such as Dvorak's AZERTY layout).

##Overview of Current Functionality:
 - On startup, the 'SDLtoFS2' map (key.cpp:60) is populated with FSO's 'KEY_##' #defines.
 - This map is used during SDL_Keyboard events inside of 'op_poll()' (osapi.cpp:384~394) to translate the SDL scan codes into FSO's keycodes.
 - FSO's keycodes combine the key's scancode along with the key modifier bitmask, which namely contains "modifier" such as Ctrl, Alt, and Shift.
   - Additionally, FSO encodes joystick axes as keycodes within controlconfig.cpp by OR'ing 'JOY_AXIS' (controlconfig.cpp:125) to allow 'CC_lines[].cc_index' to contain a 'Controls_config' index or a Joystick axis index
 - In the UI textboxes that use input from the keyboard, such as the debug console, multiplayer chat messages, and the new pilot naming textboxes, the keycode is passed through 'key_to_ascii()' (key.cpp:232) to retrieve a printable character from one of two hardcoded ascii tables, 'ascii_table[]' and 'shifted_ascii_table[]' (key.cpp:62~82)
   - Since the ascii tables are hardcoded to the default keyboard layout, any custom layout the user may have defined for their system is ignored
   
##Design Goals
 - Replace FSO's keycode with a 'std::pair<SDL_Keymod, SDL_Scancode>' structure
 - Replace 'key_to_ascii()' with 'key_to_char()'
   - 'key_to_char()' will use 'SDL_GetKeyFromScancode(SDL_Scancode)' to get the correct SDL_Keycode, which can then be used with a (different) ascii table
   - Once truetype fonts are supported, 'SDL_GetScancodeName(SDL_Scancode)' might be used to get the key name directly. Requires research


##Master branch README.md
[![Coverity](https://scan.coverity.com/projects/870/badge.svg)](https://scan.coverity.com/projects/870)
[![Build Status](https://travis-ci.org/scp-fs2open/fs2open.github.com.svg?branch=master)](https://travis-ci.org/scp-fs2open/fs2open.github.com)

Welcome to Freespace Open!

In 2002 Volition released the source code for Freespace2. In the years since dedicated individuals have added countless new features to create a flexible and powerful space combat simulation engine. More info can be found on our [wiki](http://www.hard-light.net/wiki/index.php/Source_Code_Project)

People interested in contributing are encouraged to read [this thread](http://www.hard-light.net/forums/index.php?topic=82688.0)

The "projects" directory contains ready projects for:
* Microsoft Visual C++ 6, 2005, 2008, 2010, 2011, 2013, 2015
* Apple Xcode
* Code::Blocks
* Eclipse


Review the commit logs to see what's new.

See the INSTALL file for instructions on how to build fs2_open, or open the relevant MSVC project
