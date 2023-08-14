# roombarmato

A repo to make a military version of the robotic vacuum cleaner Roomba

### TODO

#### PCB

- (OK) Posiziona fori di montaggio
- (OK) Trova valori per i condensatori del regolatore da 3.3V
- (OK) Controlla che il package dei componenti corrisponda
- (OK) Completa file ordine
- (OK) Modifica AK500
- (OK) Controlla funzionamento amplificatore audio
- (OK) Piano di massa in bottom e piano +12V in top

#### Arduino code

- (OK ?) Risolvi problema del blocco per colpa dei messaggi
- (OK) Aggiungi stop automatico dopo 2 secondi di assenza di messaggi
- Aggiungi controllo tensione della batteria e blocco di tutto quando si scende sotto i ~10.5V
- Completa comandi
- (OK) Aggiungi sistema di sincronizzazione ESP-arduino

##### Funzionalità

- (OK) Apri/Chiudi coperchio
- (OK) Spegni canne con millis() e non delay()
- (OK) Muovi
- (OK) Spegni motori dopo tot secondi di assenza messaggi
- (OK) Spegni tutto con disconnessione
- (OK) Accendi/Spegni occhi
- (OK) Controllo musica
- Controllo livello batteria
- Invio errore di lancio (perchè coperchio chiuso)
- (OK) Invio errore troppi messaggi
- Messaggio multiplo per accensione canne
- Invio stato batteria
- Invio stato coperchio

### ESP code

- Aggiungi sistema di sincronizzazione
- Aggiungi sistema di mantenimento di messaggi in modo da inviarne uno per volta

#### App

(OK) Semplifica algoritmo di gestione potenza dei motori

### Help

#### Comandi git

- Per impacchettare i file modificati: git add .
- Per assegnare il nome: git commit -m "nome alla modifica"
- Per caricare su internet: git push

- Per scaricare: git pull
