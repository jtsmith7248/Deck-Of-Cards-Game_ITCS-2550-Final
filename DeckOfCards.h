#include "HttpClient.h"
#include "json.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;
using json = nlohmann::json;

// Format for Card Number comparisons and for value charts in main()
map<string, int> CardFaceValues = { {"ACE", 1},{"2", 2},{"3", 3},{"4", 4},{"5", 5},{"6", 6},{"7", 7},{"8", 8},{"9", 9},{"10", 10},{"JACK", 11},{"QUEEN", 12},{"KING", 13} };
map<string, int> CardSuitValues = { {"Spades", 1},{"Clubs", 2},{"Diamonds", 3},{"Hearts", 4} };

struct DrawCard {
    string number;		// API refers to this as "value"
    string suit;		    // [ S(1), C(2), D(3), H(4) ] //UPDATE: Erased SUIT enum, made into string
    string code;		// For printing purposes -- Ended up not being particularly necessary, but just adds information for the user to see

    // Overload insertion "<<" operator
    // When a DrawCard object is output, this func is called
    friend ostream& operator<<(ostream& osObject, const DrawCard& draw)
    {
        osObject << setfill(' ') << setw(2) << left << "Code: "
            << draw.code << setw(4) << right << "\t"
            << "Number: " << draw.number << setw(2) << left << "\t"
            << "Suit: " << draw.suit << setw(2) << left << endl;

        return osObject;
    }
};

//DeckOfCards is declared here. This is the main class, and the parent that both minigames inherit off of. It itself inherits from HttpClient, and
	//overall is inspired and designed from the work we did in week 12 using the UnsplashClient and **Photos object
class DeckOfCards : public HttpClient {

private:

    //Two public strings here, each will contain card information. Because at the end of the game three seperate versions of the cards are output to
		//the user (The cards before the Joker(s) are replaced, the cards after the Joker(s) are replaced, and finally the cards properly sorted),
		//it is using these strings that we save a snapshot of what those cards looked like at a given time, so that the user can view them later on.
    //UPDATE: Made private
    //TODO: Create and use getters and if need be setters
	string savePreJokerOutput;
	string savePreSortOutput;

    vector<char> jsonData;
	size_t cardsDealt;

protected:

    //UPDATE: Also made protected. Holds all cards drawn 
    //TODO: Potentially change access status
    DrawCard** Card;

public:

	// Constructor
	DeckOfCards() : cardsDealt{0} {}

	// Destructor, very similar to week 12
	~DeckOfCards()
	{
		for (int i = 0; i < cardsDealt; i++) {
			delete Card[i];
		}
		delete[] Card;
	}

	// Get and return given DrawCard suit
	string GetSuits(const int count) {

        if (count < 0 || count >= cardsDealt) {
            throw "INVALID INDEX"; //TODO: Set up catch statement for when this is called
        }

		return Card[count]->suit;
	}

	// Get and return given DrawCard number
	string GetNumber(const int count) {

        if (count < 0 || count >= cardsDealt) {
            throw "INVALID INDEX"; //TODO: Set up catch statement for when this is called
        }

		return Card[count]->number;
	}

	//Takes in a spot in the index 'count' and returns the Card object at that point
	DrawCard* DrawHand(const int count) {

        if (count < 0 || count >= cardsDealt) {
            throw "INVALID INDEX"; //TODO: Set up catch statement for when this is called
        }

		return Card[count];
	}

	//UPDATED: Takes in number of cards to draw and sets that to be cardsDealt //Old: Takes in a spot in the index 'count' and returns the Card object at that point
	virtual void SetCardsDealt(const int count) = 0; //Was: GetCardCount()



    //START EDITING AGAIN FROM HERE ---------------------------------------------------------------------------------------------------------------------



	// Uses overloaded insertion "<<" operator in DrawCard struct to stream output of DeckOfCards class
	friend ostream& operator<<(ostream& osObject, const DeckOfCards& dc) {

		osObject << setfill('*') << "" << right << setw(46) << " PLAYER CARDS AFTER SORTING: " << setfill('*') << setw(28) << "" << endl << endl;
		osObject << setfill(' ');

        //Again, note the 'cardsDealt - 2', as explained in the comments in SaveOutput()
		for (int i = 0; i < dc.cardsDealt - 2; i++) {
			osObject << setw(10) << right << 1 + i << ") " << *dc.Card[i] << endl;
		}

		return osObject;
	}

	// Each *child* class must state their intro mode level with rules
	virtual void GetRules() = 0;

	//Just converts the string number to an int
	//Defined as static so that it is accessible inside of the CompareValues struct without having to declare an object of this LOTD/MG2 class
	static int GetIndividualCardValue(string cardNum)
	{
		auto search = CardFaceValues.find(cardNum);
		return search->second;
	}

protected:

	void StartofData() {}

	//takes in all the info from DeckOfCards API and fills a vector with each individual char
	void Data(const char arrData[], const unsigned int iSize)
	{
		//json parsed here
		jsonData.insert(jsonData.end(), arrData, arrData + iSize);
	}

	// Parse the jsonData char vector, and fill all of the DrawCard objects with the requisite data using the nlohmann parser
	void EndOfData()
	{
		json jp = json::parse(jsonData.begin(), jsonData.end());

		auto& results = jp["cards"];
		//auto count = results.size(); //line for debugging
		Card = new DrawCard* [results.size()];
		for (auto& individualCardEntry : results)
		{
			//Dynamic memory allocation
			DrawCard* newCard = new DrawCard;
			//Note the StringToSuit() func, explained at top of code
			newCard->suit = individualCardEntry["suit"].is_null() ? "NULL" : individualCardEntry["suit"];
			newCard->number = individualCardEntry["value"].is_null() ? "Null" : individualCardEntry["value"];
			newCard->code = individualCardEntry["code"].is_null() ? "Null" : individualCardEntry["code"];

			Card[cardsDealt++] = newCard;
		};

	}
};