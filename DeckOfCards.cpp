
//TODO: Check which is necessary and remove if not
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

// Moved enum to the DeckOfCards class

//enum class SUITS
//{
//	NULLSUIT,
//	SPADES,
//	CLUBS,
//	DIAMONDS,
//	HEARTS
//};
// 
//string SuitToString(SUITS suit)
//{
//	switch (suit)
//	{
//	case NULLSUIT:
//		return "NULL";
//	case SPADES:
//		return "SPADES";
//	case CLUBS:
//		return "CLUBS";
//	case DIAMONDS:
//		return "DIAMONDS";
//	default:
//		return "HEARTS";
//	}
//}
//
//SUITS StringToSuit(string suitString)
//{
//	if (suitString == "SPADES")
//		return SPADES;
//	else if (suitString == "CLUBS")
//		return CLUBS;
//	else if (suitString == "DIAMONDS")
//		return DIAMONDS;
//	else if (suitString == "HEARTS")
//		return HEARTS;
//	else
//		return NULLSUIT;
//}

// Main user-defined class: parsing, sorting, and virtual functions happen here
class DeckOfCards : public HttpClient
{
public:
	enum class SUITS
	{
		NULLSUIT,
		SPADES,
		CLUBS,
		DIAMONDS,
		HEARTS
	};

	struct DrawCard
	{
		// TODO: CHANGE NUMBER TO INT IN OUR ACTUAL CLASS
		string number;		// API refers to this as "value"
		SUITS suit;			// (S, C, D, H)
		string code;		// For printing purposes

		// Overload insertion "<<" operator
		friend ostream& operator<<(ostream& osObject, const DrawCard& draw)
		{
			string drawSuit = SuitToString(draw);
			osObject << setfill(' ') << setw(2) << left << "Suit: " << drawSuit << setw(4) << right << "\t"
				<< "Number: " << draw.number << setw(2) << left << "\t"
				<< "Code: " << draw.code << setw(2) << left << endl;
			return osObject;
			//cout << endl << "	Suit: " << SuitToString(draw.suit) << endl << "	Number: " << draw.number << endl << "	Code: " << draw.code << endl;
			return osObject;
		}
	};

	// Constructor
	DeckOfCards()
	{
		DrawCard** c = Card;
		//size_t cardsDealt = NULL;
		//DrawCard** card = Card;
		//card = Card;
		//
		// Initializes array of pointers
		//for (int i = 0; i < sizeof(Card); i++)
		//	Card[i] = nullptr;		// For memory that points to null
	}

	// Destructor
	virtual ~DeckOfCards()
	{
		for (int i = 0; i < cardsDealt; i++) {
			delete Card[i];
		}
		delete[] Card;
	}

	// Convert string to suit
	const SUITS StringToSuit(string suitString)
	{
		if (suitString == "SPADES")
			return SUITS::SPADES;
		else if (suitString == "CLUBS")
			return SUITS::CLUBS;
		else if (suitString == "DIAMONDS")
			return SUITS::DIAMONDS;
		else if (suitString == "HEARTS")
			return SUITS::HEARTS;
		else
			return SUITS::NULLSUIT;
	}

	// Stringify SUITS enum (convert suit to string)
	static string SuitToString(const DrawCard& draw)
	{
		switch (draw.suit)
		{
		case SUITS::NULLSUIT:
			return "NULL";
		case SUITS::SPADES:
			return "SPADES";
		case SUITS::CLUBS:
			return "CLUBS";
		case SUITS::DIAMONDS:
			return "DIAMONDS";
		case SUITS::HEARTS:
			return "HEARTS";
		default:
			return "NULL";
		}
	}

	DrawCard& operator[](int i) {
		if (i < cardsDealt)		// First loop through music types
			return *(Card[i]);
	}

	// Think of this one like a "CreateHand" type of function
	virtual DrawCard* DrawHand(int count) {
		// retrieves number of cards needed and loops through
		return Card[count];
	}

