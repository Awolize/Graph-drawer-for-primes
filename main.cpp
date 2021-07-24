#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <locale>

using namespace std;
using namespace std::chrono;

int ypos{ 780 };
int step{ 80 };

bool IsPrime(size_t n)
{
	if (n == 2 || n == 3)
		return true;

	if (n <= 1 || n % 2 == 0 || n % 3 == 0)
		return false;

	for (size_t i = 5; i * i <= n; i += 6)
	{
		if (n % i == 0 || n % (i + 2) == 0)
			return false;
	}

	return true;
}


string display(std::chrono::milliseconds ns)
{
	using namespace std;
	using namespace std::chrono;

	typedef duration<int, ratio<86400>> days;

	stringstream ss;
	char fill = ss.fill();
	ss.fill('0');

	auto d = duration_cast<days>(ns);
	ns -= d;
	auto h = duration_cast<hours>(ns);
	ns -= h;
	auto m = duration_cast<minutes>(ns);
	ns -= m;
	auto s = duration_cast<seconds>(ns);
	ns -= s;
	auto ms = duration_cast<milliseconds>(ns);

	ss << setw(2) << d.count() << "d:"
		<< setw(2) << h.count() << "h:"
		<< setw(2) << m.count() << "m:"
		<< setw(2) << s.count() << "s:"
		<< setw(2) << ms.count() << "ms";

	ss.fill(fill);
	return ss.str();
};


void calcPrimes(
	size_t start,
	size_t stop,
	vector<sf::CircleShape>& pointList,
	bool(&calcList)[8],
	int i)
{
	size_t numofPrimes = 0;
	size_t totalInteractions = 0;
	vector<size_t> primes;

	auto startTime = high_resolution_clock::now();

	for (size_t increment = start; primes.size() != stop; increment++)
	{
		if (IsPrime(increment))
		{
			numofPrimes++;
			primes.push_back(increment);
		};
		totalInteractions = increment;
	};

	auto stopTime = high_resolution_clock::now();
	auto calcDuration = std::chrono::duration_cast<milliseconds>(stopTime - startTime);


	startTime = high_resolution_clock::now();
	size_t totPrimeGaps = 0;
	for (size_t i = 0; i < (primes.size() - 1); i++)
	{
		totPrimeGaps += (primes[i + 1] - primes[i]);
	};

	stopTime = high_resolution_clock::now();
	auto gapDuration = std::chrono::duration_cast<milliseconds>(stopTime - startTime);


	pointList[i].move((((float(totPrimeGaps) / numofPrimes) * step) + 140), ypos);
	ypos -= 80;


	cout << endl << fixed << setprecision(2);
	cout << "Calculated:  " << stop << " primes." << endl;
	cout << "Stopped on:  " << totalInteractions << endl;
	cout << "Average gap: " << float(totPrimeGaps) / stop << endl;
	cout << "Calc took:   " << display(std::chrono::milliseconds(calcDuration)) << endl;
	cout << "Gap  took:   " << display(std::chrono::milliseconds(gapDuration)) << endl;
	cout << endl;

	calcList[i] = true;
}

class comma_numpunct : public std::numpunct<char>
{
protected:
	virtual char do_thousands_sep() const
	{
		return ',';
	}

	virtual std::string do_grouping() const
	{
		return "\03";
	}
};

int main()
{

	// this creates a new locale based on the current application default
// (which is either the one given on startup, but can be overriden with
// std::locale::global) - then extends it with an extra facet that 
// controls numeric output.
	std::locale comma_locale(std::locale(), new comma_numpunct());

	// tell cout to use our new locale.
	std::cout.imbue(comma_locale);

	//Declare window
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "Graph drawer");

	//Declare variables
	bool calcList[8] = { false };

	// Initiate textures
	sf::Texture graphTexture;
	if (!graphTexture.loadFromFile("graph.png")) {}
	sf::Sprite graphSprite;
	graphSprite.setTexture(graphTexture);
	sf::Texture algorithmTexture;

	if (!algorithmTexture.loadFromFile("algorithm.png")) {}
	sf::Sprite algorithmSprite;
	algorithmSprite.setTexture(algorithmTexture);
	algorithmSprite.setScale(0.4, 0.4);
	algorithmSprite.move(800, 35);

	// Make graph points
	vector<sf::CircleShape> pointList;
	for (size_t i = 0; i < 8; i++) {
		sf::CircleShape point(50);
		point.setFillColor(sf::Color(0, 0, 0));
		point.setScale(0.2, 0.2);
		pointList.push_back(point);
	}

	// run the program as long as the window is open
	size_t iteration = 0;
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();

			window.clear();
			window.draw(graphSprite);
			window.draw(algorithmSprite);

			if (iteration < 8) {
				if (!calcList[iteration]) {
					size_t start = 0; //pow(10, iteration);
					size_t end = pow(10, iteration + 1);
					calcPrimes(start, end, pointList, calcList, iteration);
				}
				iteration++;
			}

			for (size_t i = 0; i < 8; i++)
				if (calcList[i])
					window.draw(pointList[i]);

			window.display();
		}
	}
	return 0;
}

