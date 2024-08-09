#ifndef DECKOFCARDS_H
#define DECKOFCARDS_H

#include "HttpClient.h"
#include "json.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;
using json = nlohmann::json;

class InvalidEntry {};

struct DrawCard {

	string value;		// Card Face Value: JOKER < ACE < 2-10 < JACK < QUEEN < KING
	string suit;		// [ S(1), C(2), D(3), H(4) ] //UPDATE: Erased SUIT enum, made into string
	string code;		// For printing purposes -- Ended up not being particularly necessary, but just adds information for the user to see

	//Converts Value String to Numeric Value for comparison
	uint16_t GetNumericValue() const;

	//Converts Suit String to Numeric Value for comparison
	uint16_t GetSuitValue() const;

	//Assignment Operator for DrawCard objects
	DrawCard& operator= (const DrawCard& other);

	// Overload insertion "<<" operator
	// When a DrawCard object is output, this func is called
	friend ostream& operator<<(ostream& osObject, const DrawCard& draw);

};

//DeckOfCards is declared here. This is the main class, and the parent that both minigames inherit off of. It itself inherits from HttpClient, and
	//overall is inspired and designed from the work we did in week 12 using the UnsplashClient and **Photos object
class DeckOfCards : public HttpClient {

private: 
//vars

	vector<char> jsonData;
	size_t cardsDealt;

//funcs

	uint16_t GetMaxRangeVal(const uint16_t nCards) const;

// funcs // Overridden from HttpClient.h

	//takes in all the info from DeckOfCards API and fills a vector with each individual char
	void StartOfData(const char arrData[], const unsigned int iSize) override;

	// Parse the jsonData char vector, and fill all of the DrawCard objects with the requisite data using the nlohmann parser
	void EndOfData() override;

protected: 
//vars

	//UPDATE: Also made Card protected. Holds all cards drawn
	DrawCard** Card;

//funcs

	// Constructor
	DeckOfCards() : cardsDealt{ 0 }, Card{ nullptr } {
		jsonData.clear();
	}

	// Destructor, very similar to week 12
	~DeckOfCards() {
		for (uint16_t i = 0; i < cardsDealt; i++) {
			delete Card[i];
		}

		delete[] Card;
	}

	//Returns total number of cards drawn to children classes when called after EndOfData() has finished executing
	//	otherwise, will return 0;
	size_t GetCardsDealt() const {
		return cardsDealt;
	}

	//This function is called in both games, and is itself a loop ensuring the user inputs a valid entry 
	// via exception handling, so it was outsourced here 
	//GPG() takes in an int containing the number of remaining guesses a user has 
	// to guess the total value of the cards they drew, added together
	//UPDATE: Keep nCards as a parameter to allow flexible use of this function from calls originating in either child class
	uint16_t GetPlayerGuess(const uint16_t remainingGuesses, const uint16_t nCards) const;

	// Each *child* class must state their intro mode level with rules
	virtual void GetRules() const = 0; 

	// Each *child* class must have a master function that run's its respective game
	virtual void RunGame() = 0; 

	// Each *child* class must have a master function that prints the final cards
	virtual void PrintResults() const = 0; 

};

#endif //DECKOFCARDS_H