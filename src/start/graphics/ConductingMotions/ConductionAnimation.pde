import javax.sound.midi.*;

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
int meter, conductingPattern, tempo, beatPattern, beatsPerMeasure, measureNum;
int beat = 1;
int delayCounter= 0;
int delay = 5;
int measureListSize, measureTracker, beatTracker, changeMeasure;
ArrayList<int[]> motionPositions = new ArrayList<int[]>();
ArrayList<MeasureObject> measureData = new ArrayList<MeasureObject>();
String dynamics, entranceCues;
FileProcessor fp;
MeasureObject m;
PFont measureNumberFont, tempoFont, timeSigFont, dynamicsFont, cuesFont;

void settings(){
  size(650,650);
}

void setup() {
  /* Process music output file */
  fp = new FileProcessor();
  
  /* get all the measure information */
  measureData = fp.getMeasureDataList(); 
  measureListSize = measureData.size();
  measureTracker = 1;
  beatTracker = 1;
  changeMeasure = 0;
     m = measureData.get(measureTracker);
     measureNum = m.getMeasureNumber();
     tempo = m.getTempo();
     conductingPattern = m.getTimeSignaturePattern();
     beatsPerMeasure = m.getTimeSignatureBeats();
     dynamics = m.getDynamics();
     entranceCues = m.getEntranceCues();
     
  /* set initial parameters */
  meter = conductingPattern;
  bpm = tempo; 
  println("bpm: " +bpm);
  beatLength = 60/bpm;
  
  background(0);
  frameRate(60);
  measureNumberFont = createFont("Arial",18,true); // Arial, 16 point, anti-aliasing on
  dynamicsFont = createFont("Arial",18,true); // Arial, 16 point, anti-aliasing on
  cuesFont = createFont("Arial",18,true); // Arial, 16 point, anti-aliasing on
  tempoFont = createFont("Arial",18,true); // Arial, 16 point, anti-aliasing on
  
  /* calculate how many frames will be drawn between each beat*/
  framesPerBeat = (beatLength*60)- 1 - delay; //beatLength*frameRate
  
  motionPositions = conductor.doConductingMotions(meter);
  x0 = motionPositions.get(0)[0];
  y0 = motionPositions.get(0)[1];
  
  location = new PVector(x0, y0);
  acceleration = new PVector(0,0);
  
  if(meter == 1){
    x1 = motionPositions.get(1)[0];
    y1 = motionPositions.get(1)[1];
    dy = y1 - y0;
    newY = dy/framesPerBeat;
    velocity = new PVector(0,newY);
  }
  
  if(meter == 2){
    x1 = motionPositions.get(1)[0];
    y1 = motionPositions.get(1)[1];
    dy = y1 - y0;
    newY = dy/framesPerBeat;
    velocity = new PVector(0,newY);
    
    x2 = motionPositions.get(2)[0];
    y2 = motionPositions.get(2)[1];
    dy2 = y2 - y1;
    newY2 = dy2/framesPerBeat;
    velocity2 = new PVector(0,newY2);
  }
  
  if(meter == 3){
    x1 = motionPositions.get(1)[0];
    y1 = motionPositions.get(1)[1];  
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
    dx2 = x2 - x1;
    newX2 = dx2/framesPerBeat;
    dy2 = y2 - y1;
    newY2 = dy2/framesPerBeat;
    
    velocity2 = new PVector(newX2, newY2);
    
    //go back to original position x0, y0
    x3 = motionPositions.get(3)[0];
    y3 = motionPositions.get(3)[1];
    dx3 = x3 - x2;
    newX3 = dx3/framesPerBeat;
    
    dy3 = y3 - y2;
    newY3 = dy3/framesPerBeat;
    velocity3 = new PVector(newX3, 0);
    
    //go back to original position x0, y0
    x4 = motionPositions.get(0)[0];
    y4 = motionPositions.get(0)[1];
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
/***************************************************************
* Draw the animation at a frequency = frames per second.
****************************************************************/
void draw() {
  
  changeMeasure = 0;
  fill(0,12); //set back to 12
  rect(0, 0, width, height);
  fill(255);
  noStroke();
  textFont(measureNumberFont,32);                  // STEP 3 Specify font to be used
  text("Measure: " + measureNum,15,55);   // STEP 5 Display Text
  
  textFont(dynamicsFont,23);                  // STEP 3 Specify font to be used
  text("Dynamics:\n " + dynamics,15,500);   // STEP 5 Display Text
  
  String newCues = entranceCues.replaceAll(" ",",");
  textFont(dynamicsFont,23);                  // STEP 3 Specify font to be used
  text("Cues:\n " + newCues,15,580);   // STEP 5 Display Text
  
  
  if (meter == 1){
    location.add(velocity);
    
    if (location.y > y1) {
      location.y = y0;
      location.x = x0;
      changeMeasure = 1; /* trigger a measure change*/
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
          //beatTracker = 2; /* sets beatTracker to the last beat of the measure and trigger a measure change*/
        } else{
          location.x = x0;
          location.y = y0;
          beat = 1;
          delayCounter = delay;
          changeMeasure = 1;
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
          beatTracker = 3; /* sets beatTracker to the last beat of the measure and trigger a measure change*/
        } else{
          location.x = x0;
          location.y = y0;
          beat = 1;
          delayCounter = delay;
          changeMeasure = 1;
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
         // beatTracker = 4; /* sets beatTracker to the last beat of the measure and trigger a measure change*/
        } else{
          location.x = x0;
          location.y = y0;
          beat = 1;
          delayCounter = delay;
          changeMeasure = 1;
        }
      
      } else if(delayCounter > 0){
      delayCounter--;
      }
    }
    
    if((changeMeasure == 1) && (measureTracker < measureListSize-1)){
       measureTracker++;
       m = measureData.get(measureTracker);
       measureNum = m.getMeasureNumber();
       tempo = m.getTempo();
       conductingPattern = m.getTimeSignaturePattern();
       beatsPerMeasure = m.getTimeSignatureBeats();
       dynamics = m.getDynamics();
       entranceCues = m.getEntranceCues();
       //m.printMeasureObj();
       println(beatTracker);
       changeMeasure = 0;
    }
    
    //println("beat: " + beat + " " + location.x + ", " + location.y + " " + frameCount + " " + frameRate);

  // Display circle at location vector
  ellipse(location.x,location.y,60,60);
}