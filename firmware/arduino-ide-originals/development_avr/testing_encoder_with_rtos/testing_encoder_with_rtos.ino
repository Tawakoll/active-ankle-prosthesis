#include <Arduino_FreeRTOS.h>
void target_angle(void *param);
void serial_print(void *param);

int target=0;
TaskHandle_t Task_Handle1;
TaskHandle_t Task_Handle2;


void setup() {
  // put your setup code here, to run once:
 xTaskCreate(target_angle,"Task4",100,NULL,1,&Task_Handle1);
 xTaskCreate(serial_print,"Task2",100,NULL,1,&Task_Handle2);
 Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:

}


void target_angle(void *param){
  
  (void) param;
  while(1){
   target=150;
   vTaskDelay(500/portTICK_PERIOD_MS);
   target=0;
   vTaskDelay(500/portTICK_PERIOD_MS);
    }
  }

   void serial_print(void *param){
  
  (void) param;
  while(1){
   
    Serial.print("Target = ");
    Serial.println(target);
  
   vTaskDelay(1/portTICK_PERIOD_MS);
    }
  }
