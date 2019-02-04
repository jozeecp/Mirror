float r;
int offset = -100;
float t1,t2,x,y,l1 = 200, l2 = 175;

void setup(){
  size(600,600);
  stroke(255);
}

void draw() {
  background(0);
  x = -(mouseX)-(width/2+3*offset);
  y = (mouseY)-2*height/3;
  
  // inverse kinematics eqns
  t2 = -acos((sq(x)+sq(y)-sq(l1)-sq(l2))/(2*l1*l2));
  t1 = atan(y/x)-atan((l2*sin(t2))/(l1+l2*cos(t2)));
  
  
  r+=.1;
  robot(t1,t2);
}

void robot(float a, float b) {
  //float c,d;
  //c = a*(PI/180); // convert a to radians
  //d = b*(PI/180); // convert c to radians
  
  // segment 1
  pushMatrix();
  translate(width/2+offset,2*height/3);
  rotate(-a); 
  line(0,0,200,0);
    
    //segment 2
    pushMatrix();
    translate(200,0);
    rotate(-b);
    line(0,0,175,0);
    popMatrix();

  popMatrix();
  
  // base
  rect(width/2-10+offset,2*height/3,20,162.5);
  line(0,2*height/3+162.5,width,2*height/3+162.5);
}
