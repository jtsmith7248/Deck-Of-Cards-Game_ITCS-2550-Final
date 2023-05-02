// This file holds the GuessingGame child class of DeckOfCards
// A card is drawn at random, the user has a limited number of attempts to guess it right

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////////    TESTING PURPOSES   //////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include "HttpClient.h"
#pragma warning( push )
#pragma warning(disable:26819)
#include "json.hpp"
#pragma warning( pop )
using json = nlohmann::json;
using namespace std;


///////////////////////////////////////////////////////////////////
//////////////////////    DoC - 1 PURPOSES   //////////////////////
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//
//	//DECKOFCARDS FINAL PROJECT - JUSTIN SMITH, NGUYEN TRAN, VIOLET YOUSIF 
//
//	//This project builds off of the work we have been doing in ITCS 2550
//		//for weeks, and uses the HttpClient class to connect to the
//		//internet and both send requests and read from an API called 
//		//Deck of Cards. Using this API, we send in a request for a user
//		//requested number of cards, get dealt back that many cards out of 
//		//a deck of 52/54, and then have the user play a little mini game 
//		//using these randomly dealt cards. 
//
//	//There are two mini games- GuessingGame (Easy Mode) and 
//		//LuckOfTheDrawGame (Hard Mode). Using these two games within the
//		//project, we use multiple different concepts covered within the 
//		//semester. Inheritence, Polymorphism, Overloaded Operators, 
//		//Templates, Enums, Vectors, Searching, Sorting, Exception Handling, 
//		//Dynamic Memory Allocation, Double Pointers, and a Clean UI are all
//		//utilized at one point or another throughout this program. 
//
//	//In Minigame 1, aka Easy Mode or GuessingGame, the user has to guess
//		//the value of a randomly drawn card. They have five chances to get
//		//it, and the values range from "1" for any Ace drawn to "13" for a 
//		//King
//
//	//In Minigame 2, aka Hard Mode or LuckOfTheDrawGame, the user is asked 
//		//how many cards they would like to draw, from 1-5. A deck of cards 
//		//contains 54 cards instead of the standard 52 in this game, 
//		//as two jokers are mixed into the bunch. The Minigame class then 
//		//searches for any Joker in the drawn cards and replaces it if found, 
//		//otherwise outputs a 'No Jokers Found' message. They the user has
//		//to guess the total value of all of the cards added together. This
//		//ranges from  1 (a single Ace drawn) to 64 (4 Kings and a Queen 
//		//drawn). 
//
//		//After either succeeding in guessing the total value with 5 
//		//attempts or not, the game concludes by outputting 3 blocks 
//		//of text:
//			//1. All the cards the user asked for as they were originally 
//				//returned from the API
//			//2. Those same cards, but now with the Joker(s) replaced if 
//				//any were found
//			//3. All the newly updated cards, ordered by the criteria
//				//requested by the user
//
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
////Before reaching the classes and main, we have our global objects that can be called from anywhere. this includes the enum used for the suits, as they need to be
//	//accessible in multiple areas, the conversion functions that transfer the enum data type back and forth between itself and a string, and finally the function
//	//G2_GetPlayerGuess(), which takes user input inside of a loop in main, and the code looked cleaner and more understandable when it was outsourced out of main()

//Basic enum declaration. Inside of the struct DrawCard, we store a given card's suit using this enum. Jokers or any error would go into 'NULLSUIT'
enum SUITS
{
	NULLSUIT, SPADES, CLUBS, DIAMONDS, HEARTS
};

//Takes in the enum type 'suit', returns the string version of that type- needed for output to console
string SuitToString(SUITS suit)
{
	switch (suit)
	{
	case NULLSUIT:
		return "NULL";
	case SPADES:
		return "SPADES";
	case CLUBS:
		return "CLUBS";
	case DIAMONDS:
		return "DIAMONDS";
	default:
		return "HEARTS";
	}
}

//Takes in the string containing the name of a given suit 'suitString', returns the enum version of that type- needed to take the string 
	//that was read in from the API and convert it to the enum type to be stored in DrawCard object
SUITS StringToSuit(string suitString)
{
	if (suitString == "SPADES")
		return SPADES;
	else if (suitString == "CLUBS")
		return CLUBS;
	else if (suitString == "DIAMONDS")
		return DIAMONDS;
	else if (suitString == "HEARTS")
		return HEARTS;
	else
		return NULLSUIT;
}

