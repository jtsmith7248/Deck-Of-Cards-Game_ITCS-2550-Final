
#include "LuckOfTheDrawGame.h"


///////////////////////////////////////////////////////////////////
//////////////////// COMPARE STRUCTS //////////////////////////////
///////////////////////////////////////////////////////////////////


bool CompareSuits::Compare(DrawCard* c1, DrawCard* c2) // S < C < D < H
{
	uint16_t val1 = c1->GetSuitValue();
	uint16_t val2 = c2->GetSuitValue();

	if (val1 == val2)
		return CompareValues::Compare(c1, c2);

	return val1 > val2;
}

bool CompareValues::Compare(DrawCard* c1, DrawCard* c2)
{
	uint16_t val1 = c1->GetNumericValue();
	uint16_t val2 = c2->GetNumericValue();

	if (val1 == val2)
		return CompareSuits::Compare(c1, c2);

	return val1 > val2;
};



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
////////////////////  HARD MODE CLASS  ////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////



//Overwritten from HttpClient so we can place number of cards dealt into const var numCardsRequested after json parsing 
bool LuckOfTheDrawGame::Get(const string uri, const map<string, string> qp) {

	bool get = HttpClient::Get(uri, qp);

	size_t& modifiableNumCards = const_cast<size_t&>(numCardsRequested);
	modifiableNumCards = GetCardsDealt() - 2;

	return get;
}



//Overwritten from DeckOfCards.h
void LuckOfTheDrawGame::GetRules() const {
	cout << "\n" << setfill(' ') << setw(34) << right << " [HARD MODE]: " << numCardsRequested << " CARDS ARE DRAWN" << "\n";
	cout << "" << setfill(' ') << setw(60) << right << "You get 5 attempts to guess the correct card number." << "\n" << "\n";
}


//Ensures any Jokers drawn in the requested cards are replaced 
void LuckOfTheDrawGame::SearchForJoker() {
	bool jokerFound = false;
	uint16_t quantityJokersFound = 0;

	SaveOutput(true); //saves the cards to be printed later as they appeared BEFORE the Joker search

	//we loop through all user requested cards (aka all cards minus the two bonus cards) first 
	for (uint16_t i = 0; i < numCardsRequested; i++) {
		//we look at each individual card's Value and see if that card is a Joker
		if (Card[i]->value == "JOKER") {
			//if it is, we output that a joker was found to the screen, and change the bool value jokerFound to ensure the 
				//'no joker found' message is ignored
			cout << "\n" << left << setfill(' ') << setw(20) << " " << left << setfill('*') << setw(25) << " " << "\n" << "\n";
			cout << setfill(' ');
			cout << setfill(' ') << right << setw(40) << " JOKER FOUND " << setw(20) << "\n";
			cout << "\n" << left << setfill(' ') << setw(20) << " " << left << setfill('*') << setw(25) << " " << "\n" << "\n";
			cout << setfill(' ');

			jokerFound = true;

			//This if is in place to catch a very rare situation. Remember, the deck has two jokers. Usually, if one of the user requested cards
				//is a joker, it gets replaced with the first bonus card (ie Card[numCardsRequested]). However, what if a Joker exists in the user
				//requested cards, AND the first bonus card is ALSO a Joker? In that situation, this if structure increments 
				//replaces the Joker with the second bonus card, then exits the loop, as no remaining cards will be a joker (both are found and accounted for)
			
					//Note: numCardsRequested is not the total number of cards- if the user requested 5 cards, numCardsRequested will be 5 but there will 
					// actually be 7 cards total, to account for the extra cards drawn to replace potential Jokers. 

			if (Card[numCardsRequested]->value == "JOKER") { // [numCardsRequested] is the index of the first bonus card, 
				*Card[i] = *Card[numCardsRequested + 1];	 //   [numCardsRequested + 1] is the second and final bonus card
				break;
			}

			*Card[i] = *Card[numCardsRequested + quantityJokersFound];

			quantityJokersFound++;
		}
	}

	if (!jokerFound) {
		cout << "\n" << left << setfill(' ') << setw(20) << " " << left << setfill('*') << setw(25) << " " << "\n" << "\n";
		cout << setfill(' ');
		cout << setfill(' ') << right << setw(41) << " NO JOKER DRAWN " << setw(20) << "\n";
		cout << "\n" << left << setfill(' ') << setw(20) << " " << left << setfill('*') << setw(25) << " " << "\n" << "\n";
		cout << setfill(' ');
	}

	SaveOutput(false); //saves the cards to be printed later as they appeared AFTER the Joker search
}


