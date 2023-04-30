#include <iostream>
#include <shlwapi.h>	// To launch the web browser
#include <string>
#include "HttpClient.h"

#pragma warning( push )
#pragma warning(disable:26819)
#include "json.hpp"
#pragma warning( pop )
using json = nlohmann::json;
using namespace std;


// This file holds the GuessingGame child class of DeckOfCards
// A card is drawn at random, the user has a limited number of attempts to guess it right



class EasyMode : public DeckOfCards
{
private:
	const int DEFAULT_COUNT = 1;
	bool correct;
	//DrawCard** Card;

public:
	////// Made public to try different ways to get functions to work
	DrawCard** Card;

	EasyMode()
	{
		////// NOTE: Just trying nonsense at this point.....
		//DrawCard* card = *c;
		DrawCard** card = Card;
		card = Card;

		//card.number = GetNumber(defaultCount);
		//*card = DrawHand(DEFAULT_COUNT);
	}


	// Intro to Easy Mode game
	string GetRules()
	{
		string guess = "";
		cout << "[EASY MODE]: " << DEFAULT_COUNT << " CARD WAS DRAWN\n"
			<< "You get 5 guesses to guess the correct card number.\n"
			<< "NOTE: Suit values are not counted in easy mode.\n"
			<< "Guess the card value [Range: 1-13]: " << endl;
		cin >> guess;
		return guess;
	}

	// Draw card at index 1 (default value)
	DrawCard* DrawHand(int count) {
		return Card[count];
	}


	// checks and converts string response to int value
	int GetGuess(string res)
	{
		string exceptionMessage = "ERROR: not valid input";

		auto response = stoi(res);
		if (stoi(res) < 1 || stoi(res) > 13)
			throw std::out_of_range("Index is invalid.");
		return response;
	}

	// Get users guess by int value; max guess attempts = 5
	// TODO: Test the error throw message to make sure it works
	bool CheckGuess(int guess)
	{
		DrawCard** c = &*Card;
		string exceptionMessage = "Invalid Entry. Please Enter an Integer in the Given Range.";
		//int answer = stoi(card->number);
		int answer = GetNumber(*c, 1);		////// TODO: NOTE: ERROR HERE; Probably problems with how functions return the pointers or how DrawCard is passed through
		int attemps = 0;

		do {
			if (guess == answer)
				return correct = true;
			else if (guess > answer)
				cout << "Hint: Guess lower" << endl;
			else if (guess < answer)
				cout << "Hint: Guess higher" << endl;
			else
				throw exceptionMessage;
			attemps++;
		} while ((attemps < 5) || (correct != true));

		if (correct != true)
			return correct;
	}

	// Virtual void from DeckOfCards
	void PrintResponse()
	{
		string guess;
		int guessNum;
		guess = GetRules();			// assign response from intro to guess
		guessNum = GetGuess(guess);		// assign conversion from string guess to int guessNum

		correct = CheckGuess(guessNum);		// Returns if they were right or wrong & prints message
		if (correct)
			cout << "YOU WON!";
		else
			cout << "You lost! Better luck next time!";
	}

	///////////////    NOTE: THESE ARE ALL THE QUESTIONABLE FUNCTIONS; ALL ARE VIRTUAL VOIDS    //////////////////////

	//// Get and return DrawCard suit
	//string GetStringNumber(DrawCard& c, int i) const {
	//	return &(c->number);
	//}

	//// Get and return DrawCard number
	//DrawCard* GetCardNumber(int i) const {
	//	//return to_string(Card[i]->number);
	//	return (Card[i]->number);
	//}

	//// Get and return DrawCard int number
	//int GetNumber(int i) {
	//	DrawCard** c = Card;
	//	string sNum = GetStringNumber(c, i);
	//	int num = stoi(sNum);
	//	return num;
	//}

	// Get and return DrawCard string number
	string GetStringNumber(DrawCard* c, int i) {
		//DrawCard* cc = c;
		return c[i].number;
	}

	// Get and return DrawCard int number
	int GetNumber(DrawCard* c, int i) {
		string sNum = GetStringNumber(c, i);
		auto num = stoi(c[i].number);
		return num;
	}

	// Get and return DrawCard code
	string GetCode(DrawCard* c, int i) {
		//DrawCard* cc = c;
		return c[i].code;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
};