#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <Servo.h>


//red - attack motor
//white - left drive
//black - right drive

const uint8_t PWM_out_pin_left_dirve_motor = D2;
const uint8_t PWM_out_pin_right_dirve_motor = D0;
const uint8_t PWM_out_pin_attack_motor = D1;

const String WIFI_SSID = "Zow-Stack destroyer";
const String WIFI_PASS = "12345678";

const IPAddress local_IP(192,168,4,22);
const IPAddress gateway(192,168,4,9);
const IPAddress subnet(255,255,255,0);

const unsigned int LOCAL_UDP_PORT = 8001;
char incomingPacket[1024];  // buffer for incoming packets

WiFiUDP Udp;

Servo leftDrive;
Servo rightDrive;
Servo attackMotor;


void setup() {
  leftDrive.attach(PWM_out_pin_left_dirve_motor);
  rightDrive.attach(PWM_out_pin_right_dirve_motor);
  attackMotor.attach(PWM_out_pin_attack_motor);
  
  Serial.begin(9600);
//  while (!Serial) {
//  }

  Serial.println("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.println("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(WIFI_SSID,WIFI_PASS) ? "Ready" : "Failed!");

  Serial.println("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());

  while (WiFi.softAPgetStationNum()== 0){
  delay(1000);
  Serial.println("Wait for client");
  }

  Serial.printf("Stations connected to soft-AP = %d\n", WiFi.softAPgetStationNum());
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
  Serial.printf("MAC address = %s\n", WiFi.softAPmacAddress().c_str());
  
  Udp.begin(LOCAL_UDP_PORT);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.softAPIP().toString().c_str(), LOCAL_UDP_PORT);
}


void setPowerToMotors(double speedAxis, double turnAxis, double attackAxis){
  double leftDriveOutput;
  double rightDriveOutput;
  double attackMotorOutput;

  leftDriveOutput = speedAxis + turnAxis; // + na - jak cos
  rightDriveOutput = speedAxis - turnAxis; // + na - jak cos
  attackMotorOutput = attackAxis;

  if (leftDriveOutput > 100){
    leftDriveOutput = 100;
  }
  if (rightDriveOutput > 100){
    rightDriveOutput = 100;
  }
  if (attackMotorOutput > 100){
    attackMotorOutput = 100;
  }
}

auto convertToJson(char message[]){
  // Serial.printf("message in convertToJson: %s\n", message);
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, message);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    Serial.printf("Error message: %s\n", message);
  }
  return doc;
}

char* recivePacket(){
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 1024);
    if (len > 0){
      incomingPacket[len] = '\0';
    }
    // Serial.printf("UDP packet contents: %s\n", incomingPacket);

  }
  return incomingPacket;
}

StaticJsonDocument<200> separatedMessage;

char* packet;

double speedAxis;
double turnAxis;
double attackAxis;

double leftSpeed;
double rightSpeed;

void loop() {

  packet = recivePacket();
  if(packet != NULL && packet[0] != '\0' && strcmp(packet, "attack_axis") != 0){
    separatedMessage = convertToJson(packet);
    
    speedAxis = separatedMessage["speed_axis"];
    turnAxis = separatedMessage["turn_axis"];
    attackAxis = separatedMessage["attack_axis"];
  }

  leftSpeed = speedAxis + turnAxis;
  rightSpeed = speedAxis - turnAxis;

  if (abs(leftSpeed) > 1 || abs(rightSpeed) > 1){
    leftSpeed = leftSpeed/2;
    rightSpeed = rightSpeed/2;
  }
  
  Serial.print("leftSpeed: ");
  Serial.println(leftSpeed);
  Serial.print("rightSpeed: ");
  Serial.println(rightSpeed);
  
  rightDrive.write((rightSpeed*90)+90);
  leftDrive.write(-(leftSpeed*90)+90);
  attackMotor.write(((attackAxis+1)*30)+90);
  
}