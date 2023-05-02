////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


    //DECKOFCARDS FINAL PROJECT - JUSTIN SMITH, NGUYEN TRAN, YILOET YOUSIF 

    //This project builds off of the work we have been doing in ITCS 2550
        //for weeks, and uses the HttpClient class to connect to the
        //internet and both send requests and read from an API called 
        //Deck of Cards. Using this API, we send in a request for a user
        //requested number of cards, get dealt back that many cards out of 
        //a deck of 52/54, and then have the user play a little mini game 
        //using these randomly dealt cards. 

    //There are two mini games- GuessingGame (Easy Mode) and 
        //LuckOfTheDrawGame (Hard Mode). Using these two games within the
        //project, we use multiple different concepts covered within the 
        //semester. Inheritence, Polymorphism, Overloaded Operators, 
        //Templates, Enums, Vectors, Searching, Sorting, Exception Handling, 
        //Dynamic Memory Allocation, Double Pointers, and a Clean UI are all
        //utilized at one point or another throughout this program. 

    //In Minigame 1, aka Easy Mode or GuessingGame, the user has to guess
        //the value of a randomly drawn card. They have five chances to get
        //it, and the values range from "1" for any Ace drawn to "13" for a 
        //King

    //In Minigame 2, aka Hard Mode or LuckOfTheDrawGame, the user is asked 
        //how many cards they would like to draw, from 1-5. A deck of cards 
        //contains 54 cards instead of the standard 52 in this game, 
        //as two jokers are mixed into the bunch. The Minigame class then 
        //searches for any Joker in the drawn cards and replaces it if found, 
        //otherwise outputs a 'No Jokers Found' message. They the user has
        //to guess the total value of all of the cards added together. This
        //ranges from  1 (a single Ace drawn) to 64 (4 Kings and a Queen 
        //drawn). 
        
        //After either succeeding in guessing the total value with 5 
        //attempts or not, the game concludes by outputting 3 blocks 
        //of text:
            //1. All the cards the user asked for as they were originally 
                //returned from the API
            //2. Those same cards, but now with the Joker(s) replaced if 
                //any were found
            //3. All the newly updated cards, ordered by the criteria
                //requested by the user


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

//Before reaching the classes and main, we have our global objects that can be called from anywhere. this includes the enum used for the suits, as they need to be
    //accessible in multiple areas, the conversion functions that transfer the enum data type back and forth between itself and a string, and finally the function
    //G2_GetPlayerGuess(), which takes user input inside of a loop in main, and the code looked cleaner and more understandable when it was outsourced out of main()

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

//This function is called in main from within a loop, and is itself a loop ensuring the user inputs a valid entry via exception handling,
    //so it was outsorced here, into a global function. 
    //G2_GPG() takes in an int containing the number of remaining guesses a user has to guess the total value of the cards they drew, added together. This
    //is designed specifically for Hard Mode
