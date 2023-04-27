// This game recieves x-amount (based on user choice) of cards, sorts and compares values to print in order
// Maybe to make this more game-like we can draw a second hand of cards for computer-user to compare overall values?
// OR ask user if they can guess whether their hand is higher or lower than a certain number to win?
    // suits draw > 8
    // number draw > 25?

class LuckOfTheDrawGame : public DeckOfCards
{
private:
    int numOfCards = 0;

public:
    int SetNumOfCards(int num) {
        // User is asked how many cards they'd like to draw; max 5
        // exception handling if over 5
        // return numOfCards = num;
    }

    // void DrawHand(DrawCard*, int)
    DrawCard* DrawHand(int)
    {
        // Use max count of drawn cards to loop over cards
    }

    // Use DrawHand func to search for joker card
    void SearchForJoker
    {
        // check each card for joker
        // if joker is found, draw new card and pop joker card and push_back new card
    }

    DrawCard* GetSuits(string s) {
        // get arrangement of suits from BubbleSort()
        // return suit
    }

    // replaced GetValue name shown in skeleton sketch
    DrawCard* GetNumber(int n) {
        // get arrangement of numbers from BubbleSort()
        // return suit
    }

    //PrettyPrint the response?
}