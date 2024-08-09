
#include "GuessingGame.h"


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////  GUESSING GAME CLASS  //////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////



// Intro to Easy Mode game; replacing pure virtual
void GuessingGame::GetRules() const {
	cout << "\n" << setfill(' ') << setw(34) << right << " [EASY MODE]: " << GetCardsDealt() << " CARD IS DRAWN" << "\n"
		<< "" << setfill(' ') << setw(60) << right << "You get 5 attempts to guess the correct card number." << "\n" << "\n";
}



void GuessingGame::RunGame() {

	uint16_t answer = Card[0]->GetNumericValue(); //Card[0] because index 0 corresponds to the first and ONLY card in GuessingGame
	bool guessedCorrectly = false;
	string loser = " YOU LOST! BETTER LUCK NEXT TIME! ";
	string winner = " YOU GOT IT RIGHT! ";
	//this loop takes the users guesses for the total value, ensures they're valid entries using GPG(), and outputs info based on the guess
	for (uint16_t remainingGuesses = 5; remainingGuesses > 0; remainingGuesses--) {
		//the int containing number of remaining guesses is sent into GPG(), and that function returns a string containing just a number. That num is 
			//then converted into a string and compared against gameValue
		uint16_t guessValue = GetPlayerGuess(remainingGuesses, GetCardsDealt());

		if (guessValue > answer)
			cout << "Too High! Guess Again!" << "\n" << "\n";
		else if (guessValue < answer)
			cout << "Too Low! Guess Again!" << "\n" << "\n";
		else {
			cout << "\n";
			cout << setfill('*') << "" << right << setw(46) << winner << setfill('*') << setw(28) << "" << "\n" << "\n";
			cout << setfill(' ') << "The Correct Answer Was " << answer << "!" << "\n" << "\n";
			guessedCorrectly = true;
			break;
		}

	}

	if (!guessedCorrectly) {
		cout << "\n";
		cout << setfill('*') << "" << right << setw(54) << loser << setfill('*') << setw(20) << "" << "\n" << "\n";
		cout << setfill(' ') << "The Correct Answer Was " << answer << "!" << "\n" << "\n";
	}
}



void GuessingGame::PrintResults() const {
	cout << setfill('*') << "" << right << setw(46) << " PLAYER CARD DRAWN: " << setfill('*') << setw(28) << "" << "\n" << "\n";
	cout << setfill(' ') << setw(10) << right << "1) " << *Card[0] << "\n";
}