string G2_GetPlayerGuess(int remainingGuesses)
{
    bool continueGetGuess = true;
    string guessValue;
    string errorMessage2 = "Please Enter a Valid Integer Guess.";

    while (continueGetGuess)
    {
        cout << remainingGuesses << " Guesses Remaining!" << endl;
        cout << "Please Enter a Guess for the Total Value of All Cards Added Together [1-64]: ";

        try
        {
            //brings in user guess
            getline(cin, guessValue);

            //if the user enters something that 
                //1. Breaks the cin statement (ie. invalid data type)
                //2. Is a Number less than 1 or greater than 64
                //3. Has a decimal
            //an exception message will be thrown and the user will be asked to re-enter their guess
            if (cin.fail() || stoi(guessValue) < 1 || stoi(guessValue) > 64 || guessValue.find(".") != string::npos)
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

    struct DrawCard
    {
        string number;		// API refers to this as "value"
        SUITS suit;		// [ S(1), C(2), D(3), H(4) ]
        string code;		// For printing purposes -- Ended up not being particularly necessary, but just adds information for the user to see

        // Overload insertion "<<" operator
        // When a DrawCard object is output, this func is called
        friend ostream& operator<<(ostream& osObject, const DrawCard& draw)
        {
            cout << endl << "	Suit: " << SuitToString(draw.suit) << endl << "	Number: " << draw.number << endl << "	Code: " << draw.code << endl;

            return osObject;
        }

        //OutputToString takes the information that would usually be output to the screen and instead reads it into a string to be read later
            //Declared as static so that it can be accessible without having to declare a DrawCard object (ie drawCardObject.OutputToString() is
                //unneccessary using the static keyword, and this func can be accessed just using DrawCard::OutputToString(parameter) from elsewhere
        static string OutputToString(const DrawCard& draw)
        {
            string cardOutput = "\n     Suit: " + SuitToString(draw.suit) + "\n     Number: " + draw.number + "\n     Code: " + draw.code + "\n";
            return cardOutput;
        }
    };

    // Constructor, empty
    DeckOfCards() {}

    // Destructor, very similar to week 12
    ~DeckOfCards()
    {

        for (int i = 0; i < cardsDealt; i++)
        {
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

    //SaveOutput is the function, called in main() via a Hard Mode/Minigame 2 object that saves the cards as they are 
        //at a given moment into either the string savePreJokerOutput or savePreSortOutput, based on which bool value is sent in
    void SaveOutput(bool preJokerOutput)
    {
        string output;

        //Note that instead of i < cardsDealt, its i < (cardsDealt - 2)
            //This is because, later on in the code, we need to replace up to two jokers if they're found in the user requested cards.
            //To do this, the API is set to always return two more cards than the user actually asked for, so that those two bonus cards can 
            //replace a joker or two if they're found. So, regardless of what happens in SearchForJoker(), we only want the cards the user 
            //requested to be added to the strings and output to the screen. The final two bonus cards are to always stay hidden from the user.
        for (int i = 0; i < (cardsDealt - 2); i++)
        {
            output += to_string(i + 1) + ") " + DrawCard::OutputToString(*Card[i]) + "\n";
        }

        if (preJokerOutput)
            savePreJokerOutput = output;
        else
            savePreSortOutput = output;
    }

    //This function is called both in BubbleSort and in SearchForJoker()
    //It simply replaces all the information in Card 1 with the info in Card 2
    void SetNewCardEntry(int c1, int c2)
    {
        Card[c1]->code = Card[c2]->code;
        Card[c1]->number = Card[c2]->number;
        Card[c1]->suit = Card[c2]->suit;
    }

    // Uses overloaded insertion "<<" operator in DrawCard struct to stream output of DeckOfCards class
    friend ostream& operator<<(ostream& osObject, const DeckOfCards& dc) {

        //Again, note the 'cardsDealt - 2', as explained in the comments in SaveOutput()
        for (int i = 0; i < (dc.cardsDealt - 2); i++)
        {
            cout << i + 1 << ") " << *dc.Card[i] << endl;
        }
        cout << "**********************************" << endl << endl;

        return osObject;
    }

    // Sorting the order of suits and values(or numbers)
        //Textbook bubblesort problem, with added template that compares the cards either by suit or by value
    template <typename T>
    void BubbleSort()
    {
        T compareClassObject;

        //Again, note the 'cardsDealt - 2', as explained in the comments in SaveOutput()
        int jokerlessCardsDealt = cardsDealt - 2;

        for (int i = 0; i < jokerlessCardsDealt - 1; i++) {
            for (int j = 0; j < jokerlessCardsDealt - i - 1; j++) {
                if (compareClassObject.Compare(Card[j], Card[j + 1])) {
                    //Was having trouble with the assignment operator and the Card[] objects, so each element individually is swapped 
                        //instead of whole card objects at a time ala week 12
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
            //cout << "CARDS COUNT: " << cardsDealt << endl;
        };

    }

private:

    vector<char> jsonData;
    size_t cardsDealt = 0; 
    DrawCard** Card;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//Alright! Here we declare the Minigame 2 class, aka Hard Mode, aka LuckOfTheDrawGame
//This contains functions specific to the mechanics of game 2, namely the SearchforJoker() function and 
//the GetTotalValue() of all cards function
class LuckOfTheDrawGame : public DeckOfCards
{
private:
    int numCards;

public:

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
                cout << endl << "***JOKER FOUND***" << endl << endl;
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
            cout << endl << "***No Joker Drawn***" << endl << endl;

    }

    //we use a constructor with a parameter to send in the number of cards into this class
    LuckOfTheDrawGame(int numberOfCards)
    {
        numCards = numberOfCards;
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

    //Just converts the string number to an int
    //Defined as static so that it is accessible inside of the CompareValues struct without having to declare an object of this LOTD/MG2 class
    static int GetIndividualCardValue(string value)
    {
        if (value == "ACE")
            return 1;
        else if (value == "2")
            return 2;
        else if (value == "3")
            return 3;
        else if (value == "4")
            return 4;
        else if (value == "5")
            return 5;
        else if (value == "6")
            return 6;
        else if (value == "7")
            return 7;
        else if (value == "8")
            return 8;
        else if (value == "9")
            return 9;
        else if (value == "10")
            return 10;
        else if (value == "JACK")
            return 11;
        else if (value == "QUEEN")
            return 12;
        else if (value == "KING")
            return 13;
        else
        {
            cout << "ERROR" << endl;
            return 0;
        }
    }

};

//These Compare Structs rely on eachother in occasional rare situations, so they had to be defined in this way to avoid issues with circular dependancy
    //If the user decides to have the cards sorted by suit or by value, bubble sort will swap cards based on the respective compare object. However, in the case
    //we're comparing by suits and the two cards being compared have the same suit, the idea was to instead sort just those two cards by value. We also wanted it
    //to work in the opposite way- if two cards are the same value, ie a Jack and a Jack, we want them sorted by the hierarchy of their suits. So in both compare functions, 
    //if the values are equal, the other compare struct is called. This caused a problem however because now each compare function in its respecive struct now
    //technically relied on the other!

//The way this was solved was by declaring each struct piece by piece, so that each struct was aware of the other and could access the function being called without 
    //the compailer failing to understand where to go. First the conceot of CompareValues is declared, then the definition of CompareSuits, then the definition of 
    //CompareValues, then finally the actual code inside of each compare function. This allows the compiler to understand where to go when each function is called
    //within the other

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
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    string cinNumCardsString;
    string exit;
    string exceptionMessage = "Invalid Entry. Please Enter an Integer in the Given Range.";
    string gameChoice;
    int numCardsInt;
    bool continueGetNumCards = false;
    bool pickGame = true;
    bool exitProgram = false;

    //little entrance message
    cout << "*********************************************************" << endl << endl;
    cout << "Welcome To The Deck of Cards Minigame Player" << endl << endl;;
    cout << "*********************************************************" << endl << endl;

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

        //here we will add the code connecting main to minigame1/easy mode/guessing game
        if (gameChoice == "1")
        {
            //need to add some code here to bring in Violet's minigame
            cout << "Only Choice 2 for now";
        }
        //JHere, Minigame 2 is triggered
        else
        {
            continueGetNumCards = true;
            //this block ensures, using exception handling, that a user enters the proper value. Similar to the function G2_GetPlayerGuess(), see above
            while (continueGetNumCards)
            {
                cout << "How Many Cards Would You Like to See? [Enter Number 1-5]: ";

                try
                {
                    getline(cin, cinNumCardsString);

                    auto x = cinNumCardsString.find(".");

                    if (cin.fail() || stoi(cinNumCardsString) < 1 || stoi(cinNumCardsString) > 5 || cinNumCardsString.find(".") != string::npos)
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

            //bool keepGuessing = true;
            bool picksorting = true;
            bool guessedCorrectly = false;
            string pickSortMethod;
            int guessValue;
            int remainingGuesses = 5;

            //here we take the user input for number of cards to be drawn, convert it to an int, add 2 to account for the bonus cards, and place that number
                //into cinNumCardsString. This string is what is sent over to the API
            int addJokers = stoi(cinNumCardsString);
            addJokers += 2;
            cinNumCardsString = to_string(addJokers);

            //we send in the updated int value for total number of cards into the LOTDG object
            LuckOfTheDrawGame g2(addJokers);
            g2.Connect("deckofcardsapi.com");

            //here we connect to .Get, which is an inherited function from HttpClient, which then calls Data and EndOfData in Deck of Cards, as LOTDG inherits 
                //publically from both classes
            g2.Get("/api/deck/new/draw", { {"count", cinNumCardsString}, {"jokers_enabled", "true"} });

            //here we save the cards exactly as they were returned from the API into the first string
            g2.SaveOutput(true);

            cout << endl << "***************************" << endl << endl;

            //then we search for and potentially replace the jokers
            g2.SearchForJoker();

            //now we save the newly updated, potentially now jokerless cards into the second string
            g2.SaveOutput(false);

            //here, we save the value of all the cards added together into gameValue
            int gameValue = g2.GetTotalValue();

            cout << endl << "***************************" << endl << endl;

            //this loop takes the users guesses for the total value, ensures theyre valid entries using G2_GPG(), and outputs info based on the guess
            for (int i = 0; i < 5; i++)
            {
                //the int containing number of remaining guesses is sent into G2_GPG(), and that function returns a string containing just a number. That num is 
                    //then converted into a string and compared against gameValue
                guessValue = stoi(G2_GetPlayerGuess(remainingGuesses));

                if (guessValue == gameValue)
                {
                    cout << endl << "***YOU GOT IT RIGHT!***" << endl;
                    cout << "The Correct Answer Was " << gameValue << "!" << endl << endl;
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
                cout << endl << "***YOU FAILED!***" << endl;
                cout << "The Correct Answer Was " << gameValue << "!" << endl << endl;
            }

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

            //Here Bubblesort is called, using the appropriate comparefunction via templates
            if (pickSortMethod == "1")
                g2.BubbleSort<CompareSuits>();
            else
                g2.BubbleSort<CompareValues>();

            //Finally, all versions of the cards are output to the user 
            cout << endl << "************CARDS BEFORE JOKER SEARCH***************" << endl << endl;

            cout << g2.savePreJokerOutput;

            cout << endl << "************CARDS AFTER JOKER SEARCH***************" << endl << endl;

            cout << g2.savePreSortOutput;

            cout << endl << "************CARDS AFTER SORTING***************" << endl << endl;

            cout << g2;
        }
           
        //This final block just asks the user if they want to play again, ending if they enter an "x", and continuing otherwise
        cout << endl << "Do You Wish to Exit Program? [Enter \"x\" for \'Yes\', Any Other Key for \'No\']: ";
        getline(cin, exit);

        if (exit == "x")
            exitProgram = true;

        cout << endl;
    }

};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////



