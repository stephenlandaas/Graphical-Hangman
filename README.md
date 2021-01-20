# Hangman-2.0
An updated version of my old Hangman game, entirely console-based & written in C++ with a rudimentary graphical interface.

Written by an aspiring Computer Science student in Visual Studio using C++ and basic Windows Console API functions for setting colors, toggling the user cursor, and so on.
This program will run in a Windows environment and should be compiled and executed using Visual Studio (although slight changes might need to be made for new versions of VS) as this code ages.

All graphics of this game are created through ASCII art without any special graphical libraries being used other than the "Windows.h" library which only served to set colors and move the cursor around the console screen. This game is suited for a console of size 120 x 30, other sizes may result in off-centered graphics or unreadable text.

4 text files are provided alongside the code. Each of these files stores 50 words each (200 total words), where each file contains a selection of words based off of their difficulty. Users can feel free to add whatever words they wish to these text files, and they will successfully function within the game.
