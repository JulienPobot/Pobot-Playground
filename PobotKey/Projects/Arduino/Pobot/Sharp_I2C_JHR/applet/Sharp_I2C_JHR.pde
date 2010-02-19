#include "Wire.h"

// Device Address
#define DIST_ADDR 0x02

// DIST-Nx Commands
#define DIST_CMD_GP2D12      0x31
#define DIST_CMD_GP2D120     0x32
#define DIST_CMD_GP2YA21     0x33
#define DIST_CMD_GP2YA02     0x34
#define DIST_CMD_CUSTOM      0x35
#define DIST_CMD_ENERGIZED   0x45
#define DIST_CMD_DEENERGIZED 0x44
#define DIST_CMD_ADPA_ON     0x4E
#define DIST_CMD_ADPA_OFF    0x4F

// DIST-Nx Registers
#define DIST_REG_VERSION       0x00
#define DIST_REG_VENDORID      0x08
#define DIST_REG_DEVICEID      0x10
#define DIST_REG_CMD           0x41
#define DIST_REG_DIST_LSB      0x42
#define DIST_REG_DIST_MSB      0x43
#define DIST_REG_VOLT_LSB      0x44
#define DIST_REG_VOLT_MSB      0x45
#define DIST_REG_MODULE_TYPE   0x50
#define DIST_REG_NUM_POINTS    0x51
#define DIST_REG_DIST_MIN_LSB  0x52
#define DIST_REG_DIST_MIN_MSB  0x53
#define DIST_REG_DIST_MAX_LSB  0x54
#define DIST_REG_DIST_MAX_MSB  0x55
#define DIST_REG_VOLT1_LSB     0x56
#define DIST_REG_VOLT1_MSB     0x57
#define DIST_REG_DIST1_LSB     0x58
#define DIST_REG_DIST1_MSB     0x59

int distance = 0;

void setup()
{
  //
  Serial.begin(19200);
  Wire.begin();
  //
  Serial.println("Init Sharp");
  //
  Wire.beginTransmission(DIST_ADDR);
  Wire.send(0x41);
  Wire.send(0x33);
  Wire.endTransmission();
  //
  Wire.beginTransmission(DIST_ADDR);
  Wire.send(DIST_REG_CMD);
  Wire.send(DIST_CMD_ENERGIZED);
  Wire.endTransmission();
  
  Serial.println("Demarrage OK");
  
  // Affichage de l'ID  
  Wire.beginTransmission(DIST_ADDR);
  Wire.send(DIST_REG_DEVICEID);     
  Wire.endTransmission(); 
  delay(200);
  Wire.requestFrom(0x02,2);
  Serial.print("ID: ");
  while (Wire.available())
  {
    Serial.print((int)Wire.receive(),HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  // Affichage du type de module
  Wire.beginTransmission(DIST_ADDR);
  Wire.send(DIST_REG_MODULE_TYPE);
  Wire.endTransmission();     
  delay(200);
  Wire.requestFrom(0x02,2);
  Serial.print("Module: ");
  while (Wire.available())
  {
    Serial.print((int)Wire.receive(),HEX);
    Serial.print(" ");
  }
  Serial.println();

}


void loop()
{
  Serial.print("> LSB: ");   
  Wire.beginTransmission(DIST_ADDR);
  Wire.send(DIST_REG_DIST_LSB);    
  Wire.endTransmission();
  Wire.requestFrom (0x03, 2);  
  Serial.print((int)Wire.receive(),HEX);
  Serial.print(" ");
  Serial.print((int)Wire.receive(),HEX);  
  
  Serial.print("> MSB: ");   
  Wire.beginTransmission(DIST_ADDR);
  Wire.send(DIST_REG_DIST_MSB);    
  Wire.endTransmission();
  Wire.requestFrom (0x03, 2);  
  Serial.print((int)Wire.receive(),HEX);
  Serial.print(" ");
  Serial.print((int)Wire.receive(),HEX);  
  
  Serial.println();
  delay(1000);
}
