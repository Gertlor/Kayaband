#include <U8g2lib.h>
#include <U8x8lib.h>
#include <WiFi.h>
#include <SPI.h>
#include <LoRa.h>

//LoRa pins and variables
#define ss 5
#define rst 14
#define dio0 2
int packetSize;

//Wifi variables
const char* ssid = "Kayaband";
const char* psw = "123456789";
String header;
WiFiServer webServer(80);

//State variables
String gpsState = "off";
String blueState = "off";
String whiteState = "off";
String buzzerState = "off";
bool connectionAvailable = false;

//GPS data array
String gpsData[10];
String mapsLink;


// OLED Display SH1106 
U8X8_SH1106_128X64_NONAME_SW_I2C u8x8(26, 25); //13 12 //26 25


void setup() {
  Serial.begin(115200);

  // WiFi Setup
  Serial.print("Setting AP (Access Point)...");
  WiFi.softAP(ssid, psw, 11, 0, 1); //(ssid, psw, channel, ssidHidden (0,1), max_conn_users)

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP Address: ");
  Serial.println(IP);

  webServer.begin();
  Serial.println("Web Server Initializing OK");

  // LoRa setup
  while(!Serial);
  Serial.println("LoRa Sender");

  LoRa.setPins(ss, rst, dio0);

  while(!LoRa.begin(866E6)){
    Serial.println(".");
    delay(500);
  }

  LoRa.setSyncWord(0xD5);
  Serial.println("LoRa Initializing OK");

  // OLED Display setup
  u8x8.begin();
  
  getGPSData();
  updateDisplay();
}

void loop() {
  startWebServer();
}

void updateDisplay(){
  u8x8.clearDisplay();
  u8x8.setFont(u8x8_font_artossans8_r);
  u8x8.drawString(0,0, "De Kayaband"); 
  u8x8.setCursor(0,2);
  u8x8.print("SSID: " + String(ssid));
  u8x8.setCursor(0,3);
  u8x8.print("PSW: " + String(psw));
  u8x8.drawString(0,4, "IP: 192.168.4.1");
  u8x8.setCursor(0,6);
  u8x8.print("Lat: "+String(gpsData[6]));
  u8x8.setCursor(0,7);
  u8x8.print("Lon: "+String(gpsData[7]));
  
}

void showConnectionError(){
  u8x8.clearDisplay();
  u8x8.setFont(u8x8_font_artossans8_r);
  u8x8.drawString(0,2, "Kayaband not found.");
  u8x8.drawString(0,3, "Try again on the app");
  connectionAvailable = false;
}

void getGPSData(){
  for (int i = 0; i < 5; i++){
    int t = 0;
    LoRa.beginPacket();
    LoRa.print("gpsOn");
    Serial.println("Request GPS");
    LoRa.endPacket();
    do{
      packetSize = LoRa.parsePacket();
      t = t + 1;
    } while(!packetSize && t < 500);
    
    if(packetSize){
      Serial.println("Received Packet ");
      while (LoRa.available()){
        String LoRaData = LoRa.readString();
        Serial.print("GPS DATA");
        parseGPSData(gpsData, LoRaData);
        updateDisplay();
        connectionAvailable = true;
        break;
      } break;
    } else {
      showConnectionError();
    }
  }
}

