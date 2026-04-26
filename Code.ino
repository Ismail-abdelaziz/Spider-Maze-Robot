#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Create PCA9685 servo driver object (default I2C address 0x40)
Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);

// Servo pulse range (maps angle 0 → 180 degrees)
#define SERVO_MIN 150   // corresponds to 0°
#define SERVO_MAX 600   // corresponds to 180°

// Default standing angles for each servo
#define STAND_SERVO0  70
#define STAND_SERVO1  150
#define STAND_SERVO2  180
#define STAND_SERVO3  120
#define STAND_SERVO4  150
#define STAND_SERVO5  40
#define STAND_SERVO6  0
#define STAND_SERVO7  120

// Movement offsets (used to simulate walking)
int backward=25;
int forward=-30;

// Ultrasonic sensor pins
#define trigPin 9
#define echoPin 10

// Variables for distance calculation
long duration;
float distance;


// Convert angle (0–180) to PWM pulse value
int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing PCA9685...");

  // Initialize servo driver
  pca.begin();
  pca.setPWMFreq(60);  // Standard frequency for servos

  // Set robot to standing position at start
  standing();
  delay(1000);

  // Initialize ultrasonic sensor
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT); // trigger pin (send pulse)
  pinMode(echoPin, INPUT);  // echo pin (receive pulse)
}

void loop() {
  
  // Send ultrasonic trigger pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure echo time
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance in cm
  distance = duration * 0.034 / 2;

  // Print distance
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // If obstacle is close → stand
  // Otherwise → move forward
  if(distance<10){
   standing();
  }
  else{
   move_forward();
  }

  delay(100);
}

// Standing position (all legs stable)
void standing(){
  setServoAngle(0, STAND_SERVO0); // front-right thigh
  setServoAngle(1, STAND_SERVO1); // front-right leg
  setServoAngle(2, STAND_SERVO2); // front-left thigh
  setServoAngle(3, STAND_SERVO3); // front-left leg
  setServoAngle(4, STAND_SERVO4); // back-right thigh
  setServoAngle(5, STAND_SERVO5); // back-right leg
  setServoAngle(6, STAND_SERVO6); // back-left thigh
  setServoAngle(7, STAND_SERVO7); // back-left leg

  Serial.println("Standing position set");
}

// Forward walking motion (2 alternating steps)
void move_forward(){
  
  // Step 1
  setServoAngle(0, STAND_SERVO0);
  setServoAngle(1, STAND_SERVO1+backward);
  setServoAngle(2, STAND_SERVO2);
  setServoAngle(3, STAND_SERVO3+forward);
  setServoAngle(4, STAND_SERVO4);
  setServoAngle(5, STAND_SERVO5+forward);
  setServoAngle(6, STAND_SERVO6);
  setServoAngle(7, STAND_SERVO7+backward);

  delay(1000);

  // Step 2 (reverse movement)
  setServoAngle(0, STAND_SERVO0);
  setServoAngle(1, STAND_SERVO1+forward);
  setServoAngle(2, STAND_SERVO2);
  setServoAngle(3, STAND_SERVO3+backward);
  setServoAngle(4, STAND_SERVO4);
  setServoAngle(5, STAND_SERVO5+backward);
  setServoAngle(6, STAND_SERVO6);
  setServoAngle(7, STAND_SERVO7+forward);

  delay(1000);
}

// Move a specific servo to a given angle
void setServoAngle(int channel, int angle) {
  angle = constrain(angle, 0, 180); // safety limit
  int pulse = angleToPulse(angle);  // convert to PWM
  pca.setPWM(channel, 0, pulse);   // send signal
}
