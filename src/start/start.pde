import interfascia.*;

GUIController c;
IFTextField t;
IFButton inputBtn, cancelBtn;
IFLabel l;
PFrame f;

String tempo;
String beats;
int index = 0;

void setup() {
  size(250, 200);
  background(200);
  
  c = new GUIController(this);
  t = new IFTextField("Text Field", 25, 30, 150);
  inputBtn = new IFButton ("Select Input File", 10, 70, 100, 40);
  cancelBtn = new IFButton ("Cancel", 150, 70, 60, 40);
  l = new IFLabel("", 25, 70);
  
  inputBtn.addActionListener(this);
  cancelBtn.addActionListener(this);
  
  c.add(t);
  c.add(l);
  c.add(inputBtn);
  c.add(cancelBtn);
  
  t.addActionListener(this);
}

void draw() {
  background(200);
}

void actionPerformed(GUIEvent e) {
  if (e.getMessage().equals("Completed")) {
    l.setLabel(t.getValue());
    PFrame f = new PFrame();
  }
  if (e.getSource() == inputBtn) {
    /**
     *Trigger functions to select and read file 
    */
    selectInput("Select a file to process:", "fileSelected");
    
  } else if (e.getSource() == cancelBtn) {
    background(100, 100, 130);
  }
}

void fileSelected(File selection) {
  if (selection == null) {
    println("Window was closed or the user hit cancel.");
  } else {
    println("User selected " + selection.getAbsolutePath());
    t.setValue(selection.getName()); //side effect
    readFile(selection.getName()); //major side effect
  }
}

void readFile(String fname){
  
  println(fname);
  String lines[] = loadStrings(fname);
  println("there are " + lines.length + " lines");
  for (int i = 0 ; i < lines.length; i++) {
    println(lines[i]);
  }
}

/**
* Open new window when the user selects file.
*/
public class PFrame {
    public PFrame() {

    }
}