#define Width 600 //szerokość planszy
#define Height 600 //wysokość planszy

#include <iostream>
#include <cstring>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>

using namespace std;

class player //klasa gracza
{
public:
	string nick; //nick gracza
	int points = 0; //punktacja gracza
};

class ball //piłka
{
public:
	float x_pos, y_pos, x_dir = 1, y_dir = -1; //pozycja (x, y) oraz kierunki ruchu piłki (x_dir, y_dir)
	int r = 5; //r - promień piłki
	float dx, dy, dr, v; //domyślna prędkość v = 300
	float angle; //kąt ruchu piłki względem paletki
	void calculate_dx_dy(float delta_r); //policz współrzędne biegunowe x i y na podstawie prędkości i kąta
};

class paletka
{
public:
	int a = 175, b = 10; //a - długość paletki, b - grubość paletki
	float x_pos = Width / 2 - a / 2, x_dir = 0; //x_pos - pozycja x paletki, x_dir - aktualny kierunek ruchu paletki w osi x, width/2 oznacza środek ekranu na ustawienie paletki, width/2-a/2 żeby idelanie paletka była na środku ekranu
};

int main(void) {

	srand(time(NULL));
	paletka tile[2]; //wywolanie klasy paletka w zaleznosci od wyboru, bo jest gra od 1 do 2 graczy

	const int WIDTH = 600, HEIGHT = 600;
	int c_banda = 25; //grubość bandy
	const int WINDOW_WIDTH = 1000, WINDOW_HEIGHT = 800;
	int ilosc_pilek, ilosc_pilek_po, ilosc_pilek_max = 500; //ilosc_pilek - wybrana przez gracza, ilosc_pilek_po - pozostala ilość piłek
	int poziom_trudnosci;
	int ilosc_graczy;
	int czy_pilka[500] = {}; ///wielkość tablicy WPISAĆ RĘCZNIE
	int czy_paletka[2] = { 1, 0 }; // przyjecie dla paletki 0 lub 1
	int speed_min, speed_max, speed; //predkosc pilki
	int angle_min = 30, angle_max = 150; //zakres poczatkowego kata lotu pilek (0-150)
	unsigned int czas; // poniewaz czas jest dodatni od 0
	bool start_gry = 0;

	float x_min = c_banda, x_max, y_min = c_banda, y_max; //ograniczenia dla ruchu piłki

	//int paletka_x_dir = 0; //kierunek ruchu paletki

	cout << "Sterowanie: A-lewo, D-prawo" << endl << endl;

	do //wybór ilości graczy
	{
		cout << "Ilosc graczy (1 lub 2): ";
		cin >> ilosc_graczy;
	} while (ilosc_graczy != 1 && ilosc_graczy != 2); //blokada dla uzytkownika, aby nie wykroczyl poza przedzial int <1,2>

	if (ilosc_graczy == 2)
		y_min += tile[1].b;

	player player[2]; //tworzenie profilu gracza nr 1

	for (int i = 0; i < ilosc_graczy; i++) //wczytanie nicków graczy
	{
		cout << "Player" << i + 1 << " - podaj swoj nick: ";
		cin >> player[i].nick;
	}

	cout << endl;

	do //pętla wyboru ilości piłek
	{
		cout << "Ilosc pilek <1-" << ilosc_pilek_max << ">: ";
		cin >> ilosc_pilek;
	} while (ilosc_pilek < 1 || ilosc_pilek > ilosc_pilek_max);
	ilosc_pilek_po = ilosc_pilek;

	cout << endl << "Poziom trudnosci: 1. Latwy 2. Sredni 3. Trudny" << endl;

	do //pętla wyboru poziomu trudności
	{
		cout << "Wybieram: ";
		cin >> poziom_trudnosci;
	} while (poziom_trudnosci != 1 && poziom_trudnosci != 2 && poziom_trudnosci != 3); //poziom 1 -> v = 200, poziom 2 -> v = 300, poziom 3 -> v = 400


	speed = 100.f * (poziom_trudnosci + 1); //dopasowanie prędkości do wybranego poziomu trudności

	float zmiana = 0.005; //0.01, 0.005; zmienna "zmiana" odpowiada interwałowi zmian położenia piłki
	float dr = speed * zmiana;

	ball ball[500]; ///wielkość tablicy WPISAĆ RĘCZNIE
	sf::CircleShape pilka[500]; ///wielkość tablicy WPISAĆ RĘCZNIE

	for (int i = 0; i < ilosc_pilek; i++)
	{
		czy_pilka[i] = 1; //aktywowanie wybranej liczby piłek
		ball[i].r = 5; //domyślnie r = 5
		
		ball[i].v = speed;

		ball[i].x_pos = 300; //rand() % (500 - 100) + 100; //300; (koniec - start) + start
		ball[i].y_pos = HEIGHT - tile[0].b - 2 * ball[i].r;

		
		ball[i].angle = rand() % (angle_max - angle_min) + angle_min; //początkowy kąt, pod którym leci dana piłka
		ball[i].calculate_dx_dy(dr); //obliczenie przesunięcia w kierunku x i y na podstawie prędkości skierowanej pod kątem "angle" do paletki

		

		pilka[i].setFillColor(sf::Color(rand() % 206 + 50, rand() % 206 + 50, rand() % 206 + 50, 255)); //opcjonalnie losowy kolor piłki
		pilka[i].setRadius(ball[i].r);
		pilka[i].setPointCount(50);
		
	}

	sf::Clock zegar_pilki;
	sf::Clock zegar_paletki;
	sf::Clock zegar_paletki_2;

	
	system("pause");

	//ustawienie ograniczeń dla piłek, aby nie wychodziły poza obszar planszy
	x_max = WIDTH - 2 * ball[0].r;
	y_max = HEIGHT - 2 * ball[0].r - tile[0].b;

	//paletka
	sf::RectangleShape paletka[2];
	for (int i = 0; i < ilosc_graczy; i++)
	{
		paletka[i].setSize(sf::Vector2f(tile[i].a, tile[i].b));
		paletka[i].setFillColor(sf::Color::Yellow);
	}

	//górna banda
	sf::RectangleShape gorna_banda;
	gorna_banda.setSize(sf::Vector2f(WIDTH + c_banda, c_banda));
	gorna_banda.setFillColor(sf::Color(0x1199ffff));
	gorna_banda.setPosition(0, 0);

	//lewa banda
	sf::RectangleShape lewa_banda;
	lewa_banda.setSize(sf::Vector2f(c_banda, HEIGHT - c_banda));
	lewa_banda.setFillColor(sf::Color::Blue);
	lewa_banda.setPosition(0, c_banda);

	//prawa banda
	sf::RectangleShape prawa_banda;
	prawa_banda.setSize(sf::Vector2f(c_banda, HEIGHT - c_banda));
	prawa_banda.setFillColor(sf::Color::Blue);
	prawa_banda.setPosition(WIDTH, c_banda);

	//dolna banda
	sf::RectangleShape dolna_banda;
	dolna_banda.setSize(sf::Vector2f(WIDTH + c_banda, c_banda));
	dolna_banda.setFillColor(sf::Color(0x1199ffff));
	dolna_banda.setPosition(0, HEIGHT);



	sf::Font arial;
	arial.loadFromFile("arial.ttf");
	if (!arial.loadFromFile("arial.ttf"))
	{
		cout << "Error loading font..." << endl;
	}

	///// DO OGARNIECIA NON VA
	//punktacja
	sf::Text punktacja[2];
	for (int i = 0; i < 2; i++)
	{
		punktacja[i].setFont(arial);
		punktacja[i].setCharacterSize(30);
		punktacja[i].setFillColor(sf::Color::Red); ///UWAGA zmienić, gdy tło czarne
		//punktacja[i].setString(str1.str()); 
		punktacja[i].setString(player[i].nick);
	}
	
	punktacja[0].setPosition(Width + 20 + c_banda, 20);
	punktacja[1].setPosition(Width + 20 + c_banda, 100);

	sf::Text uruchom_gre;
	uruchom_gre.setFont(arial);
	uruchom_gre.setCharacterSize(60);
	//punktacja.setOutlineColor(sf::Color::Red);
	uruchom_gre.setFillColor(sf::Color::Red);
	uruchom_gre.setString("Aby uruchomic gre, \nwcisnij enter");
	uruchom_gre.setPosition(0, 250);


	//punktacja (na razie wyświetlana w konsoli)
	cout << "Gracz: " << player[0].nick << " -> Punktacja: " << player[0].points << endl;

	//KONIEC GRY - figura (krzyżyk przekreślający całą planszę)
	sf::ConvexShape gameover;
	gameover.setPointCount(8);
	gameover.setPoint(0, sf::Vector2f(x_min, y_min - tile[1].b));
	gameover.setPoint(1, sf::Vector2f(Width / 2, Height / 3));
	gameover.setPoint(2, sf::Vector2f(Width, y_min - tile[1].b));
	gameover.setPoint(3, sf::Vector2f(Width * 2 / 3 + x_min, Height / 2));
	gameover.setPoint(4, sf::Vector2f(Width, Height));
	gameover.setPoint(5, sf::Vector2f(Width / 2, Height * 2 / 3));
	gameover.setPoint(6, sf::Vector2f(x_min, Height));
	gameover.setPoint(7, sf::Vector2f(Width * 2 / 3, Height / 2 - tile[1].b));
	gameover.setFillColor(sf::Color::Red);

	

	sf::Clock czas_gry;

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Okno gry");
	// pętla dla okna gry
	while
		(window.isOpen())
	{

		if (start_gry == 0)
		{
			
			window.draw(uruchom_gre);
			window.display();
			while (start_gry == 0)
			{
				sf::Event start;
				while
					(window.pollEvent(start))
				{
					if (start.type == sf::Event::KeyPressed)
					{
						if (start.key.code == sf::Keyboard::Enter)
						{
							start_gry = 1;
							window.clear();
						}
						else if (start.key.code == sf::Keyboard::Escape)
						{
							start_gry = 1;
							window.close();
							return 0;
						}
					}
				}
			}
		}

		for (int i = 0; i < ilosc_pilek; i++)
			if (czy_pilka[i])
				pilka[i].setPosition(ball[i].x_pos, ball[i].y_pos);

		paletka[0].setPosition(tile[0].x_pos, HEIGHT - tile[0].b);
		if (ilosc_graczy == 2)
			paletka[1].setPosition(tile[1].x_pos, y_min - tile[1].b);

		//obsługa zdarzeń w oknie gry
		sf::Event event;
		while
			(window.pollEvent(event))
		{
			//zamknięcie okna gry
			if (event.type == sf::Event::Closed)
				window.close();

			//informacja zwrotna o zmianie rozmiaru okna
			if (event.type == sf::Event::Resized)
			{
				sf::Vector2u size = window.getSize();
				unsigned int width = size.x;
				unsigned int height = size.y;
				std::cout << "szerokosc=" << width << "\n";
				std::cout << "wysokosc=" << height << "\n";
			}

			//zmiana położenia - paletki
			if (zegar_paletki.getElapsedTime().asSeconds() >= 0.04) // 1/25 sekundy
			{
				if (event.type == sf::Event::KeyPressed)
				{
					if (event.key.code == sf::Keyboard::D)
					{
						zegar_paletki.restart();
						if (tile[0].x_pos < (Width - tile[0].a))
							tile[0].x_dir = 1;
						else tile[0].x_dir = 0;
					}
					else if (event.key.code == sf::Keyboard::A)
					{
						zegar_paletki.restart();
						if (tile[0].x_pos > x_min)
							tile[0].x_dir = -1;
						else tile[0].x_dir = 0;
					}
					if (event.key.code == sf::Keyboard::Right)
					{
						zegar_paletki.restart();
						if (tile[1].x_pos < (Width - tile[1].a))
							tile[1].x_dir = 1;
						else tile[1].x_dir = 0;
					}
					else if (event.key.code == sf::Keyboard::Left)
					{
						zegar_paletki.restart();
						if (tile[1].x_pos > x_min)
							tile[1].x_dir = -1;
						else tile[1].x_dir = 0;
					}
					else if (event.key.code == sf::Keyboard::F1) //pomoc w grze
					{
						cout << "Sterowanie: A-lewo, D-prawo" << endl << endl;
						system("pause");
					}
				}
				else if (event.type = sf::Event::KeyReleased)
				{
					if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::A)
						tile[0].x_dir = 0;
					else if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right)
						tile[1].x_dir = 0;
				}
			}
		}

		//zmiana położenia - piłki
		if (zegar_pilki.getElapsedTime().asSeconds() >= zmiana)
		{
			zegar_pilki.restart();

			for (int i = 0; i < ilosc_pilek; i++)
			{
				if (czy_pilka[i]) //jeśli dana piłka jest aktywna
				{
					if (ball[i].x_pos > x_max || ball[i].x_pos < x_min) //realizacja odbicia piłki od bocznych ścianek
						ball[i].x_dir *= -1;

					if (ball[i].y_pos < y_min) //jeśli piłka dotrze do górnej krawędzi
					{
						if (ilosc_graczy == 2)
						{
							if (ball[i].x_pos >= tile[1].x_pos - ball[i].r && ball[i].x_pos <= tile[1].x_pos + tile[0].a - ball[i].r)
							{
								//sound1.play(); //dźwięk odbicia piłki paletką
								ball[i].y_dir *= -1;
								player[1].points++;

								//pilka[i].setFillColor(sf::Color(rand() % 206 + 50, rand() % 206 + 50, rand() % 206 + 50, 255));

								//zmiany koloru piłek po odibiciu
								if (pilka[i].getFillColor() == sf::Color::White)
									pilka[i].setFillColor(sf::Color::Red);

								else if (pilka[i].getFillColor() == sf::Color::Red)
									pilka[i].setFillColor(sf::Color::Yellow);

								else if (pilka[i].getFillColor() == sf::Color::Yellow)
									pilka[i].setFillColor(sf::Color::Green);

								else if (pilka[i].getFillColor() == sf::Color::Green)
									pilka[i].setFillColor(sf::Color::Blue);

								else if (pilka[i].getFillColor() == sf::Color::Blue)
									pilka[i].setFillColor(sf::Color(128, 0, 128, 255));

								else if (pilka[i].getFillColor() == sf::Color(128, 0, 128, 255))
									pilka[i].setFillColor(sf::Color::White);
							}
							else
							{
								ilosc_pilek_po--;
								czy_pilka[i] = 0;
							}
						}
						else
						{
							ball[i].y_dir *= -1;
						}
					}
					else if (ball[i].y_pos > y_max) //jesli piłka dotrze do dolnej krawędzi
					{

						if (ball[i].x_pos >= tile[0].x_pos - ball[i].r && ball[i].x_pos <= tile[0].x_pos + tile[0].a - ball[i].r)
						{
							
							ball[i].y_dir *= -1;
							player[0].points++;

							

							if (pilka[i].getFillColor() == sf::Color::White)
								pilka[i].setFillColor(sf::Color::Red);

							else if (pilka[i].getFillColor() == sf::Color::Red)
								pilka[i].setFillColor(sf::Color::Yellow);

							else if (pilka[i].getFillColor() == sf::Color::Yellow)
								pilka[i].setFillColor(sf::Color::Green);

							else if (pilka[i].getFillColor() == sf::Color::Green)
								pilka[i].setFillColor(sf::Color::Blue);

							else if (pilka[i].getFillColor() == sf::Color::Blue)
								pilka[i].setFillColor(sf::Color(128, 0, 128, 255));

							else if (pilka[i].getFillColor() == sf::Color(128, 0, 128, 255))
								pilka[i].setFillColor(sf::Color::White);
						}
						else
						{
							czy_pilka[i] = 0; //usunięcie nieodbitej piłki
							ilosc_pilek_po--;
						}
					}
				}
			}

			//zakończenie gry
			if (ilosc_pilek_po == 0)
			{
				czas = czas_gry.getElapsedTime().asSeconds();
				window.draw(gameover);
				window.display();
				cout << "XXXXXXXXXXXX" << endl << " KONIEC GRY" << endl << "XXXXXXXXXXXX" << endl;
				cout << "Twoj czas gry to: " << czas / 60 << " minut i " << czas % 60 << " sekund" << endl;
				start_gry = 0;
				system("pause");
				return 0;
			}

			//zmiana położenia piłek
			for (int i = 0; i < ilosc_pilek; i++)
			{
				if (czy_pilka[i])
				{
					ball[i].x_pos += ball[i].dx * ball[i].x_dir;
					ball[i].y_pos += ball[i].dy * ball[i].y_dir;
				}
			}

			//zmiana położenia paletki, wraz ze sprawdzeniem, czy nie wykracza poza obszar planszy
			for (int i = 0; i < ilosc_graczy; i++)
			{
				if (tile[i].x_pos < x_min)
				{
					tile[i].x_dir = 0;
					tile[i].x_pos = x_min;
				}
				else if (tile[i].x_pos > x_max - tile[i].a)
				{
					tile[i].x_dir = 0;
					tile[i].x_pos = x_max - tile[i].a;
				}
				else tile[i].x_pos += 4 * tile[i].x_dir;
			}

		}
		//tile[0].x_dir = 0;

		//wyczyszczenie okna i wyświetlenie zaktualizowanej zawartości
		window.clear(sf::Color::Black);
		window.draw(prawa_banda);
		window.draw(lewa_banda);
		window.draw(dolna_banda);
		window.draw(gorna_banda);
		window.draw(paletka[0]);
		window.draw(punktacja[0]);
		window.draw(punktacja[1]);
		if (ilosc_graczy == 2)
			window.draw(paletka[1]);
		for (int i = 0; i < ilosc_pilek; i++) //rysowanie aktywnych piłek
			if (czy_pilka[i])
				window.draw(pilka[i]);

		window.display(); //wyświetlenie narysowanych obiektów
	}
	return 0;
}

void ball::calculate_dx_dy(float delta_r)
{
	dx = delta_r * cos(angle * 3.14 / 180.f);
	//dx *= v / 300.f;
	dy = delta_r * sin(angle * 3.14 / 180.f);
	//dy *= v / 300.f;
}
