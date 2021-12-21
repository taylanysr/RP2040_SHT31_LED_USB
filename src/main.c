/*
 * Name        : main.c 
 * Author      : Taylan Yasar 
 * Version     : 1.0
 * Date        : 21.12.2021
 * Description : LED and Temperature sensor control with USB Communication on FreeRTOS
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "bsp/board.h"
#include "tusb.h"
#include "pico/stdlib.h"
#include "SHT31.h"


// FreeRTOS includes.
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

// I2C AND LED PIN identification (Reference => RP2040 datasheet) 
#define SCL 0                         //I2C SCL Pin 
#define SDA 1                         //I2C SDA Pin 

#define RED_LED    18                  //RED Pin
#define GREEN_LED  19                  //Green Pin

#define GPIO_ON   1
#define GPIO_OFF  0

#define MAX_PRINTF_BUFFER 256


//Function definitions
void vRedLEDTask( void * pvParameters );
void vTempSensorTask( void * pvParameters );
void vUsbCommunication( void * pvParameters );
static void cdc_task(void);
static void echo_serial_port(uint8_t itf, uint8_t buf[], uint32_t count);
void vGreenLEDTask( void * pvParameters );

int32_t TempSensorDelay=5000;           // Time to receive data from temperature sensor. Default = 5 seconds= 5000 miliseconds
int8_t  LedStatus=1;                    // LED control. LedStatus=1  => LED OFF      LedStatus=0  => LED ON
int8_t  TempSensorStatus =0;            // Temperature sensor control  TempSensorStatus=0  => TempSensor OFF     TempSensorStatus=1  => TempSensor ON  
int8_t  DataCheck=0;                    // Error control


int main()
{

    //intialize stdio
    stdio_init_all();
    tusb_init();
    //intialize i2c1
    i2c_init(I2C_PORT, 100000);
    gpio_set_function(SCL, GPIO_FUNC_I2C);
    gpio_set_function(SDA, GPIO_FUNC_I2C);

    // need to enable the pullups
    gpio_pull_up(SCL);
    gpio_pull_up(SDA);

    gpio_init(RED_LED);                 // Red Led pin configuration  
    gpio_set_dir(RED_LED, GPIO_OUT);    // Red Led pin status (OUTPUT)
    gpio_put(RED_LED, LedStatus);


    gpio_init(GREEN_LED);                 // Red Led pin configuration  
    gpio_set_dir(GREEN_LED, GPIO_OUT);    // Red Led pin status (OUTPUT)
    gpio_put(GREEN_LED, LedStatus);
    


    xTaskCreate(    vTempSensorTask,
                    "Temperature Sensor",
                    1024,
                    NULL,
                    1,                  // configMAX_PRIORITIES     4    ==> a priority from 0 to ( configMAX_PRIORITIES - 1 )
                    NULL);

    xTaskCreate(    vUsbCommunication,
                    "USB",
                    1024,
                    NULL,
                    1,                  // configMAX_PRIORITIES     4    ==> a priority from 0 to ( configMAX_PRIORITIES - 1 )
                    NULL);
    
    xTaskCreate(     vGreenLEDTask,
                     "Yellow Led",
                     1024,
                     NULL,
                     1,                 // configMAX_PRIORITIES     4    ==> a priority from 0 to ( configMAX_PRIORITIES - 1 )
                     NULL);

    vTaskStartScheduler();              //  Starts the RTOS scheduler. After calling the RTOS kernel has control over which tasks are executed and when.

}




// Temperature Sensor task implementation /functionality 
void vTempSensorTask( void * pvParameters )
{

    float temp;
    SHT31_Reset();

    (void) pvParameters;

    while(1)
    {

        if(TempSensorStatus==0){
            printf("Temperature Sensor Status  OFF       [TempSensorStatus=ON] [TempSensorStatus=OFF]\r\n");
        }
        if(TempSensorStatus==1){
            temp = SHT31_ReadData();
            printf("Temperature: %.2f°        ON        [TempSensorDelay=%d]\r\n",temp,TempSensorDelay/1000,TempSensorDelay/1000);
        }


        if(LedStatus==0){
            printf("LED Status                 ON        [LED_ON] [LED_OFF]\r\n");
        }
        if(LedStatus==1){
            printf("LED Status                 OFF       [LED_ON] [LED_OFF]\r\n");
        }
            printf("______________________________________________________________________________________________\r\n");
        vTaskDelay(TempSensorDelay / portTICK_PERIOD_MS);
        

    }
}



// USB task implementation /functionality 
void vUsbCommunication( void * pvParameters )
{
    (void) pvParameters;

	while(1)
	{
		tud_task();
		cdc_task();
        vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}




static void echo_serial_port(uint8_t itf, uint8_t buf[], uint32_t count)
{
  for(uint32_t i=0; i<count; i++)
  {
    tud_cdc_n_write_char(itf, buf[i]);
  }
  tud_cdc_n_write_flush(itf);
}



static void cdc_task(void)
{
  uint8_t itf;
  uint8_t buf[256];
  uint32_t count;
  uint8_t cFlag=0;
  

  uint32_t i;
  char myDataSensor_delay[] ={'T','e','m','p','S','e','n','s','o','r','D','e','l','a','y','='};
  

  char bTempSensorDelay[]={};

  for (itf = 0; itf < CFG_TUD_CDC; itf++)
  {
    // connected() check for DTR bit
    // Most but not all terminal client set this when making connection
    if ( tud_cdc_n_connected(itf) )
    {
      if ( tud_cdc_n_available(itf) )
      {
        count = tud_cdc_n_read(itf, buf, sizeof(buf));           // Read data from USB (buf)
        // echo back to both serial ports
        //echo_serial_port(0, buf, count);
      }
    }
  }

    //Control based on information from console

    if(strcmp(buf,"LED_ON") == 0){                                // LED ON
        gpio_put(RED_LED, GPIO_OFF);
        LedStatus=0;
        DataCheck=1;
    }else if(strcmp(buf,"LED_OFF") == 0){                         // LED OFF
        gpio_put(RED_LED, GPIO_ON);
        LedStatus=1;
        DataCheck=1;
    }else if(strcmp(buf,"TempSensorStatus=ON") == 0){             // Temperature Sensor ON
        TempSensorStatus = 1;
        DataCheck=1;
    }else if(strcmp(buf,"TempSensorStatus=OFF") == 0){            // Temperature Sensor OFF
        TempSensorStatus = 0;
        DataCheck=1;
    }else if(strcmp(buf,"Help") == 0){                            // HELP
        printf("Commands                Descriptions\n\n");
        printf("[LED_ON]                Red LED On.\n");
        printf("[LED_OFF]               Red LED Off.\n");
        printf("[TempSensorStatus=ON]   Temperature Sensor On. \n");
        printf("[TempSensorStatus=OFF]  Temperature Sensor Off. \n");
        printf("[TempSensorDelay=x]     Default Delay time for temperature sensor.\n");
        printf("                        For use, Delay time in x seconds.\n");
        printf("[GetInfo]               Writes the current information for the sensor and LED to the screen. .\n\n");
        printf("______________________________________________________________________________________________\r\n");

        DataCheck=1;
    }else if(strcmp(buf,"GetInfo") == 0){                         // Get info from sensor and LED

        printf("Temperature Sensor Delay   %d Seconds\r\n",TempSensorDelay/1000);
        if(TempSensorStatus==0){
            printf("Temperature Sensor Status  OFF\r\n");
        }
        if(TempSensorStatus==1){
            printf("Temperature Sensor Status  ON\r\n");
        }


        if(LedStatus==0){
            printf("LED Status                 ON\r\n");
        }
        if(LedStatus==1){
            printf("LED Status                 OFF\r\n");
        }
        printf("______________________________________________________________________________________________\r\n");
        DataCheck=1;
    }else{
        DataCheck =2;
    }

    

    for(uint32_t i=0; i<count; i++)                                 // Temperature Sensor Delay set
    {
        
        if(myDataSensor_delay[i]==buf[i]){
            cFlag+=1;
        }

        if(cFlag>15){

            if(isdigit(buf[i+1])){

            bTempSensorDelay[i-15] = buf[i+1];
                if(i == (count-2)){

                    int32_t x = atoi(bTempSensorDelay);
                    TempSensorDelay = x*1000;
                    cFlag =0; 
                    DataCheck=1;
                }
            }
        }
    }   



    if(DataCheck==1 && count!=0){                       // Error Control, DataCheck=0 
        DataCheck=0;
    }

    if(DataCheck==2 && count!=0){                       // Error Control, DataCheck=1            
        printf("Error, invalid value [%s]\n",buf);
        DataCheck=0;
    }



    memset(bTempSensorDelay, 0, count);                 // Buffer clear
    memset(buf, '\0', 256);                              // Buffer clear
    fflush(stdin);                                      // Buffer clear
    cFlag =0;                                           // Buffer clear
}



void vGreenLEDTask( void * pvParameters )              // The program is running. Command awaits...
{

    (void) pvParameters;

    for( ;; )
    {
        gpio_put(GREEN_LED, 1);
        vTaskDelay(500);
        gpio_put(GREEN_LED, 0);
        vTaskDelay(500);
    }
}