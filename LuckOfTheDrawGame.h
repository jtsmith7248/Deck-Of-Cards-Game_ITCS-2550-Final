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

bool CompareSuits::Compare(DrawCard* c1, DrawCard* c2) // S < C < D < H
{
	uint8_t val1 = c1->GetSuitValue();
	uint8_t val2 = c2->GetSuitValue();

	if (val1 == val2)
		return CompareValues::Compare(c1, c2);

	return val1 > val2;
}

bool CompareValues::Compare(DrawCard* c1, DrawCard* c2)
{
	uint8_t val1 = c1->GetNumericValue();
	uint8_t val2 = c2->GetNumericValue();

	if (val1 == val2)
		return CompareSuits::Compare(c1, c2);

	return val1 > val2;
};

//TODO: Get rid of any reference to the compare structs in LOTD .CPP

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
	//Two public strings here, each will contain card information. Because at the end of the game three seperate versions of the cards are output to
		//the user (The cards before the Joker(s) are replaced, the cards after the Joker(s) are replaced, and finally the cards properly sorted),
		//it is using these strings that we save a snapshot of what those cards looked like at a given time, so that the user can view them later on.
	//UPDATE: Made private, moved from DeckOfCards
	//TODO: Create and use getters and if need be setters
	vector<string> PreJokerSearchOutput;
	vector<string> PostJokerSearchOutput;