void parseGPSData(String gpsData[], String LoRaData){
  gpsData[0] = getValue(LoRaData, ':', 0); //day
  gpsData[1] = getValue(LoRaData, ':', 1); //month
  gpsData[2] = getValue(LoRaData, ':', 2); //year
  gpsData[3] = getValue(LoRaData, ':', 3); //hour
  gpsData[4] = getValue(LoRaData, ':', 4); //minute
  gpsData[5] = getValue(LoRaData, ':', 5); //second
  gpsData[6] = getValue(LoRaData, ':', 6); //latitude
  gpsData[7] = getValue(LoRaData, ':', 7); //longitude
  gpsData[8] = getValue(LoRaData, ':', 8); //speed kmh
  gpsData[9] = getValue(LoRaData, ':', 9); //availble sattelites
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void startWebServer(){
  WiFiClient client = webServer.available();    //Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            logicWebApp(client);
            displayWebApp(client);

            break;
            
           } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
          } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void logicWebApp(WiFiClient client){
  if(header.indexOf("GET /buzzer/on") >= 0){
    Serial.println("Sending packet: BUZZER ON");
    LoRa.beginPacket();
    LoRa.print("buzzerOn");
    LoRa.endPacket();
    buzzerState = "on";
  } else if (header.indexOf("GET /buzzer/off") >= 0){
    Serial.println("Sending packet: BUZZER OFF");
    LoRa.beginPacket();
    LoRa.print("buzzerOff");
    LoRa.endPacket();
    buzzerState = "off";
  } else if (header.indexOf("GET /whiteled/on") >= 0){
    Serial.println("Sending packet: WHITE LED ON");
    LoRa.beginPacket();
    LoRa.print("whiteLedOn");
    LoRa.endPacket();
    whiteState = "on";
  } else if (header.indexOf("GET /whiteled/off") >= 0){
    Serial.println("Sending packet: WHITE LED OFF");
    LoRa.beginPacket();
    LoRa.print("whiteLedOff");
    LoRa.endPacket();
    whiteState = "off";
  } else if (header.indexOf("GET /blueled/on") >= 0){
    Serial.println("Sending packet: BLUE LED ON");
    LoRa.beginPacket();
    LoRa.print("blueLedOn");
    LoRa.endPacket();
    blueState = "on";
  } else if (header.indexOf("GET /blueled/off") >= 0){
    Serial.println("Sending packet: BLUE LED OFF");
    LoRa.beginPacket();
    LoRa.print("blueLedOff");
    LoRa.endPacket();
    blueState = "off";
  } else if(header.indexOf("GET /gps") >= 0){
    Serial.println("Sending packet: GPS ON");
    getGPSData();
  }
}

void displayWebApp(WiFiClient client){
  
  mapsLink = "https://www.google.com/maps/place/" + gpsData[6] + "," + gpsData[7] + "/";
  
  // Display the HTML web page
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  // CSS to style the on/off buttons 
  // Feel free to change the background-color and font-size attributes to fit your preferences
  client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
  client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;"); 
  client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
  client.println(".button2 {background-color: #555555;}</style></head>");
  
  // Web Page Heading
  client.println("<body><h1>Kayaband Remote</h1>");

//  if (connectionAvailable){
      // Display current state, and ON/OFF buttons for buzzer  
    client.println("<p>Buzzer state: " + buzzerState + "</p>");
    // If the buzzer is off, it displays the ON button       
    if (buzzerState =="off") {
      client.println("<p><a href=\"/buzzer/on\"><button class=\"button\">ON</button></a></p>");
    } else {
      client.println("<p><a href=\"/buzzer/off\"><button class=\"button button2\">OFF</button></a></p>");
    } 
    
    // Display current state, and ON/OFF buttons for whiteled  
    client.println("<p>Whiteled state: " + whiteState + "</p>");
    // If the whiteled is off, it displays the ON button       
    if (whiteState =="off") {
      client.println("<p><a href=\"/whiteled/on\"><button class=\"button\">ON</button></a></p>");
    } else {
      client.println("<p><a href=\"/whiteled/off\"><button class=\"button button2\">OFF</button></a></p>");
    } 
    
    // Display current state, and ON/OFF buttons for blueled  
    client.println("<p>Blueled state: " + blueState + "</p>");
    // If the blueled is off, it displays the ON button       
    if (blueState =="off") {
      client.println("<p><a href=\"/blueled/on\"><button class=\"button\">ON</button></a></p>");
    } else {
      client.println("<p><a href=\"/blueled/off\"><button class=\"button button2\">OFF</button></a></p>");
    } 
    
    client.println("<p><a href=\"/gps\"><button class=\"button\">GPS</button></a></p>");
    
    client.println("<p>GPS Data </p>");
    client.println("<p>Date: " + gpsData[0] + "/" + gpsData[1] + "/" + gpsData[2] + "  " + gpsData[3] + ":" + gpsData[4] + ":" + gpsData[5] + " UTC</p>");
    client.println("<p>Lat en Long: <a href=" +  mapsLink + "\">" + gpsData[6] + " " + gpsData[7] + "</a></p>");
    client.println("<p>Speed: " + gpsData[8] + " Kmh </p>");
    client.println("<p>Sattelites: " + gpsData[9] + "</p>");
//  } else{
//    
//    client.println("<p>NO KAYABAND FOUND</p>");
//  }
  client.println("</body></html>");
  
  // The HTTP response ends with another blank line
  client.println();
}
