#include <Arduino_FreeRTOS.h>



void Task_print1(void *param);
void Task_print2(void *param);

TaskHandle_t Task_Handle1;
TaskHandle_t Task_Handle2;


void setup() {
  // put your setup code here, to run once
  Serial.begin(9600);
  xTaskCreate(Task_print1,"Task1",100,NULL,1,&Task_Handle1);
  xTaskCreate(Task_print2,"Task2",100,NULL,2,&Task_Handle2);
}

void loop() {
  // put your main code here, to run repeatedly:

}


void Task_print1(void *param){
  (void) param;
  while(1){
    Serial.println("TASK1");
    vTaskDelay(1000/portTICK_PERIOD_MS);
    }
  
  }

  void Task_print2(void *param){
  
  (void) param;
  while(1){
    Serial.println("TASK2");
    vTaskDelay(1000/portTICK_PERIOD_MS);
    }
  }
