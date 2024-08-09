#ifndef LUCKOFTHEDRAWGAME_H
#define LUCKOFTHEDRAWGAME_H

#include "DeckOfCards.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;

///////////////////////////////////////////////////////////////////
//////////////////// COMPARE STRUCTS //////////////////////////////
///////////////////////////////////////////////////////////////////


//These Compare Structs rely on eachother in occasional rare situations, so they had to be defined in this way to avoid issues with circular dependancy
	//If the user decides to have the cards sorted by suit or by value, bubble sort will swap cards based on the respective compare object. However, in the case
	//we're comparing by suits and the two cards being compared have the same suit, the idea was to instead sort just those two cards by value. We also wanted it
	//to work in the opposite way- if two cards are the same value, ie a Jack and a Jack, we want them sorted by the hierarchy of their suits. So in both compare functions, 
	//if the values are equal, the other compare struct is called. This caused a problem, however, because now each compare function in its respecive struct
	//technically relied on the other!

//The way this was solved was by declaring each struct piece by piece, so that each struct was aware of the other and could access the function being called without 
	//the compiler failing to understand where to go. First the concept of CompareValues is declared, then the definition of CompareSuits, then the definition of 
	//CompareValues, then finally the actual code inside of each compare function. This allows the compiler to understand where to go when each function is called
	//within the other.

//We want to be careful with the way this is coded now, because an infinite loop is possible if two cards are identical in both value and suit- CompareSuit would call
	//CompareValue would call CompareSuit would call...
	//However! Only one deck is used in this game, so it is impossible for a user to end up getting two perfectly identical cards, and the loop can not be triggered

struct CompareSuits {
public:
	static bool Compare(DrawCard* c1, DrawCard* c2);
};

struct CompareValues {
public:
	static bool Compare(DrawCard* c1, DrawCard* c2);
};

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
////////////////////  HARD MODE CLASS  ////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


//Here we declare the Minigame 2 class, aka Hard Mode, aka LuckOfTheDrawGame (named to make more distinctive in code)
//This contains functions specific to the mechanics of game 2, namely the SearchforJoker() function and 
//the GetTotalValue() of all cards function
class LuckOfTheDrawGame : public DeckOfCards {

private:
	//Two public strings here, each will contain card information. Because at the end of the game three seperate versions of the cards are output to
		//the user (The cards before the Joker(s) are replaced, the cards after the Joker(s) are replaced, and finally the cards properly sorted),
		//it is using these strings that we save a snapshot of what those cards looked like at a given time, so that the user can view them later on.
	//UPDATE: Made private, moved from DeckOfCards
	vector<string> PreJokerSearchOutput;
	vector<string> PostJokerSearchOutput;
	const size_t numCardsRequested;


public:

	LuckOfTheDrawGame() : numCardsRequested{ 0 } {
		PreJokerSearchOutput.clear();
		PostJokerSearchOutput.clear();
	}

	void SearchForJoker();

	//Overwritten from HttpClient so we can place number of cards dealt into const var numCardsRequested after json parsing 
	bool Get(const string uri, const map<string, string> qp = {}) override;

	//Overwritten from DeckOfCards.h
	void GetRules() const override;

	//Overwritten from DeckOfCards.h
	void RunGame() override;

	//Overwritten from DeckOfCards.h
	void PrintResults() const;

private:

	//SaveOutput saves the cards as they are before and after looking for Jokers into two separate vectors of strings
	// based on which bool value is sent in
	// Called only in SearchForJoker()
	void SaveOutput(bool preJokerOutput);

	// Sorting the order of suits and values(or numbers)
	// Textbook bubblesort problem, with added template that compares the cards either by suit or by value
	// Called only from RunGame
	template <typename T>
	void BubbleSort() {
		T compareClassObject{};
		uint16_t nCards = static_cast<uint16_t>(numCardsRequested);

		for (uint16_t i = 0; i < nCards - 1; i++) {
			for (uint16_t j = 0; j < nCards - i - 1; j++) {
				if (compareClassObject.Compare(Card[j], Card[j + 1])) {
					DrawCard tempCard = *Card[j];
					*Card[j] = *Card[j + 1];
					*Card[j + 1] = tempCard;
				}
			}
		}
	}

};

#endif // LUCKOFTHEDRAWGAME_H