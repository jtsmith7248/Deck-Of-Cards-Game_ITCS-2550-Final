#pragma once
#include "DeckOfCards.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////  GUESSING GAME CLASS  //////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

// Also referred to as Mini Game 1; Given actual name to make noticing it easier
class GuessingGame : public DeckOfCards
{
	// Private by default
	bool correct;

public:

	//Constructor
	GuessingGame() : correct{ false } {}

	// Intro to Easy Mode game; replacing pure virtual
	void GetRules()
	{
		cout << endl << setfill(' ') << setw(34) << right << " [EASY MODE]: " << cardsDealt << " CARD IS DRAWN" << endl
			<< "" << setfill(' ') << setw(60) << right << "You get 5 attempts to guess the correct card number." << endl << endl;
	}

	// Draw card at index 1 (default value)
	DrawCard* DrawHand(int count) {
		return Card[count];
	}

	virtual void RunGame()
	{
		uint8_t answer = GetIntValue(0); //TODO: ADD CATCH BLOCK
		int remainingGuesses = 5;
		bool guessedCorrectly = false;
		string loser = " YOU LOST! BETTER LUCK NEXT TIME! ";
		string winner = " YOU GOT IT RIGHT! ";
		//this loop takes the users guesses for the total value, ensures they're valid entries using GPG(), and outputs info based on the guess
		for (int i = 0; i < 5; i++)
		{
			//the int containing number of remaining guesses is sent into G2_GPG(), and that function returns a string containing just a number. That num is 
				//then converted into a string and compared against gameValue
			int guessValue = GetPlayerGuess(remainingGuesses, 1);

			if (guessValue == answer) //gets the int value for card 0, the only card drawn
			{
				cout << endl;
				cout << setfill('*') << "" << right << setw(46) << winner << setfill('*') << setw(28) << "" << endl << endl;
				cout << setfill(' ') << "The Correct Answer Was " << answer << "!" << endl << endl;
				guessedCorrectly = true;
				break;
			}
			else if (guessValue > answer)
				cout << "Too High! Guess Again!" << endl << endl;
			else
				cout << "Too Low! Guess Again!" << endl << endl;

			remainingGuesses--;
		}

		if (!guessedCorrectly)
		{
			cout << endl;
			cout << setfill('*') << "" << right << setw(54) << loser << setfill('*') << setw(20) << "" << endl << endl;
			cout << setfill(' ') << "The Correct Answer Was " << answer << "!" << endl << endl;
		}
	}

	void PrintResults() {
		cout << setfill('*') << "" << right << setw(46) << " PLAYER CARD DRAWN: " << setfill('*') << setw(28) << "" << endl << endl;
		cout << setfill(' ') << setw(10) << right << "1) " << *Card[0] << endl;
	}
};