PGraphics pg;
PVector location;  // Location of shape
PVector velocity;  // Velocity of shape
PVector gravity;   // Gravity acts at the shape's acceleration
int xBound;
int yBound;
boolean oscillateX = false;
Conductor conductor;


void setup() {
  size(640,360);
  background(0);
  //frameRate(1.41);
  
  location = new PVector(200,100);
  velocity = new PVector(1,2.1); //velocities in x y directions
  gravity = new PVector(0,0.2);
  xBound = 500;
  yBound = 300;
  pg = createGraphics(650, 360);
}

void draw() {
  fill(0, 12);
  rect(0, 0, width, height);
  fill(255);
  noStroke();
  
  // Add velocity to the location.
  location.add(velocity);
  // Add gravity to velocity
  velocity.add(gravity);
  
  // Bounce off edges
  if ((location.x > xBound) || (location.x <= 10)) {
    velocity.x = velocity.x * -0.01;
  }
  if ((location.y > yBound || location.y <= 10)) {
    // We're reducing velocity ever so slightly 
    // when it hits the bottom of the window
    velocity.y = velocity.y * -0.1; 
    
    if(oscillateX == true){
      location.x = 250;
      location.y = 100;
      oscillateX = false;
    } else if(oscillateX == false){
      location.x = 150;
      location.y = 100;
      oscillateX = true;
    }
  }

  // Display circle at location vector
  ellipse(location.x,location.y,60,60);
  pg.beginDraw();
  pg.noFill();
 
  //stroke(255);
  //strokeWeight(2);
  //fill(127);
  //ellipse(location.x,location.y,48,48);
  pg.ellipse(location.x-150,location.y-100,60,60);
  pg.endDraw();
}