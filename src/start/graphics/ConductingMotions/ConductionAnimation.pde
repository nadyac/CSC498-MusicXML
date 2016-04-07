
PGraphics pg;
PVector location;  // Location of shape
PVector velocity, velocity2, velocity3;  // Velocity of shape
PVector acceleration;   // Gravity acts at the shape's acceleration
float x, x0, x1, x2, x3;
float y, y0, y1, y2, y3;
float dx, dx2, dx3;
float dy, dy2, dy3;
float bpm;
float newY, newY2, newY3;
float newX, newX2, newX3;
float beatLength;
float framesPerBeat;
boolean oscillateX = false;
Conductor conductor = new Conductor();
int meter;
int beat = 1;
int delayCounter= 0;
int delay = 5;
ArrayList<int[]> motionPositions = new ArrayList<int[]>();

void settings(){
  size(600,650);
}

void setup() {
  background(0);
  frameRate(60);
  meter = 3;
  bpm = 60;  // this will be obtained dynamically
  beatLength = 60/bpm;
  
  //calculate how many frames will be drawn between each beat
  framesPerBeat = (beatLength*60)- 1 - delay; //beatLength*frameRate
  
  motionPositions = conductor.doConductingMotions(meter);
  x0 = motionPositions.get(0)[0];
  y0 = motionPositions.get(0)[1];
  println(x0 + " " + y0);
  
  location = new PVector(x0, y0);
  //velocity = new PVector(0,5); //velocities in x y directions
  acceleration = new PVector(0,0);
  
  if(meter == 1){
    x1 = motionPositions.get(1)[0];
    y1 = motionPositions.get(1)[1];
    println(x1 + " " + y1);
  
    dy = y1 - y0;
    newY = dy/framesPerBeat;
    velocity = new PVector(0,newY);
  }
  
  if(meter == 3){
    x1 = motionPositions.get(1)[0];
    y1 = motionPositions.get(1)[1];
    println("x1: " + x1 + " y1: " + y1);
    
    dx = x1 - x0;
    newX = dx/framesPerBeat;
    
    dy = y1 - y0;
    newY = dy/framesPerBeat;
    velocity = new PVector(newX, newY);
    
    x2 = motionPositions.get(2)[0];
    y2 = motionPositions.get(2)[1];
    println("x2: " + x2 + " y2: " + y2);
    dx2 = x2 - x1;
    newX2 = dx2/framesPerBeat;
    
    velocity2 = new PVector(newX2, 0);
    
    //go back to original position x0, y0
    dx3 = x0 - x2;
    newX3 = dx3/framesPerBeat;
    
    dy3 = y0 - y2;
    newY3 = dy3/framesPerBeat;
    velocity3 = new PVector(newX3, newY3);
  }
}

void draw() {
  fill(0, 255); //set back to 12
  rect(0, 0, width, height);
  fill(255);
  noStroke();
  
  
  if (meter == 1){
    
    // Add velocity to the location.
    location.add(velocity);
    //println(location.y + " " + frameCount + " " + frameRate);
  
    if (location.y > y1) {
      // Reduce velocity ever so slightly  when it hits the bottom
      location.y = y0;
      location.x = x0;
  
    }
  }
  
  if (meter == 3){
    
    if(beat == 1 && delayCounter == 0){ //3->1
      if(location.x > x1 && location.y < y1){
        location.add(velocity);
      } else{
        location.x = x1;
        location.y = y1;
        beat = 2;
        delayCounter = delay;
      }
    } else if(beat == 2 && delayCounter == 0){
        if(location.x <= x2 && location.y <= y2){
          location.add(velocity2);
        } else{
          location.x = x2;
          location.y = y2;
          beat = 3;
          delayCounter = delay;
        }
    } else if (beat == 3 && delayCounter== 0){
        if(location.x >= x0 && location.y >= y0){
          location.add(velocity3);
        } else{
          location.x = x0;
          location.y = y0;
          beat = 1;
          delayCounter = delay;
        }
    } else if(delayCounter > 0){
      delayCounter--;
    }
    
    println("beat: " + beat + " " + location.x + ", " + location.y + " " + frameCount + " " + frameRate);
  }

  // Display circle at location vector
  ellipse(location.x,location.y,60,60);
}