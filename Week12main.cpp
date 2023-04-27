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
////////////////////////////////////////////////


//Here we declare our UnsplashClient Class, where everything outside of the UI and the comparisons will be taking place
class UnsplashClient : public HttpClient
{
public:
	//all the info is going to be read into our Photo Struct so each object can have multiple pieces of information, and we're setting that up here
	struct Photo
	{
		struct Author
		{
			string bio;
			string firstName;
			string lastName;
			string location;
		};

		Author author;
		string createdDate;
		string url;
		string description;
		int height;
		int likes;
		int width;

		//for this project, we only care about a few specific pieces of data being read out to the screen, so in this overloaded << func, we 
		//output the height, width, and area of a given photo, plus its date created and its Description. We also do a bit of formatting with endl's and indents
		friend ostream& operator<<(ostream& osObject, const Photo& x)
		{
			cout << "Height: " << x.height << ",		Width: " << x.width << ",		Area: " << (x.height * x.width) << ",		Date Created: " << x.createdDate << endl;
			cout << "Description: " << x.description << endl;
			return osObject;
		}
	};

	//in our destructor we just call DeleteMemory, which actually erases all the dynamically allocated data
	~UnsplashClient()
	{
		DeleteMemory();
		//added DeleteMemory to show what this would look like in production
	}

	//template goes to compare functions, T will be the compare class
	//In bubblesort, we are going to organize any given 10 objects from photos either by their date or size
	//to do this, we send in an object of the compareClass type we want
	template<typename T> 
	void BubbleSort(bool ascending)
	{

		T compareClassObject;

		//inside of the loop here we are going to be calling the 
		//compare functions from the T object and using the bool returns to determine whether the two objects 
		//we sent into photoObject are greater or less than each other , and then swapping or not swapping them as needed
		
		//photoCount will be equal to the index in photos, so it will be == 10
		//BubbleSort works by comparing each object to the next and swapping them based on a condition, usually if one is < or > the other
		//the outer loop will iterate one time less than the number of elements in the list/array/object/dataset, as 10 objects would mean that 
		//there are 9 comparisons to make. 
		//the inner loop will do the actual flipping, and it iterates one time less for each iteration of the main loop, as each time the main loop 
		//iterates once, the inner loop will have looped until the largest/smallest object has been placed at the top/bottom, and then by shrinking how many 
		//times the inner loop iterates, we make sure that largest/smallest object isnt touched/compared/swapped, lowering the number of comparisons and slightly
		//lowering how much power this sort structure takes from our machine

		for (int i = 0; i < photoCount - 1; i++) {
			for (int j = 0; j < photoCount - i - 1; j++) {
				//if the user wants it sortest largest to smallest, we enter here, otherwise we to the else
				if (ascending)
				{
					//this is where the compare classes are accesed. The comparison is outsourced there, and the func will return a 'true' value if the 
					//objects are to be swapped. Otherwise a false is returned and we move on to the next iteration
					if (compareClassObject.Compare(photos[j], photos[j + 1])) {
						auto temp1 = photos[j];
						photos[j] = photos[j + 1];
						photos[j + 1] = temp1;
					}
				}
				else
				{
					//by making this the same as above but with a '!' symbol, the sort order is inversed
					if (!compareClassObject.Compare(photos[j], photos[j+ 1])) {
						auto temp2 = photos[j];
						photos[j] = photos[j + 1];
						photos[j + 1] = temp2;
					}
				}
			}
		}
	}

	//returns a given photos object to main()
	const Photo* GetPhoto(const int i)
	{
		return photos[i];
	}

	//returns the total number of pages recieved from EndOfData() to main()
	int GetTotalPages()
	{
		return totalPages;
	}

	//overloaded << operator for this class returns all of the objects on a given page with additional number formatting
	//uses the overloaded << operator in the Photo struct to accomplish the output of each individual entry 
	friend ostream& operator<<(ostream& osObject, const UnsplashClient& x)
	{
		for (int i = 0; i < x.photoCount; i++)
		{
			cout << (i + 1) << ") " << *x.photos[i] << endl;
		}

		return osObject;
	}

