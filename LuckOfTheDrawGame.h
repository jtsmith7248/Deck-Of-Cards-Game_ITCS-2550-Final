#pragma once
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

struct CompareValues;

struct CompareSuits
{
public:
	static bool Compare(DrawCard* c1, DrawCard* c2);
};

struct CompareValues
{
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
class LuckOfTheDrawGame : public DeckOfCards
{
	// Private by default
	int numCards;
	vector<string> saveResult;
	map<int, vector<string>> cardResults;
	map<int, vector<string>> savePreJokerResults;


public:

	LuckOfTheDrawGame() {}

	// VIRTUAL: Takes in a spot in the index 'count' and returns the Card object at that point
	int GetCardCount(const int numberOfCards) {
		numCards = numberOfCards;
		return numCards;
	}

	// Intro to Hard Mode game; replacing pure virtual
	void GetRules()
	{
		cout << endl << setfill(' ') << setw(34) << right << " [HARD MODE]: " << numCards - 2 << " CARDS ARE DRAWN" << endl
			<< "" << setfill(' ') << setw(60) << right << "You get 5 attempts to guess the correct card number." << endl << endl;
	}

	//OutputToVector takes the information that would usually be output to the screen and instead reads it into a vector to be read later
	void OutputToVector(const DrawCard& draw)
	{
		saveResult.push_back(draw.suit);
		saveResult.push_back(draw.value);
		saveResult.push_back(draw.code);
	}

	//SaveOutput is the function, called in main() via a Hard Mode/Minigame 2 object that saves the cards as they are 
		//at a given moment into either map<int,vector<string>> cardResults or savePreJokerResults, based on which bool value is sent in
	void SaveOutput(bool preJokerOutput)
	{
		DrawCard* draw = *Card;
		string output;
		string num;

		//Note that instead of i < cardsDealt, its i < (numCards - 2)
			//This is because, later on in the code, we need to replace up to two jokers if they're found in the user requested cards.
			//To do this, the API is set to always return two more cards than the user actually asked for, so that those two bonus cards can 
			//replace a joker or two if they're found. So, regardless of what happens in SearchForJoker(), we only want the cards the user 
			//requested to be added to the map vector and output to the screen. The final two bonus cards are to always stay hidden from the user.


		for (int i = 0; i < (numCards - 2); i++)
		{
			OutputToVector(*Card[i]);

			// The map<int, vector<string>> receives the data from saveResult vector and places it in index i.
			// Every loop creates a new vector called from OutputToVector(), places it in cardResults map with a key int,
			//and deletes it for the next vector loop.
			cardResults[i] = saveResult;

			saveResult.clear();				// Clears results to add new vector of saveResult to next iteration of cardResults
			saveResult.shrink_to_fit();		// Frees memory space taken by saveResult
		}

		// Make a copy if it's the PreJoker Output, then clear to avoid copy for next Joker Output.
		if (preJokerOutput) {
			savePreJokerResults = cardResults;
			cardResults.clear();
		}
	}

	// This function is where the output of savePreJokerResults and cardResults is looped through and displayed
	void GetCardResults(bool results)
	{
		map<int, vector<string>> getCardResults;
		if (results == true) {
			cout << setfill('*') << "" << right << setw(46) << " CARDS BEFORE JOKER SEARCH: " << setfill('*') << setw(28) << "" << endl << endl;
			getCardResults = savePreJokerResults;
		}
		else {
			cout << setfill('*') << "" << right << setw(46) << " CARDS AFTER JOKER SEARCH: " << setfill('*') << setw(28) << "" << endl << endl;
			getCardResults = cardResults;
		}
		for (auto const& resultCount : getCardResults) {
			cout << setfill(' ') << setw(10) << right << resultCount.first + 1 << ") ";
			for (int i = 0; i < resultCount.second.size(); i++) {
				cout << setfill(' ') << setw(2) << left << "Code: " << resultCount.second[i++] << setw(4) << right << "\t"
					<< "Value: " << resultCount.second[i++] << setw(2) << left << "\t"
					<< "Suit: " << resultCount.second[i++] << setw(2) << left << endl;
			}
			cout << endl;
		}
	}

