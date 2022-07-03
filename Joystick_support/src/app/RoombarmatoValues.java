package app;

/*
 * This class contains all of the values needed so they can be changed easily
 * 
 */

public class RoombarmatoValues {
	// For connecting
	public static final String IP = "192.168.4.1";
	public static final int PORT = 4000;
	public static final int COMMAND_DELAY = 75;
	public static final int MAX_QUEUE_SIZE = 4;
	// For the music
	public static final int VOLUME_INCREASE = 5;
	public static final int MIN_VOLUME = VOLUME_INCREASE;
	public static final int MAX_VOLUME = 100;
	public static final int VOLUME_DEFAULT = MAX_VOLUME / 2;
}
