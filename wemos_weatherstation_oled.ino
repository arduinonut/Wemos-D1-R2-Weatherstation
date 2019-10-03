
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <SNTPtime.h>
#include <FS.h>

SNTPtime NTPch("ch.pool.ntp.org");
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN 14     // Digital pin connected to the DHT sensor
const byte RAIN = 12;                 //Rain meter
const byte wSpeed = 13;                 //Anemometer
const byte wDirec = A0;

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);
strDateTime dateTime;
 #define ssid "//////////////////////////"
 #define password "////////////////////////"
long lastWindChk = 0;
float rain;
volatile int tip;
float lastWindIRQ;
float hum;
float lowhum;
float highhum;
float temp = 0;
float hightemp = 0;
float  lowtemp = 50;
float windSpeed;
volatile int windClickNo;
String windRigting;
float winddir;
float Direction;
int lasthour;
int midnight;
int minutes;
int seconds; 
byte actualHour = dateTime.hour;
        byte actualMinute = dateTime.minute;
        byte actualsecond = dateTime.second;
        int actualyear = dateTime.year;
        byte actualMonth = dateTime.month;
        byte actualday = dateTime.day;
        byte actualdayofWeek = dateTime.dayofWeek;
void filewrite()
 {
   File file = SPIFFS.open("/datalog.txt", "w+");
 file.print (actualyear);
 file.print("/");
 file.print(actualMonth);
 file.print("/");
 file.print(actualday);
 file.print(",");
 file.print(actualHour);
 file.print(":");
 file.print(actualMinute);
 file.print(",");
 file.print(temp);
 file.print(",");
 file.print(hightemp);
 file.print(",");
 file.print(lowtemp);
 file.print(",");
 file.print(hum);
 file.print(",");
 file.print(rain);
 file.println("");
  file.close();
 }


void resetDay()
{
  
    filewrite();
    temp=0;
    hightemp=0;
    lowtemp=0;
    hum=0;
    rain=0;
    
 
}
void tempcalc()
{
  if (temp>hightemp)
  {
  hightemp==temp;
  }
  if (temp<lowtemp)
  {
  lowtemp==temp;
  }
  
}



void winDir()
{
  winddir = analogRead(wDirec);

  Direction = map(winddir, 80, 950, 0, 359);
}

void calC()
{
  rain =  ((tip/2) * 0.27); 
  // Get total rainfall (value can be found on rainguage)
  if (hightemp < temp)          // Calculate min$max temps and humidity
  {
    hightemp = temp;
  }
  if (lowtemp > temp)
  {
    lowtemp = temp;
  }
  if (lowhum > hum)
  {
    lowhum = hum;
  }
  if (highhum < hum)
  {
    highhum = hum;
  }
 
}

void ICACHE_RAM_ATTR rainIRQ()
{
 delayMicroseconds (18000); 
 delayMicroseconds (18000); 
  delayMicroseconds (18000); 
  delayMicroseconds (18000); 
  delayMicroseconds (18000); 
  delayMicroseconds (18000); 
  tip++;

  delayMicroseconds (18000);         //Debounce the reed switch

}

void  get_wind_speed()
{
  float dTime = millis() - lastWindChk;
  dTime /= 1000.0;                                  //Covert ms to sec
  windSpeed = windClickNo / dTime;     //3 / 0.750s = 4
  windClickNo = 0;                                  //Reset and start watching for new wind
  lastWindChk = millis();
  windSpeed *= 2.4;                               //Calculates the actual wind speed in km/h (2.4km/h if 1 rev per sec)
}

//Function for wind sensor interrupt
void ICACHE_RAM_ATTR wSpeedIRQ()
{
  if (!millis() - lastWindIRQ < 10)                  //Debounce the wind interrupt switch for 10ms after contact
  {
    lastWindIRQ = millis();                          //Set up last wind interrupt for the next interrupt
    windClickNo++;                                   //Each click per second is 1.492MPH
  }
}