	//This function is called both in BubbleSort and in SearchForJoker()
	//It simply replaces all the information in Card 1 with the info in Card 2
	void SetNewCardEntry(int c1, int c2)
	{
		Card[c1]->code = Card[c2]->code;
		Card[c1]->number = Card[c2]->number;
		Card[c1]->suit = Card[c2]->suit;
	}

	//Here is the aformentioned SearchForJoker()
	void SearchForJoker()
	{
		bool jokerFound = false;
		int quantityJokersFound = 0;

		//we loop through all user requested cards (aka all cards minus the two bonus cards) first 
		for (int i = 0; i < (numCards - 2); i++)
		{
			//we look at each individual card's Value and see if that card is a Joker
			if (GetValue(i) == "JOKER")
			{
				//if it is, we output that a joker was found to the screen, and change the bool value jokerFound to ensure the 
					//'no joker found' message is ignored
				cout << setfill(' ') << right << setw(40) << " JOKER FOUND " << setw(20) << endl;

				jokerFound = true;
				//This if is in place to catch a very rare situation. Remember, the deck has two jokers. Usually, if one of the user requested cards
					//is a joker, it gets replaced with the first bonus card (ie Card[numCards - 2]). However, what if a Joker exists in the user
					//requested cards, AND the first bonus card is ALSO a Joker? In that situation, this if structure increments 
					//quantityJokersFound, so that in the next line, Card[i] (the Joker found somewhere in the user-requested cards) is replaced 
					//not by the first bonus card, but the second!
						//Note: numCards is the total number of cards- if the user requested 5 cards, numCards will be 7, to account for the extra 
						//cards drawn to replace potential Jokers. Very Importantly- numCards is NOT THE INDEX! The index is going to, in this 
						//hypothetical, run from 0-6, not from 1-7. Card[numCards-1] is the last entry in the Card object, aka the second bonus card, and 
						//Card[numCards-2] is the second to last entry in the Cards object, aka the first bonus card. So, when quantityJokersFound increments
						//to 1 in the aforementioned hypothetical, (i) and (numCards-1) are sent into SetNewCardEntry()
				if (GetNumber((numCards - 2) + quantityJokersFound) == "JOKER")
					quantityJokersFound++;
				SetNewCardEntry(i, ((numCards - 2) + quantityJokersFound));
				quantityJokersFound++;
			}
		}

		if (!jokerFound)
			cout << setfill(' ') << right << setw(41) << " NO JOKER DRAWN " << setw(20) << endl;
	}

	// Sorting the order of suits and values(or numbers)
	//Textbook bubblesort problem, with added template that compares the cards either by suit or by value
	template <typename T>
	void BubbleSort()
	{
		T compareClassObject{};
		bool ascending = false;
		//Again, note the 'numCards - 2', as explained in the comments in SaveOutput()
		int jokerlessCardsDealt = numCards - 2;

		for (int i = 0; i < jokerlessCardsDealt - 1; i++) {
			for (int j = 0; j < jokerlessCardsDealt - i - 1; j++) {
				if (compareClassObject.Compare(Card[j], Card[j + 1])) {
					//Was having trouble with the assignment operator and the Card[] objects, so each element individually is swapped 
						//instead of whole card objects at a time like was done in week 12
					auto tempCode = Card[j]->code;
					auto tempSuit = Card[j]->suit;
					auto tempNumber = Card[j]->number;
					SetNewCardEntry(j, (j + 1));
					Card[j + 1]->code = tempCode;
					Card[j + 1]->suit = tempSuit;
					Card[j + 1]->number = tempNumber;
				}
			}
		}
	}

	//This function adds together all of the numeric values from the user-requested cards 
	int GetTotalValue()
	{
		int total = 0;

		//the number of a given card is grabed using GetNumber(i), and that string is sent into GetIndividualCardValue(value) 
		//to turn it from a string into an int to br added to the total
		for (int i = 0; i < (numCards - 2); i++)
			total += GetIndividualCardValue(GetNumber(i)); //TODO: Assert value isn't 'JOKER'

		return total;
	}

};