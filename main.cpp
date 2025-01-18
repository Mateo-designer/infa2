#define _CRT_SECURE_NO_WARNINGS          // Wyłączenie ostrzeżeń związanych z funkcjami CRT (np. fopen)
#include <SFML/Graphics.hpp>             // Dołączanie biblioteki SFML (grafika)
#include <SFML/System.hpp>               // Dołączanie biblioteki SFML (system)
#include <SFML/Main.hpp>                 // Dołączanie biblioteki SFML (główne definicje)
#include <iostream>                      // Biblioteka standardowa (wejście/wyjście)
#include <string>                        // Obsługa typu string (znaki,wyrazy)
#include <ctime>                         // Funkcje związane z czasem (time, srand)
#include <cmath>                         // Funkcje matematyczne (sin, cos, itd.)
#include <cstdlib>                       // Funkcje ogólne C (rand, srand)
#include <vector>                        

using namespace std;                    

#define WINDOW_WIDTH   1000             // Szerokość okna aplikacji
#define WINDOW_HEIGHT  800              // Wysokość okna aplikacji
#define GAME_WIDTH     800              // Szerokość pola gry
#define GAME_HEIGHT    800              // Wysokość pola gry
#define b_paletka      10               // Grubość paletki

// Klasa Board 
class Board {
public:
    sf::RectangleShape top_border;       // Prostokąt reprezentujący górną bandę
    sf::RectangleShape left_border;      // Prostokąt reprezentujący lewą bandę
    sf::RectangleShape right_border;     // Prostokąt reprezentujący prawą bandę
    sf::RectangleShape bottom_border;    // Prostokąt reprezentujący dolną bandę
    float c_banda;                       // grubość bandy (pole klasy)

    Board(float cb) {
        c_banda = cb;

        // Górna banda
        top_border.setSize(sf::Vector2f((float)GAME_WIDTH, cb));
        top_border.setFillColor(sf::Color::Blue);
        top_border.setPosition(0.f, 0.f);

        // Lewa banda
        left_border.setSize(sf::Vector2f(cb, (float)GAME_HEIGHT));
        left_border.setFillColor(sf::Color::Blue);
        left_border.setPosition(0.f, 0.f);

        // Prawa banda
        right_border.setSize(sf::Vector2f(cb, (float)GAME_HEIGHT));
        right_border.setFillColor(sf::Color::Blue);
        right_border.setPosition((float)GAME_WIDTH - cb, 0.f);

        // Dolna banda
        bottom_border.setSize(sf::Vector2f((float)GAME_WIDTH, cb));
        bottom_border.setFillColor(sf::Color::Blue);
        bottom_border.setPosition(0.f, (float)GAME_HEIGHT - cb);
    }

    void draw(sf::RenderWindow& w) {
        w.draw(top_border);
        w.draw(left_border);
        w.draw(right_border);
        w.draw(bottom_border);
    }
};

// struktura Player
struct Player {
    string nick;
    int score = 0;
};

// struktura przycisku (Button)
struct Button {
    sf::Sprite sprite;
    sf::Text   text;
    sf::RectangleShape innerRect;

    bool contains(float mx, float my) const {
        return sprite.getGlobalBounds().contains(mx, my);
    }
    void draw(sf::RenderWindow& w) {
        w.draw(sprite);
        w.draw(text);
        w.draw(innerRect);
    }
};

// Funkcja pomocnicza do ustawiania ramki w przycisku
void setInnerRect(Button& btn, float thickness) {
    sf::FloatRect b = btn.sprite.getGlobalBounds();
    float offset = 3.f;

    btn.innerRect.setPosition(b.left + offset, b.top + offset);
    btn.innerRect.setSize(sf::Vector2f(b.width - 2.f * offset,
        b.height - 2.f * offset));
    btn.innerRect.setFillColor(sf::Color::Transparent);

    if (thickness > 0.f) {
        btn.innerRect.setOutlineThickness(thickness);
        btn.innerRect.setOutlineColor(sf::Color::Red);
    }
    else {
        btn.innerRect.setOutlineThickness(0.f);
    }
}

// Klasa PongGame - logika gry
class PongGame {
public:
    void runGame(sf::RenderWindow& w, int subMode,
        int difficulty, int ballsCount,
        Player player[2]);
    void showHelp(sf::RenderWindow& w, sf::Font& f);

private:
    void showCountdown(sf::RenderWindow& w, sf::Font& f);
    void showGameOver(sf::RenderWindow& w, sf::Font& f);
    void pauseGame(sf::RenderWindow& w, sf::Font& f);
};

// odliczanie 3..2..1
void PongGame::showCountdown(sf::RenderWindow& w, sf::Font& f) {
    w.clear(sf::Color::Black);

    sf::Text info("Gra rozpocznie sie za:", f, 40);
    info.setFillColor(sf::Color::White);
    info.setPosition(200.f, 200.f);

    sf::Text countdown("", f, 80);
    countdown.setFillColor(sf::Color::Yellow);
    countdown.setPosition(370.f, 300.f);

    for (int i = 3; i >= 1; i--) {
        w.clear(sf::Color::Black);
        w.draw(info);
        countdown.setString(to_string(i));
        w.draw(countdown);
        w.display();
        sf::sleep(sf::seconds(1.f));
    }
}

// Ekran końca gry
void PongGame::showGameOver(sf::RenderWindow& w, sf::Font& f) {
    sf::Text end("KONIEC GRY", f, 80);
    end.setFillColor(sf::Color::Red);
    end.setPosition(250.f, 200.f);

    sf::Text instruction("Aby zamknac gre, wcisnij ENTER", f, 50);
    instruction.setFillColor(sf::Color::Red);
    instruction.setPosition(100.f, 300.f);

    bool visible = true;            //pokazanie napisu - KONIEC GRY
    sf::Clock clk;

    while (w.isOpen()) {
        sf::Event e;
        while (w.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                w.close();
                return;
            }
            if (e.type == sf::Event::KeyPressed &&
                e.key.code == sf::Keyboard::Enter) {
                w.close();
                return;
            }
        }
        if (clk.getElapsedTime().asSeconds() > 1.f) {       //Migotanie napisu co 1 sek
            visible = !visible;
            clk.restart();
        }
        w.clear(sf::Color::Black);
        if (visible) w.draw(end);
        w.draw(instruction);
        w.display();
    }
}

