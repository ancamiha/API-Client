Enache Anca- Mihaela
324CD
Tema 3 PC

	In realizarea acestei teme am pornit de la laboratorul 10, folosind scheletul 
dat si implementarea functiilor "compute_get_request" si "compute_post_request" din 
fisierul requests.c la care am adus niste modificari.
	Cele doua functii mentionate mai sus le-am adaptat pentru a lucra si cu token,
acesta demonstrand accesul la biblioteca. Token-ul este primit ca argument si in caz ca
este diferit de NULL se adauga la mesaj folosind formatul specificat: 
"Authorization: Bearer %s". De asemenea, am implementat si functia compute_delete_request 
ce este similara cu "compute_get_request" construind mesajul de trimis la server pentru 
ruta de acces DELETE.

	client.c
- functia "getMessage" printeaza ultimul rand din raspunsul primit de la server
	In main imi pregatesc variabilele necesare, printre care o variabila JSON_Value
si una JSON_Object pe care le initializez si doua variabile de tip bool ce-mi vor 
spune daca clientul este logat(logged_in) si daca acesta are acces la biblioteca(acces_in).
	Dinamica programului se intampla in while: se astepta o comanda de la tastatura
si in functie de aceasta se intra in if-ul corespunzator.
- register si login - comenzi ce au o implementare similara, diferind url-ul la care este
	trimis mesajul. Se deschide o conexiune spre ip si port date, se asteapta de la
	tastatura un username si o parola construindu-se cu acestea un JSON, acesta fiind
	transformat ulterior in string prin functia "json_serialize_to_string_pretty",
	apoi se compune mesajul corespunzator POST, se trimite la server, se primeste
	raspunsul si se inchide conexiunea. In final raspunsul este afisat cu ajutorul
	functiei "getMessage";
	- la comanda login se verifica suplimentar daca clientul nu este deja conectat,
	daca este deja conectat se afiseaza un mesaj, variabila logged_in se seteaza pe
	true daca autentificarea se face cu succes;
- enter_library - se realizeaza numai daca clientul este conectat, se deschide o conexiune
	se compune din nou mesajul de login pentru a se obtine un cookie valid( din cauza
	ca server-ul delogheaza destul de repede clientul), din raspunsul primit la req de
	tip POST se extrage cookie-ul ce va fi utilizat in compunerea mesajului de tip GET
	demonstrand ca suntem autentificati. Din raspunsul primit extrag bucata de tip
	JSON din care obtin token-ul(salvat in variabila token). Variabila acces_in se
	seteaza pe true;
	- daca clientul nu este autentificat se afiseaza un mesaj;
- get_books - se realizeaza daca avem acces la biblioteca(acces_in == true). Se deschide
	o conexiune se compune mesajul de tip GET pentru url-ul dat se trimite la server
	,extrag din raspunsul primit JSON-ul, il afisez si inchid conexiunea;
	- daca acces-ul nu este permis se afiseaza un mesaj;
- get_book si delete_book - se realizeaza daca este permis acces-ul la biblioteca, se ofera
	prompt pentru id, acesta fiind primit de la tastatura. Formez url-ul cu un strcat,
	deschid conexiunea, compun mesajul de tip GET si afisez raspunsul primit(pentru 
	get_book JSON-ul, pentru delete_book mesajul intors de server daca acesta exista),
	inchid conexiunea;
	- daca acces-ul nu este permis se afiseaza un mesaj;
- add_book - se realizeaza daca utilizatorul are acces la biblioteca. Se deschide o 
	conexiune, se ofera prompt pentru informatiile necesare fiind citite e la tastatura,
	se formeaza un JSON cu campurile aferente si se transforma intr-un string. Compun
	mesajul corespunzator(POST) cu stringul realizat(serialized_string), se trimite la
	server si se afiseaza mesajul primit de la server;
	- daca acces-ul nu este permis se afiseaza un mesaj;
- logout - se realizeaza daca clientul este autentificat(logged_in == true), deschid o
	conexiune, compun mesajul de login(POST) pentru a obtine un cookie valid pe care 
	il voi folosi in compunerea mesajului de tip GET. Dupa ce este primit raspunsul
	conexiunea se inchide si se afiseaza raspunsul intors de server;
- exit - se inchide conexiune cu "close_connection";
	La final eliberez memoria alocata.
	Pentru JSON am folosit biblioteca sugerata in enunt "parser.h".