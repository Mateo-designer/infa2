				                                Gra w stylu PONG

Po uruchomieniu gry uruchomiona zostaje uruchomiona konsola i zaraz z nią ekran wygenerowany za pomocą załączonej biblioteki SFML. Pierwsze okno, pokazuje MENU, w którym znajdują się tryby i opcje. Zawiera opcję : SINGLEPLAYER, MULTIPLAYER, HELP i EXIT.  W każdym momencie wciśnięcie BACKSPACE podczas wybierania opcji w MENU, powoduje powrót do poprzedniego okna lub usunięcie wpisywania z klawiatury. Wciśnięcie ENTER zatwierdza okno i przejście do następnego. Analogicznie działają przyciski “powrot” i “zatwierdz”. Opcja dowolna kliknięcie myszką na przycisk lub wciśnięcie klawisza z klawiatury.
Gdy użytkownik wybierze opcję (menu_selected) SINGLEPLAYER, przechodzi do podmenu (menu_unselected) , tzn. ma do wyboru opcję - 1 player lub training. Jeśli wybierze opcję 1 player, to jest jedna paletka do gry, lecz gdy zostanie wybrana opcja training with AI to użytkownik gra z komputerem (śledzenie piłki przez górną paletkę). 
Gdy zostanie wybrana opcja “zatwierdz”, pojawia się kolejne okno, w którym jest zapytanie o nick gracza lub graczy jeśli zostanie wybrana opcja w MENU - MULTIPLAYER. W trybie multiplayer jest analogicznie, lecz jest opcja po wybraniu MULTIPLAYER - 2 players.
Po wyborze nicku(wpisywany z klawiatury) i kliknięciu “zatwierdz” to przechodzimy do następnego okna, w którym jest do wyboru poziom gry : LATWY, SREDNI i TRUDNY. Są to poziomy zależne od prędkości piłki. Po kliknięciu przycisku “zatwierdz” jest kolejne okno z wyborem piłek z zakresu <1;500>, liczba jest wpisywana z klawiatury. 
Tutaj jest wiadomo jest przycisk “powrot” i “zatwierdz i uruchom gre”. 
Po wciśnięciu ENTER z klawiatury lub wciśnięcia myszką zatwierdzenia - następuje odliczanie wraz z napisem - Gra rozpocznie się za : 3…2…1. Następnie jest zainicjalizowana gra, składająca się z paletki lub paletek, w zależności od wcześniej wybranego trybu. 
Gra ma bandę niebieską, piłki są generowane w postaci okręgu i odbijają się od bandy, dla jednego gracza, górna banda jest zablokowana tzn. odbijają się piłki. 
Po prawej stronie jest nick zapisany gracza wraz z liczoną punktacją i ile piłek jest jeszcze w grze z wybranych. Punkty są liczone z odbitej piłki od paletki, piłki są generowane jako kolorowe, lecz gdy piłka odbije się od dolnej paletki zmienia swój kolor na czerwony i lekko przyspiesza, a odbita od górnej paletki również zmienia kolor, lecz na zielony i również lekko przyspiesza. Po wciśnięciu z klawiatury ESC, użytkownik zostanie zapytany czy chce opuścić grę, będą wyświetlone dwie odpowiedzi : TAK lub NIE. 
Jeśli wybierze “tak” to opuści grę, lecz jeśli wybierze “nie” to jest wznowiona gra od tego punktu, gdzie wcisnął. Wciśnięcie przycisku F1, daje pomoc, tzn. wyświetla komunikat o sterowaniu paletkami, przycisku ESC, przycisku F1, aby opuścić należy kliknąć myszką i gra zostanie wznowiona od punktu, którego została zatrzymana.  
Po skończeniu gry, gdy wszystkie piłki już “wylecą” pojawi się okno z komunikatem - koniec gry, który miga z częstotliwością 1 sekundy; również jest napisany komunikat - “Aby opuścić grę wciśnij ENTER” - po wciśnięciu ENTER z klawiatury, następuje opuszczenie gry, na konsoli pokaże się czas gry. 

