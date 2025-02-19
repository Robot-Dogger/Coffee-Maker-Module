#include <ESP8266WiFi.h>

// Define pin numbers
const int relayPin = D8;    // GPIO pin for the relay (D8)
const int ledPin = D5;       // GPIO pin for the status LED (D5)


void setup() {
  pinMode(relayPin, OUTPUT);   // Initialize the relay pin as an output
  pinMode(ledPin, OUTPUT);     // Initialize the LED pin as an output
  digitalWrite(relayPin, HIGH);     // Turn relay on
  digitalWrite(ledPin, HIGH);  // Turn LED on
  delay(1000);                 // Relay on for 1 second
  digitalWrite(relayPin, LOW);  // Turn relay off
  digitalWrite(ledPin, LOW);   // Turn LED off
  delay(5000);                 // Relay off for 5 seconds

  // Turn relay on for 20 minutes
  digitalWrite(relayPin, HIGH);
  digitalWrite(ledPin, HIGH);  // Turn LED on
  delay(1200000);    
}

void loop() {
  // Turn relay off
  digitalWrite(relayPin, LOW);
  digitalWrite(ledPin, LOW); 
  delay(1000);  // Turn LED off
  digitalWrite(ledPin, HIGH);
  delay(1000);  // Turn LED off
  
}