void setup() {
  Serial.begin(115200);
WiFi.mode(WIFI_STA);
        WiFi.begin (ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
                Serial.print(".");
                delay(5000);
        }
        Serial.println();
        Serial.println("WiFi connected");

        while (!NTPch.setSNTPtime()) Serial.print("."); // set internal clock
        Serial.println();
        Serial.println("Time set");
  dht.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
 bool success = SPIFFS.begin();
 
  if (success) {
    Serial.println("File system mounted with success");
  } else {
    Serial.println("Error mounting the file system");
    return;
  }
 
  File file = SPIFFS.open("/datalog.txt", "w+");
 
  if (!file) {
    Serial.println("Error opening file for writing");
    yield();
    return;
  }
   pinMode(wSpeed, INPUT_PULLUP);       //Input from anemometer
  pinMode(RAIN, INPUT_PULLUP);         //Input from rain gauge


  //Initate interrupts
  
  //Attach wind and rain interrupts to the respective IRQ functions
  attachInterrupt(digitalPinToInterrupt(RAIN), rainIRQ, RISING);

  attachInterrupt(digitalPinToInterrupt(wSpeed), wSpeedIRQ, FALLING);
  }

void loop() {
  delay(5000);
 dateTime = NTPch.getTime(1.0, 1); // get time from internal clock
        NTPch.printDateTime(dateTime);

        byte actualHour = dateTime.hour;
        byte actualMinute = dateTime.minute;
        byte actualsecond = dateTime.second;
        int actualyear = dateTime.year;
        byte actualMonth = dateTime.month;
        byte actualday = dateTime.day;
        byte actualdayofWeek = dateTime.dayofWeek;
        
  //read temperature and humidity
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  }
  // clear display
  display.clearDisplay();
  
  calC();
  delay(1000);
  get_wind_speed();
  tempcalc();
  // display temperature
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Temperature: ");
  display.setTextSize(2);
  display.setCursor(0,20);
  display.print(t);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2);
  display.print("C");
  
  // display humidity
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Humidity: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(h);
  display.print(" %"); 
  
  display.display(); 
   delay (3000);
   
  display.clearDisplay();
    display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Windspoed: ");
  display.setTextSize(2);
  display.setCursor(0,20);
  display.print(windSpeed);
  display.print(" ");
  
//  display.setTextSize(1);
//  display.setCursor(0, 35);
//  display.print("Reenval: ");
//  display.setTextSize(2);
//  display.setCursor(0, 45);
//  display.print(rain);
//  display.print(" mm"); 
   winddir = analogRead(wDirec);

  Direction = map(winddir, 80, 950, 0, 359);  //winDir();
  
 if ((Direction > 280) && (Direction) < 295) {
    Serial.print(" N ");
    display.print("N");
  }
  if ((Direction > 135) && (Direction < 190)) {
    Serial.print("NE");
    display.print("NE");
  }
  if ((Direction > 0) && (Direction < 10)) {
    Serial.print(" E");
    display.print(" E");
  }
  if ((Direction > 15) && (Direction < 75)) {
    Serial.print("SE");
    display.print("SE");
  }
  if ((Direction > 80) && (Direction < 90)) {
    Serial.print(" S");
    display.print(" S");
  }
  if ((Direction > 215) && (Direction < 230)) {
    Serial.print("SW");
    display.print("SW");
  }
  if ((Direction > 350) && (Direction < 359)) {
    Serial.print(" W");
    display.print(" W");
  }
  if ((Direction > 320) && (Direction < 340)) {
    Serial.print("NW");
    display.print("NW");
  }
  if ((Direction == 0)) {
    Serial.print(" S");
    display.print(" S");
  }
  Serial.println("");
  Serial.print(actualHour);
  Serial.print(":");
  Serial.println(actualMinute);
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Reenval: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(rain);
  display.print(" mm"); 
  display.display();
  delay(3000);
//  midnight=00;
//  lasthour=hour();
//  minutes=minute();
//  seconds=second();
// //Reset all the variables that need daily resetting
    if (lasthour == midnight && minutes == 1 && seconds <= 5)
   {
     resetDay();                         
    }
  delay(3000);      
}
