//import processing.serial.*;

//Serial myPort;  // Create object from Serial class

float r;
int offset = -100;
float t1,t2,x,y,l1 = 200, l2 = 187.5;
float t1h, t2h;
int t1hi, t2hi;

String st1,st2;

void setup(){
  size(600,600);
  stroke(255);
  
  //String portName = Serial.list()[5]; //change the 0 to a 1 or 2 etc. to match your port
  //myPort = new Serial(this, "COM5", 9600);
}

void draw() {
  background(0);
  
  /**************************************************************************/
  // KINEMATICS
  /**************************************************************************/

  x = -(mouseX)-(width/2+5*offset);
  y = (mouseY)-2*height/3;
  
  // inverse kinematics eqns
  t2 = -acos((sq(x)+sq(y)-sq(l1)-sq(l2))/(2*l1*l2));
  t1 = atan(y/x)-atan((l2*sin(t2))/(l1+l2*cos(t2)));
  
  robot(t1,t2);
  
  /**************************************************************************/
  // COMMUNICATION
  /**************************************************************************/  
  
  
  t1h = map((t1*(180/PI))+25,0,180,0,255);
  t2h = map((t2*(180/PI))+180,0,180,0,255);
  
  t1hi = int(t1h);
  t2hi = int(t1h);
  
  st1 = hex(t1hi);
  st2 = hex(t2hi);
  
  
  //myPort.write("0");
  //myPort.write(st2);
  
  textSize(15);
  
  text("Angle Sent",0,height-20);
  text(st1,100,height-20);
}

// creates robot arm
// takes angles in radians
void robot(float a, float b) {
  // segment 1
  pushMatrix();
  translate(width/2+offset,2*height/3);
  rotate(-a); 
  stroke(0,255,0);
  line(0,0,200,0);
  stroke(255);
  
  // text
  if(mousePressed == true) {
  } else {
  pushMatrix();
  rotate(a);
  text("MATH ANGLE:",20,0);
  text(a*(180/PI),120,0);
  text("SERVO ANGLE:",20,30);
  text(a*(180/PI)+25,120,30);
  popMatrix();
  }
    
    //segment 2
    pushMatrix();
    translate(200,0);
    rotate(-b);
    stroke(0,0,255);
    line(0,0,187.5,0);
    stroke(255);
    
    // end effector
    stroke(255,0,0);
    fill(255,0,0);
    rect(187.5,-5,10,10);
    fill(255);
    stroke(255);
    
    // text
    
    if(keyPressed == true){
    } else {
    pushMatrix();
    rotate(b+(PI/2));
    text("MATH ANGLE:",-150,0);
    text(b*(180/PI),-50,0);
    text("SERVO ANGLE:",-150,30);
    text(b*(180/PI)+180,-50,30);
    popMatrix();
    }
    
  
    popMatrix();

  popMatrix();
  
  // base
  stroke(100);
  fill(100);
  rect(width/2-10+offset,2*height/3,20,162.5);
  line(0,2*height/3+162.5,width,2*height/3+162.5);
  stroke(255);
  fill(255);
}
