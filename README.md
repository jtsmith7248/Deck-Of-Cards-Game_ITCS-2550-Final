# DeckOfCardsGame

 DECKOFCARDS FINAL PROJECT - JUSTIN SMITH, NGUYEN TRAN, VIOLET YOUSIF 
 
 GAME OVERVIEW:
 
- This project builds off of the work we have been doing in ITCS 2550 for weeks and uses the HttpClient class to connect to the Internet in order to send requests and read from an API called Deck of Cards. Using this API, we send in a request for a user to request a number of cards. They will get dealt back that many cards out of a deck of 52 or 54. Then, the user will play a little mini game using these randomly dealt cards.

- There are two mini games-GuessingGame (Easy Mode) and LuckOfTheDrawGame (Hard Mode). In these two games, we implement multiple different concepts covered within the semester. Inheritence, Polymorphism, Overloaded Operators, Templates, Enums, Vectors, Searching, Sorting, Exception Handling, Dynamic Memory Allocation, Double Pointers, Maps, and a Clean UI are all utilized at one point or another throughout this program.
  - In Minigame 1, aka Easy Mode or GuessingGame, the user has to guess the value of a randomly drawn card. They have five chances to get it, and the values range from "1" for any Ace drawn to "13" for a King.
  - In Minigame 2, aka Hard Mode or LuckOfTheDrawGame, the user is asked how many cards they would like to draw, from 1-5. A deck of cards contains 54 cards instead of the standard 52 in this game, as two jokers are mixed into the bunch. The Minigame class then searches for any Joker in the drawn cards and replaces it if found, otherwise outputs a 'No Jokers Found' message. Then the user has to guess the total value of all of the cards added together. This ranges from  2 (a single Ace drawn) to 64 (4 Kings and a Queen drawn). The range is dynamic based on user input of cards.
  - After either succeeding in guessing the total value with 5 attempts or not, the game concludes by outputting 3 blocks of text:
    1. All the cards the user asked for as they were originally returned from the API.
    2. Those same cards, but now with the Joker(s) replaced if any were found.
    3. All the newly updated cards, ordered by the criteria requested by the user.
