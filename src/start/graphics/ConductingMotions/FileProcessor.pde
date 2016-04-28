
class FileProcessor{
  BufferedReader reader;
  String line, timeSig;
  int[] measureNumber;
  int[] beatsPerMeasure;
  int[] tempos;
  String[] entranceCues, timeSigPieces;
  
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
        tempos = new int[n];
        
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
     
           } else{
          println("skip first line");
            }
        }
      
  } 
}