// Pauza (ESC)
void PongGame::pauseGame(sf::RenderWindow& w, sf::Font& f) {
    sf::Text question("Czy chcesz opuscic gre?", f, 40);
    question.setFillColor(sf::Color::White);
    question.setPosition(150.f, 200.f);
    
   
    sf::Text yes("TAK", f, 30);
    yes.setFillColor(sf::Color::Green);
    yes.setPosition(200.f, 300.f);

    sf::Text no("NIE", f, 30);
    no.setFillColor(sf::Color::Red);
    no.setPosition(350.f, 300.f);

    // Pętla pauzy
    bool paused = true;
    while (w.isOpen() && paused) {
        sf::Event e;
        while (w.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                w.close();
                return;
            }
            if (e.type == sf::Event::MouseButtonPressed) {
                float mx = (float)e.mouseButton.x;
                float my = (float)e.mouseButton.y;
                if (yes.getGlobalBounds().contains(mx, my)) {
                    w.close();
                }
                else if (no.getGlobalBounds().contains(mx, my)) {
                    paused = false;
                }
            }
        }
        w.clear(sf::Color::Black);
        w.draw(question);
        w.draw(yes);
        w.draw(no);
        w.display();
    }
             
}

// Help (F1)
void PongGame::showHelp(sf::RenderWindow& w, sf::Font& f) {
    sf::String helpStr =
        "HELP:\n"
        "F1 = pomoc\n"
        "ESC = pauza\n"
        "\nSTEROWANIE:\n"
        " - Dolna paletka (Gracz 1): klawisze A / D\n"
        " - Gorna paletka (Gracz 2 w multi): strzalki LEWO / PRAWO\n"
        "\nKlik myszka, aby wyjsc.";

    sf::Text help(helpStr, f, 24);
    help.setFillColor(sf::Color::White);
    help.setPosition(50.f, 50.f);

    bool endScreen = false;
    while (w.isOpen() && !endScreen) {
        sf::Event e;
        while (w.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                w.close();
                return;
            }
            if (e.type == sf::Event::MouseButtonPressed) {
                endScreen = true;
            }
        }
        w.clear(sf::Color::Black);
        w.draw(help);
        w.display();
    }
}

