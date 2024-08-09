#ifndef GUESSINGGAME_H
#define GUESSINGGAME_H

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

public:

	//Constructor
	GuessingGame() {}

	// Intro to Easy Mode game; replacing pure virtual
	void GetRules() const override;

	//The brain of Minigame 1
	void RunGame() override;

	//Outputs the single drawn card
	void PrintResults() const override;
};

#endif // GUESSINGGAME_H