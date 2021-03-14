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
double dt = 0.002; // t increments

bool scan = false; // scanning for light source

enum MODE {manual, automatic};
MODE mode = manual;

// Communication
const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars]; // temporary array for use when parsing
// variables to hold the parsed data
char messagePC[numChars] = {0};
int val1PC = 0;
int val2PC = 0;
boolean newData = false;

void setup() {
  servo_z.attach(9, 400, 2250); // servos are not standard so put min and max pulse width
  servo_z.write(z_angle);

  servo_x.attach(10, 300, 2150);
  servo_x.write(x_angle);

  pinMode(pResistor, INPUT);

  Serial.begin(9600);
  Serial.println("Arduino ready");
  Serial.println("Enter data in this style <HelloWorld, 12, 24.7>  ");
}

void loop() {
  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
      // this temporary copy is necessary to protect the original data
      // because strtok() used in parseData() replaces the commas with \0
    parseData();
    //showParsedData();
    applyData();
    newData = false;
  }

  int lightI = analogRead(pResistor); // light intensity
  // plotting values
  /*
  Serial.print(lightI);
  Serial.print(",");
  Serial.println(best_intensity);
  */

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


void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void parseData() {      // split the data into its parts
    char* strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars,",");      // get the first part - the string
    strcpy(messagePC, strtokIndx); // copy it to messageFromPC
 
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    val1PC = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ",");
    val2PC = atoi(strtokIndx);     // convert this part to a float
}

void applyData() {
  if (compareMsg(messagePC, "man")) {
    mode = manual;
    z_angle = val1PC;
    x_angle = val2PC;
    servo_z.write(z_angle);
    servo_x.write(x_angle);
  }
  else if(compareMsg(messagePC, "auto")) {
    t = 0;
    mode = automatic;
    scan = true;
  }
}

bool compareMsg(const char* input, const char* test) {
  if (strlen(input) != strlen(test)) return false;
  for (int i = 0; i < strlen(input); i++) {
    if (input[i] != test[i]) return false;
  }
  return true;
}

void showParsedData() {
    Serial.print("Message ");
    Serial.println(messagePC);
    Serial.print("Val1 ");
    Serial.println(val1PC);
    Serial.print("Val2 ");
    Serial.println(val2PC);
}
