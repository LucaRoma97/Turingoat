
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

int angle = 0;
Servo myServo;
int tmp;

uint8_t uid1[] = { 0xB3, 0x90, 0x58, 0x40, 0, 0, 0, 0 };

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (3)
#define PN532_RESET (2)  // Not connected by default on the NFC Shield

// Or use this line for a breakout or shield with an I2C connection:
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

LiquidCrystal_I2C lcd(0x27, 16, 2);

void InitializeLcd(){
  lcd.clear();
  lcd.print(" Access Control ");
  lcd.setCursor(0, 1);
  lcd.print("Scan Your Card>>");
}

void AccessLcd(){
  lcd.clear();
  lcd.print(" Access Granted>> ");
  lcd.setCursor(0, 1);
  lcd.print(" Welcome! ");
}

void GoodbyeLcd(){
  lcd.clear();
  lcd.print(" Thank you! ");
  lcd.setCursor(0, 1);
  lcd.print(" Goodbye! ");
}

void DeniedLcd(){
  lcd.clear();
  lcd.print(" Access Denied ");
  lcd.setCursor(0, 1);
  lcd.print(" No permissions ");
}


void setup(void) {
  Serial.begin(115200);
  //while (!Serial) delay(10); // for Leonardo/Micro/Zero

  myServo.attach(9);
  lcd.begin();
  lcd.backlight();
  InitializeLcd();

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
 /* if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }*/

  // configure board to read RFID tags
  nfc.SAMConfig();
  
}


void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {

    nfc.PrintHex(uid, uidLength);
    
    if (uidLength == 4)
    {
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	  
	  // Start with block 4 (the first block of sector 1) since sector 0
	  // contains the manufacturer data and it's probably better just
	  // to leave it alone unless you know what you're doing
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
	  
      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data[16];
		
        // If you want to write something to block 4 to test with, uncomment
		// the following line and this text should be read back in a minute
        //memcpy(data, (const uint8_t[]){ 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0 }, sizeof data);
        // success = nfc.mifareclassic_WriteDataBlock (4, data);

        // Try to read the contents of block 4
        success = nfc.mifareclassic_ReadDataBlock(4, data);
		
        if (success)
        {
          // Data seems to have been read ... spit it out
          nfc.PrintHexChar(data, 16);
		  
          // Wait a bit before reading the card again
          delay(1000);
        }

    }
    
    if (uidLength == 7)
    {
      
    }
      uint8_t data[32];
      success = nfc.mifareultralight_ReadPage (4, data);
      if (success)
      {
        // Data seems to have been read ... spit it out
        nfc.PrintHexChar(data, 4);
		
        // Wait a bit before reading the card again
        delay(1000);
      }

    }
  
  if (uid[0] == uid1[0] && tmp==0)
    {
      AccessLcd();
      tmp = 1;
      //delay(200);
      myServo.write(300);
      delay(5000);
      InitializeLcd();
    }

  else if (uid[0] != uid1[0])
    {
      DeniedLcd();
      delay(5000);
      InitializeLcd();
    }

   else if (uid[0] == uid1[0] && tmp == 1)
   {
      GoodbyeLcd();
      //delay(200);
      myServo.write(1300);
      delay(5000);
      tmp = 0;
      InitializeLcd();
   }
   
  }
    
}
