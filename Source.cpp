// Program: Hangman 2.0 
// =================================================== 
// Programmer: Stephen Landaas
// Creation Date: June 2020
// ===================================================

#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <cctype>
#include <iomanip>
#include <conio.h>
#include "Man.h"  // Also includes <string>
#include "Windows.h"

void generateWordList(std::vector<std::string> &wordList);
char getGuess(std::vector<char> &lettersGuessed, std::string userWord, int lives);
void update(char letterGuessed, std::string &randomWord, std::string &userWord, int &lives);
void gotoxy(int x, int y);
void ShowConsoleCursor(bool showFlag);
void cleanup(int xPos, int yPos);
int mainMenu();
void displayStatus(std::vector<char> lettersGuessed, std::string userWord, int lives);
int getDifficulty();
void showCredits();
void runGame(std::vector<std::string> &wordList, int &streak);

int main() {
	std::vector<std::string> wordList; // List to be loaded with words depending on the difficulty
	int streak = 0; // User's streak, how many games they are able to win in a row
	int menuOp = 0; // The user's menu selection (1 - 4)

	srand(time(NULL));

	while (true) {
		ShowConsoleCursor(false); // Hide user input prompt (blinking light)
		menuOp = mainMenu(); // Initiate main menu interface and return the user's choice to be stored in menuOp var.
		switch (menuOp) {
			case 1:
				ShowConsoleCursor(true); 
				runGame(wordList, streak); 
				break;
			case 2:
				generateWordList(wordList); // Read in words from selected file, store them wordList vector 
				break;
			case 3:
				showCredits();
				break;
			case 4:
				return 0;
		}

		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // Flush buffer so user can't hold down arrow keys to mess with input
	}

	return 0;
}



// Reads in words from 1 of 4 files (Easy - Expert), and stores them into the wordList vector.
void generateWordList(std::vector<std::string> &wordList) {
	std::ifstream wordsFile;
	std::string word = "";
	int difficulty = 0;

	wordList.clear(); // Clears old word list to allow new list to be read in
	
	difficulty = getDifficulty(); // Opens difficulty menu, returns difficulty level

	switch (difficulty) {
		case 1: 
			wordsFile.open("easy-words.txt");
			break;
		case 2:
			wordsFile.open("medium-words.txt");
			break;
		case 3:
			wordsFile.open("hard-words.txt");
			break;
		default: 
			wordsFile.open("expert-words.txt");
	}

	// Read in the words from the specified file
	while (!wordsFile.eof()) {
		getline(wordsFile, word);
		wordList.push_back(word); // Push each word into the wordList vector
	}

	wordsFile.close();
}



// Prompt the user to guess their letter (returns if letter is alphabetic and not already guessed)
char getGuess(std::vector<char> &lettersGuessed, std::string userWord, int lives) {
	char letterGuessed = '\0';
	bool validInput = false; // Flag to control whether or not the user should input their guess again
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	while (!validInput) {
		system("cls");
		displayStatus(lettersGuessed, userWord, lives); // Display the status of the game (letters guessed, word progress, and hanged man graphic).
		std::cout << "\n> Enter your input:  ";
		std::cin.get(letterGuessed);
		letterGuessed = toupper(letterGuessed);
		std::cin.ignore(1000, '\n');
		validInput = true;

		// Ask the user to try again if their input isn't alphabetic (a letter)
		if (!isalpha(letterGuessed)) {
			SetConsoleTextAttribute(hConsole, 12); // Make text red to show they made an error.
			std::cout << "\n  Try again!\n";
			Sleep(2000);
			validInput = false;
		}
		else {
			// Verify that the letter hasn't already been guessed and isn't in the lettersGuessed vector
			for (int i = 0; i < lettersGuessed.size(); i++) {
				if (lettersGuessed[i] == letterGuessed) {
					SetConsoleTextAttribute(hConsole, 12);
					std::cout << "\n  You already guessed this input!\n";
					Sleep(2000);
					validInput = false;
					break;
				}
			}
		}
	}

	return letterGuessed;
}



// Fill in the blanks of the userWord variable, update the user's status in how far they are in completing the word.
void update(char letterGuessed, std::string &randomWord, std::string &userWord, int &lives) {
	bool correct = false; // Determines if the user's guess is correct or incorrect

	/* Loop through the random word, check if the letter the user guessed is in our word.
	   If it is, flag that the user's guess is correct, and update the userWord (which contains blanks), and fill in each blank
	   with the properly guessed letter, thus updating the user's status. */
	for (int i = 0; i < randomWord.length(); i++) {
		if (randomWord[i] == letterGuessed) {
			correct = true;
			userWord[i] = randomWord[i];
		}
	}

	if (correct == false) {
		lives--; // Deduct a life from the user if their guess is incorrect.
		Beep(250, 100); // Play a low tone to signal that they're wrong.
	}
	else {
		Beep(650, 100); // Play a higher tone to signal that they are correct.
	}
}