// Właściwa logika gry
void PongGame::runGame(sf::RenderWindow& w, int subMode,
    int difficulty, int ballsCount,
    Player player[2])
{
    // Ładujemy czcionkę lokalnie
    sf::Font locF;
    locF.loadFromFile("resources/arial.ttf");

    showCountdown(w, locF);

    // Tworzymy bandy
    Board board(25.f);  //wywołanie konstruktora i zdeklarowanie grubosci bandy

    // Zerujemy punkty
    player[0].score = 0;
    player[1].score = 0;

    // Ustalamy prędkość bazową w zależności od difficulty
    float baseSpeed = 700.f; 
    switch (difficulty) {
    case 1: baseSpeed = 700.f;  break;
    case 2: baseSpeed = 900.f; break;
    case 3: baseSpeed = 1100.f; break;
    }

    float zmiana = 0.005f; // do timera

    // Struktura piłki
    struct MyBall {
        bool active;
        float x, y;
        float angle;
        float v;
        float x_dir;
        float y_dir;
        sf::CircleShape shape;

        MyBall() :
            active(false),
            x(0.f), y(0.f),
            angle(0.f),
            v(0.f),
            x_dir(1.f),
            y_dir(1.f)
        {
            shape.setRadius(5.f);
            shape.setFillColor(sf::Color::White);
            shape.setPosition(x, y);
        }
    };

    // Wektor piłek
    vector<MyBall> balls(ballsCount); //ballsCount - wybrane przez użytkownika

    // Inicjalizacja piłek
    for (int i = 0; i < ballsCount; i++) {
        balls[i].x = GAME_WIDTH / 2.f;
        balls[i].y = GAME_HEIGHT / 2.f;
        balls[i].active = true;
        balls[i].shape.setFillColor(sf::Color(rand() % 206 + 50, rand() % 206 + 50, rand() % 206 + 50));
        int angle_min = 15;
        int angle_max = 170;
        float a = (float)(rand() % (angle_max - angle_min) + angle_min);
        balls[i].angle = a;
        balls[i].v = baseSpeed;
        balls[i].x_dir = 1.f;
        balls[i].y_dir = (rand() % 2 == 0) ? -1.f : 1.f;
        balls[i].shape.setPosition(balls[i].x, balls[i].y);
    }

    int balls_amount_after = ballsCount;

    // Klasa paletki
    class MyPaddle {
    public:
        float xPos;
        float xDir;
        float speed;
        int length;
        int thickness;
        sf::RectangleShape shape;

        MyPaddle(float startX, float startY, int len, int thick, float spd, sf::Color color) {
            xPos = startX;
            xDir = 0.f;
            speed = spd;
            length = len;
            thickness = thick;
            shape.setSize(sf::Vector2f((float)length, (float)thickness));
            shape.setFillColor(color);
            shape.setPosition(xPos, startY);
        }
        void move() {
            xPos += xDir * speed;
            if (xPos < 25.f) xPos = 25.f;
            if (xPos > (float)GAME_WIDTH - 25.f - length)
                xPos = (float)GAME_WIDTH - 25.f - length;
            shape.setPosition(xPos, shape.getPosition().y);
        }
    };

    // Dolna paletka
    MyPaddle paddle1(
		(float)GAME_WIDTH / 2.f - 100.f,    //pozycja startowa - ustwenie pośrodku
        (float)GAME_HEIGHT - 25.f - b_paletka,
        200, //długość
        10, // grubość
        12.f, //prędkość przesuwania
        sf::Color::Yellow
    );

    // Górna paletka (AI albo 2. gracz)
    MyPaddle paddle2(
        (float)GAME_WIDTH / 2.f - 100.f,
        (float)GAME_HEIGHT - 25.f - b_paletka,
        200, 10, 12.f, sf::Color::Magenta
    );

    sf::Clock clockLogic;
    sf::Clock clockAI;
    sf::Clock clockTime;
    clockTime.restart();

    bool running = true;
    while (w.isOpen() && running) {
        sf::Event ev;
        while (w.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                w.close();
                return;
            }
            if (ev.type == sf::Event::KeyPressed) {
                if (ev.key.code == sf::Keyboard::Escape) {
                    pauseGame(w, locF);
                }
                if (ev.key.code == sf::Keyboard::F1) {
                    showHelp(w, locF);
                }
            }
        }

        // Sterowanie dolnej paletki (A/D)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            paddle1.xDir = -1.f;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            paddle1.xDir = 1.f;
        }
        else {
            paddle1.xDir = 0.f;
        }
        paddle1.move();

        // Górna paletka
        if (subMode == 0) { // AI
            if (clockAI.getElapsedTime().asSeconds() > 0.03f) {         //Śledzenie pierwszej piłki wygenerowanej przez wektor piłek z częstotliwością np. 0.03 sekundy
                clockAI.restart();
				for (auto& b : balls) {                                 //kontener od wektora piłek jest ampersand, aby operować na prawdziwych piłkach, a nie ich kopii
                    if (b.active) {
                        float center = paddle2.xPos + paddle2.length / 2.f; // ustawienie paletki pośrodku bandy, ale również warunek, aby paletka została utworzona pośrodku samej siebie
						if (b.x < center - 25.f)        paddle2.xDir = -1.f; //warunki, aby paletka poruszała się w lewo, prawo lub stała w miejscu w zależności od piłki proste śledzenie piłki
                        else if (b.x > center + 25.f)   paddle2.xDir = 1.f;
                        else                            paddle2.xDir = 0.f;
                        break;
                    }
                }
            }
        }
        else if (subMode == 2) { // multiplayer
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                paddle2.xDir = -1.f;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                paddle2.xDir = 1.f;
            }
            else {
                paddle2.xDir = 0.f;
            }
        }
        if (subMode == 0 || subMode == 2) {
            paddle2.move();
        }

        // Logika piłek
        if (clockLogic.getElapsedTime().asSeconds() > zmiana) {
            clockLogic.restart();

            float paddle2BottomY = 25.f + paddle2.thickness;
            float paddle1TopY = (float)GAME_HEIGHT - 25.f - b_paletka;

            for (auto& b : balls) {
                if (!b.active) continue;   //jeśli piłka nieaktywna, przechodzimy do następnej iteracji, czyli inaczej koniec gry w logice gry

                // Lewa/prawa banda (odbicie w poziomie)
                if (b.x < 25.f) {
                    b.x = 25.f;
                    b.x_dir *= -1.f;
                    b.shape.setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
                    b.v += 50.f; //przyspieszenie piłki odbitej od lewej bandy
                }
                else if (b.x > GAME_WIDTH - 25.f - 2.f * b.shape.getRadius()) {
                    b.x = GAME_WIDTH - 25.f - 2.f * b.shape.getRadius();
                    b.x_dir *= -1.f;
                    b.shape.setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
                    b.v += 50.f; //przyspieszenie piłi odbitej od prawej bandy
                }

                // Górna paletka
                if (subMode == 0 || subMode == 2) {
                    if (b.y_dir < 0.f) {
                        float ballTop = b.y;
                        if (ballTop <= paddle2BottomY) {
                            float ballLeft = b.x;
                            float ballRight = b.x + 2.f * b.shape.getRadius();
                            float pLeft = paddle2.xPos;
                            float pRight = paddle2.xPos + paddle2.length;
                            if (ballRight >= pLeft && ballLeft <= pRight) {
                                // odbicie w dół
                                b.y_dir = 1.f;
                                b.y = paddle2BottomY + 0.1f;
                                b.shape.setFillColor(sf::Color::Green); 
                                b.v += 50.f;
                                // punkt
                                player[1].score++;
                            }
                            else {
                                // wylatuje
                                b.active = false;
                                balls_amount_after--;
                            }
                        }
                    }
                }
                else {
                    // Single z jedną paletką (bez górnej)
                    if (b.y < 25.f) {
                        b.y = 25.f;
                        b.y_dir *= -1.f;
                        b.shape.setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
                        b.v += 50.f;
                    }
                }

                // Dolna paletka
                if (b.y_dir > 0.f) {
                    float ballBottom = b.y + 2.f * b.shape.getRadius();
                    float padLeft = paddle1.xPos;
                    float padRight = paddle1.xPos + paddle1.length;

                    if (ballBottom >= paddle1TopY) {
                        float ballLeft = b.x;
                        float ballRight = b.x + 2.f * b.shape.getRadius();
                        if (ballRight >= padLeft && ballLeft <= padRight) {
                            // Odbicie w górę
                            b.y_dir = -1.f;
                            b.y = paddle1TopY - 2.f * b.shape.getRadius() - 0.1f;
                            b.shape.setFillColor(sf::Color::Red);
                            b.v += 50.f;
                            // punkt
                            player[0].score++;
                        }
                        else {
                            b.active = false;
                            balls_amount_after--;
                        }
                    }
                }

                // Ruch piłki
                float rad = (b.angle * 3.14159f) / 180.f;
                float cosA = cos(rad);
                float sinA = sin(rad);
                float dist = b.v * zmiana;
                float dx = dist * cosA;
                float dy = dist * sinA;
                b.x += dx * b.x_dir;
                b.y += dy * b.y_dir;
            }

            // Jeśli brak piłek -> koniec
            if (balls_amount_after == 0) {
                showGameOver(w, locF);
                unsigned int sec = (unsigned int)clockTime.getElapsedTime().asSeconds();
                cout << "Koniec gry. Czas: "
                    << sec / 60 << " min, " << sec % 60 << " sek\n";
                return;
            }
        }

        // Rysowanie
        w.clear(sf::Color::Black);

        board.draw(w);

        w.draw(paddle1.shape);
        if (subMode == 0 || subMode == 2) {
            w.draw(paddle2.shape);
        }

        for (auto& b : balls) {
            if (b.active) {
                b.shape.setPosition(b.x, b.y);
                w.draw(b.shape);
            }
        }

        // Panel boczny
        {
            sf::RectangleShape panelBG;
            panelBG.setSize(sf::Vector2f((float)(WINDOW_WIDTH - GAME_WIDTH), (float)WINDOW_HEIGHT));
            panelBG.setFillColor(sf::Color(50, 50, 50));
            panelBG.setPosition((float)GAME_WIDTH, 0.f);
            w.draw(panelBG);

            // Nick gracza 1
            sf::Text nick1(player[0].nick + " (G1)", locF, 20);
            nick1.setFillColor(sf::Color::Yellow);
            nick1.setPosition((float)GAME_WIDTH + 20.f, 50.f);
            w.draw(nick1);

            // Nick/punkty gracza 2
            if (subMode == 0) {
                sf::Text nick2("AI (G2)", locF, 20);
                nick2.setFillColor(sf::Color::Cyan);
                nick2.setPosition((float)GAME_WIDTH + 20.f, 80.f);
                w.draw(nick2);
            }
            else if (subMode == 2) {
                sf::Text nick2(player[1].nick + " (G2)", locF, 20);
                nick2.setFillColor(sf::Color::Cyan);
                nick2.setPosition((float)GAME_WIDTH + 20.f, 80.f);
                w.draw(nick2);
            }

            // Pozostałe piłki
            sf::Text ballsTxt("PILKI: " + to_string(balls_amount_after) + "/" + to_string(ballsCount), locF, 20);
            ballsTxt.setFillColor(sf::Color::White);
            ballsTxt.setPosition((float)GAME_WIDTH + 20.f, 130.f);
            w.draw(ballsTxt);

            // Wynik G1
            sf::Text score1("Score G1: " + to_string(player[0].score), locF, 20);
            score1.setFillColor(sf::Color::Yellow);
            score1.setPosition((float)GAME_WIDTH + 20.f, 180.f);
            w.draw(score1);

            // Wynik G2
            if (subMode == 0) {
                sf::Text score2("Score AI: " + to_string(player[1].score), locF, 20);
                score2.setFillColor(sf::Color::Cyan);
                score2.setPosition((float)GAME_WIDTH + 20.f, 210.f);
                w.draw(score2);
            }
            else if (subMode == 2) {
                sf::Text score2("Score G2: " + to_string(player[1].score),
                    locF, 20);
                score2.setFillColor(sf::Color::Cyan);
                score2.setPosition((float)GAME_WIDTH + 20.f, 210.f);
                w.draw(score2);
            }
        }

        w.display();

        // Limit czasowy gry - max 3600s (1h)
        if (clockTime.getElapsedTime().asSeconds() > 3600.f) {
            showGameOver(w, locF);
            running = false;
        }
    }
}