	//just sets the initial photoCount to 0 before it is added to through iterations in EndOfData()
	UnsplashClient()
	{
		photoCount = 0;
	}

protected:
	//takes in all the info from UnsplashClient API and fills a vector woth each individual char
		//its a lot of chars
	void Data(const char arrData[], const unsigned int iSize)
	{
		//json parsed here, replicate a previous week
		jsonData.insert(jsonData.end(), arrData, arrData + iSize);
	}

	//*****TO DO***** [Find a use for this after everything else is done]
	void StartOfData()
	{
		//where do i take the query term from here? put it in the uc.Get func in main()?
		//maybe shut something else off in program when data begins to be parsed
	}

	//here we parse the jsonData char vector, and fill all of our photo objects with the requisite data using the nlohmann parser, one at a time 
	void EndOfData()
	{
		json jp = json::parse(jsonData.begin(), jsonData.end());
		//we grab totalPages, as it is one of three individual objects returned on their own by the API, before diving into the results array
		totalPages = jp["total_pages"];
		auto& results = jp["results"];
		//auto count = results.size(); //line for debugging
		photos = new Photo*[results.size()];
		for (auto& photo : results)
		{
			//each iteration through results fills a new photo object with the requisite data, then adds that object to our double pointer object 'photos'
			Photo* p = new Photo;
			//each line is a tertiary statement, making sure to add the phrase "Null" to p instead of the actual data object NULL if that is what is found
			//this prevents the program from breaking
			p->createdDate = photo["created_at"].is_null() ? "Null" : photo["created_at"];
			p->height = photo["height"].is_null() ? "Null" : photo["height"];
			p->width = photo["width"].is_null() ? "Null" : photo["width"];
			p->likes = photo["likes"].is_null() ? "Null" : photo["likes"];
			p->description = photo["alt_description"].is_null() ? "Null" : photo["alt_description"];

			p->url = photo["urls"]["full"].is_null() ? "Null" : photo["urls"]["full"];

			p->author.bio = photo["user"]["bio"].is_null() ? "Null" : photo["user"]["bio"];
			p->author.firstName = photo["user"]["first_name"].is_null() ? "Null" : photo["user"]["first_name"];
			p->author.lastName = photo["user"]["last_name"].is_null() ? "Null" : photo["user"]["last_name"];
			p->author.location = photo["user"]["location"].is_null() ? "Null" : photo["user"]["location"];
			//finish filling p
			photos[photoCount++] = p;
			//cout << "PHOTOS COUNT: " << photoCount << endl;
		};
		
		//cout << "*****************BEFORE COMPARING*****************" << endl;
		//for (int i = 0; i < photoCount; i++)
		//{
		//	cout << i + 1 << ") " << * photos[i] << endl;
		//}
		//cout << "*****************END BEFORE COMPARING*****************" << endl << endl;

	}

private:
	//here we are deleteing all tyhe dynamically allocated memory, fist by stepping through each object in photos[], then deleting photos itself
	//prevents data leaks- called from the destructor
	void DeleteMemory()
	{
		//used to free all dynamically allocated memory when required before a new API call
		for (int i = 0; i < photoCount; i++)
		{
			delete photos[i];
		}

		delete[] photos;
	}

	vector<char> jsonData;
	size_t photoCount;
	Photo** photos;
	int totalPages;

};


struct CompareArea
{
	//True is area1>area2, false is area1<=area2
	//True will be a swap, ordered smallest to largest, false will be a no swap
	bool Compare(UnsplashClient::Photo* p1, UnsplashClient::Photo* p2)
	{
		int area1 = p1->width * p1->height;
		int area2 = p2->width * p2->height;
		return (area1 > area2) ? true : false;
	}
};

struct CompareCreatedAt
{	
	///////////////////////01234     //note: index 0-24, length: 25
	/////////////0123456789
	///0123456789
	//"2017-05-19T14:01:58-04:00"
	bool date1Younger = false;

