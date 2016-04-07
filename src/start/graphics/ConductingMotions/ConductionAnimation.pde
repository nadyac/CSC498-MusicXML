
PGraphics pg;
PVector location;  // Location of shape
PVector velocity;  // Velocity of shape
PVector acceleration;   // Gravity acts at the shape's acceleration
PVector velocity2;
float xBound;
float yBound;
float x0;
float y0;
float x;
float y;
float dx;
float dy;
float bpm;
float newY;
float beatLength;
float framesPerBeat;
boolean oscillateX = false;
Conductor conductor = new Conductor();
int meter;
ArrayList<int[]> motionPositions = new ArrayList<int[]>();

void settings(){
  size(600,350);
}

void setup() {
  background(0);
  frameRate(60);
  meter = 1;
  bpm = 85;  // this will be obtained dynamically
  beatLength = 60/bpm;
  
  //calculate how many frames will be drawn between each beat
  framesPerBeat = beatLength*60; //beatLength*frameRate
  
  motionPositions = conductor.doConductingMotions(meter);
  x0 = motionPositions.get(0)[0];
  y0 = motionPositions.get(0)[1];
  println(x0 + " " + y0);
  
  location = new PVector(x0, y0);
  velocity = new PVector(0,5); //velocities in x y directions
  acceleration = new PVector(0,0);
  xBound = motionPositions.get(1)[0];
  yBound = motionPositions.get(1)[1];
  println(xBound + " " + yBound);
  
  dy = yBound - y0;
  newY = dy/framesPerBeat;
  velocity2 = new PVector(0,newY);
}

void draw() {
  fill(0, 255); //set back to 12
  rect(0, 0, width, height);
  fill(255);
  noStroke();
  
  // Add velocity to the location.
  //location.add(velocity);
  location.add(velocity2);
  //println(location.y + " " + frameCount + " " + frameRate);
  

  if (location.y > yBound) {
    
    // Reduceg velocity ever so slightly  when it hits the bottom
    location.y = y0;
    location.x = x0;

  }

  // Display circle at location vector
  ellipse(location.x,location.y,60,60);
}