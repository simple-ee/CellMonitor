#include <Adafruit_SSD1306.h>

///////////////////////////////////////////////////////////////////////////
// Date: 2019/12/04
// By: Steven Guzman
// Hardware: Atmega328PB
// Bootloader: minicore 
//             atmega328PB
//             16MHz Internal clock
//             UART0
//  Description: Arduino based up to 8S lithium cell monitor.  Will monitor
//               voltage levels, ability to adjust min and max alarm for
//               individual cells, and control relays
// Resources: 
// 
////////////////////////////////////////////////////////////////////////////

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define ADDR0 0x40 // I2C address of ADC 1
#define ADDR1 0x41 // I2C address of ADC 2
int address[2] = {ADDR0,ADDR1}; // Int array to store I2C address of ADCs

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#define RESET 0x06     // Command for resetting the ADCs
#define START 0x08     // Command for starting the conversino of ADC channel
#define POWERDOWN 0x02 // Command to powerdown the ADC after measurement
#define WRITENOW 0x40  // Command to point to write register
#define RDATA 0x10     // Command to read the ADC channel
#define RREG 0x20      // Command to read back config value

#define AIN0 0X60 // CH0, GAIN 1, 90SPS, SINGLE-SHOT, 2.048V REF
#define AIN1 0X80 // CH1, GAIN 1, 90SPS, SINGLE-SHOT, 2.048V REF
#define AIN2 0XA0 // CH2, GAIN 1, 90SPS, SINGLE-SHOT, 2.048V REF
#define AIN3 0XC0 // CH3, GAIN 1, 90SPS, SINGLE-SHOT, 2.048V REF
byte CH[8] = {AIN0,AIN1,AIN2,AIN3,AIN0,AIN1,AIN2,AIN3}; // Array for channel filter

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#define READY0 12
#define READY1 11
int READY[2] = {READY0, READY1};

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/* Arrays values used for storing cell values and calibration values */

float adc[8] = {0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000};
float cell[8] = {0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000};
float bat[8] = {0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000};
float cal[8] = {0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000};
String menu[7] = {"Cell UVP","Cell OVP","Pack UVP","Pack OVP","Buzzer","# Cells","Exit"};
/* 
1. values[0] = Undervoltage for individual cell value
2. values[1] = Overvoltage for individual cell value
3. values[2] = Undervoltage for pack value
4. values[3] = Overvoltage for pack value
*/
float values[8] = {2.500,3.650,20.000,29.200,0.000,0.000,0.000,0.000};
int valuesInt[2] = {0,8};

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

uint16_t ReadValue[8]; // 16-Bit array to store adc value after conversion
byte tempValue[2];     // Used to convert two 8-bit values to 1 16-bit value

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/* Fixed array values */

const int numOfInputs = 4; // used for setting the number of inputs pins
const int inputPins[numOfInputs] = {5,6,7,8}; // Placing input pins in array
int inputState[numOfInputs]; // Array for number of inputs
int UVPcell[8] = {0,0,0,0,0,0,0,0}; // Array for indicating which cell is UV
int OVPcell[8] = {0,0,0,0,0,0,0,0}; // Array for indicating which cell is OV

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/* Fixed variables */

int samples = 100;

int menuHit = 0;
int upArrow = 0;
int downArrow = 0;
int okButton = 0;
int i = 0;
int currentScreen = 0;
int x = 0;
int y = 0;
int temp = 0;
int resetD = 0;
int resetU = 0;
int resetP = 0;
int countUVP = 0;
int countOVP = 0;

float Ratio = 0.0000625;
float scale = 0.068;


