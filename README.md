# Studio Laganne - Damien's Coffee Cup
Arduino Smart Coffee Cup for Damien <[@roy204](http://github.com/roy204)> Chavarria, made by John <[@psish](http://github.com/psish)> Chavarria.

## History
My brother always forgets its coffee. Either directly on the coffee machine after launching it, or right in front of him while working. As a result, he's always drinking his cup cold, and he hates it.

I decided I would make him a small arduino device below the cup that would detect those slacking behavior!

*Note: This is my **first ever** Arduino's Project :)*

## Components
- [Arduino Uno Rev3](https://store.arduino.cc/arduino-uno-rev3)
- [Adafruit's TMP006 - Contact-less Infrared Thermopile Sensor](https://www.adafruit.com/product/1296)
- [Adafruit's MMA8451 - Triple-Axis Accelerometer](https://www.adafruit.com/product/2019)
- RGB LED
- Passive Buzzer
- [Adafruit's Mini Lipo charger](https://www.adafruit.com/product/1905)
- [Lithium Ion Polymer Battery - 3.7v 500mAh](https://www.adafruit.com/product/1578)
- [QI Wireless Receiver](https://www.adafruit.com/product/1901)

## How does it works?
The device is placed right below the coffee cup.
- The contact-less infrared thermopile sensor detects if the cup is getting hotter than a certain temperature (45 degrees Celsius)
- If for 45 seconds the cup has not moved from its position, it'll buzz and blink the LED.
- Until the MMA8451 has not detected any movement (under a threshold), it'll buzz and light up the LED every 45 seconds.
- Once it has moved, the MMA8451 only task will be to detect tilts.
- If the cup has not been tilted since 60 seconds, it'll buzz and light up the LED.
- Until the cup has been tilted 5 times, it'll buzz and light up the LED every 60 seconds.
- After a full cycle of coffee > movement > 5 x tilts, the devices goes in sleep mode for 5 minutes not detecting anything.

## PCB
I replaced the Arduino UNO by a custom-made PCB that just adds the parts for a standalone Arduino ATMEGA328, it just connects the pins for the RGB LED, the passive buzzer and series the SCL and SDA for both I2C sensors.

The project is powered by a [Lithium Ion Polymer Battery - 3.7v 500mAh](https://www.adafruit.com/product/1578) and charged through a [QI Wireless Receiver](https://www.adafruit.com/product/1901) via [Adafruit's Mini Lipo charger](https://www.adafruit.com/product/1905).
