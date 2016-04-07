PGraphics pg;

float beginX = 100.0;  // Initial x-coordinate
float beginY = 350.0;  // Initial y-coordinate
float endX = 300.0;   // Final x-coordinate
float endY = 120.0;   // Final y-coordinate
float distX;          // X-axis distance to move
float distY;          // Y-axis distance to move
float exponent = 3;   // Determines the curve
float x = 0.0;        // Current x-coordinate
float y = 0.0;        // Current y-coordinate
float step = 0.01;    // Size of each step along the path
float pct = 0.0;      // Percentage traveled (0.0 to 1.0)

void setup() {
  size(640, 460);
  distX = endX - beginX;
  distY = endY - beginY;
  pg = createGraphics(650, 360);
}

void draw() {
  fill(0, 12);
  rect(0, 0, width, height);
  fill(255);
  noStroke();
  pct += step;
  //if(pct < 2.0){
  //    x = beginX + (pct * distX);
  //    y = beginY + (pow(pct,exponent)*distY);
  //}
  
  //if((pct > 2.0) && (pct < 3.0)){
  //   x = endX - (pct * distX);
  //   y = endY - (pow(pct,exponent)*distY);
  //}
  ellipse(mouseX, mouseY, 60, 60);
  pg.beginDraw();
  //pg.background(51);
  pg.noFill();
  //pg.stroke(255);
  pg.ellipse(mouseX-120, mouseY-60, 60, 60);
  pg.endDraw();
}