void setup()
{
    Serial.begin(9600);
    Wire.begin();

    pinMode(READY0,INPUT_PULLUP); // configure pull up resistor
    pinMode(READY1,INPUT_PULLUP); // configure pull up resistor
    pinMode(2,OUTPUT); // Configured digital pin 2 as output for relay control

    /* By default, we'll generate the high voltage from 3.3V line internally! */
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Initialize with the I2C addr 0x3C
                                              // (for the 128x64)
    /////////////////////////////////////////////////////////////////////////////////
    /* Show image buffer on the display hardware. Since the buffer is intialized 
       with an adafruit splashscreen internally, this will display the 
       splashscreen 
    */

    display.display();
    delay(500);
    display.clearDisplay(); // This clears the buffer

    /* Configure buttons as input and low using for loop */
    for (int i = 0; i < numOfInputs; i++)
    {
        pinMode(inputPins[i], INPUT);
        digitalWrite(inputPins[i], LOW);
    }

    digitalWrite(2,LOW); // Set relay control pin as high output

    /////////////////////////////////////////////////////////////////////////////////
    /* Writing to configuration address for ADC0 and ADC1 and then reading back the
       from the read register to make sure there is proper communication between
       MCU and both ADCs 
    */

    for (int i = 0; i < 2; i++)
    {
        Wire.beginTransmission(address[i]); // Start communication with adc
        Wire.write(RESET);                  // Reset command
        Wire.endTransmission();             // End transmission

        Wire.beginTransmission(address[i]); // Start communication with adc
        Wire.write(WRITENOW);
        Wire.write(AIN1);
        Wire.endTransmission();

        Wire.beginTransmission(address[i]); // Start communication with adc
        Wire.write(RREG);
        Wire.endTransmission();

        Wire.requestFrom(address[i],1); // Request one byte from adc
        int default01 = Wire.read();    // Save to temp variable
        
        Serial.print("Default address: ");
        Serial.print(address[i],HEX);
        Serial.print(", ");
        Serial.println(default01,HEX);
        delay(1000);
    }

    EEPROM.get(0,values[0]);    // Cell UVP variable
    EEPROM.get(4,values[1]);    // Cell OVP variable
    EEPROM.get(8,values[2]);    // Pack UVP variable
    EEPROM.get(12,values[3]);   // Pack OVP variable

    EEPROM.get(16,valuesInt[0]);    // Buzzer on/off variable
    EEPROM.get(20,valuesInt[1]);    // # of cells variable

    for (int t = 0; t<8; t++)
    {
        EEPROM.get((t*4)+100, cal[t]);
        Serial.print("cal ");
        Serial.print((t*4)+100);
        Serial.print(": ");
        Serial.println(cal[t]);
    }
}

