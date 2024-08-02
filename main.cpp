#include <iostream>
#include <string>
#include "HttpClient.h"
#pragma warning( push )
#pragma warning(disable:26819)
#include "json.hpp"
#pragma warning( pop )
using json = nlohmann::json;
using namespace std;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////    GAME OVERVIEW   ////////////////////////////////
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
//		//Dynamic Memory Allocation, Double Pointers, Maps, and a Clean UI are all
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
//		//otherwise outputs a 'No Jokers Found' message. Then the user has
//		//to guess the total value of all of the cards added together. This
//		//ranges from  2 (a single Ace drawn) to 64 (4 Kings and a Queen 
//		//drawn). The range is dynamic based on user input of cards.
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
string SuitToString(SUITS suit) {
	switch (suit) {
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

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////  GUESSING GAME CLASS  //////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

// Also referred to as Mini Game 1; Given actual name to make noticing it easier
class GuessingGame : public DeckOfCards
{
	// Private by default
	int numCards;
	bool correct;

public:

	//Constructor
	GuessingGame() {}

	// Intro to Easy Mode game; replacing pure virtual
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
		saveResult.push_back(SuitToString(draw.suit));
		saveResult.push_back(draw.number);
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
					<< "Number: " << resultCount.second[i++] << setw(2) << left << "\t"
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
			//we look at each individual card's Number and see if that card is a Joker
			if (GetNumber(i) == "JOKER")
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
		int value = 0;

		//the number of a given card is grabed using GetNumber(i), and that string is sent into GetIndividualCardValue(value) 
		//to turn it from a string into an int to br added to the total
		for (int i = 0; i < (numCards - 2); i++)
			value += GetIndividualCardValue(GetNumber(i));

		return value;
	}

};

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
	static bool Compare(LuckOfTheDrawGame::DrawCard* c1, LuckOfTheDrawGame::DrawCard* c2);
};

struct CompareValues
{
public:
	static bool Compare(LuckOfTheDrawGame::DrawCard* c1, LuckOfTheDrawGame::DrawCard* c2);
};

bool CompareSuits::Compare(LuckOfTheDrawGame::DrawCard* c1, LuckOfTheDrawGame::DrawCard* c2)
{
	SUITS suit1 = c1->suit;
	SUITS suit2 = c2->suit;

	if (suit1 == suit2)
		return CompareValues::Compare(c1, c2);

	return suit1 > suit2;
}

