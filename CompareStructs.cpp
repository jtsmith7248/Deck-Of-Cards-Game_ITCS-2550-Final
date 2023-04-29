// This file houses the struct CompareXXX templated functions

// BubbleSort pulls in these Compare functions using a template

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