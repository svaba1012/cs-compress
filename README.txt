Program csp (cs program) namenjen je za kompresiju .wav i .txt u .cs format(slucajno se poklopilo sa C# ekstenzijom) i dekompresiju .cs formata u original. Ima mogucnost prosirenja tj dodavanja novih formata koje moze da kompresuje i dekompresuje. WAV fajlove kompresuje od 25% do 30% od originalne velicine, podesavanjem parametara(konstanti) DELTA_DB i 
DELTA_PHASE (17 i 18 linija u csw.c)moze se dodatno smanjiti velicina ali se gubi na kvalitetu. TXT zavisi od velicine fajla i raspodele verovatnoca karaktera, u najgorem slucaju povecava velicinu.
Program csp poziva se iz terminala sa odredjenim argumentima. Pravila prihvatanja argumenata definisana su dole nize.

Program csp je pravljen u text editoru Visual Studio Code. Koriscen je alat Make radi automatizacije kompajlovanja, bildovanja i linkovanja.

Koriscene shell skripte za dodatnu automtizaciju, koje su podesene kao neizvrsne, zbog slanja mejlom.

Pred kraj ukljucen alat git radi verzionisanja koda.

Za testiranje brzine izvrsenja koriscen je alat gprof. Za automatizaciju procesa testiranja koriscena skripta time_test.sh. NAPOMENA(otkomentarisati liniju 18 u main funkciji radi poziva proba2() f-ja koriscena za testiranje brzine)

Za kompajlovanje i linkovanje koriscena skripta build.sh

Za pokretanje bez argumenata skripta run.sh 

U test_files folderu su smesteni test fajlovi

POMOC za koriscenje
-----------------------------------------------------------------------------------
----------------CSP-cs program za manipulaciju cs kompresovanim formatima----------

Postojece opcije:
-h{opcija za pomoc}
-c{za kompresovanje fajla, naredni string uzima kao ulazni fajl za kompajlovanje}
-d{za dekompresovanje fajla, naredni string uzima kao ulazni fajl za dekompajlovanje}
-o{opciono ovim se dodaje zeljeni naziv izlaznog fajla}
-p{opcija za otvaranje kompresovanih formata cs, ne moze se koristiti opciona opcija -o uz ovu}

Primeri upotrebe:
1)Kompresovanje
a)csp -c {NAZIV_FAJLA}
b)csp -c {NAZIV_FAJLA} -o {NAZIV_IZALZNOG_FAJLA}
2)Dekompresovanje
a)csp -d {NAZIV_FAJLA}
b)csp -d {NAZIV_FAJLA} -o {NAZIV_IZALZNOG_FAJLA}
3)Otvaranje kompresovanog fajla
csp -p {NAZIV_FAJLA}
4)Pomoc
csp -h

*** Postoje odredjeni bagovi (npr. 1)oslobadjanje memorije nakon dekompresovanja linije 429-432 u csw.c, 2) -p opcija ne radi za TXT fajlove...)

*** Dodatan problem je odsecanje par sekundi s kraja wav fajla zbog podele fajla na blokove velicine 2^16 zbog fft a poslednji blok koji je manji se odbacuje. Moguce je nadograditi da se dopuni nulama pa onda fft a da se pri dekompresiji nule odbace.

*** Koriscena je moja neka improvizovana verzija fft ------> Moguce znacajno ubrzanje koriscenjem neke brze verzije.

*** Moguce je nadograditi da otvaranje fajla bude visenitno jer se dekopresuje blok po blok uz poziv odredjene funkcije koja ima mogucnost pustanja niza na zvucnik. Na taj nacin ne bi se cekalo na otvaranje.

*** Nisam siguran da sam nasao celokupan standard za wav fajlove, tako da postoji verovatnoca da oznaci wav format kao nepodrzani. 

Mejl: stefanivanovic1012@gmail.com
