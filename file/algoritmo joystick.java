/*
 * Spiegazione funzioni utilizzate:
 * 
 * joystick:
 * .percentualeX() restituisce la posizione x della manopola come float che va da -100 a 100 dove 0 è al centro
 * .percentualeY() come per la x ma con la coordinata y della manopola
 * .percentualeTot() resitiuisce la distanza dal centro della manopola in percentuale (pitagora sulle coordinate x e y e poi divido per il raggio massimo). Il valore va da 0 a 100
 * 
 */
public static void main(String args[]) {

}

public void onPositionChanged(float[] position, float[] oldPosition) {
    // Le potenze sono contenute in un array.
    // Nella posizione 0 c'è la potenza del motore destro, nella posizione 1 quella per il motore sinistro
    // Queste potenze avranno un valore che va da -100 a 100 dove il segno indica la direzione di movimento del motore
    // e il valore indica la potenza in percentuale da dare al motore
    float[] potenze = calcolaPotenze(joystick.percentualeX(), joystick.percentualeY(), joystick.percentualeTot());

    // Ora converto il valore delle potenze in comando da passare alla funzione di invio
    Command comando = costruisciComando(potenze[0], potenze[1]);
    send(comando);
}

public float[] calcolaPotenze(float percentualeX, float percentualeY, float percentualeTot) {
    float potenzaDx = 0;
    float potenzaSx = 0;

    // Se la manopola è puntata verso l'avanti
    if (percentualeY >= 0) {
        if (percentualeX < 0)
            potenzaDx = 1;
        else {
            // percentualeX va da 0 a 1 (dividendolo per 100) e lo devo fare andare da 1 a -1
            potenzaDx = percentualeX / 100f;
            potenzaDx *= -2;     // Inverto il senso e aumento il range
            potenzaDx += 1;      // Sommo l'offset
        }
        if (percentualeX > 0)
            potenzaSx = 1;
        else {
            potenzaSx = percentualeX / 100f;
            potenzaSx *= 2;      // Aumento il range
            potenzaSx += 1;      // Sommo l'offset
        }
    }
    // Nel caso in cui la manopola fosse sotto lo zero (puntata verso il dietro) gestisco i due motori in modo inverso
    else {
        // Invertendo la potenza (e quindi la direzione)
        percentualeTot *= -1;
        if (percentualeX < 0)
            potenzaSx = 1;
        else {
            potenzaSx = percentualeX / 100f;
            potenzaSx *= -2;
            potenzaSx += 1;
        }
        if (percentualeX > 0)
            potenzaDx = 1;
        else {
            potenzaDx = percentualeX / 100f;
            potenzaDx *= 2;
            potenzaDx += 1;
        }
    }
    // potenzaDx e potenzaSx vanno da -1 a 1 e li faccio andare da -100 a 100 in base a quanto dista
    // la manopola dal centro del joystick
    potenzaDx *= percentualeTot;
    potenzaSx *= percentualeTot;

    return new float[] {potenzaDx, potenzaSx};
}

public Command costruisciComando(float potenzaDx, float potenzaSx) {
    // Conversione dei dati
    /* In data[0] è contenuta la potenza da dare al motore DX
     * In data[1] quella per il motore SX
     * Il dato è strutturato in modo da contenere nel LSB la direzione (1 = avanti, 0 = indietro)
     * mentre nei successivi bit la potenza effettiva (che va quindi da 0 a 255 a passo di 2)
     */

    // Recupero la direzione (vero = avanti, falso = indietro)
    boolean direzioneDx = potenzaDx >= 0;
    boolean direzioneSx = potenzaSx >= 0;
    // Espando il range da [0,100] (togliendo il segno) a [0,255]
    potenzaDx = (float) Math.abs(potenzaDx) * 2.55f;
    if (potenzaDx > 255)
        potenzaDx = 255;
    
        potenzaSx = (float) Math.abs(potenzaSx) * 2.55f;
    if (potenzaSx > 255)
        potenzaSx = 255;
    // Converto il dato in byte che devo inviare
    byte datoDx = (byte) potenzaDx;
    byte datoSx = (byte) potenzaSx;
    // Sovrascrivo la direzione sul LSB del dato da inviare
    if (direzioneDx)
        datoDx = (byte) (datoDx | 0x01);
    else 
        datoDx = (byte) (datoDx & 0xFE);
    
    if (direzioneSx) 
        datoSx = (byte) (datoSx | 0x01);
    else 
        datoSx = (byte) (datoSx & 0xFE);
    
    return makeCommand(Commands.MOVE, datoDx, datoSx);
}

// IMPORTANTE: ricordarsi della questione dei messaggi importanti!!!
// Serve se abbiamo il problema dell'accumulo di messaggi
// Vedere prima se si riesce a risolvere
// Se i motori si devono fermare (pot = 00000001, in caso di "0" la direzione è preimpostata in avanti)
// segna il messaggio come importante (qualsiasi altro messaggio in lista viene eliminato e viene inviato solo questo)