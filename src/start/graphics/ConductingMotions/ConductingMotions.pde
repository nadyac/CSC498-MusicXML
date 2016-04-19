class Conductor{
  
   int x0;
   int y0;
   // Beat patterns, number of motions can be 1, 2, 3, 4 
   int motions;
   
   // Default constructor, meter set to a default value.
   Conductor(){
     setMeterTo(3);
   }
   
   // Non-Default Constructor
   Conductor(int m){
     setMeterTo(m);
   }
   
   // Set the meter of the beat pattern (# of motions)
   void setMeterTo(int m){
     if((m >= 1) && (m <= 4)){
       motions = m;
     } else {
       motions = 3;
     }
   }
   
   int getMeter(){
     return motions; 
   } 
   
   
   
   // Conducting motions specifications
   ArrayList<int[]> doConductingMotions(int meter){
     ArrayList<int[]> motionLocations = new ArrayList<int[]>();
     
     if(meter == 1){
      //do motions for meter 1 
      int[] position0 = {200,100};
      int[] position1 = {300,300};
      motionLocations.add(position0);
      motionLocations.add(position1);
     }
     
     if(meter == 2){
       //do motions for meter 2
       int[] position0 = {150,150};
       int[] position1 = {200, 300};
       int[] position2 = {350, 300};
       int[] position3 = {370, 250};
       int[] position4 = {350, 265};
       int[] position5 = {150, 150};
       motionLocations.add(position0);
       motionLocations.add(position1);
       motionLocations.add(position2);
       motionLocations.add(position3);
       motionLocations.add(position4);
       motionLocations.add(position5);
     }
     
     if(meter == 3){
      //do motions for meter 3 
       int[] position0 = {200,100};
       int[] position1 = {100, 300};
       int[] position2 = {300, 300};
       motionLocations.add(position0);
       motionLocations.add(position1);
       motionLocations.add(position2);
     }
     
     if(meter == 4){
       //do motions for meter 4
       int[] position0 = {300,170};
       int[] position1 = {300, 350};
       int[] position2 = {210, 300};
       int[] position3 = {390, 300};
       motionLocations.add(position0);
       motionLocations.add(position1);
       motionLocations.add(position2);
       motionLocations.add(position3);
     }
     return motionLocations;
   }
   
}