public:

	LuckOfTheDrawGame() {}

	// Intro to Hard Mode game; replacing pure virtual
	void GetRules() //TODO: Replace all endl's as necessary with '\n'
	{
		cout << endl << setfill(' ') << setw(34) << right << " [HARD MODE]: " << cardsDealt - 2 << " CARDS ARE DRAWN" << endl;
		cout << "" << setfill(' ') << setw(60) << right << "You get 5 attempts to guess the correct card number." << endl << endl;

		cout << endl << left << setfill(' ') << setw(20) << " " << left << setfill('*') << setw(25) << " " << endl << endl;
		cout << setfill(' ');
	}

	////OutputToVector takes the information that would usually be output to the screen and instead reads it into a vector to be read later
	//void OutputToVector(const DrawCard& draw)
	//{
	//	saveResult.push_back(draw.suit);
	//	saveResult.push_back(draw.value);
	//	saveResult.push_back(draw.code);
	//}

	//SaveOutput is the function, called in main() via a Hard Mode/Minigame 2 object that saves the cards as they are 
		//at a given moment into either map<int,vector<string>> cardResults or savePreJokerResults, based on which bool value is sent in
	void SaveOutput(bool preJokerOutput) //TODO: Go through all comments and ensure they're updated
	{
		//Note that instead of i < cardsDealt, its i < (cardsDealt - 2)
			//This is because, later on in the code, we need to replace up to two jokers if they're found in the user requested cards.
			//To do this, the API is set to always return two more cards than the user actually asked for, so that those two bonus cards can 
			//replace a joker or two if they're found. So, regardless of what happens in SearchForJoker(), we only want the cards the user 
			//requested to be added to the map vector and output to the screen. The final two bonus cards are to always stay hidden from the user.
		vector<string>* outputVector = nullptr;

		if (preJokerOutput) {
			PreJokerSearchOutput.resize(cardsDealt);
			outputVector = &PreJokerSearchOutput;
		}
		else {
			PostJokerSearchOutput.resize(cardsDealt);
			outputVector = &PostJokerSearchOutput;
		}
		
		
		for (int i = 0; i < (cardsDealt - 2); i++)
		{
			stringstream card;
			card << *Card[i];
			outputVector->push_back(card.str());
		}

	}

	//// This function is where the output of savePreJokerResults and cardResults is looped through and displayed
	//void GetCardResults(bool results)
	//{
	//	map<int, vector<string>> getCardResults;
	//	if (results == true) {
	//		cout << setfill('*') << "" << right << setw(46) << " CARDS BEFORE JOKER SEARCH: " << setfill('*') << setw(28) << "" << endl << endl;
	//		getCardResults = savePreJokerResults;
	//	}
	//	else {
	//		cout << setfill('*') << "" << right << setw(46) << " CARDS AFTER JOKER SEARCH: " << setfill('*') << setw(28) << "" << endl << endl;
	//		getCardResults = cardResults;
	//	}
	//	for (auto const& resultCount : getCardResults) {
	//		cout << setfill(' ') << setw(10) << right << resultCount.first + 1 << ") ";
	//		for (int i = 0; i < resultCount.second.size(); i++) {
	//			cout << setfill(' ') << setw(2) << left << "Code: " << resultCount.second[i++] << setw(4) << right << "\t"
	//				<< "Value: " << resultCount.second[i++] << setw(2) << left << "\t"
	//				<< "Suit: " << resultCount.second[i++] << setw(2) << left << endl;
	//		}
	//		cout << endl;
	//	}
	//}

	void PrintResults() { //TODO: Change order of functions to be order of calls
		cout << setfill('*') << "" << right << setw(46) << " CARDS BEFORE JOKER SEARCH: " << setfill('*') << setw(28) << "" << endl << endl;
		for (uint8_t i = 0; i < PreJokerSearchOutput.size(); ++i) {
			cout << setfill(' ') << setw(10) << right << i + 1 << ") ";
			cout << PreJokerSearchOutput[i] << "\n";
		}

		cout << setfill('*') << "" << right << setw(46) << " CARDS AFTER JOKER SEARCH: " << setfill('*') << setw(28) << "" << endl << endl;
		for (uint8_t i = 0; i < PostJokerSearchOutput.size(); ++i) {
			cout << setfill(' ') << setw(10) << right << i + 1 << ") ";
			cout << PostJokerSearchOutput[i] << "\n";
		}

		cout << setfill('*') << "" << right << setw(46) << " PLAYER CARDS AFTER SORTING: " << setfill('*') << setw(28) << "" << endl << endl << setfill(' ');
		for (int i = 0; i < cardsDealt - 2; i++) {
			cout << setw(10) << right << i + 1 << ") " << Card[i] << endl;
		}

	}

	//Here is the aformentioned SearchForJoker()
	void SearchForJoker()
	{
		bool jokerFound = false;
		uint8_t quantityJokersFound = 0; //TODO: Upade all 'int' to uint8_t

		SaveOutput(true); //saves the cards to be printed later as they appeared before the Joker search

		//we loop through all user requested cards (aka all cards minus the two bonus cards) first 
		for (uint8_t i = 0; i < (cardsDealt - 2); i++)
		{
			//we look at each individual card's Value and see if that card is a Joker
			if (Card[i]->value == "JOKER")
			{
				//if it is, we output that a joker was found to the screen, and change the bool value jokerFound to ensure the 
					//'no joker found' message is ignored
				cout << setfill(' ') << right << setw(40) << " JOKER FOUND " << setw(20) << endl;

				jokerFound = true;
				
				//This if is in place to catch a very rare situation. Remember, the deck has two jokers. Usually, if one of the user requested cards
					//is a joker, it gets replaced with the first bonus card (ie Card[cardsDealt - 2]). However, what if a Joker exists in the user
					//requested cards, AND the first bonus card is ALSO a Joker? In that situation, this if structure increments 
					//quantityJokersFound, so that in the next line, Card[i] (the Joker found somewhere in the user-requested cards) is replaced 
					//not by the first bonus card, but the second!
						//Note: cardsDealt is the total number of cards- if the user requested 5 cards, numCards will be 7, to account for the extra 
						//cards drawn to replace potential Jokers. Very Importantly- cardsDealt is NOT THE INDEX! The index is going to, in this 
						//hypothetical, run from 0-6, not from 1-7. Card[cardsDealt - 1] is the last entry in the Card object, aka the second bonus card, and 
						//Card[cardsDealt-2] is the second to last entry in the Cards object, aka the first bonus card. So, when quantityJokersFound increments
						//to 1 in the aforementioned hypothetical, (i) and (cardsDealt-1) are sent into ReplaceCard()
				if (Card[cardsDealt - 2]->value == "JOKER") {
					*Card[i] = *Card[cardsDealt - 1];
					break; //If we get here, both Joker's have been found, though only 1 was in the original group of 
							//requested cards- the other is at [cardsDealt - 2] in the bonus cards. Bc both found, we can break. There
							//will be no more to search for
				}

				*Card[i] = *Card[cardsDealt - 2 + quantityJokersFound];

				quantityJokersFound++;
			}
		}

		if (!jokerFound)
			cout << setfill(' ') << right << setw(41) << " NO JOKER DRAWN " << setw(20) << endl;

		SaveOutput(false); //saves the cards to be printed later as they appeared after the Joker search
	}

	// Sorting the order of suits and values(or numbers)
	//Textbook bubblesort problem, with added template that compares the cards either by suit or by value
	template <typename T>
	void BubbleSort()
	{
		T compareClassObject{};
		uint8_t nCards = static_cast<uint8_t>(cardsDealt - 2);

		for (uint8_t i = 0; i < nCards - 1; i++) {
			for (uint8_t j = 0; j < nCards - i - 1; j++) {
				if (compareClassObject.Compare(Card[j], Card[j + 1])) {

					/*auto tempCode = Card[j]->code;
					auto tempSuit = Card[j]->suit;
					auto tempNumber = Card[j]->number;
					SetNewCardEntry(j, (j + 1));
					Card[j + 1]->code = tempCode;
					Card[j + 1]->suit = tempSuit;
					Card[j + 1]->number = tempNumber;*/

					DrawCard tempCard = *Card[j]; //TODO: Ensure this is not shallow
					*Card[j] = *Card[j + 1];
					*Card[j + 1] = tempCard;

				}
			}
		}
	}

	virtual void RunGame() {
		//UPDATE: This block of 3 lines of code replaces 'GetTotalValue()'
		uint8_t answer = 0;
		for (int i = 0; i < (cardsDealt - 2); i++)
			answer += Card[i]->GetNumericValue(); //TODO: Assert value isn't 'JOKER'

		bool guessedCorrectly = false;
		string loser = " YOU LOST! BETTER LUCK NEXT TIME! ";
		string winner = " YOU GOT IT RIGHT! ";
		//this loop takes the users guesses for the total value, ensures they're valid entries using GPG(), and outputs info based on the guess
		for (uint8_t remainingGuesses = 5; remainingGuesses > 0; remainingGuesses--)
		{
			//the int containing number of remaining guesses is sent into GPG(), and that function returns a string containing just a number. That num is 
				//then converted into a string and compared against gameValue
			uint8_t guessValue = GetPlayerGuess(remainingGuesses, static_cast<uint8_t>(cardsDealt - 2));

			if (guessValue > answer)
				cout << "Too High! Guess Again!" << endl << endl;
			else if (guessValue < answer)
				cout << "Too Low! Guess Again!" << endl << endl;
			else { //you found the correct answer!
				cout << endl;
				cout << setfill('*') << "" << right << setw(46) << winner << setfill('*') << setw(28) << "" << endl << endl;
				cout << setfill(' ') << "The Correct Answer Was " << answer << "!" << endl << endl;
				guessedCorrectly = true;
				break;
			}
		}

		if (!guessedCorrectly)
		{
			cout << endl;
			cout << setfill('*') << "" << right << setw(54) << loser << setfill('*') << setw(20) << "" << endl << endl;
			cout << setfill(' ') << "The Correct Answer Was " << answer << "!" << endl << endl;
		}

		cout << endl << left << setfill(' ') << setw(20) << " " << left << setfill('*') << setw(25) << " " << endl << endl;
		cout << setfill(' ');

		bool picksorting = true;
		string pickSortMethod;

		//Sorting method picked, using loop to ensure valid entry
		while (picksorting)
		{
			try {
				cout << "Would You Like to Sort By Suits [Enter 1] or By Value [Enter 2]: ";
				getline(cin, pickSortMethod);

				if (pickSortMethod == "1" || pickSortMethod == "2")
					picksorting = false;
				else
					throw InvalidEntry();

			}
			catch (const InvalidEntry&) {
				cout << endl << "Please Input \"1\" or \"2\"." << endl << endl;
			}
		}

		cout << endl;

		//Here Bubblesort is called, using the appropriate comparefunction via templates
		if (pickSortMethod == "1")
			BubbleSort<CompareSuits>();
		else
			BubbleSort<CompareValues>();
	}

	//TODO: Ensure credit is given to my prof for HttpClient.h
	//TODO: Move most func defs into .cpp files respectively

};