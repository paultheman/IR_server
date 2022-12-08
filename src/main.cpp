#include <Arduino.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremote.h>
#include "mainpage.h"

ESP8266WebServer server(80);
WiFiManager wm;
bool shouldSaveConfig = false;

const int RECV_PIN = D4;
const int SEND_PIN = D7;

IRrecv IrRx;
IRsend IrTx;
IRData lastDecodedRawData;
unsigned long rec_timestamp = 0;
bool rec_val = false;
int rec_cter = 0;
String address, protocol, command, rawData, repeats, post;
String _protocol, _address, _command, _rawData;

/**************************************************
** This is the main html page
**************************************************/
void handleRoot()
{
  String s = MAIN_page;
  server.send(200, "text/html", MAIN_page);
}

/**************************************************
** Returns the position of the protocol from the ProtocolNames array
**************************************************/
int getPosition (const String& toSearch)
{
  const int max_prot = sizeof(ProtocolNames)/sizeof(ProtocolNames[0]);
  for (int i = 0; i<max_prot; ++i)
  {
    String t_str = String(ProtocolNames[i]);
    if (toSearch == t_str)
      return i;
  }
  return 0;
}

/**************************************************
** Responds to ajax request and sends read IR value
**************************************************/
void readVal()
{
  post = "Protocol=" + protocol + " Address=0x" + address + " Command=0x" + command + " Raw-Data=0x" 
          + rawData + " Repeats=" + repeats;
  server.send(200, "text/plane", post);

  // Webpage does not display any Strings if protocol = '0x ' so we are only clearing protocol
  protocol.clear();
  _protocol.clear();
}

/**************************************************
** Sends the value from html to IR led
**************************************************/
void sendVal()
{
  const String prot = server.arg("Prot");
  const int addr = strtol(server.arg("Addr").c_str(), 0, 16);
  const int cmd = strtol(server.arg("Cmd").c_str(),0,16);
  const int rpt = server.arg("Rpt").toInt();
  Serial.println("** Received data from server **");
  Serial.print("Protocol=");
  Serial.print(prot);
  Serial.print(" Address=0x");
  Serial.print(addr, HEX);
  Serial.print(" Command=0x");
  Serial.print(cmd, HEX);
  Serial.print(" Repetitions=");
  Serial.println(rpt);
  Serial.println();
  server.send(200, "text/plane");

  int pos = getPosition(prot);

  IrRx.stop(); // Stop the reciever so that sent values are not detected
  switch (pos)
  {
    case decode_type_t::UNKNOWN:
      Serial.println("Unknown protocol... aborting.");
      break;
    case decode_type_t::NEC:
      IrTx.sendNEC(addr, cmd, rpt);
      break;
    case decode_type_t::SAMSUNG:
      IrTx.sendSamsung(addr, cmd, rpt);
      break;
    case decode_type_t::SAMSUNG_LG:
      IrTx.sendSamsungLG(addr, cmd, rpt);
      break;
    case decode_type_t::SONY:
      IrTx.sendSony(addr, cmd, rpt);
      break;
    case decode_type_t::PANASONIC:
      IrTx.sendPanasonic(addr, cmd, rpt);
      break;
    case decode_type_t::DENON:
      IrTx.sendDenon(addr, cmd, rpt);  
      break;
    case decode_type_t::SHARP:
      IrTx.sendSharp(addr, cmd, rpt);
      break;
    case decode_type_t::LG:
      IrTx.sendLG(addr, cmd, rpt);
      break;
    case decode_type_t::LG2:
      IrTx.sendLG2(addr, cmd, rpt);
      break;
    case decode_type_t::JVC:
      IrTx.sendJVC((uint8_t) addr, (uint8_t)cmd, rpt);
      break;
    case decode_type_t::RC5:
      IrTx.sendRC5(addr, cmd, rpt, 0);
      break;
    case decode_type_t::RC6:
      IrTx.sendRC6(addr, cmd, rpt, 0);
      break;
    case decode_type_t::KASEIKYO_JVC:
      IrTx.sendKaseikyo_JVC(addr, cmd, rpt);
      break;
    case decode_type_t::KASEIKYO_DENON:
      IrTx.sendKaseikyo_Sharp(addr, cmd, rpt);
      break;
    case decode_type_t::KASEIKYO_SHARP:
      IrTx.sendKaseikyo_Sharp(addr, cmd, rpt);
      break;
    case decode_type_t::KASEIKYO_MITSUBISHI:
      IrTx.sendKaseikyo_Mitsubishi(addr, cmd, rpt);
      break;
    case decode_type_t::NEC2:
      IrTx.sendNEC2(addr, cmd, rpt);
      break;
    case decode_type_t::ONKYO:
      IrTx.sendOnkyo(addr, cmd, rpt);
      break;
    case decode_type_t::APPLE:
      IrTx.sendApple(addr, cmd, rpt);
      break;
    default:
      Serial.println("Error occured finding protocol...");
      break;
  }
  IrRx.start(); // Start the reciever after values have been sent.
}

void handleNotFound() //handles not found URI
{ 
  String message = "Nothing to see here\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) { message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; }

  server.send(404, "text/plain", message);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  wm.setMinimumSignalQuality();
  wm.setTimeout(180); // timeout for the WiFi connect webpage
  if(!wm.autoConnect("AutoConnectAP"))  // this checks if valid WiFi credentials are saved in ESP
  {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(2000);
  }
  Serial.println("connected...yeey :)");

  if (MDNS.begin("ir_server")) { Serial.println("MDNS responder started"); } // find yourt ESP device in DHCP list

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.on("/sendVal", sendVal);
  server.on("/readVal", readVal);
  server.begin();
  Serial.println("HTTP server started");

  //if Wemos D1 is used Led Feedback should be disabled, Pin D4 is also the LED pin which will actually enable Led Feedback
  IrRx.begin(RECV_PIN, DISABLE_LED_FEEDBACK);
  IrTx.begin(SEND_PIN, DISABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN);
}

void loop() {
  server.handleClient();
  MDNS.update();

  if (IrRx.decode()) 
  { 
    if (IrRx.decodedIRData.protocol == UNKNOWN) 
    {
      Serial.println(F("Received noise or an unknown (or not yet enabled) protocol."));
    }
    else
    {
      IrRx.printIRResultShort(&Serial);
      Serial.println();
      
      if ((IrRx.decodedIRData.flags == IRDATA_FLAGS_IS_REPEAT) || (IrRx.decodedIRData.flags == IRDATA_FLAGS_IS_AUTO_REPEAT))
      {
        ++rec_cter;
        _rawData = String(lastDecodedRawData.decodedRawData, HEX);
      }
      else
      {
        rec_cter = 0;
        _rawData = String(IrRx.decodedIRData.decodedRawData, HEX);
        lastDecodedRawData.decodedRawData = IrRx.decodedIRData.decodedRawData;
      }
      
      _rawData.toUpperCase();
      _protocol = String(getProtocolString(IrRx.decodedIRData.protocol));
      _address = String (IrRx.decodedIRData.address, HEX);
      _address.toUpperCase();
      _command = String(IrRx.decodedIRData.command ,HEX);        
      _command.toUpperCase();
 
      rec_timestamp = millis();
      rec_val = true;
    }
    IrRx.resume(); // Enable receiving of the next value
  }

  // The 200ms defined below are used to determine if multiple signals are sent on one actuation or button is held down
  if (rec_val == true && millis() - rec_timestamp > 200)
  {
    protocol = _protocol;
    address = _address;
    command = _command;
    rawData = _rawData;
    repeats = String(rec_cter);
    rec_val = false;
  }
}