void loop()
{
    menuScreen();
    DisplayBat();
    ButtonMenu();
    
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/* Function used for incrementing integer variable with push buttons */
void ButtonInt(int t)
{
    inputState[0] = digitalRead(inputPins[0]);
    inputState[2] = digitalRead(inputPins[2]);
    inputState[3] = digitalRead(inputPins[3]);

    if (inputState[0] == 1)
    {
        valuesInt[t] = valuesInt[t] + 1;
    }
    if (inputState[3] == 1)
    {
        valuesInt[t] = valuesInt[t] - 1;
    }
    if (inputState[2] == 1)
    {
        currentScreen = 0;
        EEPROM.put(((t+4)*4), valuesInt[t]); // Store current value state to eeprom
    }
    delay(75);  // Delay is to help with push button response
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/* Function used for incrementing float variable with push buttons */

void ButtonFloat(int s)
{
    inputState[0] = digitalRead(inputPins[0]);
    inputState[2] = digitalRead(inputPins[2]);
    inputState[3] = digitalRead(inputPins[3]);

    if (inputState[0] == 1)
    {
        values[s] = values[s] + 0.05;
    }
    if (inputState[3] == 1)
    {
        values[s] = values[s] - 0.05;
    }
    if (inputState[2] == 1)
    {
        currentScreen = 0;
        EEPROM.put((s*4), values[s]);
        Serial.print("Location: ");
        Serial.println(s);
    }
    delay(75);  // Delay is to help with push button response
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/* Function used for entering into the menu screen */

void ButtonMenu()
{
    inputState[1] = digitalRead(inputPins[1]);
    delay(10);
    if (inputState[1])
    {
        menuHit = 1;
        delay(10);
    }
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/* Function used for Displaying battery status levels */

void DisplayBat()
{
    if (menuHit == 0)
    {
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.print("V1:");
        display.print(bat[0],3);
        display.println("V");
        display.print("V2:");
        display.print(bat[1],3);
        display.println("V");
        display.print("V3:");
        display.print(bat[2],3);
        display.println("V");
        display.print("V4:");
        display.print(bat[3],3);
        display.println("V");

        display.setCursor(72,0);
        display.print("V5:");
        display.print(bat[4],3);
        display.println("V");

        display.setCursor(72,8);
        display.print("V6:");
        display.print(bat[5],3);
        display.println("V");

        display.setCursor(72,16);
        display.print("V7:");
        display.print(bat[6],3);
        display.println("V");

        display.setCursor(72,24);
        display.print("V8:");
        display.print(bat[7],3);
        display.println("V");
        Serial.print("V8: ");
     

        display.display();
        delay(10);
        display.clearDisplay();

        for (int i = 0; i < 8; i++)
        {
            if (i < 4)
            {
                ReadADC(i,0);
                cell[i] = (ReadValue[i] * Ratio)/scale;
               
            }
            if (i >= 4)
            {
                ReadADC(i,1);
                cell[i] = (ReadValue[i] * Ratio)/scale;
            }
        }

        for (int b = 0; b < 8; b++)
        {
	        if (b == 0)
	        {
		        bat[b] = cell[b] + cal[b];
	        }

            if (b > 0)
            {
                bat[b] = (cell[b] - cell[b-1]) + cal[b];
            }
            /////////////////////////////////////////////////////////////////////////
            // Checks to see if any of the cells are undervoltage and stores the 
            // the value in a counter 
            /////////////////////////////////////////////////////////////////////////
            if (bat[b] < values[0])
            {
                countUVP = countUVP + 1;
                UVPcell[b] = 1;
            }
            else if (bat[b] > values[0])
            {
                UVPcell[b] = 0;
            }
            /////////////////////////////////////////////////////////////////////////
            // Checks to see if any of the cells are overvoltage and stores the 
            // the value in a counter 
            /////////////////////////////////////////////////////////////////////////
            if (bat[b] > values[1])
            {
                countOVP = countOVP + 1;
                OVPcell[b] = 1;
            }
            else if (bat[b] < values[1])
            {
                OVPcell[b] = 0;
            }
        }
        /////////////////////////////////////////////////////////////////////////////
        // Sets digital pin low if any cell is undervoltage and then sets the reset
        // variable to 1 and can only be reset via serial monitor
        /////////////////////////////////////////////////////////////////////////////
        if (countUVP >= 1 && resetP == 0)
        {
            /////////////////////////////////////////////////////////////////////////
            // If buzzer is set to 1 (on) then it will trigger the buzzer
            /////////////////////////////////////////////////////////////////////////
            if (valuesInt[0] == 1)
            {
              digitalWrite(2,HIGH);
            }
           
            countUVP = 0;
            resetP = 1;

            for (int t = 0; t < 8; t++)
            {
                if (UVPcell[t] == 1)
                {
                    Serial.print("Cell ");
                    Serial.print(t+1);
                    Serial.print(" is under ");
                    Serial.println(values[0],3);
                }
            }
        }
        /////////////////////////////////////////////////////////////////////////////
        // Restores default settings when fault condition is removed and reset value
        // is set back to 0
        /////////////////////////////////////////////////////////////////////////////
        else if (countUVP == 0 && resetP == 0)
        {
            digitalWrite(2,LOW);
            countUVP = 0;
        }
        /////////////////////////////////////////////////////////////////////////////
        // Sets digital pin low if any cell is overvoltage and then sets the reset
        // variable to 1 and can only be reset via serial monitor
        /////////////////////////////////////////////////////////////////////////////
        if (countOVP >=1 && resetP)
        {
            /////////////////////////////////////////////////////////////////////////
            // If buzzer is set to 1 (on) then it will trigger the buzzer
            /////////////////////////////////////////////////////////////////////////
            if (valuesInt[0] == 1)
            {
              digitalWrite(2,HIGH);
            }
            countOVP = 0;
            resetP = 1;

            for (int t = 0; t<8; t++)
            {
                if (OVPcell[t] == 1)
                {
                    Serial.print("Cell ");
                    Serial.print(t+1);
                    Serial.print(" is over ");
                    Serial.println(values[1],3);
                }
            }
        }
        else if (countOVP == 0 && resetP == 0)
        {
            digitalWrite(2,LOW);
            countOVP = 0;
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/* Function used for Displaying and moving the menu screen */
void menuScreen()
{
    if (menuHit == 1)
    {
        switch (currentScreen)
        {
            case 0:
                menuSign(); // Displays a quick menu splashscreen

                display.setTextSize(1);
                display.setTextColor(WHITE);
                display.setCursor(x,y);
                display.print(">");
                display.setCursor(5,0);
                display.println(menu[0]);
                display.setCursor(5,8);
                display.println(menu[1]);
                display.setCursor(5,16);
                display.println(menu[2]);
                display.setCursor(5,24);
                display.println(menu[3]);
                display.setCursor(75,0);
                display.println(menu[4]);
                display.setCursor(75,8);
                display.println(menu[5]);
                display.setCursor(75,16);
                display.println(menu[6]);
                display.display();
                delay(10);
                display.clearDisplay();


                inputState[0] = digitalRead(inputPins[0]);
                inputState[2] = digitalRead(inputPins[2]);
                inputState[3] = digitalRead(inputPins[3]);
                delay(75);

                if (inputState[3] == 1)
                {
                    i++;
                    y = y + 8;

                    if ( i > 3 && i < 6 && resetD == 0)
                    {
                        x = 70;
                        y = 0;
                        resetD = 1;
                    }
                    else if (i > 6)
                    {
                        i = 0;
                        x = 0;
                        y = 0;
                        resetD = 0;
                    }
                    Serial.println(i);
                }

                else if ( inputState[0] == 1)
                {
                    i--;
                    y = y - 8;
          
                    if (i < 4 && i > 0 && resetU == 0)
                    {
                        x = 0;
                        y = 24;
                        resetU = 1;
                    }
                    else if ( i < 0)
                    {
                        i = 6;
                        x = 70;
                        y = 16;
                        resetU = 0;
                    }
                    Serial.println(i);
                }
                else if (inputState[2] == 1)
                {
                    currentScreen = i + 1;
                }
                break;

            case 1:
                submenu(i);
                ButtonFloat(i);
                break;

            case 2:
                submenu(i);
                ButtonFloat(i);
                break;

            case 3:
                submenu(i);
                ButtonFloat(i);
                break;

            case 4:
                submenu(i);
                ButtonFloat(i);
                break;

            case 5:
                submenu(i);
                ButtonInt(0);
                break;

            case 6:
                submenu(i);
                ButtonInt(1);
                break;

            case 7:
                menuHit = 0;
                delay(10);
                currentScreen = 0;
                i = 0;
                x = 0;
                y = 0;
                temp = 0;
                break;
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/* Function used for changing to the submenu */
void submenu(int v)
{
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println(menu[v]);
    if ( v == 4 || v ==5)
    {
        display.print(valuesInt[v-4]);
    }
    else
    {
        display.print(values[v]);
    }
    display.display();
    delay(10);
    display.clearDisplay();
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/* Function used for reading adc */
void ReadADC( int x, int y)
{
    delay(100);
    Wire.beginTransmission(address[y]);
    Wire.write(WRITENOW);
    Wire.write(CH[x]);
    Wire.endTransmission();

    Wire.beginTransmission(address[y]);
    Wire.write(START);
    Wire.endTransmission();
    delay(60);

    if (digitalRead(READY[y] == 0))
    {
        Wire.beginTransmission(address[y]);
        Wire.write(RDATA);
        Wire.endTransmission();

        Wire.requestFrom(address[y],2);
        while(Wire.available())
        {
            tempValue[0] = Wire.read();
            tempValue[1] = Wire.read();
        }

        Wire.beginTransmission(address[y]);
        Wire.write(POWERDOWN);
        Wire.endTransmission();

        ReadValue[x] = tempValue[0] << 8 | tempValue[1];
  }
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/* Function used for splashing of menu */
void menuSign()
{
    if (temp == 0)
    {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(35,10);
        display.print("MENU");
        display.display();
        delay(1000);
        display.clearDisplay();
        temp = 1;
    }
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/* Function used for serial event condition used to send commands to controller
   via serial command */
void serialEvent()
{
    while (Serial.available())
    {
        int mode = Serial.parseInt();

        /////////////////////////////////////////////////////////////////////////////
        // Send: 1001 in order to send default values to eeprom for uvp,ovp,pack uvp,
        //       pack ovp, buzzer on/off, and # of cells
        /////////////////////////////////////////////////////////////////////////////
        if (mode == 1001)
        {
            EEPROM.put(0,2.50);     // cell UVP
            EEPROM.put(4,3.65);     // cell OVP
            EEPROM.put(8,20.00);    // pack UVP
            EEPROM.put(12,29.20);   // pack OVP
            EEPROM.put(16,0);       // buzzer on/off
            EEPROM.put(20,0);       // # of cells
      
            EEPROM.get(0,values[0]);
            EEPROM.get(4,values[1]);
            EEPROM.get(8,values[2]);
            EEPROM.get(12,values[3]);
            EEPROM.get(16,valuesInt[0]);
            EEPROM.get(20,valuesInt[1]);

            Serial.print("Values are reset");
        }
        if (mode == 1002)
        {
            float vref[8];
            vref[0] = Serial.parseFloat();
            vref[1] = Serial.parseFloat();
            vref[2] = Serial.parseFloat();
            vref[3] = Serial.parseFloat();
            vref[4] = Serial.parseFloat();
            vref[5] = Serial.parseFloat();
            vref[6] = Serial.parseFloat();
            vref[7] = Serial.parseFloat();

            delay(100);
      
            for (int i = 0; i < 8; i++)
            {
                if (i < 4)
                {
                    ReadADC(i,0);
                    cell[i] = (ReadValue[i] * Ratio)/scale;
                }
                if (i >= 4)
                {
                    ReadADC(i,1);
                    cell[i] = (ReadValue[i] * Ratio)/scale;
                }
            }

            bat[0] = cell[0];
            bat[1] = (cell[1] - cell[0]);
            bat[2] = (cell[2] - cell[1]);
            bat[3] = (cell[3] - cell[2]);
            bat[4] = (cell[4] - cell[3]);
            bat[5] = (cell[5] - cell[4]);
            bat[6] = (cell[6] - cell[5]);
            bat[7] = (cell[7] - cell[6]);
      
            for (int temp = 0; temp < 8; temp++)
            {
                cal[temp] = vref[temp] - bat[temp];
                Serial.print("Cell: ");
                Serial.print(temp+1);
                Serial.print(", Cal Value: ");
                EEPROM.put((temp*4)+100,cal[temp]);
                EEPROM.get((temp*4)+100,cal[temp]);
                Serial.println(cal[temp]);
            }
      
        }
        /////////////////////////////////////////////////////////////////////////////
        // Send: 1003
        // This will reset any fault condition that occured like OVP or UVP
        /////////////////////////////////////////////////////////////////////////////
        if (mode == 1003)
        {
            resetP = 0;
            Serial.println("Protection has been reset");
        }
    } 
}
