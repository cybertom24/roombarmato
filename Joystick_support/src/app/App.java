package app;

import cyberLib.*;
import cyberLib.io.*;
import cyberLib.io.Joystick.*;
import cyberLib.io.Output.DecorationType;
import cyberLib.connection.*;
import net.java.games.input.Controller;
import net.java.games.input.ControllerEnvironment;

/* CONTROLLI
 * 
 * Levetta SX = movimento motori
 * Triangolo = Y = Emergenza
 * L1 + R1 = LB + RB = fuoco
 * Levetta DX = angolazione canne
 * tasto SU = alza volume
 * tasto GIU = abbassa volume
 * tasto DX = canzone successiva
 * tasto SX = canzone precedente
 * X = A = play/pausa musica
 * cerchio = B = muta/smuta
 * start = connetti / disconnetti 
 * 
*/

public class App {

	private Udp udp;
	private Loop loop;
	private int delay = 100;
	private Joystick joystick = null;
	private boolean playing = false, muted = false;
	private int volume = RoombarmatoValues.VOLUME_DEFAULT;
	private String ip = "192.168.56.1";
	// private String ip = RoombarmatoValues.IP;
	private int port = RoombarmatoValues.PORT;

	public static void main(String[] args) {
		new App();
	}

	public App() {
		// In questo modo non mi muovo più negli static ma in quelli definiti
		System.out.println("Script per gestire il roombarmato tramite il controller PS3");

		Controller[] controllers;
		controllers = loadController();

		Menu controllerMenu = new Menu("Scegli il controller");

		for (Controller controller : controllers)
			controllerMenu.add(controller.getName());

		udp = new Udp(ip, port)
				.setDelay(RoombarmatoValues.COMMAND_DELAY)
				.setMaxQueueSize(RoombarmatoValues.MAX_QUEUE_SIZE);

		gestisciController(controllers[controllerMenu.select()]);
	}

	// Funzione che cerca tutti i joystick collegati al computer e li ritorna in un
	// array
	private Controller[] loadController() {
		return ControllerEnvironment.getDefaultEnvironment().getControllers();
	}