// Klasa MenuSystem
class MenuSystem {
public:
    MenuSystem(sf::RenderWindow* w,
        sf::Font* f,
        sf::Texture* tNormal,
        sf::Texture* tSelected)
    {
        window = w;
        font = f;
        texNormal = tNormal;
        texSelected = tSelected;
        runningMenu = true;
        initButtons();
    }

    void runMenu();

    Player gracze[2];

private:
    sf::RenderWindow* window;
    sf::Font* font;
    sf::Texture* texNormal;
    sf::Texture* texSelected;
    bool runningMenu;

    Button btnSingle, btnMulti, btnHelp, btnExit;
    PongGame gameCore;

    void initButtons();
    void handleEvents();
    void drawMenu();
    // Podmenu
    int  chooseSubModeSingle();
    int  chooseSubModeMulti();
    int  chooseDifficulty();
    int  chooseBallsAmount();
	// Zwraca: -2 = cofnięcie (Powrót), inaczej 0 brak nicku, 1 - zatwierdzenie dalej
    int  enterNickSFML(string& nick, int pIndex); 
};

void MenuSystem::initButtons() {
    // SINGLE
    btnSingle.sprite.setTexture(*texNormal);
    btnSingle.text.setFont(*font);
    btnSingle.text.setString("SINGLEPLAYER");
    btnSingle.text.setCharacterSize(40);
    btnSingle.text.setFillColor(sf::Color::Red);
    btnSingle.sprite.setPosition((float)(WINDOW_WIDTH - 500) / 2.f,
        (float)(WINDOW_HEIGHT - 80 * 4) / 2.f + 0 * 80.f);
    btnSingle.text.setPosition(btnSingle.sprite.getPosition().x + 20.f,
        btnSingle.sprite.getPosition().y + 10.f);
    setInnerRect(btnSingle, 0.f);

    // MULTI
    btnMulti.sprite.setTexture(*texNormal);
    btnMulti.text.setFont(*font);
    btnMulti.text.setString("MULTIPLAYER");
    btnMulti.text.setCharacterSize(40);
    btnMulti.text.setFillColor(sf::Color::Red);
    btnMulti.sprite.setPosition((float)(WINDOW_WIDTH - 500) / 2.f,
        (float)(WINDOW_HEIGHT - 80 * 4) / 2.f + 1 * 80.f);
    btnMulti.text.setPosition(btnMulti.sprite.getPosition().x + 20.f,
        btnMulti.sprite.getPosition().y + 10.f);
    setInnerRect(btnMulti, 0.f);

    // HELP
    btnHelp.sprite.setTexture(*texNormal);
    btnHelp.text.setFont(*font);
    btnHelp.text.setString("HELP");
    btnHelp.text.setCharacterSize(40);
    btnHelp.text.setFillColor(sf::Color::Red);
    btnHelp.sprite.setPosition((float)(WINDOW_WIDTH - 500) / 2.f,
        (float)(WINDOW_HEIGHT - 80 * 4) / 2.f + 2 * 80.f);
    btnHelp.text.setPosition(btnHelp.sprite.getPosition().x + 20.f,
        btnHelp.sprite.getPosition().y + 10.f);
    setInnerRect(btnHelp, 0.f);

    // EXIT
    btnExit.sprite.setTexture(*texNormal);
    btnExit.text.setFont(*font);
    btnExit.text.setString("EXIT");
    btnExit.text.setCharacterSize(40);
    btnExit.text.setFillColor(sf::Color::Red);
    btnExit.sprite.setPosition((float)(WINDOW_WIDTH - 500) / 2.f,
        (float)(WINDOW_HEIGHT - 80 * 4) / 2.f + 3 * 80.f);
    btnExit.text.setPosition(btnExit.sprite.getPosition().x + 20.f,
        btnExit.sprite.getPosition().y + 10.f);
    setInnerRect(btnExit, 0.f);
}

void MenuSystem::runMenu() {
    while (window->isOpen() && runningMenu) {
        handleEvents();
        drawMenu();
    }
}

