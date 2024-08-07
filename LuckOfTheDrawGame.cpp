
#include "LuckOfTheDrawGame.h"

///////////////////////////////////////////////////////////////////
//////////////////// COMPARE STRUCTS //////////////////////////////
///////////////////////////////////////////////////////////////////

bool CompareSuits::Compare(DrawCard* c1, DrawCard* c2)
{
	SUITS suit1 = c1->suit;
	SUITS suit2 = c2->suit;

	if (suit1 == suit2)
		return CompareValues::Compare(c1, c2);

	return suit1 > suit2;
}

bool CompareValues::Compare(DrawCard* c1, DrawCard* c2)
{

	if (c1->number == c2->number)
		return CompareSuits::Compare(c1, c2);

	return c1->number > c2->number;
};