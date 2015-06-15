#include <stdio.h>
#include "ADC.h"
#include "Serial.h"
#include "Timing.h"


void ADC_Test_Code(){
		int ADC_Bit_Conversion = 0;
		float Voltage_Reading = 0.0;
	
		ADC_Bit_Conversion = ADC_Pin(15); //PORTC-Pin5
		Voltage_Reading = Voltage_Conversion(ADC_Bit_Conversion);
		
		printf("Voltage: %f\r\n", Voltage_Reading); //PORTC-Pin5
		Delay(1000); //Delay 1 s
}