	// Get the Card values 
	virtual DrawCard* GetNewCard()
	{
		if (Card != nullptr) {
			return *Card;
		}
		else
			throw std::out_of_range("Index is invalid.");	// throw error
	}

	// Get and return DrawCard index
	virtual DrawCard* GetCardIndex(int i) {
		return Card[i];
	}

	// Get and return DrawCard suit
	virtual string GetSuits(int i)
	{
		string cSuit;
		DrawCard* card = *(Card);
		for (int iter = 0; iter < i; iter++) {
			card[iter];
		}
		cSuit = SuitToString(*card);
		return cSuit;
	}

	// Get and return DrawCard int number
	virtual int GetNumber(DrawCard* c, int i) {
		auto num = stoi(c[i].number);
		return num;
	}

	// Get and return DrawCard code
	virtual string GetCode(DrawCard* c, int i) {
		//DrawCard* cc = c;
		return c[i].code;
	}

	// Get and return DrawCard string number
	virtual string GetStringNumber(DrawCard* c, int i) {
		//DrawCard* cc = c;
		return c[i].number;
	}

	//// Get and return DrawCard suit
	//virtual DrawCard* GetCardNumber(int i) {
	//	return stoi(Card[i]->number);
	//	//return *this->&(*Card[i]->number);
	//}

	//// Get and return DrawCard number
	//virtual int GetNumberInteger(int i)
	//{
	//	return Card[i]->number;
	//}


	size_t ReturnCardsDealt() {
		return cardsDealt;
	}

	// Uses overloaded insertion "<<" operator in DrawCard struct to stream output of DeckOfCards class
	friend ostream& operator<<(ostream& osObject, const DeckOfCards& dc)
	{
		osObject << setfill(' ') << setw(44) << right << " PLAYER CARDS DRAWN: " << endl << endl;
		for (int i = 0; i < dc.cardsDealt; i++)
		{
			osObject << setw(10) << right << 1 + i << ") " << *dc.Card[i] << endl;
		}
		return osObject;
	}

	virtual void PrintResponse() {
		return;
	};

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
			newCard->suit = individualCardEntry["suit"].is_null() ? StringToSuit("NULL") : StringToSuit(individualCardEntry["suit"]);
			newCard->number = individualCardEntry["value"].is_null() ? "Null" : individualCardEntry["value"];
			newCard->code = individualCardEntry["code"].is_null() ? "Null" : individualCardEntry["code"];

			Card[cardsDealt++] = newCard;
			//cout << "CARDS COUNT: " << cardsDealt << endl;
		};
	}

private:
	//here we are deleteing all the dynamically allocated memory, fist by stepping through each object in photos[], then deleting photos itself
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
	int cardsDealt;
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





////////////////////////////////////////////////////////////////////////////////

using namespace std;	// NOTE: ADDED THIS BACK IN TO GET RID OF STD ERRORS

void GreetMessage();
void ExitMessage();

// TODO: Make a struct or enum to establish the value of cards (A,1,2,3,4....Queen)
#include <map>

// Declare a map from string keys to integer values
// and Assign the key-value pairs 
map<string, int> CardFaceValues = { {"Ace", 1},
									{"2", 2},
									{"3", 3},
									{"4", 4},
									{"5", 5},
									{"6", 6},
									{"7", 7},
									{"8", 8},
								    {"9", 9},
									{"10", 10},
									{"Jack", 11},
									{"Queen", 12},
									{"King", 13} };

