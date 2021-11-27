#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

int PWM_out_pin_left_dirve_motor = 1;
int PWM_out_pin_right_dirve_motor = 2;
int PWM_out_pin_attack_motor = 3;

String WIFI_SSID = "Zow-Stack destroyer";
String WIFI_PASS = "12345678";

IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

unsigned int LOCAL_UDP_PORT = 8001;
char incomingPacket[1024];  // buffer for incoming packets

WiFiUDP Udp;

void setup() {
  pinMode(PWM_out_pin_left_dirve_motor, OUTPUT);
  pinMode(PWM_out_pin_right_dirve_motor, OUTPUT);
  pinMode(PWM_out_pin_attack_motor, OUTPUT);
  
  Serial.begin(115200);
  while (!Serial) {
  }

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

void setPWMOutput(double leftDriveOutput, double rightDriveOutput, double attackMotorOutput){

  int leftDrivePWMOutput = 2.55*leftDriveOutput;
  int rightDrivePWMOutput = 2.55*rightDriveOutput;
  int attackMotorPWMOutput = 2.55*attackMotorOutput + 255;
    
  Serial.println("leftDrivePWMOutput ");
  Serial.println(leftDrivePWMOutput);

  Serial.println("rightDrivePWMOutput ");
  Serial.println(rightDrivePWMOutput);
 
  Serial.println("attackMotorPWMOutput ");
  Serial.println(attackMotorPWMOutput);

  // leftDrivePWMOutpu, rightDrivePWMOutput, attackMotorPWMOutput - gotowe wartośći PWM na śilniki

  analogWrite(PWM_out_pin_left_dirve_motor, leftDrivePWMOutput);
  analogWrite(PWM_out_pin_right_dirve_motor, rightDrivePWMOutput);
  analogWrite(PWM_out_pin_attack_motor, attackMotorPWMOutput);
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

  setPWMOutput(leftDriveOutput, rightDriveOutput, attackMotorOutput);
}

void convertToJson(char message[]){
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, message);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }
  double speedAxis = doc["speed_axis"];
  double turnAxis = doc["turn_axis"];
  double attackAxis = doc["attack_axis"];
  
  setPowerToMotors(speedAxis * 100, turnAxis * 100, attackAxis * 100);
}

void recivePacket(){
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 1024);
    if (len > 0){
      incomingPacket[len] = '\0';
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);
    convertToJson(incomingPacket);
    
  }  
}

void loop() {
  recivePacket();
}
