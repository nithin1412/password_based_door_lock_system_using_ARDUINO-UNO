#include<SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#include <SPI.h>
#include <Wire.h> 
// Create instances]
SoftwareSerial SIM900A(3, 4); // SoftwareSerial SIM900A(Rx, Tx)
LiquidCrystal_I2C lcd(0x3F,20,4);
Servo sg90;
constexpr uint8_t servoPin = 9;
constexpr uint8_t buzzerPin = 5;
char initial_password[4] = {'1', '2', '3', '4'};  // Variable to store initial password
char password[4];   // Variable to store users password
boolean NormalMode = true; // boolean to change modes
char key_pressed = 0; // Variable to store incoming keys
uint8_t i = 0;  // Variable used for counter
// defining how many rows and columns our keypad have
const byte rows = 4;
const byte columns = 4;
// Keypad pin map
char hexaKeys[rows][columns] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
// Initializing pins for keypad
byte row_pins[rows] = {A0, A1, A2, A3};
byte column_pins[columns] = {2, 1, 0};
// Create instance for keypad
Keypad keypad_key = Keypad( makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);
void setup() {
  // Arduino Pin configuration
   lcd.init();  
  pinMode(buzzerPin, OUTPUT);
  sg90.attach(servoPin);  //Declare pin 8 for servo
  sg90.write(0); // Set initial position at 0 degrees   // LCD screen
  lcd.backlight();
  SPI.begin(); 
// Init SPI bus
  // Arduino communicates with SIM900A GSM shield at a baud rate of 19200
  // Make sure that corresponds to the baud rate of your module
  SIM900A.begin(19200);
  // AT command to set SIM900A to SMS mode
  SIM900A.print("AT+CMGF=1\r");
  delay(100);
  // Set module to send SMS data to serial out upon receipt
  SIM900A.print("AT+CNMI=2,2,0,0,0\r");
  delay(100);
  lcd.clear(); // Clear LCD screen
}
// Function to send the message
void send_message(String message)
{
  SIM900A.println("AT+CMGF=1");    //Set the GSM Module in Text Mode
  delay(100);
  SIM900A.println("AT+CMGS=\"+919972981769\""); // Replace it with your mobile number
  delay(100);
  SIM900A.println(message);   // The SMS text you want to send
  delay(100);
  SIM900A.println((char)26);  // ASCII code of CTRL+Z
  delay(100);
  SIM900A.println();
  delay(1000);
}
void loop()
{
    lcd.setCursor(0, 0);
      lcd.print("   Door Lock");
      lcd.setCursor(0, 1);
      lcd.print(" enter password");
   key_pressed = keypad_key.getKey(); // Storing keys
      if (key_pressed)
      {
        password[i++] = key_pressed; // Storing in password variable
        lcd.clear();
        lcd.print("*");
      }
      if (i == 4) // If 4 keys are completed
      {
        delay(200);
        if (!(strncmp(password, initial_password, 4))) // If password is matched
        {
          lcd.clear();
          lcd.print("Pass Accepted");
          sg90.write(90); // Door Opened
          send_message("Door Opened \nIf it was't you, type 'close' to close the system.");
          delay(3000);
          sg90.write(0); // Door Closed
          lcd.clear();
          i = 0;
        }
        else    // If password is not matched
        {
          lcd.clear();
          lcd.print("Wrong Password");
          digitalWrite(buzzerPin, HIGH);
          send_message("Someone Tried with the wrong Password \nType 'close' to close the system.");
          delay(3000);
          digitalWrite(buzzerPin, LOW);
          lcd.clear();
          i = 0;
        }
      }
  }
