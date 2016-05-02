import javax.sound.midi.*;
//import arb.soundcipher.*;
PGraphics pg;
PVector location;  // Location of shape
PVector velocity, velocity2, velocity3, velocity4;  // Velocity of shape
PVector acceleration;   // Gravity acts at the shape's acceleration
float x, x0, x1, x2, x3, x4;
float y, y0, y1, y2, y3, y4;
float dx, dx2, dx3, dx4;
float dy, dy2, dy3, dy4;
float bpm;
float newY, newY2, newY3, newY4;
float newX, newX2, newX3, newX4;
float beatLength;
float framesPerBeat;
boolean oscillateX = false;
Conductor conductor = new Conductor();
int meter;
int beat = 1;
int delayCounter= 0;
int delay = 5;
ArrayList<int[]> motionPositions = new ArrayList<int[]>();
FileProcessor fp;

void settings(){
  size(600,650);
}

void setup() {
  fp = new FileProcessor();
  background(0);
  frameRate(60);
  meter = 2;
  bpm = 60;  // this will be obtained dynamically
  beatLength = 60/bpm;
  
  //calculate how many frames will be drawn between each beat
  framesPerBeat = (beatLength*60)- 1 - delay; //beatLength*frameRate
  
  motionPositions = conductor.doConductingMotions(meter);
  x0 = motionPositions.get(0)[0];
  y0 = motionPositions.get(0)[1];
  //println("x0: "+ x0 + " y0: " + y0);
  
  location = new PVector(x0, y0);
  //velocity = new PVector(0,5); //velocities in x y directions
  acceleration = new PVector(0,0);
  
  if(meter == 1){
    x1 = motionPositions.get(1)[0];
    y1 = motionPositions.get(1)[1];
    //println("x1: " + x1 + " y1: " + y1);
  
    dy = y1 - y0;
    newY = dy/framesPerBeat;
    velocity = new PVector(0,newY);
  }
  
  if(meter == 2){
    x1 = motionPositions.get(1)[0];
    y1 = motionPositions.get(1)[1];
    //println("x1: " + x1 + " y1: " + y1);
  
    dy = y1 - y0;
    newY = dy/framesPerBeat;
    velocity = new PVector(0,newY);
    
    x2 = motionPositions.get(2)[0];
    y2 = motionPositions.get(2)[1];
    ////println("x1: " + x1 + " y1: " + y1);
  
    dy2 = y2 - y1;
    newY2 = dy2/framesPerBeat;
    velocity2 = new PVector(0,newY2);
  }
  
  if(meter == 3){
    x1 = motionPositions.get(1)[0];
    y1 = motionPositions.get(1)[1];
    //println("x1: " + x1 + " y1: " + y1);
    
    dx = x1 - x0;
    newX = dx/framesPerBeat;
    
    dy = y1 - y0;
    newY = dy/framesPerBeat;
    velocity = new PVector(newX, newY);
    
    x2 = motionPositions.get(2)[0];
    y2 = motionPositions.get(2)[1];
    //println("x2: " + x2 + " y2: " + y2);
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
  
  if(meter == 4){
    x1 = motionPositions.get(1)[0];
    y1 = motionPositions.get(1)[1];
    //println("x1: " + x1 + " y1: " + y1);
    
    dx = x1 - x0;
    newX = dx/framesPerBeat;
    
    dy = y1 - y0;
    newY = dy/framesPerBeat;
    velocity = new PVector(newX, newY);
    
    x2 = motionPositions.get(2)[0];
    y2 = motionPositions.get(2)[1];
    //println("x2: " + x2 + " y2: " + y2);
    dx2 = x2 - x1;
    newX2 = dx2/framesPerBeat;
    dy2 = y2 - y1;
    newY2 = dy2/framesPerBeat;
    
    velocity2 = new PVector(newX2, newY2);
    
    //go back to original position x0, y0
    x3 = motionPositions.get(3)[0];
    y3 = motionPositions.get(3)[1];
    //println("x3: " + x3 + " y3: " + y3);
    dx3 = x3 - x2;
    newX3 = dx3/framesPerBeat;
    
    dy3 = y3 - y2;
    newY3 = dy3/framesPerBeat;
    velocity3 = new PVector(newX3, 0);
    
    //go back to original position x0, y0
    x4 = motionPositions.get(0)[0];
    y4 = motionPositions.get(0)[1];
    //println("x4: " + x4 + " y4: " + y4);
    dx4 = x0 - x3;
    newX4 = dx4/framesPerBeat;
    
    dy4 = y0 - y3;
    newY4 = dy4/framesPerBeat;
    velocity4 = new PVector(newX4, newY4);
  }
  try {
    File midiFile = new File(dataPath("C:\\Users\\Nadya\\OneDrive\\school-stuff\\CSC498\\CSC498-MusicXML\\src\\start\\graphics\\ConductingMotions\\ActorPreludeMidi.mid"));
    Sequencer sequencer = MidiSystem.getSequencer();
    sequencer.open();
    Sequence sequence = MidiSystem.getSequence(midiFile);
    sequencer.setSequence(sequence);
    sequencer.start();
    } catch(Exception e) {
        e.printStackTrace();
        println("Cannot Play MIDI file.");
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
  
  if (meter == 2){
    
    if(beat == 1 && delayCounter == 0){ //3->1
      if(location.y <= y1){
        location.add(velocity);
      } else{
        location.y = y1;
        beat = 2;
        delayCounter = delay;
      }
    } else if(beat == 2 && delayCounter == 0){
        if(location.y >= y2){
          location.add(velocity2);
        } else{
          location.x = x0;
          location.y = y0;
          beat = 1;
          delayCounter = delay;
        }
    }else if(delayCounter > 0){
      delayCounter--;
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
  }
    
    if (meter == 4){
    
      if(beat == 1 && delayCounter == 0){ //3->1
        if(location.y < y1){
          location.add(velocity);
        } else{
          location.x = x1;
          location.y = y1;
          beat = 2;
          delayCounter = delay;
        }
      } else if(beat == 2 && delayCounter == 0){
        if(location.x >= x2 && location.y >= y2){
          location.add(velocity2);
        } else{
          location.x = x2;
          location.y = y2;
          beat = 3;
          delayCounter = delay;
        }
      } else if (beat == 3 && delayCounter== 0){
        if(location.x <= x3 && location.y >= y3){
          location.add(velocity3);
        } else{
          location.x = x3;
          location.y = y3;
          beat = 4;
          delayCounter = delay;
        }
      } else if (beat == 4 && delayCounter== 0){
        if(location.x >= x0 && location.y >= y0){
          location.add(velocity4);
        } else{
          location.x = x0;
          location.y = y0;
          beat = 1;
          delayCounter = delay;
        }
      
      } else if(delayCounter > 0){
      delayCounter--;
      }
    }
    
    //println("beat: " + beat + " " + location.x + ", " + location.y + " " + frameCount + " " + frameRate);

  // Display circle at location vector
  ellipse(location.x,location.y,60,60);
}