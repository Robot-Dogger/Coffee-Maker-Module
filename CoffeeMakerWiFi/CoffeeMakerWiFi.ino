/**
 * WiFiManager advanced demo, contains advanced configurartion options
 * Implements TRIGGEN_PIN button press, press for ondemand configportal, hold for 3 seconds for reset settings.
 */
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#define TRIGGER_PIN 0

// wifimanager can run in a blocking mode or a non blocking mode
// Be sure to know how to process loops with no delay() if using non blocking
bool wm_nonblocking = false; // change to true to use non blocking
bool brewingState = false; // change to true when brew cycle starts
int BrewStartTime = 0;
WiFiManager wm; // global wm instance
WiFiManagerParameter coffeeParam; // global param ( for non blocking w params )
const int relayPin = D8; // Change this to the pin controlling your relay
const int GreenledPin   = D5;  // Pin controlling the Green LED
const int BlueledPin   = D6;   // Pin controlling the Blue LED

void setup() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  
  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  delay(3000);
  Serial.println("\n Starting");

  pinMode(TRIGGER_PIN, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(GreenledPin, OUTPUT);
  pinMode(BlueledPin, OUTPUT);
  // Note: Red LED is always on (hardwired to 3v3), so no code needed
  
  // wm.resetSettings(); // wipe settings

  if(wm_nonblocking) wm.setConfigPortalBlocking(false);

  // add a custom input field
  int customFieldLength = 40;


  const char* custom_radio_str = "<br/><label for='coffeeMakerState'>Coffee Maker State><br><br></label><input type='radio' name='coffeeMakerStateid' value='0' checked> Off<br><input type='radio' name='coffeeMakerStateid' value='1'> On";
  new (&coffeeParam) WiFiManagerParameter(custom_radio_str); // custom html input
  
  wm.addParameter(&coffeeParam);
  
  wm.setSaveParamsCallback(saveParamCallback);
  
  // custom menu via array or vector
  // 
  // menu tokens, "wifi","wifinoscan","info","param","close","sep","erase","restart","exit" (sep is seperator) (if param is in menu, params will not show up in wifi page!)
  // const char* menu[] = {"wifi","info","param","sep","restart","exit"}; 
  // wm.setMenu(menu,6);
  std::vector<const char *> menu = {"wifi","info","param","sep","restart","exit"};
  wm.setMenu(menu);

  // set dark theme
  wm.setClass("invert");


  //set static ip
  // wm.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0)); // set static ip,gw,sn
  // wm.setShowStaticFields(true); // force show static ip fields
  // wm.setShowDnsFields(true);    // force show dns field always

  // wm.setConnectTimeout(20); // how long to try to connect for before continuing
  wm.setConfigPortalTimeout(300); // auto close configportal after n seconds
  // wm.setCaptivePortalEnable(false); // disable captive portal redirection
  // wm.setAPClientCheck(true); // avoid timeout if client connected to softap

  // wifi scan settings
  // wm.setRemoveDuplicateAPs(false); // do not remove duplicate ap names (true)
  // wm.setMinimumSignalQuality(20);  // set min RSSI (percentage) to show in scans, null = 8%
  // wm.setShowInfoErase(false);      // do not show erase button on info page
  // wm.setScanDispPerc(true);       // show RSSI as percentage not graph icons
  
  // wm.setBreakAfterConfig(true);   // always exit configportal even if wifi save fails

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap

  res = wm.autoConnect("Coffee Maker Setup","TestPassword851"); // password protected ap

  if(!res) {
    Serial.println("Failed to connect or hit timeout");
    // ESP.restart();
  } 
  else {
    //if you get here you have connected to the WiFi    
    Serial.println("connected...yeey :)");
  }
}

void checkButton(){
  // check for button press
  if ( digitalRead(TRIGGER_PIN) == LOW ) {
    // poor mans debounce/press-hold, code not ideal for production
    delay(50);
    if( digitalRead(TRIGGER_PIN) == LOW ){
      Serial.println("Button Pressed");
      // still holding button for 3000 ms, reset settings, code not ideaa for production
      delay(3000); // reset delay hold
      if( digitalRead(TRIGGER_PIN) == LOW ){
        Serial.println("Button Held");
        Serial.println("Erasing Config, restarting");
        wm.resetSettings();
        ESP.restart();
      }
      
      // start portal w delay
      Serial.println("Starting config portal");
      wm.setConfigPortalTimeout(120);
      
      if (!wm.startConfigPortal("Coffee Maker Config","TestPassword851")) {
        Serial.println("failed to connect or hit timeout");
        delay(3000);
        // ESP.restart();
      } else {
        //if you get here you have connected to the WiFi
        Serial.println("connected...yeey :)");
      }
    }
  }
}


String getParam(String name){
  //read parameter from server, for customhmtl input
  String value;
  if(wm.server->hasArg(name)) {
    value = wm.server->arg(name);
  }
  return value;
}

void saveParamCallback(){
  Serial.println("[CALLBACK] saveParamCallback fired");
  Serial.println("PARAM coffeeMakerStateid = " + getParam("coffeeMakerStateid"));
  
  checkCoffeeState();
}
void checkCoffeeState(){
  Serial.println("started checkCoffeeState() function");

  String coffeeMakerState = getParam("coffeeMakerStateid");

if (strcmp(coffeeMakerState.c_str(), "1") == 0) {
    if(brewingState = 0){  
      digitalWrite(relayPin, HIGH);     // Turn coffee maker ON
      digitalWrite(GreenledPin, HIGH);    // Green LED on indicates "on"
      digitalWrite(BlueledPin, LOW);      // Blue LED off

      Serial.println("Coffee Maker ON");

      brewingState = true;
      BrewStartTime = millis();
      delay(4);
      }

    if(millis() - BrewStartTime >= 120000){
      brewingState = 0;
      digitalWrite(relayPin, LOW);      // Turn coffee maker OFF
      digitalWrite(GreenledPin, LOW);     // Green LED off
      digitalWrite(BlueledPin, HIGH);     // Blue LED on indicates "off"
       Serial.println("Coffee Maker OFF");
     }
   
  } else {
    digitalWrite(relayPin, LOW);      // Turn coffee maker OFF
    digitalWrite(GreenledPin, LOW);     // Green LED off
    digitalWrite(BlueledPin, HIGH);     // Blue LED on indicates "off"
    Serial.println("Coffee Maker OFF");
  }

}
void loop() {
  if(wm_nonblocking) wm.process(); // avoid delays() in loop when non-blocking and other long running code  
  checkButton();
  checkCoffeeState();
  // put your main code here, to run repeatedly:
}