void MenuSystem::handleEvents() {
    sf::Event e;
    while (window->pollEvent(e)) {
        if (e.type == sf::Event::Closed) {
            window->close();
        }
        if (e.type == sf::Event::MouseMoved) {
            float mx = (float)e.mouseMove.x;
            float my = (float)e.mouseMove.y;
            btnSingle.sprite.setTexture(btnSingle.contains(mx, my) ? *texSelected : *texNormal);
            btnMulti.sprite.setTexture(btnMulti.contains(mx, my) ? *texSelected : *texNormal);
            btnHelp.sprite.setTexture(btnHelp.contains(mx, my) ? *texSelected : *texNormal);
            btnExit.sprite.setTexture(btnExit.contains(mx, my) ? *texSelected : *texNormal);
        }
        if (e.type == sf::Event::MouseButtonPressed &&
            e.mouseButton.button == sf::Mouse::Left)
        {
            float mx = (float)e.mouseButton.x;
            float my = (float)e.mouseButton.y;

            // SINGLE
            if (btnSingle.contains(mx, my)) {
                // Zamiast jednorazowego wywołania - pętla pozwala cofnąć się tylko o jeden poziom
                while (true) {
                    int subM = chooseSubModeSingle();
                    if (subM == -2) {
                        // Powrót z okna chooseSubModeSingle -> wracamy do menu głównego
                        break;
                    }
                    if (subM == -1) { 
                        runningMenu = false;
                        break;
                    }
                    // Gdy mamy subM = 0 lub 1
                    while (true) {
                        // Nick G1
                        int retNick = enterNickSFML(gracze[0].nick, 0);
                        if (retNick == -2) {
                            // cofnięcie z ekranu nicku -> wracamy do "chooseSubModeSingle"
                            break;
                        }

                        if (subM == 0) gracze[1].nick = "(AI)";
                        else           gracze[1].nick = "(N/A)";

                        // Wybór trudności
                        while (true) {
                            int diff = chooseDifficulty();
                            if (diff == -2) {
                                // cofka do ekranu nicku
                                break;
                            }
                            if (diff == 0) {
                                runningMenu = false;
                                return;
                            }

                            // Ilość piłek
                            while (true) {
                                int bc = chooseBallsAmount();
                                if (bc == -2) {
                                    // cofka do wyboru trudności
                                    break;
                                }
                                if (bc == 0) {
                                    runningMenu = false;
                                    return;
                                }

                                // Mamy subM, diff, bc, gracze
                                gameCore.runGame(*window, subM, diff, bc, gracze);
                                // Po wyjściu z gry -> wracamy do menu głównego
                                return;
                            } 
                            
                        } 
                        
                    } 
                   
                }
            }

            // MULTI
            if (btnMulti.contains(mx, my)) {
                while (true) {
                    int subM = chooseSubModeMulti();
                    if (subM == -2) {
                        // cofniecie do menu głównego
                        break;
                    }
                    if (subM == -1) {
                        runningMenu = false;
                        break;
                    }
                    // subM = 2 => 2 graczy
                    while (true) {
                        int retNick1 = enterNickSFML(gracze[0].nick, 0);
                        if (retNick1 == -2) {
                            // cofka do chooseSubModeMulti
                            break;
                        }
                        int retNick2 = enterNickSFML(gracze[1].nick, 1);
                        if (retNick2 == -2) {
                            // cofka do wpisywania 1
                            continue; // spowoduje ponownie prośbę o nick gracza pierwszego (0)
                        }

                        // wybór trudności
                        while (true) {
                            int diff = chooseDifficulty();
                            if (diff == -2) {
                                // cofka do drugiego nicku
                                break;
                            }
                            if (diff == 0) {
                                runningMenu = false;
                                return;
                            }

                            // ilość piłek
                            while (true) {
                                int bc = chooseBallsAmount();
                                if (bc == -2) {
                                    // cofka do chooseDifficulty
                                    break;
                                }
                                if (bc == 0) {
                                    runningMenu = false;
                                    return;
                                }
                                // mamy subM=2, diff, bc, gracze
                                gameCore.runGame(*window, subM, diff, bc, gracze);
                                // powrót do menu
                                return;
                            }
                            
                        }
                        
                    }
                    
                }
            }

            // HELP
            if (btnHelp.contains(mx, my)) {
                PongGame pg;
                pg.showHelp(*window, *font);
            }

            // EXIT
            if (btnExit.contains(mx, my)) {
                window->close();
            }
        }
    }
}

void MenuSystem::drawMenu() {
    window->clear(sf::Color(181, 230, 29));
    btnSingle.draw(*window);
    btnMulti.draw(*window);
    btnHelp.draw(*window);
    btnExit.draw(*window);
    window->display();
}

// 1) SINGLE
int MenuSystem::chooseSubModeSingle() {
    sf::RenderWindow& w = *window;
    sf::Text title("Wybierz tryb SINGLEPLAYER:", *font, 40);
    title.setFillColor(sf::Color::White);
    title.setPosition(50.f, 50.f);

    sf::Texture texN = *texNormal, texS = *texSelected;
    Button btnTrain, btnOneP, btnOK, btnPowrot;

    btnTrain.sprite.setTexture(texN);
    btnTrain.sprite.setScale(0.6f, 0.6f);
    btnTrain.text.setFont(*font);
    btnTrain.text.setString("TRAINING");
    btnTrain.text.setCharacterSize(30);
    btnTrain.text.setFillColor(sf::Color::Black);
    btnTrain.sprite.setPosition(50.f, 150.f);
    btnTrain.text.setPosition(btnTrain.sprite.getPosition().x + 20.f,
        btnTrain.sprite.getPosition().y + 10.f);
    setInnerRect(btnTrain, 0.f);

    btnOneP.sprite.setTexture(texN);
    btnOneP.sprite.setScale(0.6f, 0.6f);
    btnOneP.text.setFont(*font);
    btnOneP.text.setString("1 PLAYER");
    btnOneP.text.setCharacterSize(30);
    btnOneP.text.setFillColor(sf::Color::Black);
    btnOneP.sprite.setPosition(50.f, 250.f);
    btnOneP.text.setPosition(btnOneP.sprite.getPosition().x + 20.f,
        btnOneP.sprite.getPosition().y + 10.f);
    setInnerRect(btnOneP, 0.f);

    btnOK.sprite.setTexture(texN);
    btnOK.sprite.setScale(0.5f, 0.5f);
    btnOK.text.setFont(*font);
    btnOK.text.setString("Zatwierdz");
    btnOK.text.setCharacterSize(24);
    btnOK.text.setFillColor(sf::Color::Black);
    btnOK.sprite.setPosition((float)GAME_WIDTH - 130.f,
        (float)GAME_HEIGHT - 70.f);
    btnOK.text.setPosition(btnOK.sprite.getPosition().x + 20.f,
        btnOK.sprite.getPosition().y + 10.f);
    setInnerRect(btnOK, 0.f);

    btnPowrot.sprite.setTexture(texN);
    btnPowrot.sprite.setScale(0.5f, 0.5f);
    btnPowrot.text.setFont(*font);
    btnPowrot.text.setString("Powrot");
    btnPowrot.text.setCharacterSize(24);
    btnPowrot.text.setFillColor(sf::Color::Black);
    btnPowrot.sprite.setPosition(10.f,
        (float)w.getSize().y - 70.f);
    btnPowrot.text.setPosition(btnPowrot.sprite.getPosition().x + 10.f,
        btnPowrot.sprite.getPosition().y + 5.f);
    setInnerRect(btnPowrot, 0.f);

    int chosen = -1;
    bool run = true;
    while (w.isOpen() && run) {
        sf::Event e;
        while (w.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                w.close();
                return -1;
            }
            // BACKSPACE = powrot
            if (e.type == sf::Event::KeyPressed &&
                e.key.code == sf::Keyboard::Backspace) {
                return -2;     
            }
            // ENTER = zatwierdz, jeśli coś wybrano
            if (e.type == sf::Event::KeyPressed &&
                e.key.code == sf::Keyboard::Enter) {
                if (chosen != -1) return chosen;
            }
            if (e.type == sf::Event::MouseMoved) {
                float mx = (float)e.mouseMove.x;
                float my = (float)e.mouseMove.y;
                if (btnTrain.contains(mx, my))
                    setInnerRect(btnTrain, (chosen == 0) ? 5.f : 3.f);  //jesli przez myszkę zostało to wybrano to ma się pogrubić ramka , podświetlenie
                else
                    setInnerRect(btnTrain, (chosen == 0) ? 5.f : 0.f);

                if (btnOneP.contains(mx, my))
                    setInnerRect(btnOneP, (chosen == 1) ? 5.f : 3.f);
                else
                    setInnerRect(btnOneP, (chosen == 1) ? 5.f : 0.f);

                if (btnOK.contains(mx, my))
                    setInnerRect(btnOK, 3.f);
                else
                    setInnerRect(btnOK, 0.f);

                if (btnPowrot.contains(mx, my))
                    setInnerRect(btnPowrot, 3.f);
                else
                    setInnerRect(btnPowrot, 0.f);
            }
            if (e.type == sf::Event::MouseButtonPressed &&
                e.mouseButton.button == sf::Mouse::Left) {
                float mx = (float)e.mouseButton.x;
                float my = (float)e.mouseButton.y;
                if (btnTrain.contains(mx, my)) chosen = 0;
                if (btnOneP.contains(mx, my))  chosen = 1;
                if (btnOK.contains(mx, my)) {
                    if (chosen != -1) return chosen;
                }
                if (btnPowrot.contains(mx, my)) {
                    return -2;
                }
            }
        }
        w.clear(sf::Color::Black);
        w.draw(title);
        btnTrain.draw(w);
        btnOneP.draw(w);
        btnOK.draw(w);
        btnPowrot.draw(w);
        w.display();
    }
    return chosen;
}

