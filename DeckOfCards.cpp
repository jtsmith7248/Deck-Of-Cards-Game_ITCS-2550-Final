#include <iostream>
#include <shlwapi.h>	// To launch the web browser
#include <string>
#include <conio.h>
#include "HttpClient.h"

#pragma warning( push )
#pragma warning(disable:26819)
#include "json.hpp"
#pragma warning( pop )
using json = nlohmann::json;

using namespace std;
////////////////////////////////////////////////


// Main user-defined class: parsing, sorting, and virtual functions happen here
class DeckOfCards : public HttpClient
{
public:
	struct DrawCard 
	{
		string number;		// API refers to this as "value"
		string suit;		// (S, C, D, H)
		string code;		// For printing purposes

		// Overload insertion "<<" operator
		friend ostream& operator<<(ostream& osObject, const DrawCard& draw) 
		{
			cout << endl << "	Suit: " << draw.suit << endl << "	Number: " << draw.number << endl << "	Code: " << draw.code << endl;

			return osObject;
		}
	};

	// Constructor
	DeckOfCards() {}

	// Destructor
	~DeckOfCards() 
	{
	
		for (int i = 0; i < cardsDealt; i++)
		{
			delete Card[i];
		}

		delete[] Card;
	
	}

	//UPDATE: Removed Bubblesort from DeckOfCards, it is only overloaded in HardMode (LOTD) and not in EasyMode (GG),
	//so it doesn't need to be in the parent class to be overloaded

		//template<typename T>
		//void BubbleSort(bool ascending) {	// Call in DrawCard* ?
		//	// change bool name if necessary
		//}

	// Get and return DrawCard suit
	virtual string GetSuits(int count) {  // const?

		return Card[count]->suit;
	}

	// Get and return DrawCard number
	virtual string GetNumber(int count) {   // const?

		return Card[count]->number;
	}

	// might be a void function instead with (DrawCard*, int) as parameters instead
	// Think of this one like a "CreateHand" type of function
	virtual DrawCard* DrawHand(int count) {
		// retrieves number of cards needed and loops through

		return Card[count];
	}

	size_t ReturnCardsDealt()
	{
		return cardsDealt;
	}

	// Uses overloaded insertion "<<" operator in DrawCard struct to stream output of DeckOfCards class
	friend ostream& operator<<(ostream& osObject, const DeckOfCards& dc) {

		cout << endl << "*****************OUTPUT*****************" << endl;
		for (int i = 0; i < dc.cardsDealt; i++)
		{
			cout << i + 1 << ") " << *dc.Card[i] << endl;
		}
		cout << "*****************END OUTPUT*****************" << endl << endl;

		return osObject;
	}

	// Might use if needed for child class
	//virtual void PrettyPrint() {} 

protected:

	void StartofData() {}

	//takes in all the info from UnsplashClient API and fills a vector woth each individual char
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
		Card = new DrawCard * [results.size()];
		for (auto& individualCardEntry : results)
		{
			DrawCard* newCard = new DrawCard;
			newCard->number = individualCardEntry["value"].is_null() ? "Null" : individualCardEntry["value"];
			newCard->suit = individualCardEntry["suit"].is_null() ? "Null" : individualCardEntry["suit"];
			newCard->code = individualCardEntry["code"].is_null() ? "Null" : individualCardEntry["code"];
			
			Card[cardsDealt++] = newCard;
			//cout << "CARDS COUNT: " << cardsDealt << endl;
		};

	}

private:
	//here we are deleteing all tyhe dynamically allocated memory, fist by stepping through each object in photos[], then deleting photos itself
	//prevents data leaks - called from the destructor
	//void DeleteMemory()
	//{
	//	//used to free all dynamically allocated memory when required before a new API call
	//	for (int i = 0; i < photoCount; i++)
	//	{
	//		delete photos[i];
	//	}
	//	delete[] photos;
	//}

	vector<char> jsonData;
	size_t cardsDealt = 0;  // Default is 1 card
	DrawCard** Card;

};

////////////////////////////////////////////////////////////////////////////////
///////////////    TO BE INSERTED ONCE ALL FILES ARE COMPLETE:   ///////////////
////////////////////////////////////////////////////////////////////////////////

// INSERT STRUCT COMPARESUITS

// INSERT STRUCT COMPARENUMBERS

// INSERT MINIGAME1 : Public DeckOfCards

// INSERT MINIGAME2 : Public DeckOfCards

// INSERT/WORK ON MAIN AFTER ALL FILES/FUNCTIONS ARE ADDED

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	string cinNumCardsString;
	string exit;
	string exceptionMessage = "Invalid Entry. Please Enter an Integer in the Given Range.";
	int numCardsInt;
	bool continueGetNumCards = true;
	bool exitProgram = false;

	//little entrance message
	cout << "*********************************************************" << endl << endl;
	cout << "Welcome To The Deck of Cards Minigame Player" << endl << endl;;
	cout << "*********************************************************" << endl << endl;

	while (!exitProgram)
	{
		while (continueGetNumCards)
		{
			cout << "How Many Cards Would You Like to See? [Enter Number 1-54]: ";

			try
			{
				getline(cin, cinNumCardsString);

				auto x = cinNumCardsString.find(".");

				if (cin.fail() || stoi(cinNumCardsString) < 1 || stoi(cinNumCardsString) > 54 || cinNumCardsString.find(".") != string::npos)
				{
					throw exceptionMessage;
				}

				continueGetNumCards = false;
			}
			catch (string s)
			{
				cout << s << endl;
			}
			catch (invalid_argument&)
			{
				cout << exceptionMessage << endl;
			}
		}

		cout << endl;

		DeckOfCards dc;
		dc.Connect("deckofcardsapi.com");

		//TO UPDATE: Add if structure to decide which request to send in depending on the minigame
		dc.Get("/api/deck/new/draw", { {"count", cinNumCardsString}, {"jokers_enabled", "true"} });

		cout << dc;

		continueGetNumCards = true;

		cout << endl << "Do You Wish to Exit Program? [Enter \"x\" for \'Yes\', Any Other Key for \'No\': ";
		getline(cin, exit);

		if (exit == "x")
			exitProgram = true;

		cout << endl;
	}

};