map<string, int> CardSuitValues = { {"Spades", 1}, 
									{"Clubs", 2},
									{"Diamonds", 3},
									{"Hearts", 4} };

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	DeckOfCards dc;
	EasyMode em;	// Will call Easy Mode Game once added
	HardMode hm;	// Will call Hard Mode Game once added

	string cinNumCardsString = "1";
	string exit;
	string exceptionMessage = "Invalid Entry. Please Enter an Integer in the Given Range.";
	string mode;
	int numCardsInt = 1;	// default count
	bool continueGetNumCards = false;
	bool exitProgram = false;

	GreetMessage();		// Greeting message intro

	while (!exitProgram)
	{
		while (mode != "1" || mode != "2") {
			cout << "Choose Difficulty Level: [Enter 1: Easy Mode, 2: Hard Mode] ";
			getline(cin, mode);
			cout << endl;
			if (stoi(mode) == 1) {
				em.Connect("deckofcardsapi.com");
				em.Get("/api/deck/new/draw", { {"count", cinNumCardsString}, {"jokers_enabled", "false"} });
				em.PrintResponse();
				break;
			}
			else if (stoi(mode) == 2) {
				continueGetNumCards = true;
				break;
			}
			else {
				throw exceptionMessage;
			}
		}

		// Only enter for Hard Mode
		while (continueGetNumCards)
		{
			// TODO: Change max limit once we're done
			cout << "How Many Cards Would You Like to See? [Enter Number 2-54]: ";
			try
			{
				getline(cin, cinNumCardsString);
				auto x = cinNumCardsString.find(".");

				// 54 cards includes jokers
				if (cin.fail() || stoi(cinNumCardsString) < 2 || stoi(cinNumCardsString) > 54 || cinNumCardsString.find(".") != string::npos)
				{
					throw exceptionMessage;
				}
				continueGetNumCards = false;
			}
			catch (string s)
			{
				cout << s << endl;
			}
			// Throws error message when non-integer value is entered into a stoi() conversion
			catch (invalid_argument&)
			{
				cout << exceptionMessage << endl;
			}

			cout << endl << endl;

			////// NOTE: THIS ISN'T SWITCHED TO HARD MODE YET! 
			////// NOTE: Left DeckOfCards dc for testing capabilities
			dc.Connect("deckofcardsapi.com");
			dc.Get("/api/deck/new/draw", { {"count", cinNumCardsString}, {"jokers_enabled", "true"} });
			cout << dc;

			// TODO: ADD TO GAME INTERFACE; create separate functions for each or use class for it
			/*hm.Connect("deckofcardsapi.com");
			hm.Get("/api/deck/new/draw", { {"count", cinNumCardsString}, {"jokers_enabled", "true"} });
			cout << hm;*/

			//continueGetNumCards = false;
			cout << endl << "Do You Wish to Exit Program? [Enter \"x\" for \'Yes\', Any Other Key for \'No\']: ";
			getline(cin, exit);
			if (exit == "x")
				exitProgram = true;

			// TODO: Create conditional to allow to loop again
			//else
			//	continueGetNumCards = true;
		}
	}

	ExitMessage();	// Exit message
	return 0;
}

void GreetMessage()
{
	// prints the heart, spade, club, and diamond extended ASCII characters
	string cardSymbols = "\x03  \x04  \x05  \x06  ";
	string greeting = "   Welcome To The DECK OF CARDS Minigame!    ";
	cout << setfill('*') << setw(70) << " " << endl;
	cout << setfill('_') << setw(70) << " " << endl;
	cout << endl << " " << cardSymbols << greeting << cardSymbols << endl;
	cout << setw(70) << setfill('_') << " " << endl << endl << endl;
}

void ExitMessage()
{
	// prints the heart, spade, club, and diamond extended ASCII characters
	string cardSymbols = "\x03  \x04  \x05  \x06  ";
	string exitMessage = "  GAME TERMINATED: Thank you for playing!    ";
	cout << endl << setfill('_') << setw(70) << right << " " << endl;
	cout << endl << " " << cardSymbols << exitMessage << cardSymbols << endl;
	cout << setw(70) << setfill('_') << " " << endl << endl;
	cout << setw(70) << setfill('*') << " " << endl << endl;
}