bool CompareValues::Compare(LuckOfTheDrawGame::DrawCard* c1, LuckOfTheDrawGame::DrawCard* c2)
{
	int value1 = LuckOfTheDrawGame::GetIndividualCardValue(c1->number);
	int value2 = LuckOfTheDrawGame::GetIndividualCardValue(c2->number);


	if (value1 == value2)
		return CompareSuits::Compare(c1, c2);

	return value1 > value2;
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
	int numOfCards;
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

		// Here we will add the code connecting main to Minigame1 (easy mode; guessing game)
		if (gameChoice == "1")
		{
			// numOfCards defaulted to 1 for Guessing Game. Changes when user is prompted to declare number of cards to draw for Luck Of The Draw Game.
			// Must be declared in this loop to override the player input if they replayed the game and chose mini game 1 instead of 2.
			numOfCards = 1;

			GuessingGame g1;
			g1.Connect("deckofcardsapi.com");
			
			// Sends in the parameters to the API call for number of cards (defaulted as 1 for this game) and disables the API from bringing in jokers.
			g1.Get("/api/deck/new/draw", { {"count", to_string(numOfCards)}, {"jokers_enabled", "false"} });
			g1.GetCardCount(numOfCards);

			// Gets card value that was drawn to pass into RunNumberOfTries
			gameValue = g1.GetTotalValue();

			// Prints rules of game and level of difficulty
			g1.GetRules();

			// Function to loop over gameplay guesses for player.
			// The player has 5 guess attempts and is given hints of too high or too low.
			RunNumberOfTries(numOfCards, gameValue);

			// Prints the drawn cards info
			cout << g1 << endl;

		}

		// This is where Minigame 2 (hard mode; Luck of the Draw game) is triggered instead
		else
		{
			continueGetNumCards = true;

			// This block ensures, using exception handling, that a user enters the proper value (similar to the function G2_GetPlayerGuess())
			while (continueGetNumCards)
			{
				cout << "How Many Cards Would You Like to See? [Enter Number 2-5]: ";
				try
				{
					getline(cin, cinNumCardsString);
					auto x = cinNumCardsString.find(".");

					if (cin.fail() || stoi(cinNumCardsString) < 2 || stoi(cinNumCardsString) > 5 || cinNumCardsString.find(".") != string::npos) {
						throw exceptionMessage;
					}
					continueGetNumCards = false;
				}
				catch (string s) {
					cout << s << endl;
				}
				catch (invalid_argument&) {
					cout << exceptionMessage << endl;
				}
			}
			cout << endl;

			bool picksorting = true;
			string pickSortMethod;

			//here we take the user input for number of cards to be drawn, convert it to an int, add 2 to account for the bonus cards, and place that number
				//into cinNumCardsString. This string is what is sent over to the API
			int addJokers = stoi(cinNumCardsString);
			numOfCards = addJokers;     // Used in the GetRange function for card guesses
			addJokers += 2;
			cinNumCardsString = to_string(addJokers);

			//we send in the updated int value for total number of cards into the LOTDG object
			LuckOfTheDrawGame g2;

			// Declares number of cards that will be drawn
			g2.GetCardCount(addJokers);

			// Prints rules of game and level of difficulty
			g2.GetRules();

			g2.Connect("deckofcardsapi.com");

			//This connects to .Get, which is an inherited function from HttpClient, which then calls Data and EndOfData in Deck of Cards, as LOTDG inherits 
			//publically from both classes. We set jokers to true so that we can implement the joker search function.
			g2.Get("/api/deck/new/draw", { {"count", cinNumCardsString}, {"jokers_enabled", "true"} });

			//here we save the cards exactly as they were returned from the API into the first vector
			g2.SaveOutput(true);

			cout << endl << left << setfill(' ') << setw(20) << " " << left << setfill('*') << setw(25) << " " << endl << endl;
			cout << setfill(' ');

			//then we search for and potentially replace the jokers
			g2.SearchForJoker();

			//now we save the newly updated, potentially now jokerless cards into the second vector
			g2.SaveOutput(false);

			//here, we save the value of all the cards added together into gameValue
			gameValue = g2.GetTotalValue();

			cout << endl << left << setfill(' ') << setw(20) << " " << left << setfill('*') << setw(25) << " " << endl << endl;
			cout << setfill(' ');

			// Function to loop over gameplay guesses for player.
			// The player has 5 guess attempts and is given hints of too high or too low.
			RunNumberOfTries(numOfCards, gameValue);

			//Sorting method picked, using loop to ensure valid entry
			while (picksorting)
			{
				cout << "Would You Like to Sort By Suits [Enter 1] or By Value [Enter 2]: ";
				getline(cin, pickSortMethod);

				if (pickSortMethod == "1" || pickSortMethod == "2")
					picksorting = false;
				else
					cout << endl << "Please Input \"1\" or \"2\"." << endl << endl;
			}
			cout << endl;

			//Here Bubblesort is called, using the appropriate comparefunction via templates
			if (pickSortMethod == "1")
				g2.BubbleSort<CompareSuits>();
			else
				g2.BubbleSort<CompareValues>();

			//Finally, all versions of the cards are output to the user 
			g2.GetCardResults(true);
			g2.GetCardResults(false);
			cout << g2;
		}

		//This final block just asks the user if they want to play again, ending if they enter an "x", and continuing otherwise
		// If they do, they're looped back and given the option again to choose the level of difficulty
		cout << endl << "Do You Wish to Exit Program? [Enter \"x\" for \'Yes\', Any Other Key for \'No\']: ";
		getline(cin, exit);
		pickGame = true;
		if (exit == "x")
			exitProgram = true;

	}

	// Exit message displayed
	ExitMessage();
	return 0;
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
