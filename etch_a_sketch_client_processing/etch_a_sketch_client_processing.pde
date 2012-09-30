import processing.serial.*;

Serial myPort;
boolean firstContact = false;

int startX;
int startY;

void setup()
{
  size(800, 600);
  smooth();

  println(Serial.list());

  String portName = Serial.list()[0];
  myPort = new Serial(this, portName, 9600);
  myPort.bufferUntil('\n');
  
  startX = width/2;
  startY = height/4;
  
}

void draw()
{
  //add paddle animation here 
}

void serialEvent(Serial myPort)
{
  String myString = myPort.readStringUntil('\n');
  if (myString != null)
  {
    myString = trim(myString);
    if (firstContact == false)
    {
      if (myString.equals("hello"))
      {
        myPort.clear();
        firstContact = true;
        myPort.write('A');
        println("first contact");
      } 
    }
    else
    {
      if (myString.equals("l"))
      {
         println("left");
         pushMatrix();
         translate(startX, startY);
         line(0, 0, -20, 0);
         popMatrix();
         startX-=20;
      }
      else if (myString.equals("r"))
      {
         println("right");
         pushMatrix();
         translate(startX, startY);
         line(0, 0, 20, 0);
         popMatrix();
         startX+=20;
      }
      else if (myString.equals("u"))
      {
         println("up");
         pushMatrix();
         translate(startX, startY);
         line(0, 0, 0, -5);
         popMatrix();
         startY-=5;
      }
      else if (myString.equals("d"))
      {
         println("down");
         pushMatrix();
         translate(startX, startY);
         line(0, 0, 0, 5);
         popMatrix();
         startY+=5;
      }
    }
    myPort.write("A");
  } 
}