//Overwritten from DeckOfCards.h
void LuckOfTheDrawGame::RunGame() {
	//UPDATE: This block of 3 lines of code replaces 'GetTotalValue()'
	uint16_t answer = 0;
	for (uint16_t i = 0; i < numCardsRequested; i++)
		answer += Card[i]->GetNumericValue();

	bool guessedCorrectly = false;
	string loser = " YOU LOST! BETTER LUCK NEXT TIME! ";
	string winner = " YOU GOT IT RIGHT! ";
	//this loop takes the users guesses for the total value, ensures they're valid entries using GPG(), and outputs info based on the guess
	for (uint16_t remainingGuesses = 5; remainingGuesses > 0; remainingGuesses--) {
		//the int containing number of remaining guesses is sent into GPG(), and that function returns a string containing just a number. That num is 
			//then converted into a string and compared against answer
		uint16_t guessValue = GetPlayerGuess(remainingGuesses, static_cast<uint16_t>(numCardsRequested));

		if (guessValue > answer)
			cout << "Too High! Guess Again!" << "\n" << "\n";
		else if (guessValue < answer)
			cout << "Too Low! Guess Again!" << "\n" << "\n";
		else { //you found the correct answer!
			cout << "\n";
			cout << setfill('*') << "" << right << setw(46) << winner << setfill('*') << setw(28) << "" << "\n" << "\n";
			cout << setfill(' ') << "The Correct Answer Was " << answer << "!" << "\n" << "\n";
			cout << "**************************************************************************" << "\n" << "\n";
			guessedCorrectly = true;
			break;
		}
	}

	if (!guessedCorrectly)
	{
		cout << "\n";
		cout << setfill('*') << "" << right << setw(54) << loser << setfill('*') << setw(20) << "" << "\n" << "\n";
		cout << setfill(' ') << "The Correct Answer Was " << answer << "!" << "\n" << "\n";
		cout << "**************************************************************************" << "\n" << "\n";
	}


	//Sorting method picked, using loop to ensure valid entry
	bool picksorting = true;
	string pickSortMethod;
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
			cout << "\n" << "Please Input \"1\" or \"2\"." << "\n" << "\n";
		}
	}

	cout << "\n";

	//Here Bubblesort is called, using the appropriate comparefunction via templates
	if (pickSortMethod == "1")
		BubbleSort<CompareSuits>();
	else
		BubbleSort<CompareValues>();
}


//Overwritten from DeckOfCards.h
void LuckOfTheDrawGame::PrintResults() const {
	cout << setfill('*') << "" << right << setw(46) << " CARDS BEFORE JOKER SEARCH: " << setfill('*') << setw(28) << "" << "\n" << "\n";
	for (uint16_t i = 0; i < PreJokerSearchOutput.size(); ++i) {
		cout << setfill(' ') << setw(10) << right << i + 1 << ") ";
		cout << PreJokerSearchOutput[i] << "\n";
	}

	cout << setfill('*') << "" << right << setw(46) << " CARDS AFTER JOKER SEARCH: " << setfill('*') << setw(28) << "" << "\n" << "\n";
	for (uint16_t i = 0; i < PostJokerSearchOutput.size(); ++i) {
		cout << setfill(' ') << setw(10) << right << i + 1 << ") ";
		cout << PostJokerSearchOutput[i] << "\n";
	}

	cout << setfill('*') << "" << right << setw(46) << " PLAYER CARDS AFTER SORTING: " << setfill('*') << setw(28) << "" << "\n" << "\n" << setfill(' ');
	for (uint16_t i = 0; i < numCardsRequested; i++) {
		cout << setw(10) << right << i + 1 << ") " << *Card[i] << "\n";
	}

}


//SaveOutput saves the cards as they are before and after looking for Jokers into two separate vectors of strings
// based on which bool value is sent in
void LuckOfTheDrawGame::SaveOutput(bool preJokerOutput) {

	vector<string>* outputVector = nullptr;

	if (preJokerOutput) {
		PreJokerSearchOutput.reserve(numCardsRequested);
		outputVector = &PreJokerSearchOutput;
	}
	else {
		PostJokerSearchOutput.reserve(numCardsRequested);
		outputVector = &PostJokerSearchOutput;
	}


	for (uint16_t i = 0; i < numCardsRequested; i++) {
		stringstream card;
		card << *Card[i];
		outputVector->push_back(card.str());
	}

}