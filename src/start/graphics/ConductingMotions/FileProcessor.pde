
class FileProcessor{
  BufferedReader reader;
  String line, timeSig, thisMeasure;
  int[] measureNumber;
  int[] beatsPerMeasure, beatPatternInMeasure;
  int[] tempos;
  String[] entranceCues, timeSigPieces, dynamicsPieces, dynamics;
  int cueMeasureNumber;
  ArrayList<int[]> measureData = new ArrayList<int[]>();
  
  FileProcessor(){
   readFile();
  }
  void setupFileReader() {
    // Open the file
    reader = createReader("timewise-ActorPreludexml-output.xml");    
  }
  
  void readFile() {

        String[] pieces;
        String[] lines = loadStrings("timewise-ActorPreludexml-output.xml");
        int n = lines.length;
        measureNumber = new int[n];
        beatsPerMeasure = new int[n];
        beatPatternInMeasure = new int[n];
        tempos = new int[n];
        dynamicsPieces = new String[n];
        dynamics = new String[n]; //get exact size later
        entranceCues = new String[n]; //need to get the size of instruments array
        
        
        for (int i = 0; i < lines.length; i++){
          /*
            Parse each piece of the measure data and store it.
          */
           pieces = split(lines[i], ",");
           measureNumber[0] = 0;
           if(i > 0){
             measureNumber[i] = Integer.parseInt(pieces[0]); //measure number
             /*
               Parse the time signature fraction and store it.
             */
             timeSigPieces = split(pieces[1],"/");
             int len = timeSigPieces.length;
             if(len > 1){
                 beatPatternInMeasure[0] = Integer.parseInt(timeSigPieces[0]); //beat pattern
                 beatsPerMeasure[i] = Integer.parseInt(timeSigPieces[1]); //beats per measure
             }
             /*
               Parse the tempo and store it.
             */
             if(pieces[2] != ""){
               tempos[i] = Integer.parseInt(pieces[2]);
             } else{
               tempos[i] = tempos[i-1]; //if tempo doesn't change, use previously noted tempo; shouldn't be null
             }
             /*
               Get dynamics and store them.
             */
             if(pieces[3] != ""){
               dynamics[i] = pieces[3];
             }
             
             /*
               Get entrance cues and store them
               **** NEED TO BE READJUSTED FOR TWO MEASURES BACK.
             */
             if(i>1){
               cueMeasureNumber = i - 2; 
               if(pieces[4] != ""){
                 entranceCues[i] = pieces[4].replaceAll("_","");
                 //println(entranceCues[i]);
               }
             }else{
               entranceCues[i] = "";
             }
             
           } else{
          println("skip first line");
            }
            
            /**
            * Create the new measure with all the attributes and add to list
            * measure Number, beat pattern, beats per measure, tempo, dynamics, entrance cues
            */
            //String entrances = entranceCues[i].replaceAll("_","");
            MeasureObject currentMeasure = new MeasureObject(measureNumber[i],beatPatternInMeasure[0],beatsPerMeasure[i],tempos[i],dynamics[i],entranceCues[i]);
            thisMeasure = currentMeasure.printMeasureObj();
            println(thisMeasure);
        }
      
  } 
}

class MeasureObject{
  int measureNumber;
  int timeSignaturePattern, timeSignatureBeats;
  int tempo;
  String dynamics;
  String entranceCues;
  
  MeasureObject(int mN, int tSP, int tSB, int t, String dy, String eC){
    this.measureNumber = mN;
    this.timeSignaturePattern = tSP;
    this.timeSignatureBeats = tSB;
    this.tempo = t;
    this.dynamics = dy;
    this.entranceCues = eC;
  }
  
  int getMeasureNumber(){
    return this.measureNumber;
  }
  
  int getTimeSignaturePattern(){
    return this.timeSignaturePattern;
  }
  
  int getTimeSignatureBeats(){
    return this.timeSignatureBeats;
  }
  
  int getTempo(){
    return this.tempo;
  }
  
  String getDynamics(){
    return this.dynamics;
  }
  
  String getEntranceCues(){
    return this.entranceCues;
  }
  
  String printMeasureObj(){
    String str = "";
    str +=this.measureNumber;
    str +=", ";
    str +=this.timeSignaturePattern;
    str +="/";
    str +=this.timeSignatureBeats;
    str +=", ";
    str +=this.tempo;
    str +=", ";
    str +=this.dynamics;
    str +=", ";
    str +=this.entranceCues;
    return str;
  }
}