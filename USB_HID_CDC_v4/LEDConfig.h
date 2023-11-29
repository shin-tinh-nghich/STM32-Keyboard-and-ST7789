// LED PIN OUTPUT
#define LED_PIN PC13
#define LED_CUSTOM PB0

uint8_t status_LED = 3;

void setupLED() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  pinMode(LED_CUSTOM, OUTPUT);
  digitalWrite(LED_CUSTOM, HIGH);
}

// LED task
static void vLED_Task(void *pvParameters) {
  for (;;) {
    switch (status_LED) {
      case 1:
        vTaskDelay(1000);
        digitalWrite(LED_CUSTOM, LOW);
        vTaskDelay(1000);
        digitalWrite(LED_CUSTOM, HIGH);
        break;
      case 2:
        vTaskDelay(100);
        digitalWrite(LED_CUSTOM, LOW);
        vTaskDelay(100);
        digitalWrite(LED_CUSTOM, HIGH);
        break;
      case 3:
        digitalWrite(LED_CUSTOM, HIGH);
        break;
      case 4:
        digitalWrite(LED_CUSTOM, LOW);
        break;
    }
  }
}
