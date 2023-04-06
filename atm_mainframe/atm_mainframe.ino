/*
  ENGR 467 Project - RTOS Scheduling and Mutex
  Mouhamed Jaber (79588794)
  Darren Widjaja (43027291)
  Edwin Firmansyah (90320219)
  This code demonstrates the use of FreeRTOS to schedule tasks as well as
  using Mutex for task resource sharing. In this code, we apply a mutex on a critical
  section of code.
*/
// Libraries import
#include <Arduino_FreeRTOS.h>  //FreeRTOS
#include <Wire.h>              //Serial Coms Library
#include "semphr.h"            //Semaphore
#include <LiquidCrystal_I2C.h> //LCD screen

// initialization
SemaphoreHandle_t mutex; // create a mutex handler, used to reference mutex
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
#define button1 2
#define button2 3
int balance = 100;
bool bt1 = true;
bool bt2 = true;
int n_accessed = 0;
int n_requested = 0;

void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);                // set up the LCD's number of columns and rows
  mutex = xSemaphoreCreateMutex(); // create mutex and assign it to its handler

  // initialize digital pins
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);

  // initialize LCD display
  lcd.setCursor(0, 0);
  lcd.print("$");
  lcd.print(balance);

  xTaskCreate(printSerial, "mainframe infoview", 100, NULL, 2, NULL);
  xTaskCreate(pollButton1, "button1 poller", 100, NULL, 1, NULL);
  xTaskCreate(pollButton2, "button2 poller", 100, NULL, 1, NULL);
  vTaskStartScheduler();
}

// printing key parameters to serial monitor
void printSerial()
{
  while (1)
  {
    Serial.print("NR: ");
    Serial.print(n_requested);
    Serial.print(" | NB: ");
    int n_blocked = n_requested - n_accessed;
    Serial.print(n_blocked);
    Serial.print(" | NA: ");
    Serial.print(n_accessed);
    Serial.print(" | B: ");
    Serial.println(balance);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void pollButton1()
{
  while (1)
  {
    bt1 = !digitalRead(button1); // when button is pressed bt1 = true
    if (bt1)
    {
      deposit();
    }
    // make periodic about 100ms
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void pollButton2(void *pvParameters)
{
  while (1)
  {
    bt2 = !digitalRead(button2); // when button is pressed bt2 = true
    if (bt2)
    {
      withdraw();
    }
    // make periodic about 100ms
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// view of User A
void deposit()
{
  n_requested++;
  if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE)
  {
    n_accessed++;
    balance += 20;

    // print deposit amount
    lcd.clear();
    lcd.print("+$20");
    
    delay(2000); // artificial execution time

    // print balance
    lcd.clear();
    lcd.print("B = ");
    lcd.print("$");
    lcd.print(balance);

    // decrement counters upon task completion
    n_requested--;
    n_accessed--;

    // make mutex available
    xSemaphoreGive(mutex);
  }
}

void withdraw()
{
  n_requested++;
  // Serial.println(balance);
  if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE)
  {
    n_accessed++;
    balance -= 20;

    // print transaction occuring
    lcd.clear(); 
    lcd.print("tx occurring...");
    
    delay(2000); // artificial execution time

    // print balance
    lcd.clear(); 
    lcd.print("B = ");
    lcd.print("$");
    lcd.print(balance);

    // decrement counters upon task completion
    n_requested--;
    n_accessed--;

    // make mutex available
    xSemaphoreGive(mutex);
  }
}

void loop()
{
}