	//True is p1 is younger/more recent than p2, False is p1 is as old/older than p2
	//True will be a swap, ordered oldest to youngest, false will be a no swap
	bool Compare(UnsplashClient::Photo* p1, UnsplashClient::Photo* p2)
	{

		/*cout << "*****************COMPARING*****************" << endl;
		cout << *p1 << endl;
		cout << *p2 << endl;
		cout << "*****************END COMPARING*****************" << endl << endl;*/

		//here we get the full date strings for each object, and if the first is null/the second is null while the first is NOT we return false/true 
		//respectively in order to send all "Null" dates to the top/bottom. 
		string date1 = p1->createdDate;
		string date2 = p2->createdDate;

		if (date1 == "Null")
			return false;
		else if (date2 == "Null")
			return true;
		//otherwise we parse the date strings func by func. Each one compares one part of the string, returning true into date1Younger if date one is 
		//the more recent, meaning they need to be swapped, and returning false if date1 is Larger, meaning they are not to be swapped. Then we check to see
		//if they are the same. If they arent, a false is returned from the func and the if breaks, and then the newly set date1Younger bool object is 
		//returned to BubbleSort(). If they are the same, d1Y would not have yet been set, and a true is returned from the func, so we climb deeper into 
		//the structure seen below. If the dates&times are the same down to the last second, a false is returned and they are not swapped. Otherwise, at some
		//point the structure is exited and d1Y is returned
		else
		{
			//the aformentioned structure
			if (CompYear(date1, date2))
				if (CompMonth(date1, date2))
					if (CompDay(date1, date2))
						if (CompHour(date1, date2))
							if (CompMin(date1, date2))
								if (CompSec(date1, date2))
									return false;   //if you make it all the way through each condition, then every value in the date strings are equal and they are the exact same time, down to the exact same second, and don't need to be swapped

			return date1Younger;
		}
		
	}

	bool CompYear(string d1, string d2)
	{
		int y1 = stoi(d1.substr(0, 4));
		int y2 = stoi(d2.substr(0, 4));

		if (y1 < y2)
			date1Younger = false;
		else if (y1 > y2)
			date1Younger = true;

		return (y1 == y2) ? true : false;
	}

	bool CompMonth(string d1, string d2)
	{
		int m1 = stoi(d1.substr(5, 7));
		int m2 = stoi(d2.substr(5, 7));

		if (m1 < m2)
			date1Younger = false;
		else if (m1 > m2)
			date1Younger = true;

		return (m1 == m2) ? true : false;
	}

	bool CompDay(string d1, string d2)
	{
		int day1 = stoi(d1.substr(8, 10));
		int day2 = stoi(d2.substr(8, 10));

		if (day1 < day2)
			date1Younger = false;
		else if (day1 > day2)
			date1Younger = true;

		return (day1 == day2) ? true : false;
	}

	bool CompHour(string d1, string d2)
	{
		//Getting the hour time and then subtracting the offsets from UTC
		//
		//UST follows the Prime Meridian in the Atlantic through England. 
		//As hours INCREASE away from UST, we approach the International Date Line from the front, finding the places in the world 
		//that experience a time FIRST. 1PM in the new time will have happened BEFORE 1pm UST. 
		//As we DECREASE away from UST, we approach the IDL from the back, finding the places that experience a time LAST. 1PM in these
		//time zones happen AFTER 1pm UST. This is why New Year's in Sydney, Australia happens 20 hours before New Year's in Honolulu, 
		//Hawaii, despite both occuring, according to the clock, at 12AM Jan 1. 
		// 
		//So, if two times were exactly the same at 5AM except h1 is in EST (UTC-4) and h2 is in JST(UTC+8), we get
		//h1 = 5-(-4)=9 and h2=5-(+8)=-3
		//The smaller int time is the oldest. We want to set date1Younger = false and avoid swapping if h1 is older, aka h1<h2
		//In our example, by including the offsets, we find that 5am JST occurs before 5am EST, and h2<h1, meaning they need to be
		//swapped and we can return true
		//
		//If no time zone is given, the 20th char in the string will be a Z, and in that case we ignore the impact of timezones

		int hour1, hour2;

		if (d1.substr(19, 20) == "Z" || d2.substr(19, 20) == "Z")
		{
			hour1 = stoi(d1.substr(11, 13));
			hour2 = stoi(d2.substr(11, 13));
		}
		else
		{
			hour1 = stoi(d1.substr(11, 13)) - stoi(d1.substr(19, 22));
			hour2 = stoi(d2.substr(11, 13)) - stoi(d2.substr(19, 22));
		}

		if (hour1 < hour2)
			date1Younger = false;
		else if (hour1 > hour2)
			date1Younger = true;

		return (hour1 == hour2) ? true : false;

		//Small note: there are non-standard time zones, where instead of being different by an even number of hours, theyre off by 
		//1 1/2 or even 1 3/4s hours. Here, if they are off by an extra 30 or 45 minutes, that leftover difference isn't caught. Consider
		//it a bug to be fixed later haha
	}

