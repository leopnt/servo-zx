#include <Servo.h>

const int pResistor = A0;

Servo servo_z;
Servo servo_x;
int z_angle = 90; // in degrees
int x_angle = 90;

int z_best = 90; // best angle at which the light is the most powerfull
int x_best = 90;

int best_intensity = 0;

double t = 0.0; // to control sin functions
double dt = 0.02; // t increments

bool paused = false; // pause the automatic movement
bool scan = false; // scanning for light source

enum MODE {manual, automatic};
MODE mode = manual;

void setup() {
  servo_z.attach(9, 400, 2250); // servos are not standard so put min and max pulse width
  servo_z.write(z_angle);

  servo_x.attach(10, 300, 2150);
  servo_x.write(x_angle);

  pinMode(pResistor, INPUT);

  Serial.begin(9600);
  Serial.println("Arduino is ready");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    if (mode == manual) {
      z_angle = Serial.parseInt();
      if (z_angle == -1) {
        // special code to return to automatic
        mode = automatic;
        scan = true;
        t = 0.0;
        return;
      }
      x_angle = Serial.parseInt();
      servo_z.write(z_angle);
      servo_x.write(x_angle);
    }
    else if (mode == automatic) {
      char input = Serial.read();
      if (input == 'p') {
        paused = !paused;
      }
      if (input == 'm') {
        mode = manual;
      }
    }
  }

  // plotting values
  int lightI = analogRead(pResistor); // light intensity
  Serial.print(lightI);
  Serial.print(",");
  Serial.println(best_intensity);

  if (mode == automatic && scan) {
    fetch_best_intensity(lightI);

    // scan movement
    z_angle = 90 + 90 * sin(0.05*t + PI/2);
    servo_z.write(z_angle);
    x_angle = 90 + 45 * sin(1.5*t);
    servo_x.write(x_angle);

    if (is_scan_finished()) {
      on_scan_finished();
      mode = manual;
    }
  }
  
  if (!paused) t += dt;
  if (t >= 32 * TWO_PI) t = 0.0;
}

void fetch_best_intensity(int light_intensity) {
  if (light_intensity > best_intensity) {
      best_intensity = light_intensity;
      z_best = servo_z.read();
      x_best = servo_x.read();
    }
}

bool is_scan_finished() {
  if (z_angle == 0) return true;
  return false;
}

void on_scan_finished() {
  z_angle = z_best;
  x_angle = x_best;
  servo_z.write(z_angle);
  servo_x.write(x_angle);
  scan = false;

  // reset old values
  z_best = 0;
  x_best = 0;
  best_intensity = 0;
}