// 2) MULTI
int MenuSystem::chooseSubModeMulti() {
    sf::RenderWindow& w = *window;
    sf::Text title("Wybierz tryb MULTIPLAYER:", *font, 40);
    title.setFillColor(sf::Color::White);
    title.setPosition(50.f, 50.f);

    sf::Texture texN = *texNormal;
    Button btn2p, btnOK, btnPowrot;

    btn2p.sprite.setTexture(texN);
    btn2p.sprite.setScale(0.6f, 0.6f);
    btn2p.text.setFont(*font);
    btn2p.text.setString("2 PLAYERS");
    btn2p.text.setCharacterSize(30);
    btn2p.text.setFillColor(sf::Color::Black);
    btn2p.sprite.setPosition(50.f, 150.f);
    btn2p.text.setPosition(btn2p.sprite.getPosition().x + 20.f,
        btn2p.sprite.getPosition().y + 10.f);
    setInnerRect(btn2p, 0.f);

    btnOK.sprite.setTexture(texN);
    btnOK.sprite.setScale(0.5f, 0.5f);
    btnOK.text.setFont(*font);
    btnOK.text.setString("Zatwierdz");
    btnOK.text.setCharacterSize(24);
    btnOK.text.setFillColor(sf::Color::Black);
    btnOK.sprite.setPosition((float)GAME_WIDTH - 130.f, (float)GAME_HEIGHT - 70.f);
    btnOK.text.setPosition(btnOK.sprite.getPosition().x + 20.f,
        btnOK.sprite.getPosition().y + 10.f);
    setInnerRect(btnOK, 0.f);

    btnPowrot.sprite.setTexture(texN);
    btnPowrot.sprite.setScale(0.5f, 0.5f);
    btnPowrot.text.setFont(*font);
    btnPowrot.text.setString("Powrot");
    btnPowrot.text.setCharacterSize(24);
    btnPowrot.text.setFillColor(sf::Color::Black);
    btnPowrot.sprite.setPosition(10.f, (float)w.getSize().y - 70.f);
    btnPowrot.text.setPosition(btnPowrot.sprite.getPosition().x + 10.f,
        btnPowrot.sprite.getPosition().y + 5.f);
    setInnerRect(btnPowrot, 0.f);

    bool selected = false;
    bool run = true;
    int chosen = -1;
    while (w.isOpen() && run) {
        sf::Event e;
        while (w.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                w.close();
                return -1;
            }
            // BACKSPACE = powrot
            if (e.type == sf::Event::KeyPressed &&
                e.key.code == sf::Keyboard::Backspace) {
                return -2;
            }
            // ENTER = zatwierdz
            if (e.type == sf::Event::KeyPressed &&
                e.key.code == sf::Keyboard::Enter) {
                if (selected) return chosen;
            }
            if (e.type == sf::Event::MouseMoved) {
                float mx = (float)e.mouseMove.x;
                float my = (float)e.mouseMove.y;
                if (btn2p.contains(mx, my))
                    setInnerRect(btn2p, (selected ? 5.f : 3.f));
                else
                    setInnerRect(btn2p, (selected ? 5.f : 0.f));

                if (btnOK.contains(mx, my))
                    setInnerRect(btnOK, 3.f);
                else
                    setInnerRect(btnOK, 0.f);

                if (btnPowrot.contains(mx, my))
                    setInnerRect(btnPowrot, 3.f);
                else
                    setInnerRect(btnPowrot, 0.f);
            }
            if (e.type == sf::Event::MouseButtonPressed &&
                e.mouseButton.button == sf::Mouse::Left)
            {
                float mx = (float)e.mouseButton.x;
                float my = (float)e.mouseButton.y;
                if (btn2p.contains(mx, my)) {
                    chosen = 2;
                    selected = true;
                }
                if (btnOK.contains(mx, my)) {
                    if (selected) return chosen;
                }
                if (btnPowrot.contains(mx, my)) {
                    return -2;
                }
            }
        }
        w.clear(sf::Color::Black);
        w.draw(title);
        btn2p.draw(w);
        btnOK.draw(w);
        btnPowrot.draw(w);
        w.display();
    }
    return chosen;
}

