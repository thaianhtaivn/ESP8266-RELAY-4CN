# Relay Box 4 channel

### Concept

![Diagram](/.pic/diagram.png)

- Shopee: _https://shopee.vn/product/47566847/15595970731_
- Webapp: _https://app4relay.netlify.app_

## Feature

-  Relay SRD 10A/250VAC
-  AC/DC already integrated
-  ESP8266-12F
-  DC/DC buck converter LM2596 5V integrated (7-30VDC)

### Pinout:

| Relay   | ESP8266 Pins |
| ------- | ------------ |
| Relay 1 | 16           |
| Relay 2 | 14           |
| Relay 3 | 12           |
| Relay 4 | 13           |

> uint8_t RELAY_PINS[4] = {16, 14, 12, 13};