// Helper function, places the cursor for input/placing down characters anywhere on the console screen.
void gotoxy(int x, int y) {
	static HANDLE h = NULL;
	if (!h)
		h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD c = { x, y };
	SetConsoleCursorPosition(h, c);
}



// Helper function, toggles the user's input blinking (turns off during menu screens)
void ShowConsoleCursor(bool showFlag) {
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}


/* Another helper function, essentially, the arrow movement works by going to a certain (x,y) position, and placying a '>' 
  character there. However, when the user moves downward or upward, a '>' is left at wherever their cursor previously was.
  This function will place several blank spaces at the spots above and below the user's cursor, to "clean up" the cursor
  marks from where the user previously was. This gives the illusion that the user's cursor is moving. The highest cursor position
  is 8, and the lowest cursor position is 14. The cursor moves by increments of 2. */
void cleanup(int xPos, int yPos) {
		gotoxy(xPos, yPos - 2); // Erase the cursor at the position above (if applicable).
		std::cout << "   "; 
		gotoxy(xPos, yPos + 2); // Erase the cursor at the position below (if applicable).
		std::cout << "   ";
}



// Main Menu GUI, generates a main menu box and allows the user to pick their option. Returns an int (1-4) depending on the player's selection..
int mainMenu() {
	int keyPress = 0; // Integer value of the current key pressed 
	int option = 1; // User's current option, starting at 1.
	int yPos = 8; // User's current vertical (y) position. Highest (closest to the top of the screen) is 8, and the lowest (farthest down) is 14.
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	system("cls");

	// Generate the menu GUI
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << std::endl << std::endl << std::endl << std::endl << std::endl;
	std::cout << std::setw(56) << "\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB";
	SetConsoleTextAttribute(hConsole, 142);
	std::cout << " MAIN MENU ";
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 142);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 142);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 142);
	std::cout << "       1. Play Hangman (2.0)         ";
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 142);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 142);
	std::cout << "       2. Set Difficulty             ";
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 142);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 142);
	std::cout << "       3. View Credits               ";
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 142);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 142);
	std::cout << "       4. Exit (Close Program)       ";
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 142);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 142);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 142);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 142);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 142);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 142);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(81) << "\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB";
	std::cout << std::endl << std::endl;
	std::cout << std::setw(98) << "USE YOUR ARROW KEYS TO MOVE THE CURSOR, HIT [ENTER] TO SELECT AN OPTION";
	std::cout << std::endl << std::endl << std::endl;

	// Begin process of displaying user's cursor (arrow) on the screen.
	SetConsoleTextAttribute(hConsole, 143); 
	gotoxy(45, yPos); // Put cursor on first menu option (x,y) = (45,8)
	std::cout << "-->";

	// While the user hasn't pressed the enter key (code 13), continuously wait for input and allow them to move their arrow using up/down
	while (keyPress != 13) {
		keyPress = _getch(); // Get the code for whatever key the user is currently pressing.

		switch (keyPress) {
			case 72: // 72 = arrow code for UP key
				// Only move up (subtract from yPos) if the user isn't at the highest position (8).
				if (yPos > 8) {
					yPos -= 2; // Move the arrow up (shifts it up by 2).
					cleanup(45, yPos); // Remove arrows above and below (if applicable)
					gotoxy(45, yPos); 
					std::cout << "-->"; // Place the arrow at the selected (x,y) position.
					Beep(250, 70); // Beep to indicate movement.
					option--; // Subtract 1 from the user's option.
				}
				break;
			case 80: // 80 = arrow code for DOWN key
				// Only move down (add to yPos) if the user isn't at the lowest position (14).
				if (yPos < 14) {
					yPos += 2; // Move the arrow down (shifts it down by 2).
					cleanup(45, yPos); 
					gotoxy(45, yPos);
					std::cout << "-->"; 
					Beep(250, 70);
					option++; // Add 1 to the user's option.
				}
				break;
			}
	}

	// Reset position and text color to standard.
	gotoxy(0, 26);
	SetConsoleTextAttribute(hConsole, 15);

	return option;
}


/* Displays the current game progress of the user. Includes the full list of letters guessed, the status of their potential word,
   and a graphical depiction of their progress (the hanging man), generated by the 'lives' they have left. */
