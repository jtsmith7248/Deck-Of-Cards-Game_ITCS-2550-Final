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
	int numCards;
	bool correct;

public:

	//Constructor
	GuessingGame() {}

	// Intro to Easy Mode game; replacing pure virtual
	void GetRules()
	{
		cout << endl << setfill(' ') << setw(34) << right << " [EASY MODE]: " << numCards << " CARD IS DRAWN" << endl
			<< "" << setfill(' ') << setw(60) << right << "You get 5 attempts to guess the correct card value." << endl << endl;
	}

	// VIRTUAL: Takes in a spot in the index 'count' and returns the Card object at that point
	int GetCardCount(const int numberOfCards) {
		numCards = numberOfCards;
		return numCards;
	}

	// Draw card at index 1 (default value)
	DrawCard* DrawHand(int count) {
		return Card[count];
	}

	//This function adds together all of the numeric values from the user-requested cards 
	int GetTotalValue()
	{
		int value = 0;

		//to turn it from a string into an int to be added to the total
		for (int i = 0; i < numCards; i++)
			value += GetNumber(i);

		return value;
	}


	// Uses overloaded insertion "<<" operator in DrawCard struct to stream output of DeckOfCards class
	friend ostream& operator<<(ostream& osObject, const GuessingGame& gg)
	{
		// Only has 1 card as option for easy mode
		osObject << setfill('*') << "" << right << setw(46) << " PLAYER CARD DRAWN: " << setfill('*') << setw(28) << "" << endl << endl;
		osObject << setfill(' ') << setw(10) << right << "1) " << *gg.Card[0] << endl;
		return osObject;
	}

};