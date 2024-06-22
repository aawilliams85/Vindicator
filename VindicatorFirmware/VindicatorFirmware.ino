#include <Adafruit_MCP23X17.h>
#include "tundra_mapped_input.h"

#define RP2040_BB_LED           (25)

/////////////////////////////////////////////////////////////////////
// I/O Expander Pinout
/////////////////////////////////////////////////////////////////////
#define JOY1_BTN1_PIN 0    # GPA0 / 21
#define JOY1_BTN2_PIN 1    # GPA1 / 22
#define JOY1_BTN3_PIN 2    # GPA2 / 23
#define JOY1_BTN4_PIN 12   # GPB4 / 5
#define JOY1_BTN5_PIN 8    # GPB0 / 1

#define JOY2_BTN1_PIN 3    # GPA3 / 24
#define JOY2_BTN2_PIN 4    # GPA4 / 25
#define JOY2_BTN3_PIN 5    # GPA5 / 26
#define JOY2_BTN4_PIN 14   # GPB6 / 7
#define JOY2_BTN5_PIN 15   # GPB7 / 8

#define BTN1_PIN 13        # GPB5 / 6
#define BTN2_PIN 11        # GPB3 / 4

#define DIP1_PIN 9         # GPB1 / 2
#define DIP2_PIN 10        # GPB2 / 3
#define DIP3_PIN 6         # GPA6 / 27
#define DIP4_PIN 7         # GPA7 / 28

Adafruit_MCP23X17 mcp;
/////////////////////////////////////////////////////////////////////


// Create TMI object to communicate with Tundra Tracker
TMI tundra_tracker;
void csn_irq( uint gpio, uint32_t event_mask );  // Chip select IRQ must be top level so let's make one and connect it later in setup

// Use packed structs to define the inputs for your controller
// First we make a struct of buttons
typedef struct __attribute__( ( packed, aligned( 1 ) ) )
{

}
controller_buttons_t;

// Make a top level struct that packs the button data along with the rest of the controller analog values
typedef struct __attribute__( ( packed, aligned( 1 ) ) )
{
  uint16_t      buttons         : 16;
} 
controller_data_t;

// declare the controller struct for use
controller_data_t controller_data;

uint16_t input_data = 0; // I/O data from this scan
uint16_t input_data_last = 0; // I/O data from the previous scan
uint32_t interval_gpio = 25; // Time interval to check GPIO for new data (ms)
uint32_t next_time_gpio = 0;

uint32_t interval_send = 1000; // Time interval to send data to tracker, even if no data has changed (ms)
uint32_t next_time_send = 0;
bool send_requested = false;
bool led_state = false;

void setup() {
  
  pinMode(RP2040_BB_LED, OUTPUT);

  // init the communication to Tundra Tracker, setup the CS irq callback (this has to be at Top level in arduino it seems)
  tundra_tracker.init( );
  gpio_set_irq_enabled_with_callback( tundra_tracker.get_cs_pin(), GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &csn_irq );

  Serial.begin();
  delay(1000);
  //while(!Serial){} //Debug operation only
  while (!mcp.begin_I2C(0x20, &Wire1)){
    Serial.println("Waiting for i2c");
    delay(250);
  }
  for (int i = 0; i < 16; i++){
    mcp.pinMode(i, INPUT_PULLUP);
  }
}


// the loop function runs over and over again forever
void loop() {
  if (millis() > next_time_gpio) {
    input_data = ~mcp.readGPIOAB();
    //Serial.println(input_data);
    next_time_gpio = millis() + interval_gpio;
  }

  if ((millis() > next_time_send)||(input_data != input_data_last)) {
    controller_data.buttons = input_data;
    send_requested = true;
    next_time_send = millis() + interval_send;
  }

  if (send_requested) {
    if (tundra_tracker.data_ready()){
      tundra_tracker.send_data(&controller_data, sizeof(controller_data));
      tundra_tracker.handle_rx_data();
      digitalWrite(RP2040_BB_LED, led_state ^= true);
      input_data_last = input_data;
      send_requested = false;
    }
  }
}

// Callback for SPI Chipselect, just connect in the tmi irq function
void csn_irq( uint gpio, uint32_t event_mask )
{
  tundra_tracker.csn_irq( gpio, event_mask );
}