void displayStatus(std::vector<char> lettersGuessed, std::string userWord, int lives) {
	std::string str = " LETTERS GUESSED: {"; // Letters guessed string, text representation of the vector.
	int width = 0; // Dimension used to keep our letters guessed status and word status centered
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	SetConsoleTextAttribute(hConsole, 143);

	// Move everything from the lettersGuessed vector into our string (str).
	for (int i = 0; i < lettersGuessed.size(); i++) {
		if (i != lettersGuessed.size() - 1) {
			str.push_back(lettersGuessed[i]);
			str.append(", ");
		}
		else {
			str.push_back(lettersGuessed[i]);
		}
	}

	str.append("} ");

	// Create a dimension based off of the current length of our letters guessed display.
	width = (60 - str.length() / 2);

	// Create the borders of the letters guessed box...
	// Top border
	SetConsoleTextAttribute(hConsole, 4);
	gotoxy(width, 1);
	for (int i = 0; i < str.length() + 2; i++) {
		std::cout << "\xDB";
	}

	// Sides and middle fill
	gotoxy(width, 2);
	std::cout << "\xDB";
	SetConsoleTextAttribute(hConsole, 143);
	std::cout << str;
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "\xDB";

	// Bottom border 
	gotoxy(width, 3);
	for (int i = 0; i < str.length() + 2; i++) {
		std::cout << "\xDB";
	}

	// Display the graphical representation of the hanging man
	SetConsoleTextAttribute(hConsole, 15);
	std::cout << std::endl << std::endl << MAN[6 - lives];

	// Update our dimension based off of the length of the user's current word progress and keep it centered.
	width = (60 - ((userWord.length() * 2 - 1) / 2));

	// Create border of the user's current word status.
	// Top border
	SetConsoleTextAttribute(hConsole, 11);
	gotoxy(width - 1, 19);
	for (int i = 0; i < userWord.length() * 2 + 3; i++) {
		std::cout << "\xDB";
	}

	// Sides and middle fill
	gotoxy(width - 1, 20);
	SetConsoleTextAttribute(hConsole, 143);
	for (int i = 0; i < userWord.length() * 2 + 3; i++) {
		if ((i == 0) || (i == userWord.length() * 2 + 2)) {
			SetConsoleTextAttribute(hConsole, 11);
		}
		else {
			SetConsoleTextAttribute(hConsole, 8);
		}
		std::cout << "\xDB";
	}
	
	gotoxy(width - 1, 21);
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "\xDB";
	SetConsoleTextAttribute(hConsole, 143);
	std::cout << ' ';
	for (int i = 0; i < userWord.length(); i++) {
		std::cout << userWord[i];
		if (i != userWord.length() - 1) {
			std::cout << ' ';
		}
	}
	std::cout << ' ';
	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "\xDB";

	gotoxy(width - 1, 22);
	SetConsoleTextAttribute(hConsole, 143);
	for (int i = 0; i < userWord.length() * 2 + 3; i++) {
		if ((i == 0) || (i == userWord.length() * 2 + 2)) {
			SetConsoleTextAttribute(hConsole, 11);
		}
		else {
			SetConsoleTextAttribute(hConsole, 8);
		}
		std::cout << "\xDB";
	}

	// Bottom border
	gotoxy(width - 1, 23);
	SetConsoleTextAttribute(hConsole, 11);
	for (int i = 0; i < userWord.length() * 2 + 3; i++) {
		std::cout << "\xDB";
	}
}



// Menu GUI to take in the desired level of difficulty from the player.
int getDifficulty() {
	int keyPress = 0; // Integer value of the current key pressed 
	int option = 1; // User's current option (1-4), determines which difficulty is selected
	int yPos = 8; // The y-position of the user's cursor (arrow). Starting value of 8, highest value is 14.
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	system("cls");

	// Generate the difficulty menu GUI
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << std::endl << std::endl << std::endl << std::endl << std::endl;
	std::cout << std::setw(54) << "\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB";
	SetConsoleTextAttribute(hConsole, 143);
	std::cout << " SET DIFFICULTY ";
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 143);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 143);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 143);
	std::cout << "             1. Easy                 ";
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 143);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 143);
	std::cout << "             2. Medium               ";
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 143);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 143);
	std::cout << "             3. Hard                 ";
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 143);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 143);
	std::cout << "             4. Expert               ";
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 143);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 143);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 143);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 143);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 143);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(43) << "\xDB";
	SetConsoleTextAttribute(hConsole, 143);
	std::cout << "                                     ";
	SetConsoleTextAttribute(hConsole, 4);
	std::cout << "\xDB" << std::endl;

	std::cout << std::setw(81) << "\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB";
	std::cout << std::endl << std::endl;

	SetConsoleTextAttribute(hConsole, 15);
	std::cout << std::setw(94) << "NOTE: DIFFICULTY IS BASED OFF OF READING LEVEL, NOT WORD LENGTH";
	std::cout << std::endl << std::endl << std::endl;

	// Once the difficulty GUI menu box is created, place the arrow/cursor for the user to make their selection
	SetConsoleTextAttribute(hConsole, 143);
	gotoxy(51, yPos);
	std::cout << "-->";

	// While the enter key hasn't been pressed, allow the user to move their arrow with the UP/DOWN arrow keys.
	while (keyPress != 13) {
		keyPress = _getch(); // Receive an input key from the user

		switch (keyPress) {
		case 72: // UP arrow key
			if (yPos > 8) { // If the user isn't as far up as possible, allow their cursor to move upward (-2 in y)
				yPos -= 2;
				cleanup(51, yPos); // Remove arrows next to surrounding options if applicable
				gotoxy(51, yPos);
				std::cout << "-->"; // Place down arrow that indicates the current selection
				Beep(250, 70); // Beep to indicate movement
				option--; // Deduct 1 from the user's current option
			}
			break;
		case 80: // DOWN arrow key
			if (yPos < 14) {  // Allow the user to move downward if they aren't at the maximum yPos (14)
				yPos += 2;
				cleanup(51, yPos);
				gotoxy(51, yPos);
				std::cout << "-->";
				Beep(250, 70);
				option++;
			}
			break;
		}
	}

	// Reset (x,y) position and text color.
	gotoxy(0, 26);
	SetConsoleTextAttribute(hConsole, 15);

	return option;
}



