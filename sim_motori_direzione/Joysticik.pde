class Joystick {
  PVector pos;
  PVector kPos;
  float radius;
  
  Joystick(float x, float y, float r) {
    pos = new PVector(x, y);
    radius = r;
    kPos = new PVector();
  }
  
  void update() {
    kPos.set(mouseX, height - mouseY);
    kPos.sub(pos);
    if(kPos.mag() > radius)
      kPos.setMag(radius);
  }
  
  void draw() {
    pushMatrix();
      translate(pos.x, pos.y);
      noStroke();
      fill(255);
      ellipse(0, 0, radius*2, radius*2);
      fill(150);
      ellipse(kPos.x, kPos.y, radius * 0.3, radius * 0.3);
    popMatrix();
  }
  
  float value() {
    if(kPos.heading() > PI)
      return - kPos.mag() / radius;
    return kPos.mag() / radius;
  }
  
  float angle() {
    return kPos.heading();
  }
  
  void set(float x, float y) {
    kPos.set(x, y);
    if(kPos.mag() > radius)
      kPos.setMag(radius);
  }
  
  float x() {
    return kPos.x / radius;
  }
  
  float y() {
    return kPos.y / radius;
  }
}
