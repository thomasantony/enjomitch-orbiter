---============INTRO=============---
Jest to kalkulator rzeczywistego azymutu z ktorym musisz wystartowac
aby miec pozadana inklinacje po wejsciu na orbite (np inklinacje ISS).
Uzyte rownania biora pod uwage rotacje planet. Poniewaz azymut jest 
funkcja polozenia, jesli zboczysz z kursu niemozliwe jest dowiedzenie
sie jaki powinienes miec azymut uzywajac tego programu. 
Jest on przeznaczony jedynie do startow.
Po info "co, jak, po co, na co" udaj sie na http://orbiter.rox.pl
i szukaj w "Tlumaczeniach" "Obliczania azymutu i czasu startu"
Program jest napisany w C++ win32 wiec nie potrzeba zadnych dodatkowych
instalacji bibliotek, i zostal stworzony z mysla o kompatybilnosci z nowym
wieloplatformowym symulatorem Kosmos (hail!).
Cel zostal osiagniety - Program dziala pod *NIX'ami za pomoca Wine 

---========AKTUALIZACJE==========---

20.11.2006 v.1.5

Stworzylem interfejs graficzny. Poza drobnymi usprawnieniami,
algorytm jest taki sam (o ile czegos nie schrzanilem :) ) 


27.08.2006 v.1.2

Zecydowalem sie jednak uwzglednic rotacje Ksiezyca
Sorry jesli jej brak spowodowal jakies problemy :)

26.08.2006 v.1.1

1) !!Program uzywa nowiuskich algorytmow, ktore daja...
prawie taki sam wynik :) . Powaznie to nowe algorytmy pozwolily na
polepszenie interfejsu uzytkownika. Teraz wprowadzasz wysokosc docelowej
orbity zamiast jej predkosc orbitalna. (V orb. jest obliczana)
Skad mialbys znac v=f(wysokosc) ?

2) Dodalem opcje wybory wlasnej planety, gdzie mozesz wprowadzic parametry
dowolnego ciala (opcja nastawiona na obce uklady planetarne), jesli 
nie siedzisz w C++ na tyle ze moglbys zmodyfikowac zrodla do swoich celow.
Po prostu otworz plik konfiguracyjny ciala w ..\Orbiter\Config i wprowadz dane.
Instrukcje beda wyswietlane w miare jak bedziesz wpisywal kolejne wartosci.

3) Pare podkrecen, wlaczajac te dla orbit wstecznych. Kalkulator jest teraz
bardziej informatywny. Sprobuj na przyklad wprowadzic "23" jako inklinacje
oraz wieksza od tego szerokosc geograficzna. Potem np. inklinacje = "162"
a szerokosc geogr. = "38".
_____________________________________

Na m6 odbyly sie dyskusje o wymaganym azymucie Promow podczas startu na ISS,
i sugerowanym azymutem bylo 42.4, podczas kiedy dla tej samej konfiguracji
moj program wyswietli okolo 42.85. Nastepujace zdanie jest tylko moja spekulacja
ale jest mozliwe ze to autopilot Promow wprowadza blad do ostatecznej inklinacji
uzywajac tego algorytmu lub moze byc to zwykly fenomen lotow kosmicznych,
ktorego nie potrafie wytlumaczyc,...lub obie rzeczy naraz.

Oto co zrobilem zeby sprawdzic poprawnosc algorytmu:
1) Uruchomilem scenariusz Cape Canaveral, wlaczylem Edytor Scenariuszy 
 i obliczylem azymut uzywajac szerokosci z Ed. Scn. i arbitralnie wybralem wysokosc 250 km
2) Zapauzowalem symulator
3) Uzylem Ed. Scn. aby dopasowac moj azymut do obliczonego 42.85
4) Uzylem dodatku "Impulsive burn" i wykonalem "znormalizowany wektor startu" z programu.
5) Odpauzowalem

Ostateczna inklinacja wyniosla 51.61 zamiast 51.57 (d = 0.04) ale zauwaz ze ekscentrycznosc
wynosila 0.004, co spowodowalo ze PeA wynosilo 205.9 i ApA 250, co powoduje ze mysle ze 
manerw impulsowy nie byl skierowany czysto Prograde.
Zrzuty ekranu:
http://www.elwico.pl/~ender-sz/img/orb/az_calculator1-pl.png
http://www.elwico.pl/~ender-sz/img/orb/az_calculator2.png

Podziekowania:
BEEP (podstawowe rownania) 
Urwumpe (poprzednie rownania i toche informacji merytorycznych)
Andy44 (dostarczyciel nowych rownan)
Jerry Jon Sellers (ksiazka "Zrozumiec Kosmos" skad pochodza nowe rownania)
Sputnik (za pomysl w jaki sposob mozna przetestowac program...)
Dcjacob (... i dla autora dodatku Impulsive Burn ktory pozwolil na wykonanie tego testu)
Ziomki na IRCu Orbitera, w szczegolnosci Daver (jak zwykle - pomoc w programowaniu :> )

Zrodla z ktorych korzystalem:
http://www.winprog.net/tutorial/
http://www.catch22.net/tuts/
http://www.foosyerdoos.fsnet.co.uk/
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winprog/winprog/functions_in_alphabetical_order.asp

I wspanialy program WinDialogEditor - alternatywa edytora VC++,
zmodyfikowany przez Davera w celu osiagniecia kompatybilnosci z Wine


Copyright (C) 2006 Szymon Ender "Enjo"

Niniejszy program jest wolnym oprogramowaniem; moesz go 
rozprowadza�dalej i/lub modyfikowa�na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundacj�Wolnego
Oprogramowania - wedug wersji 2-giej tej Licencji lub kt�ej
z p�iejszych wersji. 
Niniejszy program rozpowszechniany jest z nadziej, i b�zie on 
uyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domylnej 
gwarancji PRZYDATNOCI HANDLOWEJ albo PRZYDATNOCI DO OKRELONYCH 
ZASTOSOWA� W celu uzyskania bliszych informacji - Powszechna 
Licencja Publiczna GNU. 
Z pewnoci wraz z niniejszym programem otrzymae te egzemplarz 
Powszechnej Licencji Publicznej GNU (GNU General Public License);
jeli nie - napisz do Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.

/***************************************************************************
 *   Copyright (C) 2006 by Szymon Ender "Enjo"			  				   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/