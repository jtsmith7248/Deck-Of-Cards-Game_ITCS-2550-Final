
#include "DeckOfCards.h"

///////////////////////////////////////////////////////////////////
//////////////////// DRAWCARD STRUCT //////////////////////////////
///////////////////////////////////////////////////////////////////

uint16_t DrawCard::GetNumericValue() const {
	if (value.size() == 1) { //aka if its a value of "2"-"9"
		return static_cast<uint16_t>(stoi(value));
	}
	else if (value == "10") {
		return 10;
	}
	else if (value == "JACK") {
		return 11;
	}
	else if (value == "QUEEN") {
		return 12;
	}
	else if (value == "KING") {
		return 13;
	}
	else if (value == "ACE") {
		return 1;
	}
	else {
		return 0;
	}
}

uint16_t DrawCard::GetSuitValue() const {
	if (suit[0] == 'S') { //SPADES
		return 1;
	}
	else if (suit[0] == 'C') { //CLUBS
		return 2;
	}
	else if (suit[0] == 'D') { //DIAMONDS
		return 3;
	}
	else if (suit[0] == 'H') { //HEARTS
		return 4;
	}
	else {
		return 0;
	}
}

DrawCard& DrawCard::operator= (const DrawCard& other) {
	if (this == &other)
		return *this;

	this->value = other.value;
	this->suit = other.suit;
	this->code = other.code;

	return *this;
}

// Overload insertion "<<" operator
// When a DrawCard object is output, this func is called
ostream& operator<<(ostream& osObject, const DrawCard & draw)
{
	osObject << setfill(' ') << setw(2) << left << "Code: "
		<< draw.code << setw(4) << right << "\t"
		<< "Value: " << draw.value << setw(2) << left << "\t"
		<< "Suit: " << draw.suit << setw(2) << left << "\n";

	return osObject;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////  DECKOFCARDS CLASS  ///////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

	uint16_t DeckOfCards::GetMaxRangeVal(const uint16_t nCards) const {
		// There are 4 possible suits of each number value in a deck.
		// If the max card value in a deck is 13 per game chart, then the highest added value in each suit is 52.
		// If the player chooses 5 cards, since there's only 4 suits possible for each card, then we must use one card for the remaining value.
		// The next highest value is 12. Therefore, the range would be: (4 cards * 13) = 52, (1 card * 12) = 12. TOTAL RANGE: 52 + 12 = 64
		if (nCards == 1) {
			return 13; //1 Card, 1 King
		}
		else if (nCards == 2) {
			return 26; //2 Cards, 2 Kings
		}
		else if (nCards == 3) {
			return 39; //3 Cards, 3 Kings
		}
		else if (nCards == 4) {
			return 52; //4 Cards, 4 Kings
		}
		else {
			return 64; //5 Cards, 4 Kings + 1 Queen
		}
	}


	//This function is called in both games, and is itself a loop ensuring the user inputs a valid entry 
	// via exception handling, so it was outsourced here 
	//GPG() takes in an int containing the number of remaining guesses a user has 
	// to guess the total value of the cards they drew, added together
	//UPDATE: Keep nCards as a parameter to allow flexible use of this function from calls originating in either child class
	uint16_t DeckOfCards::GetPlayerGuess(const uint16_t remainingGuesses, const uint16_t nCards) const {
		bool continueGetGuess = true;
		string guessValue;

		// Uses the range functions to update range for player based on amount of cards chosen.
		uint16_t maxVal = GetMaxRangeVal(nCards);

		// For 5 cards drawn, the minimum value possible is 6. Therefore, we adjust the value of nCards.
		// Explained: 4 suits at the lowest number is 4*1. Second lowest number is 2. Therefore 4(1) + 1(2) = 6.
		uint16_t minVal = (nCards < 5) ? nCards : 6;

		while (continueGetGuess)
		{
			cout << remainingGuesses << " Guesses Remaining!" << "\n";
			if (nCards == 1)
				cout << "Please Guess The Card Number Value [" << minVal << "-" << maxVal << "]: ";
			else
				cout << "Please Enter a Guess for the Total Value of All Cards Added Together [" << minVal << "-" << maxVal << "]: ";

			try
			{
				getline(cin, guessValue);

				//if the user enters something that 
					//1. Breaks the cin statement (ie. invalid data type)
					//2. Is a Number less than 1 or greater than 64
					//3. Has a decimal
				//an exception message will be thrown and the user will be asked to re-enter their guess
				if (cin.fail() || stoi(guessValue) < minVal || stoi(guessValue) > maxVal || guessValue.find(".") != string::npos)
				{
					throw InvalidEntry();
				}

				//if they enter a valid guess, continueGetGuess is set to False and the loop breaks, code then continues
				continueGetGuess = false;
			}
			//if the if statement functions properly and an exception is thrown, it is read out here
			catch (const InvalidEntry&)
			{
				cout << '\n';
				cout << "Please Enter a Valid Integer Guess." << "\n";
				cout << '\n';
			}
			//however, if the user enters something really wrong, like a string, ie "example string" or "three", the portion of the if conditional 
				//"stoi(guessValue) < 1 || stoi(guessValue) > 64" will break completely and throw an invalid argument. A string cannot be converted 
				//into an int, so this second catch block will receive that specific error and read the error message to the UI, and allow the 
				//user to continue entering guesses until they put in a valid one
			catch (const invalid_argument&)
			{
				cout << '\n';
				cout << "Please Enter a Valid Integer Guess." << "\n";
				cout << '\n';
			}
		}

		return static_cast<uint16_t>(stoi(guessValue));
	}


	//takes in all the info from DeckOfCards API and fills a vector with each individual char
	void DeckOfCards::StartOfData(const char arrData[], const unsigned int iSize) {
		jsonData.insert(jsonData.end(), arrData, arrData + iSize);
	}

	// Parse the jsonData char vector, and fill all of the DrawCard objects with the requisite data using the nlohmann parser
	void DeckOfCards::EndOfData() {
		json jp = json::parse(jsonData.begin(), jsonData.end());

		auto& results = jp["cards"];
		//auto count = results.size(); //line for debugging
		Card = new DrawCard * [results.size()];
		for (auto& jsonCard : results) //UPDATE: Changed Name of 'individualCardEntry' to the shorter 'jasonCard'
		{
			//Dynamic memory allocation
			DrawCard* newCard = new DrawCard;

			newCard->suit = jsonCard["suit"].is_null() ? "NULL" : jsonCard["suit"];
			newCard->value = jsonCard["value"].is_null() ? "Null" : jsonCard["value"];
			newCard->code = jsonCard["code"].is_null() ? "Null" : jsonCard["code"];

			Card[cardsDealt++] = newCard;
		};

	}

	
