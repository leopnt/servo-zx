import controlP5.*;
import processing.serial.*;

Serial port; // communication object to arduino
ControlP5 cp5;

void setup() {
  size(300, 450);
  
  printArray(Serial.list()); // prints all available serial ports
  
  port = new Serial(this, "/dev/ttyUSB1", 9600); // put used arduino port here
  
  cp5 = new ControlP5(this); 
  
  cp5.addButton("alloff")
    .setPosition(80, 60)
    .setSize(140, 200)
  ;
  
  cp5.addSlider("slider_z")
    .setPosition(20, 20)
    .setSize(200, 20)
    .setRange(0, 180)
    .setValue(90.0)
    .setTriggerEvent(Slider.RELEASE)
    ;
  
  cp5.addSlider("slider_x")
    .setPosition(20, 60)
    .setSize(20, 200)
    .setRange(0, 180)
    .setValue(90.0)
    .setTriggerEvent(Slider.RELEASE)
    ;
  
  cp5.addButton("manual")
    .setPosition(80, 300)
    .setSize(140, 50)
  ;
  
  cp5.addButton("scan")
    .setPosition(80, 370)
    .setSize(140, 50)
  ;
}

void draw() {
  background(50, 50, 90);
}

void alloff(){
  cp5.getController("slider_z").setValue(90.0);
  cp5.getController("slider_x").setValue(90.0);
  send_pos();
}

void slider_z() {
  send_pos();
}

void slider_x() {
  send_pos();
}

void scan() {
  port.write("-1");
}

void manual() {
  port.write("m");
}

void send_pos() {
  int z = (int) cp5.getController("slider_z").getValue();
  int x = (int) cp5.getController("slider_x").getValue();

  port.write(str(z) + "," + str(x));
}
