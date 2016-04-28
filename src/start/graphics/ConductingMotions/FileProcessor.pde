
class FileProcessor{
  BufferedReader reader;
  String line, timeSig;
  int[] measureNumber;
  int[] timeSignatures;
  int[] tempos;
  String[] entranceCues, timeSigPieces;
  
  FileProcessor(){
   //setupFileReader();
   readFile();
  }
  void setupFileReader() {
    // Open the file from the createWriter() example
    reader = createReader("timewise-ActorPreludexml-output.xml");    
  }
  void readFile() {

        String[] pieces;
        String[] lines = loadStrings("timewise-ActorPreludexml-output.xml");
        int n = lines.length;
        measureNumber = new int[n];
        
        for (int i = 0; i < lines.length; i++){
          /*
            Parse each piece of the measure data
          */
           pieces = split(lines[i], ",");
           measureNumber[0] = 0;
           if(i > 0){
             measureNumber[i] = Integer.parseInt(pieces[0]);
             /*
               Parse the time signature fraction
             */
               timeSigPieces = split(pieces[1],"/");
               int len = timeSigPieces.length;
               if(len > 1){
                 println(timeSigPieces[1]);
               }
               //if(timeSigPieces[1] != null){
               //  println(timeSigPieces[0]);
               //} else {
               //  println("timeSigPieces is NULL");
               //}
     
           } else{
          println("skip first line");
            }
        }
      
  } 
}