// Format for Card Number comparisons and for value charts in main()
map<string, int> CardFaceValues = { {"ACE", 1},{"2", 2},{"3", 3},{"4", 4},{"5", 5},{"6", 6},{"7", 7},{"8", 8},{"9", 9},{"10", 10},{"JACK", 11},{"QUEEN", 12},{"KING", 13} };
map<string, int> CardSuitValues = { {"Spades", 1},{"Clubs", 2},{"Diamonds", 3},{"Hearts", 4} };

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

//DeckOfCards is declared here. This is the main class, and the parent that both minigames inherit off of. It itself inherits from HttpClient, and
	//overall is inspired and designed from the work we did in week 12 using the UnsplashClient and **Photos object
class DeckOfCards : public HttpClient
{
public:

	//Two public strings here, each will contain card information. Because at the end of the game three seperate versions of the cards are output to
		//the user (The cards before the Joker(s) are replaced, the cards after the Joker(s) are replaced, and finally the cards properly sorted),
		//it is using these strings that we save a snapshot of what those cards looked like at a given time, so that the user can view them later on.
	string savePreJokerOutput;
	string savePreSortOutput;
	SUITS suit;

	struct DrawCard
	{
		string number;		// API refers to this as "value"
		SUITS suit;		    // [ S(1), C(2), D(3), H(4) ]
		string code;		// For printing purposes -- Ended up not being particularly necessary, but just adds information for the user to see

		// Overload insertion "<<" operator
		// When a DrawCard object is output, this func is called
		friend ostream& operator<<(ostream& osObject, const DrawCard& draw)
		{
			osObject << setfill(' ') << setw(2) << left << "Code: " << draw.code << setw(4) << right << "\t"
				<< "Number: " << draw.number << setw(2) << left << "\t"
				<< "Suit: " << SuitToString(draw.suit) << setw(2) << left << endl;
			return osObject;
		}
	};

	// Constructor
	DeckOfCards() {}

	DrawCard** Card;

	// Destructor, very similar to week 12
	~DeckOfCards()
	{
		for (int i = 0; i < cardsDealt; i++) {
			delete Card[i];
		}
		delete[] Card;
	}

	// Get and return given DrawCard suit
	string GetSuits(int count) {
		return SuitToString(Card[count]->suit);
	}

	// Get and return given DrawCard number
	string GetNumber(int count) {
		return Card[count]->number;
	}

	//Takes in a spot in the index 'count' and returns the Card object at that point
	DrawCard* DrawHand(int count) {
		return Card[count];
	}

	//Takes in a spot in the index 'count' and returns the Card object at that point
	virtual int GetCardCount(const int count) {
		return static_cast<int>(cardsDealt);
	}

	// Uses overloaded insertion "<<" operator in DrawCard struct to stream output of DeckOfCards class
	friend ostream& operator<<(ostream& osObject, const DeckOfCards& dc) {

		//Again, note the 'cardsDealt - 2', as explained in the comments in SaveOutput()
		osObject << setfill('*') << "" << right << setw(46) << " PLAYER CARDS AFTER SORTING: " << setfill('*') << setw(28) << "" << endl << endl;
		osObject << setfill(' ');

		for (int i = 0; i < dc.cardsDealt - 2; i++)
		{
			osObject << setw(10) << right << 1 + i << ") " << *dc.Card[i] << endl;
		}
		return osObject;
	}

	// Each class must state their intro mode level with rules
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

	//takes in all the info from DeckOfCards API and fills a vector woth each individual char
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
			//Dynamic memory allocation
			DrawCard* newCard = new DrawCard;
			//Note the StringToSuit() func, explained at top of code
			newCard->suit = individualCardEntry["suit"].is_null() ? StringToSuit("NULL") : StringToSuit(individualCardEntry["suit"]);
			newCard->number = individualCardEntry["value"].is_null() ? "Null" : individualCardEntry["value"];
			newCard->code = individualCardEntry["code"].is_null() ? "Null" : individualCardEntry["code"];

			Card[cardsDealt++] = newCard;
		};

	}

private:

	vector<char> jsonData;
	size_t cardsDealt = 0;
};


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////  GUESSING GAME CLASS  //////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


class GuessingGame : public DeckOfCards
{
	// Private by default
	int numCards;
	bool correct;

public:

	//Constructor
	GuessingGame() {}

	// Intro to Easy Mode game
	void GetRules()
	{
		cout << endl << setfill(' ') << setw(34) << right << " [EASY MODE]: " << numCards << " CARD IS DRAWN" << endl
			<< "" << setfill(' ') << setw(60) << right << "You get 5 attempts to guess the correct card number." << endl << endl;
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
			value += GetIndividualCardValue(GetNumber(i));

		return value;
	}

