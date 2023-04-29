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

    // Sorting the order of suits and values(or numbers)
    template <class T>
    void BubbleSort(bool ascending)
    {
        // Instantiate a comparer object of the template type T
        T comparer;

        // Loop through the array of cards
        for (size_t i = 0; i < numOfCards - 1; i++)
        {
            // The inner loop will go through the unsorted part of the array 
            for (size_t j = 0; j < numOfCards - i - 1; j++)
            {
                // Determine whether the current element should be swapped with the next element
                // depending on whether the list is in ascending or descending order
                bool shouldSwap = false;

                // NOTES: in main(), we can ask the user which order they want to sort their cards in
                // Please check the commented code below to see an example of the implementation
                // If the list is in ascending order
                if (ascending)
                {
                    // Compare the current element with the next element
                    shouldSwap = comparer.Compare(Card[j], Card[j + 1]);
                }
                // If the list is in descending order
                else
                {
                    // Compare the next element with the current element
                    shouldSwap = comparer.Compare(Card[j + 1], Card[j]);
                }
                // If the current element should be swapped with the next element, 
                if (shouldSwap)
                {
                    // then swap them
                    swap(Card[j], Card[j + 1]);
                }
            }
        }
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
};

// WITH ENUM
struct CompareSuitEnum
{
public:
    bool Compare(DeckOfCards::DrawCard* c1, DeckOfCards::DrawCard* c2)
    {
        // Set the suits to numeric values
        int suitNum1 = 0;
        int suitNum2 = 0;

        switch (c1->suit)
        {
        case "HEARTS":
            suitNum1 = DeckOfCards::SUITS::HEARTS;
            break;
        case "DIAMONDS":
            suitNum1 = DeckOfCards::SUITS::DIAMONDS;
            break;
        case "CLUBS":
            suitNum1 = DeckOfCards::SUITS::CLUBS;
            break;
        case "SPADES":
            suitNum1 = DeckOfCards::SUITS::SPADES;
            break;
        default:
            suitNum1 = DeckOfCards::SUITS::NULLSUIT;
            break;
        }

        switch (c2->suit)
        {
        case "HEARTS":
            suitNum2 = DeckOfCards::SUITS::HEARTS;
            break;
        case "DIAMONDS":
            suitNum2 = DeckOfCards::SUITS::DIAMONDS;
            break;
        case "CLUBS":
            suitNum2 = DeckOfCards::SUITS::CLUBS;
            break;
        case "SPADES":
            suitNum2 = DeckOfCards::SUITS::SPADES;
            break;
        default:
            suitNum2 = DeckOfCards::SUITS::NULLSUIT;
            break;
        }

        return suitNum1 > suitNum2;
    }
};

// WITHOUT ENUM
struct CompareSuits
{
public:
    bool Compare(DeckOfCards::DrawCard* c1, DeckOfCards::DrawCard* c2)
    {
        // Set the suits to numeric values
        int suitNum1 = 0;
        int suitNum2 = 0;

        switch (c1->suit)
        {
        case "HEARTS":
            suitNum1 = 4;
            break;
        case "DIAMONDS":
            suitNum1 = 3;
            break;
        case "CLUBS":
            suitNum1 = 2;
            break;
        case "SPADES":
            suitNum1 = 1;
            break;
        default:
            suitNum1 = 0;
            break;
        }

        switch (c2->suit)
        {
        case "HEARTS":
            suitNum2 = 4;
            break;
        case "DIAMONDS":
            suitNum2 = 3;
            break;
        case "CLUBS":
            suitNum2 = 2;
            break;
        case "SPADES":
            suitNum2 = 1;
            break;
        default:
            suitNum2 = 0;
            break;
        }

        return suitNum1 > suitNum2;
    }
};

// WITH/WITHOUT ENUM
struct CompareValues 
{
public:
    bool Compare(DeckOfCards::DrawCard* c1, DeckOfCards::DrawCard* c2)
    {
        int value1 = c1->number;
        int value2 = c2->number;

        return value1 > value2;
    }
};

/*
* 
// Asking the user for their choice of data sorting
    int dataSortedAnswer = 0;
    cout << "How would you like the data sorted? (1 for Suits or 2 for Values): ";
    cin >> dataSortedAnswer;

    // Ascending and Descending options
    char dataOrder = ' ';
    cout << "Choose A for Ascending and D for Descending: ";
    cin >> dataOrder;
    dataOrder = toupper(dataOrder);
    cout << endl;
    bool order = true;
    if (dataOrder == 'D')
        order = false;

    // Displaying data
    switch (dataSortedAnswer) {
    case 1:
        uc.BubbleSort<CompareSuits>(order);
        break;
    case 2:
        uc.BubbleSort<CompareValues>(order);
        break;
    default:
        break;
    }
*/