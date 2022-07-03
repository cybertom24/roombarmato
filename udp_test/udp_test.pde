/**
 * (./) udp.pde - how to use UDP library as unicast connection
 * (cc) 2006, Cousot stephane for The Atelier Hypermedia
 * (->) http://hypermedia.loeil.org/processing/
 *
 * Create a communication between Processing<->Pure Data @ http://puredata.info/
 * This program also requires to run a small program on Pd to exchange data  
 * (hum!!! for a complete experimentation), you can find the related Pd patch
 * at http://hypermedia.loeil.org/processing/udp.pd
 * 
 * -- note that all Pd input/output messages are completed with the characters 
 * ";\n". Don't refer to this notation for a normal use. --
 */

// import UDP library
import hypermedia.net.*;

UDP udp;  // define the UDP object
String senderIP = "192.168.4.1";
int senderPort = 4000;

void setup() {
  size(200, 100);

  udp = new UDP(this, 4001);
  udp.log(true);     // <-- printout the connection activity
  udp.listen(true);
}

void draw() {
}

void keyPressed() {
  // send the message
  switch(keyCode) {
  case UP:
    udp.send(new byte[]{0x69, 0x09, 0x02,(byte) 0xA0, (byte) 0xC8, (byte) 0xFA}, senderIP, senderPort);
    break;
  case DOWN:
    udp.send(new byte[]{0x69, 0x09, 0x02, 0x64, 0x64, (byte) 0xFA}, senderIP, senderPort);
    break;
    case LEFT:
    udp.send(new byte[]{0x69, 0x00, 0x01, 0x01, 0x01, (byte) 0xFA}, senderIP, senderPort);
    break;
  case RIGHT:
    udp.send(new byte[]{0x69, 0x02, 0x01, 0x01, 0x01, (byte) 0xFA}, senderIP, senderPort);
    break;
  }
}

/**
 * To perform any action on datagram reception, you need to implement this 
 * handler in your code. This method will be automatically called by the UDP 
 * object each time he receive a nonnull message.
 * By default, this method have just one argument (the received message as 
 * byte[] array), but in addition, two arguments (representing in order the 
 * sender IP address and his port) can be set like below.
 */
// void receive( byte[] data ) {       // <-- default handler
void receive(byte[] data, String ip, int port) {  // <-- extended handler
  // save the address and port
  senderIP = ip;
  senderPort = port;
  println("è arrivato: " + data);
}