	private void gestisciController(Controller c) {
		Output.printDecoration(DecorationType.DASH);
		println("Hai scelto il controller: " + c.getName());

		try {
			joystick = new Joystick(c);
		} catch (Exception e) {
			e.printStackTrace();
			return;
		}

		// Genera il listener per i pulsanti (che riceve il cambiamento di stato dei
		// bottoni ed esegue i comandi giusti
		joystick.addListener(new ButtonListener() {

			@Override
			public void buttonReleased(int which) {
				switch (which) {
				case Buttons.START:
					break;
				case Buttons.A:
					break;
				case Buttons.B:
					break;
				case Buttons.Y:
					break;
				case Buttons.RIGHT:
					break;
				case Buttons.LEFT:
					break;
				case Buttons.UP:
					break;
				case Buttons.DOWN:
					break;
				}
			}

			@Override
			public void buttonPressed(int which) {
				byte[] cmd;
				switch (which) {
				case Buttons.START:
					if (udp.isGoing())
						udp.stop();
					else
						udp.start();
					break;

				case Buttons.A:
					if (playing)
						cmd = Commands.makeCommand(Commands.PAUSE);
					else
						cmd = Commands.makeCommand(Commands.PLAY);
					udp.send(cmd);
					playing = !playing;
					break;

				case Buttons.B:
					if (muted) {
						if (volume > RoombarmatoValues.MAX_VOLUME)
							volume = RoombarmatoValues.MAX_VOLUME;
						if (volume < 0)
							volume = 0;
						cmd = Commands.makeCommand(Commands.SET_VOLUME, volume);
					} else
						cmd = Commands.makeCommand(Commands.SET_VOLUME, 0);

					udp.send(cmd);
					muted = !muted;
					break;

				case Buttons.Y:
					cmd = Commands.makeCommand(Commands.MOVE, 1, 1);
					udp.send(cmd);
					break;

				case Buttons.RIGHT:
					cmd = Commands.makeCommand(Commands.NEXT_SONG);
					udp.send(cmd);
					break;

				case Buttons.LEFT:
					cmd = Commands.makeCommand(Commands.PREVIOUS_SONG);
					udp.send(cmd);
					break;

				case Buttons.UP:
					volume += RoombarmatoValues.VOLUME_INCREASE;
					if (volume > RoombarmatoValues.MAX_VOLUME)
						volume = RoombarmatoValues.MAX_VOLUME;

					cmd = Commands.makeCommand(Commands.SET_VOLUME, volume);
					udp.send(cmd);
					break;

				case Buttons.DOWN:
					volume -= RoombarmatoValues.VOLUME_INCREASE;
					if (volume < RoombarmatoValues.MIN_VOLUME)
						volume = RoombarmatoValues.MIN_VOLUME;

					cmd = Commands.makeCommand(Commands.SET_VOLUME, volume);
					udp.send(cmd);
					break;
				}
			}
		});

		joystick.addListener(new AxisListener() {

			@Override
			public void axisChanged(int which) {
				if (which == Axis.LEFT_X || which == Axis.LEFT_Y) {
					float valD = 0;
					float valS = 0;
					float valX = joystick.getValue(Axis.LEFT_X) * 100, valY = joystick.getValue(Axis.LEFT_Y) * 100;
					float valP = (float) Math.sqrt(valX * valX + valY * valY);

					if (valY >= 0) {
						if (valX < 0)
							valD = 1;
						else {
							// valX va da 0 a 1 (dividendolo per 100) e lo devo fare andare da 1 a -1
							valD = valX / 100f;
							valD *= -2; // Inverto il senso e aumento il range
							valD += 1; // Sommo l'offset
						}

						if (valX > 0)
							valS = 1;
						else {
							valS = valX / 100f;
							valS *= 2; // Aumento il range
							valS += 1; // Sommo l'offset
						}
					}
					// Nel caso in cui la manopola fosse sotto lo zero gestisco i due motori in modo
					// inverso
					else {
						// Invertendo la potenza (e quindi la direzione)
						valP *= -1;
						if (valX < 0)
							valS = 1;
						else {
							valS = valX / 100f;
							valS *= -2;
							valS += 1;
						}

						if (valX > 0)
							valD = 1;
						else {
							valD = valX / 100f;
							valD *= 2;
							valD += 1;
						}
					}
					// valD e valS vanno da -1 a 1 e li faccio andare da -100 a 100 in base a quanto
					// dista
					// la manopola dal centro del joystick
					valD *= valP;
					valS *= valP;

					// Conversione dei dati
					/*
					 * In data[0] è contenuta la potenza da dare al motore DX In data[1] quella per
					 * il motore SX Il dato è strutturato in modo da contenere nel LSB la direzione
					 * (1 = avanti, 0 = indietro) mentre nei successivi bit la potenza effettiva
					 * (che va quindi da 0 a 255 a passo di 2)
					 */
					// Recupero la direzione
					boolean dirD = valD >= 0;
					boolean dirS = valS >= 0;
					// Espando il range da 0 a 100 (togliendo il segno) a da 0 a 255
					valD = (float) Math.abs(valD) * 2.55f;
					if (valD > 255)
						valD = 255;
					valS = (float) Math.abs(valS) * 2.55f;
					if (valS > 255)
						valS = 255;
					// Genero il dato in byte che devo inviare
					byte potD = (byte) valD;
					byte potS = (byte) valS;
					// Sovrascrivo la direzione sul LSB del dato da inviare
					if (dirD)
						potD = (byte) (potD | 0x01);
					else
						potD = (byte) (potD & 0xFE);
					if (dirS)
						potS = (byte) (potS | 0x01);
					else
						potS = (byte) (potS & 0xFE);

					// Se i motori si devono fermare (pot = 00000001, in caso di "0" la direzione è
					// preimpostata in avanti)
					// segna il messaggio come importante (qualsiasi altro messaggio in lista viene
					// eliminato e viene inviato solo questo)
					udp.send(Commands.makeCommand(Commands.MOVE, potD, potS), (potD == 1) && (potS == 1));
				}
			}
		});

		println("> Lettura dati iniziata");
		// Loop che viene inizializzato con la chiamata dell poll ovvero registra lo
		// stato di ciascun tasto
		loop = new Loop(new Loop.Job() {
			
			@Override
			public void stop() {
				// TODO Auto-generated method stub
				
			}
			
			@Override
			public void start() {
				// TODO Auto-generated method stub
				
			}
			
			@Override
			public void interrupted() {
				// TODO Auto-generated method stub
				
			}
			
			@Override
			public void execute() {
				if(!joystick.update())
					loop.stop();
			}
		}, delay);
		loop.start();
	}

	private static void println(String s) {
		System.out.println(s);
	}

	private static void print(String s) {
		System.out.print(s);
	}
}