	//TODO: MOVE THIS TO GLOBAL FOR MAIN AND BOTH GAMES
	//Just converts the string number to an int
	//Defined as static so that it is accessible inside of the CompareValues struct without having to declare an object of this LOTD/MG2 class
	int GetIndividualCardValue(string cardNum)
	{
		auto search = CardFaceValues.find(cardNum);
		return search->second;
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



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////////    MAIN() FUNCTION   ///////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

void GreetMessage();
void GetValueChart();
void RunNumberOfTries(int, int);
int GetRange(int);
string G2_GetPlayerGuess(int, int);
void ExitMessage();

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	string cinNumCardsString;
	string exit;
	string exceptionMessage = "Invalid Entry. Please Enter an Integer in the Given Range.";
	string gameChoice;
	bool continueGetNumCards = false;
	bool pickGame = true;
	bool exitProgram = false;
	int numOfCards = 1;		// Defaulted to 1 for Guessing Game. Changes when user is prompted to declare number of cards to draw for Luck Of The Draw Game.
	int gameValue;

	// Intro message
	GreetMessage();

	//this loop ensures that a user can keep playing the minigames until they decide to quit 
	while (!exitProgram)
	{
		//this loop ensures a user inputs either a "1" or a "2" to select a game in an accepted mannor
		while (pickGame)
		{
			cout << "Would You Like to Play Easy Mode [Enter 1] or Hard Mode [Enter 2]: ";
			getline(cin, gameChoice);
			if (gameChoice == "1" || gameChoice == "2")
				pickGame = false;
			else
				cout << endl << "Please Input \"1\" or \"2\"." << endl << endl;
		}

		// Here we will add the code connecting main to minigame1/easy mode/guessing game
		//if (gameChoice == "1") {
		
		// ONLY ALLOW 1 AS OPTION FOR NOW
			GuessingGame g1;
			g1.Connect("deckofcardsapi.com");
			g1.Get("/api/deck/new/draw", { {"count", to_string(numOfCards)}, {"jokers_enabled", "false"} });
			g1.GetCardCount(numOfCards);

			// Gets card value that was drawn to pass into RunNumberOfTries
			gameValue = g1.GetTotalValue();

			// Prints rules of game and level of difficulty
			g1.GetRules();

			// Loops over game guess attempts 
			RunNumberOfTries(numOfCards, gameValue);

			// Prints the drawn cards info
			cout << g1 << endl;

		//}
		//JHere, Minigame 2 is triggered
		//else
		//{
		//	continueGetNumCards = true;

		//	//this block ensures, using exception handling, that a user enters the proper value. Similar to the function G2_GetPlayerGuess(), see above
		//	while (continueGetNumCards)
		//	{
		//		cout << "How Many Cards Would You Like to See? [Enter Number 2-5]: ";
		//		try
		//		{
		//			getline(cin, cinNumCardsString);
		//			auto x = cinNumCardsString.find(".");

		//			if (cin.fail() || stoi(cinNumCardsString) < 2 || stoi(cinNumCardsString) > 5 || cinNumCardsString.find(".") != string::npos) {
		//				throw exceptionMessage;
		//			}
		//			continueGetNumCards = false;
		//		}
		//		catch (string s) {
		//			cout << s << endl;
		//		}
		//		catch (invalid_argument&) {
		//			cout << exceptionMessage << endl;
		//		}
		//	}
		//	cout << endl;

		//	bool picksorting = true;
		//	string pickSortMethod;

		//	//here we take the user input for number of cards to be drawn, convert it to an int, add 2 to account for the bonus cards, and place that number
		//		//into cinNumCardsString. This string is what is sent over to the API
		//	int addJokers = stoi(cinNumCardsString);
		//	numOfCards = addJokers;     // Used in the GetRange function for card guesses
		//	addJokers += 2;
		//	cinNumCardsString = to_string(addJokers);


			////we send in the updated int value for total number of cards into the LOTDG object
			//LuckOfTheDrawGame g2;

			//// Declares number of cards that will be drawn
			//g2.GetCardCount(addJokers);

			//// Prints rules of game and level of difficulty
			//g2.GetRules();

			//g2.Connect("deckofcardsapi.com");

			////here we connect to .Get, which is an inherited function from HttpClient, which then calls Data and EndOfData in Deck of Cards, as LOTDG inherits 
			////publically from both classes
			//g2.Get("/api/deck/new/draw", { {"count", cinNumCardsString}, {"jokers_enabled", "true"} });

			////here we save the cards exactly as they were returned from the API into the first string
			//g2.SaveOutput(true);

			//cout << endl << left << setfill(' ') << setw(20) << " " << left << setfill('*') << setw(25) << " " << endl << endl;
			//cout << setfill(' ');

			////then we search for and potentially replace the jokers
			//g2.SearchForJoker();

			////now we save the newly updated, potentially now jokerless cards into the second string
			//g2.SaveOutput(false);

			////here, we save the value of all the cards added together into gameValue
			//gameValue = g2.GetTotalValue();

			//cout << endl << left << setfill(' ') << setw(20) << " " << left << setfill('*') << setw(25) << " " << endl << endl;
			//cout << setfill(' ');

			//// Function to loop over gameplay guesses for player.
			//// The player has 5 guess attempts and is given hints of too high or too low.
			//RunNumberOfTries(numOfCards, gameValue);

			////Sorting method picked, using loop to ensure valid entry
			//while (picksorting)
			//{
			//	cout << "Would You Like to Sort By Suits [Enter 1] or By Value [Enter 2]: ";
			//	getline(cin, pickSortMethod);

			//	if (pickSortMethod == "1" || pickSortMethod == "2")
			//		picksorting = false;
			//	else
			//		cout << endl << "Please Input \"1\" or \"2\"." << endl << endl;
			//}
			//cout << endl;
			////Here Bubblesort is called, using the appropriate comparefunction via templates
			//if (pickSortMethod == "1")
			//	g2.BubbleSort<CompareSuits>();
			//else
			//	g2.BubbleSort<CompareValues>();

			////Finally, all versions of the cards are output to the user 
			//g2.GetCardResults(true);
			//g2.GetCardResults(false);
			//cout << g2;
		//}

		//This final block just asks the user if they want to play again, ending if they enter an "x", and continuing otherwise
		cout << endl << "Do You Wish to Exit Program? [Enter \"x\" for \'Yes\', Any Other Key for \'No\']: ";
		getline(cin, exit);
		pickGame = true;
		if (exit == "x")
			exitProgram = true;

	}
	ExitMessage();

};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


void GreetMessage()
{
	// prints the heart, spade, club, and diamond extended ASCII characters
	string cardSymbols = "\x03  \x04  \x05  \x06  ";
	string greeting = "   Welcome To The DECK OF CARDS Minigame!    ";
	cout << setfill('*') << setw(70) << " " << endl;
	cout << setfill('_') << setw(70) << " " << endl;
	cout << endl << " " << cardSymbols << greeting << cardSymbols << endl;
	cout << setw(70) << setfill('_') << " " << endl << endl << endl;
	cout << setfill(' ');

	// Print Chart for face (number) values to show player rules of guessing range
	// Print Chart for suit values to show player how suits will be sorted if chosen in sort and if used in compareXXX against numbers that are the same
	GetValueChart();

}

// Print Chart for face (number) values and suit values to show player rules of guessing card range
void GetValueChart()
{
	// Using arrays to print charts because using the global map<int,vector<string>> would cause instant reordering due to its nature as a container
	string tempNumVect[13] = { "ACE","2","3","4","5","6","7","8","9","10","JACK","QUEEN","KING" };
	string tempSuitVect[4] = { "Spades","Clubs","Diamonds","Hearts" };

	// Print Chart for Number Values
	cout << setfill('*') << "" << right << setw(46) << " ORDER FOR NUMBER VALUE CHART: " << setfill('*') << setw(17) << "" << endl << endl;
	for (int i = 0; i < 13; i++)
		cout << setfill(' ') << setw(30) << right << tempNumVect[i] << " : " << i + 1 << endl;
	cout << endl;

	// Print Chart for Suit Values
	cout << setfill('*') << "" << right << setw(46) << " ORDER FOR SUIT VALUE CHART: " << setfill('*') << setw(18) << "" << endl << endl;
	for (int i = 0; i < 4; i++)
		cout << setfill(' ') << setw(30) << right << tempSuitVect[i] << " : " << i + 1 << endl;
	cout << endl;
}


void RunNumberOfTries(int numCards, int gameValue)
{
	int guessValue;
	int remainingGuesses = 5;
	bool guessedCorrectly = false;
	string loser = " YOU LOST! BETTER LUCK NEXT TIME! ";
	string winner = " YOU GOT IT RIGHT! ";
	//this loop takes the users guesses for the total value, ensures they're valid entries using G2_GPG(), and outputs info based on the guess
	for (int i = 0; i < 5; i++)
	{
		//the int containing number of remaining guesses is sent into G2_GPG(), and that function returns a string containing just a number. That num is 
			//then converted into a string and compared against gameValue
		guessValue = stoi(G2_GetPlayerGuess(remainingGuesses, numCards));

		if (guessValue == gameValue)
		{
			cout << endl;
			cout << setfill('*') << "" << right << setw(46) << winner << setfill('*') << setw(28) << "" << endl << endl;
			cout << setfill(' ') << "The Correct Answer Was " << gameValue << "!" << endl << endl;
			guessedCorrectly = true;
			break;
		}
		else if (guessValue > gameValue)
		{
			cout << "Too High! Guess Again!" << endl << endl;
			remainingGuesses--;
		}
		else
		{
			cout << "Too Low! Guess Again!" << endl << endl;
			remainingGuesses--;
		}
	}

	if (!guessedCorrectly)
	{
		cout << endl;
		cout << setfill('*') << "" << right << setw(54) << loser << setfill('*') << setw(20) << "" << endl << endl;
		cout << setfill(' ') << "The Correct Answer Was " << gameValue << "!" << endl << endl;
	}
}


//This function is called in main from within a loop, and is itself a loop ensuring the user inputs a valid entry via exception handling,
	//so it was outsorced here, into a global function. 
	//G2_GPG() takes in an int containing the number of remaining guesses a user has to guess the total value of the cards they drew, added together. This
	//is designed specifically for Hard Mode
string G2_GetPlayerGuess(int remainingGuesses, int nCards)
{
	bool continueGetGuess = true;
	string guessValue;
	string errorMessage2 = "Please Enter a Valid Integer Guess.";
	int highestValueRange;
	highestValueRange = GetRange(nCards);	// Uses the range functions to update range for player based on amount of cards chosen.

	// For 5 cards drawn, the minimum value possible is 6. Therefore, we adjust the value of nCards.
	// Explained: 4 suits at the lowest number is 4*1. Second lowest number is 2. Therefore 4(1) + 1(2) = 6.
	if (nCards == 5)
		nCards += 1;

	while (continueGetGuess)
	{
		cout << remainingGuesses << " Guesses Remaining!" << endl;
		if (nCards == 1)
			cout << "Please Guess The Card Number Value [" << to_string(nCards) + "-" + to_string(highestValueRange) << "]: ";
		else
			cout << "Please Enter a Guess for the Total Value of All Cards Added Together [" << to_string(nCards) + "-" + to_string(highestValueRange) << "]: ";

		try
		{
			//brings in user guess
			getline(cin, guessValue);

			//if the user enters something that 
				//1. Breaks the cin statement (ie. invalid data type)
				//2. Is a Number less than 1 or greater than 64
				//3. Has a decimal
			//an exception message will be thrown and the user will be asked to re-enter their guess
			if (cin.fail() || stoi(guessValue) < nCards || stoi(guessValue) > highestValueRange || guessValue.find(".") != string::npos)
			{
				throw errorMessage2;
			}

			//if they enter a valid guess, continueGetGuess is set to False and the loop breaks, code then continues
			continueGetGuess = false;
		}
		//if the if statement functions properly and an exception is thrown, it is read out here
		catch (string s)
		{
			cout << s << endl;
		}
		//however, if the user enters something really wrong, like a string, ie "example string" or "three", the portion of the if conditional 
			//"stoi(guessValue) < 1 || stoi(guessValue) > 64" will break completely and throw an invalid argument. A string cannot be converted 
			//into an int, so this second catch block will receive that specific error and read the error message to the UI, and allow the 
			//user to continue entering guesses until they put in a valid one
		catch (invalid_argument&)
		{
			cout << errorMessage2 << endl;
		}
	}
	return guessValue;
}

int GetRange(int nCards)
{
	// There are 4 possible suits of each number value in a deck.
	// If the max card value in a deck is 13 per game chart, then the highest added value in each suit is 52.
	// If the player chooses 5 cards, since there's only 4 suits possible for each card, then we must use one card for the remaining value.
	// The next highest value is 12. Therefore, the range would be: (4 cards * 13) = 52, (1 card * 12) = 12. TOTAL RANGE: 52 + 12 = 64
	int cardRange;
	if (nCards > 4)
		return cardRange = ((nCards - 1) * 13) + 12;

	return cardRange = (nCards * 13);
}


void ExitMessage()
{
	string cardSymbols = "\x03  \x04  \x05  \x06  ";		// prints the heart, spade, club, and diamond extended ASCII characters
	string exitMessage = "  GAME TERMINATED: Thank you for playing!    ";
	cout << endl << endl << setfill('_') << setw(70) << right << " " << endl;
	cout << endl << " " << cardSymbols << exitMessage << cardSymbols << endl;
	cout << setw(70) << setfill('_') << " " << endl << endl;
	cout << setw(70) << setfill('*') << " " << endl << endl;
}
