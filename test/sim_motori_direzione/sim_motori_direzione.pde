PVector motD, motS;
PVector posD, posS;
PVector roomba, momento;

float braccio = 100;

Joystick joystick;

void setup() {
  size(500, 500); 
  motD = new PVector(0, -100);
  motS = new PVector(0, 100);
  roomba = new PVector(width/2, height/2);
  posD = new PVector(braccio/2, 0);
  posS = new PVector(-braccio/2, 0);
  momento = new PVector(0, 0);
  joystick = new Joystick(width/4, height/4, 75);
}

void draw() {

  if (keyPressed && key == ' ')
    joystick.update();

  float valD = 0;
  float valS = 0;
  float valX = joystick.x(), valP = joystick.value() * 100;

  if (joystick.y() >= 0) {
    if (valX < 0)
      valD = 1;
    else {
      // valX va da 0 a 1 e lo devo fare andare da 1 a -1
      valD = valX;
      valD *= -2;     // Inverto il senso e aumento il range
      valD += 1;      // Sommo l'offset
    }

    if (valX > 0)
      valS = 1;
    else {
      valS = valX;
      valS *= 2;      // Aumento il range
      valS += 1;      // Sommo l'offset
    }
  }
  // Nel caso in cui la manopola fosse sotto zero gestisco i due motori in senso contrario
  else {
    valP *= -1;
    if (valX < 0)
      valS = 1;
    else {
      valS = valX;
      valS *= -2;
      valS += 1;
    }

    if (valX > 0)
      valD = 1;
    else {
      valD = valX;
      valD *= 2;
      valD += 1;
    }
  }
  // valD e valS vanno da -1 a 1 e li faccio andare da -100 a 100 in base a quanto dista
  // la manopola dal centro del joystick
  valD *= valP;
  valS *= valP;

  motD.set(0, valD);
  motS.set(0, valS);

  momento = posD.cross(motD);
  momento.add(posS.cross(motS));
  //println(momento.mag());

  background(0);
  scale(1, -1);
  translate(0, -height);

  pushMatrix();
  translate(roomba.x, roomba.y);
  stroke(255);
  strokeWeight(2);
  pushMatrix();
  translate(posD.x, posD.y);
  line(0, 0, motD.x, motD.y);
  stroke(255, 0, 0);
  strokeWeight(4);
  point(motD.x, motD.y);
  popMatrix();

  stroke(255);
  strokeWeight(2);
  pushMatrix();
  translate(posS.x, posS.y);
  line(0, 0, motS.x, motS.y);
  stroke(255, 0, 0);
  strokeWeight(4);
  point(motS.x, motS.y);
  popMatrix();

  stroke(0, 255, 0);
  strokeWeight(2);
  pushMatrix();
  stroke(0, 120, 255);
  strokeWeight(10);
  point(0, 0);

  stroke(255, 255, 0);
  strokeWeight(4);
  noFill();

  float angle = momento.mag();
  angle = map(angle, 0, 10000, 0, PI/2);
  //println("a: " + angle);
  // Se il momento è negativo (gira in senso orario)
  rotate(PI/2);
  if (momento.z > 0)
    arc(0, 0, width/3, height/3, 0, angle);
  else
    arc(0, 0, width/3, height/3, -angle, 0);
  popMatrix();

  stroke(0, 120, 255);
  strokeWeight(6);
  point(roomba.x, roomba.y);
  popMatrix();

  joystick.draw();
}

void keyPressed() {
  switch(key) {
  case 'a':
  case 'A':
    joystick.set(0, 0);
    break;
  }

  //println("Sx: " + motS.y + " / Dx: " + motD.y);
}