// 3) Difficulty
int MenuSystem::chooseDifficulty() {
    sf::RenderWindow& w = *window;
    sf::Text title("Wybierz poziom trudnosci:", *font, 40);
    title.setFillColor(sf::Color::White);
    title.setPosition(50.f, 50.f);

    sf::Texture texN = *texNormal;
    Button b1, b2, b3, bOK, bPowrot;
    b1.sprite.setTexture(texN);
    b1.sprite.setScale(0.6f, 0.6f);
    b1.text.setFont(*font);
    b1.text.setString("LATWY");
    b1.text.setCharacterSize(30);
    b1.text.setFillColor(sf::Color::Black);
    b1.sprite.setPosition(50.f, 150.f);
    b1.text.setPosition(b1.sprite.getPosition().x + 20.f,
        b1.sprite.getPosition().y + 10.f);
    setInnerRect(b1, 0.f);

    b2.sprite.setTexture(texN);
    b2.sprite.setScale(0.6f, 0.6f);
    b2.text.setFont(*font);
    b2.text.setString("SREDNI");
    b2.text.setCharacterSize(30);
    b2.text.setFillColor(sf::Color::Black);
    b2.sprite.setPosition(50.f, 250.f);
    b2.text.setPosition(b2.sprite.getPosition().x + 20.f,
        b2.sprite.getPosition().y + 10.f);
    setInnerRect(b2, 0.f);

    b3.sprite.setTexture(texN);
    b3.sprite.setScale(0.6f, 0.6f);
    b3.text.setFont(*font);
    b3.text.setString("TRUDNY");
    b3.text.setCharacterSize(30);
    b3.text.setFillColor(sf::Color::Black);
    b3.sprite.setPosition(50.f, 350.f);
    b3.text.setPosition(b3.sprite.getPosition().x + 20.f,
        b3.sprite.getPosition().y + 10.f);
    setInnerRect(b3, 0.f);

    bOK.sprite.setTexture(texN);
    bOK.sprite.setScale(0.5f, 0.5f);
    bOK.text.setFont(*font);
    bOK.text.setString("Zatwierdz");
    bOK.text.setCharacterSize(24);
    bOK.text.setFillColor(sf::Color::Black);
    bOK.sprite.setPosition((float)GAME_WIDTH - 130.f, (float)GAME_HEIGHT - 70.f);
    bOK.text.setPosition(bOK.sprite.getPosition().x + 20.f,
        bOK.sprite.getPosition().y + 10.f);
    setInnerRect(bOK, 0.f);

    bPowrot.sprite.setTexture(texN);
    bPowrot.sprite.setScale(0.5f, 0.5f);
    bPowrot.text.setFont(*font);
    bPowrot.text.setString("Powrot");
    bPowrot.text.setCharacterSize(24);
    bPowrot.text.setFillColor(sf::Color::Black);
    bPowrot.sprite.setPosition(10.f, (float)w.getSize().y - 70.f);
    bPowrot.text.setPosition(bPowrot.sprite.getPosition().x + 10.f,
        bPowrot.sprite.getPosition().y + 5.f);
    setInnerRect(bPowrot, 0.f);

    int chosen = 0;
    bool run = true;
    while (w.isOpen() && run) {
        sf::Event ev;
        while (w.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                w.close();
                return 0;
            }
            // BACKSPACE = powrot
            if (ev.type == sf::Event::KeyPressed &&
                ev.key.code == sf::Keyboard::Backspace) {
                return -2;
            }
            // ENTER => zatwierdz (jeśli chosen!=0)
            if (ev.type == sf::Event::KeyPressed &&
                ev.key.code == sf::Keyboard::Enter) {
                if (chosen != 0) return chosen;
            }

            if (ev.type == sf::Event::MouseMoved) {
                float mx = (float)ev.mouseMove.x;
                float my = (float)ev.mouseMove.y;
                if (b1.contains(mx, my)) setInnerRect(b1, (chosen == 1) ? 5.f : 3.f);
                else                     setInnerRect(b1, (chosen == 1) ? 5.f : 0.f);

                if (b2.contains(mx, my)) setInnerRect(b2, (chosen == 2) ? 5.f : 3.f);
                else                     setInnerRect(b2, (chosen == 2) ? 5.f : 0.f);

                if (b3.contains(mx, my)) setInnerRect(b3, (chosen == 3) ? 5.f : 3.f);
                else                     setInnerRect(b3, (chosen == 3) ? 5.f : 0.f);

                if (bOK.contains(mx, my)) setInnerRect(bOK, 3.f);
                else                      setInnerRect(bOK, 0.f);

                if (bPowrot.contains(mx, my)) setInnerRect(bPowrot, 3.f);
                else                          setInnerRect(bPowrot, 0.f);
            }
            if (ev.type == sf::Event::MouseButtonPressed &&
                ev.mouseButton.button == sf::Mouse::Left) {
                float mx = (float)ev.mouseButton.x;
                float my = (float)ev.mouseButton.y;
                if (b1.contains(mx, my)) chosen = 1;
                if (b2.contains(mx, my)) chosen = 2;
                if (b3.contains(mx, my)) chosen = 3;
                if (bOK.contains(mx, my)) {
                    if (chosen != 0) return chosen;
                }
                if (bPowrot.contains(mx, my)) {
                    return -2;
                }
            }
        }
        w.clear(sf::Color::Black);
        w.draw(title);
        b1.draw(w);
        b2.draw(w);
        b3.draw(w);
        bOK.draw(w);
        bPowrot.draw(w);
        w.display();
    }
    return chosen;
}

