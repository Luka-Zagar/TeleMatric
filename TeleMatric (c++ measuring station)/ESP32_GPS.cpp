/*  ------------------------------
    TELEMATRIC [DATA-FLOW project]
    Created by: Luka Zagar
    Date: 10. Oct. 2023
    ------------------------------
*/

/* WiFi and HPPT protocols */
#include <WiFi.h>
#include <HTTPClient.h>

/* GPS and SS protocols */
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

SoftwareSerial GPS_SoftSerial(22, 23); // (Rx, Tx)
TinyGPSPlus gps;

volatile float minutes, seconds;
volatile int degree, secs, mins;

const int Station_ID = 11;
const char* ssid = "7E8329";
const char* password = "xc9jvceinp";

String URL = "http://192.168.0.27/GPS.Data_project/test_data.php";

void setup() {
    Serial.begin(115200);
    GPS_SoftSerial.begin(9600); // Define baud rate for software serial communication

    Serial.print("\n");    
    connectWiFi();
}

void loop() {
    if(WiFi.status() != WL_CONNECTED) {
        connectWiFi();
    }
    
    smartDelay(4900); // Generate precise delay of 4900 -> 5000ms before GPS.read (compensation for code running time f(k) = 0,9806)
    unsigned long start;
    unsigned int year_val;
    double lat_val, lng_val, alt_m_val, setellites_num, course_val, speed_val;
    uint8_t hr_val, min_val, sec_val, day_val, month_val;
    bool loc_valid, alt_valid, time_valid, date_valid, course_valid, speed_valid;

    loc_valid = gps.location.isValid(); /* Check if valid location data is available */
    lat_val = gps.location.lat(); /* Get latitude data */
    lng_val = gps.location.lng(); /* Get longtitude data */

    alt_valid = gps.altitude.isValid(); /* Check if valid altitude data is available */
    alt_m_val = gps.altitude.meters();  /* Get altitude data in meters */

    setellites_num = gps.satellites.value();
    
    time_valid = gps.time.isValid();  /* Check if valid time data is available */
    hr_val = gps.time.hour(); /* Get hour */
    min_val = gps.time.minute();  /* Get minutes */
    sec_val = gps.time.second();  /* Get seconds */

    date_valid = gps.date.isValid();  /* Check if valid date data is available */
    day_val = gps.date.day(); /* Get date */
    month_val = gps.date.month(); /* Get month */
    year_val = gps.date.year(); /* Get year */

    course_valid = gps.course.isValid();  /* Check if valid course data is available */
    course_val = gps.course.deg();  /* Get course data in degrees */

    speed_valid = gps.speed.isValid();  /* Check if valid speed data is available */
    speed_val = gps.speed.kmph(); /* Get speed data in km/h */

    if ((loc_valid == true) && (alt_valid == true) && (time_valid == true) && (date_valid == true) && (course_valid == true) && (speed_valid == true)) {
        
        String postData = "id=" + String(Station_ID) + "&la=" + String(lat_val, 6) + "&lo=" + String(lng_val, 6) + "&al=" + String(alt_m_val, 1) + "&dr=" + String(course_val, 1) + "&sp=" + String(speed_val, 2) + "&st=" + String(setellites_num, 0);

        HTTPClient http;
        http.begin(URL);

        http.addHeader("Content-Type", "application/x-www-form-urlencoded"); /* Telling .php server what type of data is getting */
        int httpCode = http.POST(postData);
        String payload = http.getString();

        Serial.println("GPS DATA SEND SUCCESSFUL");
        Serial.print("Number of satellites: ");             Serial.println(setellites_num);
        Serial.print("Latitude in Decimal Degrees: ");      Serial.println(lat_val, 6);
        Serial.print("Longitude in Decimal Degrees: ");     Serial.println(lng_val, 6);
        Serial.print("Altitude: ");                         Serial.print(alt_m_val, 2);   Serial.println(" m");
        
        char time_string[32];
        sprintf(time_string, "Time: %02d:%02d.%02d \n", hr_val + 2, min_val, sec_val);
        Serial.print(time_string);  

        Serial.print("Date: ");     Serial.print(day_val);          Serial.print("/");    Serial.print(month_val);    Serial.print("/");  Serial.println(year_val);
        Serial.print("Course: ");   Serial.print(course_val, 1);    Serial.println(" °");
        Serial.print("Speed: ");    Serial.print(speed_val, 2);     Serial.println(" km/h");  Serial.print("\n");

        Serial.print("URL: ");      Serial.println(URL);
        Serial.print("Data: ");     Serial.println(postData);
        Serial.print("httpCode: "); Serial.println(httpCode);
        Serial.print("payload: ");  Serial.println(payload);
        Serial.println("--------------------------------------------------------------"); 
    } 
    else {
        Serial.println("GPS DATA NOT SEND SUCCESSFUL / RECEIVED");
        Serial.print("Validation checker: ");   Serial.print(loc_valid);  Serial.print(alt_valid);  Serial.print(time_valid);  Serial.print(date_valid);  Serial.print(course_valid);  Serial.println(speed_valid);
            
        if (!loc_valid) {             
            Serial.print("Number of satellites: "); Serial.println(setellites_num);  
            Serial.print("Latitude: "); Serial.println("*****");
            Serial.print("Longitude: ");    Serial.println("*****");
        }
        
        if (!alt_valid) {
            Serial.print("Altitude: "); Serial.println("*****");
        }
    
        if (!time_valid)  {
            Serial.print("Time: "); Serial.println("*****");
        }

        if (!date_valid) {
            Serial.print("Date: "); Serial.println("*****"); 
        }

        if (!course_valid) {
            Serial.print("Course: ");   Serial.println("*****");
        }

        if (!speed_valid) {
            Serial.print("Speed: ");    Serial.println("*****");
        }
        Serial.println("--------------------------------------------------------------"); 
    }
}

void connectWiFi() {
    WiFi.mode(WIFI_OFF);
    delay(1000);
  
    WiFi.mode(WIFI_STA); //This line hides the viewing of ESP as wifi hotspot
    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi ");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
  
    Serial.print("\n");
    Serial.print("Connected to: "); Serial.println(ssid);
    Serial.print("IP address: "); Serial.println(WiFi.localIP());
}

static void smartDelay(unsigned long ms) {
    unsigned long start = millis();
    do {
        while (GPS_SoftSerial.available())  /* Encode data read from GPS while data is available on serial port */
        gps.encode(GPS_SoftSerial.read());  /* Encode basically is used to parse the string received by the GPS and to store it in a buffer so that information can be extracted from it */
    } 
    while (millis() - start < ms);
}