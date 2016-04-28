
class FileProcessor{
  BufferedReader reader;
  String line;
  int[] measureNumber;

  
  FileProcessor(){
   //setupFileReader();
   readFile();
  }
  void setupFileReader() {
    // Open the file from the createWriter() example
    reader = createReader("timewise-ActorPreludexml-output.xml");    
  }
  void readFile() {
    //try {
        println("HEYYYYY");
        String[] pieces;
        String[] lines = loadStrings("timewise-ActorPreludexml-output.xml");
        int n = lines.length;
        measureNumber = new int[n];
        println("there are " + n + " lines");
        
        for (int i = 0; i < lines.length; i++){
           pieces = split(lines[i], ",");
           measureNumber[0] = 0;
           if(i > 0){
             println(lines[i]);
             measureNumber[i] = Integer.parseInt(pieces[0]);
             println(pieces[0]); 
           } else{
          println("skip first line");
            }
        }
      
      //if (line == null) {
      //  // Stop reading because of an error or file is empty
      //  println("input file is empty");
      //  noLoop();  
      //} else {
      //  //String[] pieces = split(line, ",");
      //  //String measureNum = pieces[0];
      //  //while(line != null){
      //  //  println(line);
      //  //}
      //}
    //} catch (IOException e) {
    //  e.printStackTrace();
    //  line = null;
    //}
  } 
}