// 4) BallsAmount
int MenuSystem::chooseBallsAmount() {
    sf::RenderWindow& w = *window;
    sf::Text prompt("Ile wybierasz pilek (1-500)?", *font, 30);
    prompt.setFillColor(sf::Color::White);
    prompt.setPosition(50.f, 100.f);

    sf::Text input("", *font, 30);
    input.setFillColor(sf::Color::Yellow);
    input.setPosition(50.f, 160.f);

    sf::Texture texN = *texNormal;
    Button bOK, bPowrot;
    bOK.sprite.setTexture(texN);
    bOK.sprite.setScale(0.6f, 0.6f);
    bOK.text.setFont(*font);
    bOK.text.setString("Zatwierdz i uruchom gre");
    bOK.text.setCharacterSize(25);
    bOK.text.setFillColor(sf::Color::Black);
    bOK.sprite.setPosition(50.f, 300.f);
    bOK.text.setPosition(bOK.sprite.getPosition().x + 10.f,
        bOK.sprite.getPosition().y + 5.f);
    setInnerRect(bOK, 0.f);

    bPowrot.sprite.setTexture(texN);
    bPowrot.sprite.setScale(0.5f, 0.5f);
    bPowrot.text.setFont(*font);
    bPowrot.text.setString("Powrot");
    bPowrot.text.setCharacterSize(24);
    bPowrot.text.setFillColor(sf::Color::Black);
    bPowrot.sprite.setPosition(10.f, (float)w.getSize().y - 70.f);
    bPowrot.text.setPosition(bPowrot.sprite.getPosition().x + 10.f,
        bPowrot.sprite.getPosition().y + 5.f);
    setInnerRect(bPowrot, 0.f);

    std::string userStr;
    int val = 0;
    bool run = true;
    while (w.isOpen() && run) {
        sf::Event ev;
        while (w.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                w.close();
                return 0;
            }
            // BACKSPACE = cofka (jeśli userStr puste)
            if (ev.type == sf::Event::KeyPressed &&
                ev.key.code == sf::Keyboard::Backspace) {
                if (!userStr.empty()) {
                    userStr.pop_back();
                }
                else {
                    return -2;
                }
            }
            // ENTER => zatwierdz
            if (ev.type == sf::Event::KeyPressed &&
                ev.key.code == sf::Keyboard::Enter) {
                if (!userStr.empty()) {
                    try {
                        val = stoi(userStr);
                        if (val >= 1 && val <= 500) {
                            return val;
                        }
                        else {
                            userStr.clear();
                        }
                    }
                    catch (...) {
                        userStr.clear();
                    }
                }
            }
            if (ev.type == sf::Event::MouseMoved) {
                float mx = (float)ev.mouseMove.x;
                float my = (float)ev.mouseMove.y;
                if (bOK.contains(mx, my))
                    setInnerRect(bOK, 3.f);
                else
                    setInnerRect(bOK, 0.f);

                if (bPowrot.contains(mx, my))
                    setInnerRect(bPowrot, 3.f);
                else
                    setInnerRect(bPowrot, 0.f);
            }
            if (ev.type == sf::Event::TextEntered) {
                if (ev.text.unicode < 128) {
                    char c = (char)ev.text.unicode;
                    if (isdigit(c)) {
                        userStr.push_back(c);
                    }
                }
            }
            if (ev.type == sf::Event::MouseButtonPressed &&
                ev.mouseButton.button == sf::Mouse::Left) {
                float mx = (float)ev.mouseButton.x;
                float my = (float)ev.mouseButton.y;
                if (bOK.contains(mx, my)) {
                    if (!userStr.empty()) {
                        try {
                            val = stoi(userStr);            //string to int
                            if (val >= 1 && val <= 500) {
                                return val;
                            }
                            else {
                                userStr.clear();
                            }
                        }
                        catch (...) {
                            userStr.clear();
                        }
                    }
                }
                if (bPowrot.contains(mx, my)) {
                    return -2;
                }
            }
        }
        input.setString(userStr);

        w.clear(sf::Color::Black);
        w.draw(prompt);
        w.draw(input);
        bOK.draw(w);
        bPowrot.draw(w);
        w.display();
    }
    return val;
}

// 5) Wczytywanie nicku
int MenuSystem::enterNickSFML(string& nick, int pIndex) {
    sf::RenderWindow& w = *window;
    sf::Text prompt("Podaj nick gracza: " + to_string(pIndex + 1) + ":", *font, 30);
    prompt.setFillColor(sf::Color::White);
    prompt.setPosition(50.f, 100.f);

    sf::Text input("", *font, 30);
    input.setFillColor(sf::Color::Yellow);
    input.setPosition(50.f, 160.f);

    sf::Texture tN = *texNormal;
    Button btnOK, btnPowrot;
    btnOK.sprite.setTexture(tN);
    btnOK.sprite.setScale(0.5f, 0.5f);
    btnOK.text.setFont(*font);
    btnOK.text.setString("Zatwierdz");
    btnOK.text.setCharacterSize(25);
    btnOK.text.setFillColor(sf::Color::Black);
    btnOK.sprite.setPosition( 50.f,300.f);
    btnOK.text.setPosition(btnOK.sprite.getPosition().x + 20.f,
        btnOK.sprite.getPosition().y + 10.f);
    setInnerRect(btnOK, 0.f);

    btnPowrot.sprite.setTexture(tN);
    btnPowrot.sprite.setScale(0.5f, 0.5f);
    btnPowrot.text.setFont(*font);
    btnPowrot.text.setString("Powrot");
    btnPowrot.text.setCharacterSize(25);
    btnPowrot.text.setFillColor(sf::Color::Black);
    btnPowrot.sprite.setPosition(10.f,
        (float)w.getSize().y - 70.f);
    btnPowrot.text.setPosition(btnPowrot.sprite.getPosition().x + 10.f,
        btnPowrot.sprite.getPosition().y + 5.f);
    setInnerRect(btnPowrot, 0.f);

    nick.clear();
    bool done = false;
    while (w.isOpen() && !done) {
        sf::Event ev;
        while (w.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                w.close();
                return 0;
            }
            // BACKSPACE => jeśli nick niepusty, usuwamy znak; jeśli pusty => cofka
            if (ev.type == sf::Event::KeyPressed &&
                ev.key.code == sf::Keyboard::Backspace) {
                if (!nick.empty()) {
                    nick.pop_back();
                }
                else {
                    return -2;
                }
            }
            // ENTER => zatwierdz (jeśli cokolwiek wpisane)
            if (ev.type == sf::Event::KeyPressed &&
                ev.key.code == sf::Keyboard::Enter) {
                if (!nick.empty()) {
                    done = true;
                }
            }
            if (ev.type == sf::Event::MouseMoved) {
                float mx = (float)ev.mouseMove.x;
                float my = (float)ev.mouseMove.y;
                if (btnOK.contains(mx, my)) setInnerRect(btnOK, 3.f);
                else                       setInnerRect(btnOK, 0.f);

                if (btnPowrot.contains(mx, my)) setInnerRect(btnPowrot, 3.f);
                else                            setInnerRect(btnPowrot, 0.f);
            }
            if (ev.type == sf::Event::TextEntered) {
                if (ev.text.unicode < 128) {
                    char c = (char)ev.text.unicode;
                    if (isprint(c) && c != '\r' && c != '\n') {
                        nick.push_back(c);
                    }
                }
            }
            if (ev.type == sf::Event::MouseButtonPressed &&
                ev.mouseButton.button == sf::Mouse::Left) {
                float mx = (float)ev.mouseButton.x;
                float my = (float)ev.mouseButton.y;
                if (btnOK.contains(mx, my)) {
                    if (!nick.empty()) {
                        done = true;
                    }
                }
                if (btnPowrot.contains(mx, my)) {
                    return -2;
                }
            }
        }
        input.setString(nick);
        w.clear(sf::Color::Black);
        w.draw(prompt);
        w.draw(input);
        btnOK.draw(w);
        btnPowrot.draw(w);
        w.display();
    }

    return 1; // zatwierdzono
}

int main() {
    srand((unsigned)time(NULL));

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
        " Gra PONG ");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("resources/arial.ttf")) {
        cout << "Blad ladowania czcionki (arial.ttf)!" << endl;
    }

    sf::Texture texNormal, texSelected;
    if (!texNormal.loadFromFile("resources/menu_unselected.png")) {
        cout << "Blad ladowania menu_unselected.png!" << endl;
    }
    if (!texSelected.loadFromFile("resources/menu_selected.png")) {
        cout << "Blad ladowania menu_selected.png!" << endl;
    }

    MenuSystem menu(&window, &font, &texNormal, &texSelected);
    menu.runMenu();

    return 0;
}