// Function to display the credits of the game (programmer, start date, and completion date)
void showCredits() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	system("cls");

	/* Starts the cursor at a low position on the console, outputs the following text, clears the screen,
	   then moves the yPos of the cursor further up the screen (by subtracting 1 from the yPos var.) and 
	   re-outputs the same information, giving the appearance of text crawling upward. */
	for (int yPos = 22; yPos >= 0; yPos--) {
		gotoxy(10, yPos);
		SetConsoleTextAttribute(hConsole, 11);
		std::cout << std::setw(65) << "PROGRAMMER: Stephen Landaas\n";
		gotoxy(10, yPos + 2);
		std::cout << std::setw(63) << "START DATE: 06/14/2020\n";
		gotoxy(10, yPos + 4);
		std::cout << std::setw(66) << "COMPLETETION DATE: 06/25/2020\n";
		Sleep(450);
		system("cls");
	}
}



/* Function that is initiated when the user chooses to play the game from the menu. Will continuously run until the player is 
   out of lives, or they are able to win the game. */
void runGame(std::vector<std::string> &wordList, int &streak) {
	std::vector<char> lettersGuessed; // Vector that stores which letters have been guessed.
	std::string randomWord = ""; // Word to be selected from the wordList vector (which consists of words imported from the respective file)
	std::string userWord = ""; // The user's current word (will begin as blanks and will be filled out as the game runs)
	char letterGuessed = '\0'; // Letter that the user has guessed
	int lives = 6; // Number of lives remaining (graphical representation in Man.h

	system("cls");
	
	/* If the user hasn't selected a difficulty (and thus has a respective word list with a size of zero, run the generateWordList
	   function, which will open the difficulty menu and create a word list */
	if (wordList.size() == 0) {
		ShowConsoleCursor(false);
		generateWordList(wordList);
		ShowConsoleCursor(true);
		system("cls");
	}

	// Select a random word from our wordList vector
	randomWord = wordList[rand() % wordList.size()];
	userWord = randomWord;  // Assign the userWord a copy of randomWord so they have the same length

	// Replace all letters in userWord with blanks, for a blank starting template for the player to fill in with their guesses.
	for (int i = 0; i < userWord.length(); i++) {
		userWord[i] = '_';
	}

	while (lives > 0) {
		letterGuessed = getGuess(lettersGuessed, userWord, lives); // Get the player's guess
		lettersGuessed.push_back(letterGuessed); 
		update(letterGuessed, randomWord, userWord, lives); // Update the game status (letters guessed, hanged man graphic, word progress

		// Check if the user has filled their word out completely
		if (userWord == randomWord) {
			streak++; // Increment the streak if they won
			displayStatus(lettersGuessed, userWord, lives); // Show the final game status before a new round begins.
			std::cout << "\n\n\n  YOU WIN!!!";
			std::cout << "\n\n  CURRENT STREAK: " << streak << std::endl;
			
			// Do a fun green flicker display to show that the player has won!
			for (int i = 0; i < 8; i++) {
				system("Color 0a");
				Beep(500, 150); Sleep(100);
				Sleep(100);
				system("Color 0f");
				Sleep(100);
			}
			break;
		}
		else if (lives <= 0) {
			streak = 0; // Reset the player's streak to 0 if they lost.
			displayStatus(lettersGuessed, userWord, lives); // Show the final game status before a new round starts.
			std::cout << "\n\n\n  YOU LOSE";
			std::cout << "\n\n  CURRENT STREAK: " << streak << std::endl;

			// Do a red flicker display to show that the player has lost the game.
			for (int i = 0; i < 8; i++) {
				system("Color 0c");
				Beep(300, 150); Sleep(100);
				Sleep(100);
				system("Color 0f");
				Sleep(100);
			}
			break;
		}
	}
}