	bool CompMin(string d1, string d2)
	{
		int min1 = stoi(d1.substr(14, 16));
		int min2 = stoi(d2.substr(14, 16));

		if (min1 < min2)
			date1Younger = false;
		else if (min1 > min2)
			date1Younger = true;

		return (min1 == min2) ? true : false;
	}

	bool CompSec(string d1, string d2)
	{
		int s1 = stoi(d1.substr(17, 19));
		int s2 = stoi(d2.substr(17, 19));

		if (s1 < s2)
			date1Younger = false;
		else if (s1 > s2)
			date1Younger = true;

		return (s1 == s2) ? true : false;
	}
};




////////////////////////////////////////////////
//int main(int argc, char* argv[])
//{
//	// Leave this, as it will tell you if you have any memory leaks.
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//
//	// See https://stackoverflow.com/questions/24708700/c-detect-when-user-presses-arrow-key
//	const int ARROW_UP	 = 72;
//	const int ARROW_DOWN = 80;
//	const int VIEW = 118;
//	const int EXTENDED_KEY = 224;
//	const int NEW = 110;
//
//	string queryTerm; //
//	int pageNum = 1;
//	int compSelection = 0; //0
//	int ascendingDescendingInput = 0; //0
//	int picSelection;
//	int continuationChoice;
//	bool runProgram = true;
//	bool noExtended = true;
//	bool newTermSearch = true;
//	bool breakSwitch = false;
//	bool grabURL = false;
//	bool ascendingBool;
//
//	//little entrance message
//	cout << "*********************************************************" << endl << endl;
//	cout << "Welcome To My Unsplash API App" << endl << endl;;
//	cout << "*********************************************************" << endl << endl;
//
//	//the whole program is one big loop that can eventually be exited
//	while (runProgram)
//	{
//		//if we are looking for a newSearchTerm, we get that and take a few more user inputs. This should obviously be true when the program first runs, 
//		//but as the user scrolls through new pages, they won't be looking for a new term, so this block is then skipped
//		if (newTermSearch)
//		{
//			cout << "Please Enter a Query Term Here: ";
//			cin >> queryTerm;
//			cout << endl;
//			//these loops prevent the user from entering invalid entries into the program. can only be exited when they make an allowed choice
//			while (compSelection != 1 && compSelection != 2)
//			{
//				cout << "Sort by Date Created [Enter 1] or Picture Area [Enter 2]: ";
//				cin >> compSelection;
//			}
//			cout << endl;
//			while (ascendingDescendingInput != 1 && ascendingDescendingInput != 2)
//			{
//				cout << "Sort Ascending [Enter 1] or Descending [Enter 2]: ";
//				cin >> ascendingDescendingInput;
//			}
//			cout << endl << endl;
//
//			//takes the 1 or 2 they entered coresponding to ascending/descending and converts it to a boolean var
//			ascendingBool = ascendingDescendingInput == 1 ? true : false;
//		}
//		
//		// An example use invoking an GET on the Unsplash API.
//		// Notice the query term and the page number.
//		// only use once per loop, create and destroy each iteration, each query should be a different unsplash client
//		
//		UnsplashClient uc;
//		uc.Connect("api.unsplash.com");
//		//my individual UnsplashClient API key is added
//		uc.SetAuthToken("Client-ID", "s17CxGFZrH782-tlJMRgpmVP9b4npi2IibLunn13c-4");
//		//queryTerm and pageNum sent to the API is dynamic
//		uc.Get("/search/photos", { {"query", queryTerm}, {"page", to_string(pageNum)} });
//
//		//User decides how they want data sorted here
//		if (compSelection == 1)
//			uc.BubbleSort<CompareCreatedAt>(ascendingBool);
//		else
//			uc.BubbleSort<CompareArea>(ascendingBool);
//
//		//output that data to the screen
//		cout << uc;
//
//		//use of GetTotalPages func to do just that 
//		cout << "------------- Page " << pageNum << " of " << uc.GetTotalPages() << " -------------" << endl << endl;
//
//		breakSwitch = false;
//		/*while (true)
//		{
//			auto input = _getch();
//			cin >> input;
//			cout << "INPUT: " << input << endl << endl;
//		}*/
//
//		//This structure uses _getch() to take keystrokes from the user and convert them into choices. I used many bool values to control what happens here, 
//		//and each is explained here, basically explaining what is happening overall:
//			//breakSwitch: controls the looping of the end of page user input. when a user reaches the end of a page, they have many options explained in
//				//line 486. If breakSwitch == false, we are NOT breaking out of this loop that repeats the _getch() switch statement. We stay within this loop
//				//for many reasons; namely if the user views an image and needs to be asked what to do next, if they try to see before the first page ot after
//				//the last, or if _getch() catches an extended key and not the user choice
//			//noExtended: this bool catches whether or not to ask the user for input. Added because when using the arrow keys on my machine, pressing any of
//				//the 4 arrow keys would send in two values: the extended key value (224) and then also the proper value; 72 for up, 80 for down. when the switch
//				//catches the extended key, we just set noExtended to true because the extended key WAS found, and we proceed to process the actual entry given by
//				//the user (ie ARROW_UP, etc). If anything except case EXTENDED_KEY is truggered, noExtended is set to true to ensure the user is given the input 
//				// statement
//			//newTermSearch: mentioned on line 422-423. When a user wants to see the previos/next page, they are NOT looking for a new search term, but they 
//				//are looking for the next array of results from the API, so we set this to false to skip over the inputs at the start of the main loop and just
//				//grab the infor from the API with the same search term but on the next/previous page. This is only set back to true when the user decides to make 
//				//a new search by entering 'n'
//			//runProgram: when this is true, which it is by default, the whole program runs. If the user enters a key not found by the switch structure, theyve
//				//decided to end the program, so we break out of the switch loop by changing breakSwitch, output a goodbye message, and come to the end of main()
//			//grabURL: when the user chooses to view an image and enters which array entry they want, this is set to true, and then at the end of the switch structure
//				//a new if structure is triggered that opens the given url in their default browser and we reset this back to false, until the user chooses to view
//				//another image 
//			
//		
//		while (!breakSwitch)
//		{
//			if (noExtended)
//				cout << "End of Data... Press Up/Down Arrow Keys to Paginate, \"v\" to View an Image, \"n\" to Search a New Term, or Any Other Key to Exit: ";
//
//			switch (continuationChoice = _getch())
//			{
//				case EXTENDED_KEY:
//					noExtended = false;
//					break;
//
//				case ARROW_UP:
//					noExtended = true;
//					if (pageNum == 1)
//					{
//						cout << endl << "You're On The First Page!" << endl;
//						break;
//					}
//					else
//					{
//						pageNum--;
//						breakSwitch = true;
//						newTermSearch = false;
//						cout << endl << endl;
//						break;
//					}
//
//				case ARROW_DOWN:
//					noExtended = true;
//					if (pageNum == uc.GetTotalPages())
//					{
//						cout << endl << "You're On The Last Page!" << endl;
//						break;
//					}
//					else
//					{
//						pageNum++;
//						breakSwitch = true;
//						newTermSearch = false;
//						cout << endl << endl;
//						break;
//					}
//
//				case VIEW:
//					noExtended = true;
//					do
//					{
//						cout << endl << "Select Image You Wish To View [Enter 1-10]: ";
//						cin >> picSelection;
//					} while (picSelection < 1 || picSelection > 10);
//					grabURL = true;
//					break;
//
//				case NEW:
//					noExtended = true;
//					breakSwitch = true;
//					newTermSearch = true;
//					pageNum = 1;
//					cout << endl << endl;
//					break;
//
//				default:
//					noExtended = true;
// 					breakSwitch = true;
//					runProgram = false;
//			}
//
//			if (grabURL)
//			{
//				string url = uc.GetPhoto(picSelection - 1)->url;
//				// An example of launching a browser with a URL to a page. 
//				// See:https://docs.microsoft.com/en-us/windows/win32/shell/launch for more details.
//				ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
//				grabURL = false;
//			}
//		}
//
//		if (!runProgram)
//		{
//			cout << endl << endl;
//			cout << "*********************************************************" << endl << endl;
//			cout << "I Hope You Enjoyed My Unsplash API App" << endl << endl;;
//			cout << "*********************************************************" << endl << endl;
//		}
//	}
//
//	
//}