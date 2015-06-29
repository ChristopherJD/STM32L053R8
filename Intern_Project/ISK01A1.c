/*----------------------------------------------------------------------------
 * Name:    ISK01A1.c
 * Purpose: Initialize ISK01A1 expansion board and get data from sensors
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *----------------------------------------------------------------------------
 * Note(s): All sensors use I2C for communication. Each sensor will output
						in one-shot mode, meaning there is no bursts of data, just a 
						single read of the sensors output registers.
						
						Sensor Readings:
						----------------
						*	Temperature: Measured in degrees Fahrenheit
						*	Humidity: Measured in relative humidty (rH%)
						*	Pressure: Measured in hectopascals (mbar) 1 mbar = 100 Pa
						*	Magnetic Field (XYZ): Measured in milliGauss (mG)
						*	Linear Acceleration: Measured in milli-g-force (mg)
						*	Angular Acceleration: Measured in milli-degrees/second (mdps)
						
						Sensor Accuracy:
						----------------
						**Note all sensors generally operate between -40 to 80 degrees C
						**1 mbar = 1 hPa
						*	Temperature:
							------------
								*15-40 degrees C +/- 0.5 degrees C
								*0-60 degrees C +/- 1 degrees C
						*	Humidity: 
							---------
								*20-80 % rH +/- 4.5 %rH
								*0-100 % rH +/- 6 %rH
						*	Pressure:
							---------
								*20-60 degrees C +/- 0.2 mbar
								*0-80 degrees C +/- 1	mbar
						*	Magnetic Field (XYZ): 
							---------------------
								*Full-Scale +/- 4 is about 0.15 mG
						*	Linear Acceleration: 
							--------------------
								*Full-Scale +/- 2 is about 0.061 mg
						*	Angular Acceleration:
							---------------------
								*Full-Scale +/- 245 is about 8.75 mdps 
						
 *----------------------------------------------------------------------------*/

/*------------------------------------------Include Statements------------------------------------*/
#include "stm32l053xx.h"								// Specific Device Header
#include <stdio.h>											// Standard Input Output
#include <math.h>												// Standard math operations
#include "Serial.h"											// Serial Communication
#include "GPIO.h"
#include "HTS221.h"											// Temperature and humidity Drivers
#include "LPS25HB.h"										// Pressure sensor Drivers
#include "LIS3MDL.h"										// Magnetometer drivers
#include "LSM6DS0.h"										// Accelerometer and gyroscope
#include "ISK01A1.h"
/*------------------------------------------Structure Inits----------------------------------------*/
Pressure_Data Pressure;
/*------------------------------------------Functions----------------------------------------------*/

/**
  \fn          void ISK01A1_Init(void)
  \brief       Initialize the ISK01A1 expansion board
*/

void ISK01A1_Init(void){
	
	//Found Devices Variables
	uint8_t HTS221_Found = 0;
	uint8_t LPS25HB_Found = 0;
	uint8_t LIS3MDL_Found = 0;
	uint8_t LSM6DS0_Found = 0;
	
	//Temperature Sensor Initialize
	HTS221_Found = HTS221_Init();		//Initializes the device if found
	
	//Pressure Sensor Initialize
	LPS25HB_Found = LPS25HB_Init();	//Initializes the device if found
	Pressure.Initial = ISK01A1_Get_Altitude();	//Get the Initial reading
	
	//Magnetometer Initialize
	LIS3MDL_Found = LIS3MDL_Init();	//Initializes the device if found
	
	//Accelerometer and Gyroscope Initialize
	LSM6DS0_Found = LSM6DS0_Init();	//Initializes the device if found
	
	if(HTS221_Found & LPS25HB_Found & LIS3MDL_Found & LSM6DS0_Found){
		printf("#####  ISK01A1 Devices Initialized  #####\r\n");
	}
	else{
		
		if(HTS221_Found){
			printf("#####  HTS221 Found  #####\r\n");
		}
		else printf("#####  HTS221 Not Connected  #####\r\n");
		
		if(LPS25HB_Found){
			printf("#####  LPS25HB Found  #####\r\n");
		}
		else printf("#####  LPS25HB Not Connected  #####\r\n");

		if(LIS3MDL_Found){
			printf("#####  LIS3MDL Found  #####\r\n");
		}
		else printf("#####  LIS3MDL Not Connected  #####\r\n");
		
		if(LSM6DS0_Found){
			printf("#####  LSM6DS0 Found  #####\r\n");
		}
		else printf("#####  LSM6DS0 Not Connected  #####\r\n");
	}
}

/**
  \fn          void ISK01A1_Configuration(void)
  \brief       Displays the configuration registers of the sensors
*/

void ISK01A1_Configuration(void){
	HTS221_Configuration();		//Prints the configuration
	LPS25HB_Configuration();	//Prints the configuration
	LIS3MDL_Configuration();	//Prints the configuration
	LSM6DS0_Configuration();	//Prints the configuration
}

/**
  \fn					float ISK01A1_Get_Temperature(void)
  \brief			Retrieves the temperature in degrees C
	\returns		float Temperature: HTS221 Temperature
*/

float ISK01A1_Get_Temperature(void){
	//Local Variables
	float Temperature = 0;
	
	//Read Temperature
	Temperature = HTS221_Temp_Read();
	
	return(Temperature);
}

/**
  \fn					float ISK01A1_Get_Humidity(void)
  \brief			Retrieves the Humidity in rH%
	\returns		float Humidity: HTS221 Humidity
*/

float ISK01A1_Get_Humidity(void){
	//Local Variables
	float Humidity = 0;
	
	//Read Humidity
	Humidity = HTS221_Humidity_Read();
	
	return(Humidity);
}

/**
  \fn					float ISK01A1_Get_Pressure(void)
  \brief			Retrieves the Pressure in mbar
	\returns		float Pressure: LPS25HB Pressure
*/

float ISK01A1_Get_Pressure(void){
	//Local Variables
	float Pressure = 0;
	
	//Read Pressure
	Pressure = LPS25HB_Pressure_Read();
	
	return(Pressure);
}

/**
  \fn					float ISK01A1_Get_Magnetic_X(void)
  \brief			Retrieves X-direction Magnetic Field
	\returns		float X_Magnetic_Field: LIS3MDL Magnetic Field
*/

float ISK01A1_Get_Magnetic_X(void){
	
	//Local Variables
	float X_Magnetic_Field = 0;
	
	//Read Magnetic field
	X_Magnetic_Field = LIS3MDL_X_Read();
	
	return(X_Magnetic_Field);
}

/**
  \fn					float ISK01A1_Get_Magnetic_Y(void)
  \brief			Retrieves Y-direction Magnetic Field
	\returns		float Y_Magnetic_Field: LIS3MDL Magnetic Field
*/

float ISK01A1_Get_Magnetic_Y(void){
	
	//Local Variables
	float Y_Magnetic_Field = 0;
	
	//Read Magnetic field
	Y_Magnetic_Field = LIS3MDL_Y_Read();
	
	return(Y_Magnetic_Field);
}

/**
  \fn					float ISK01A1_Get_Magnetic_Z(void)
  \brief			Retrieves Z-direction Magnetic Field
	\returns		float Z_Magnetic_Field: LIS3MDL Magnetic Field
*/

float ISK01A1_Get_Magnetic_Z(void){
	
	//Local Variables
	float Z_Magnetic_Field = 0;
	
	//Read Magnetic field
	Z_Magnetic_Field = LIS3MDL_Z_Read();
	
	return(Z_Magnetic_Field);
}

/**
  \fn					float ISK01A1_Get_Acceleration_X(void)
  \brief			Retrieves X-direction Acceleration
	\returns		float Acceleration_X: LSM6DS0 Accelerometer
*/

float ISK01A1_Get_Acceleration_X(void){
	
	//Local Variables
	float Acceleration_X = 0;
	
	//Read Acceleration
	Acceleration_X = LSM6DS0_X_Acceleration_Read();
	
	return(Acceleration_X);
}

/**
  \fn					float ISK01A1_Get_Acceleration_Y(void)
  \brief			Retrieves Y-direction Acceleration
	\returns		float Acceleration_Y: LSM6DS0 Accelerometer
*/


float ISK01A1_Get_Acceleration_Y(void){
	
	//Local Variables
	float Acceleration_Y = 0;
	
	//Read Acceleration
	Acceleration_Y = LSM6DS0_Y_Acceleration_Read();
	
	return(Acceleration_Y);
}

/**
  \fn					float ISK01A1_Get_Acceleration_Z(void)
  \brief			Retrieves Z-direction Acceleration
	\returns		float Acceleration_Z: LSM6DS0 Accelerometer
*/


float ISK01A1_Get_Acceleration_Z(void){
	
	//Local Variables
	float Acceleration_Z = 0;
	
	//Read Acceleration
	Acceleration_Z = LSM6DS0_Z_Acceleration_Read();
	
	return(Acceleration_Z);
}

/**
  \fn					float ISK01A1_Get_Roll(void)
  \brief			Retrieves X-direction(roll) Gyroscope reading
	\returns		float Roll: LSM6DS0 Gyroscope
*/


float ISK01A1_Get_Roll(void){
	
	//Local Variables
	float Roll = 0;
	
	//Read Roll
	Roll = LSM6DS0_Gyroscope_Roll_Read();
	
	return(Roll);
}

/**
  \fn					float ISK01A1_Get_Roll(void)
  \brief			Retrieves Y-direction(Pitch) Gyroscope reading
	\returns		float Pitch: LSM6DS0 Gyroscope
*/

float ISK01A1_Get_Pitch(void){
	
	//Local Variables
	float Pitch = 0;
	
	//Read Pitch
	Pitch = LSM6DS0_Gyroscope_Pitch_Read();
	
	return(Pitch);
}

/**
  \fn					float ISK01A1_Get_Roll(void)
  \brief			Retrieves Z-direction(Yaw) Gyroscope reading
	\returns		float Yaw: LSM6DS0 Gyroscope
*/

float ISK01A1_Get_Yaw(void){
	
	//Local Variables
	float Yaw = 0;
	
	//Read Yaw
	Yaw = LSM6DS0_Gyroscope_Yaw_Read();
	
	return(Yaw);
}

/**
  \fn					ISK01A1_Get_Altitude(void)
  \brief			Calculates the altitude based on the pressure
	\returns		float Z: Altitude calculation in feet
*/

float ISK01A1_Get_Altitude(void){
	
	/* Calculation should be good up to 11km */
	/* Local Variables */
	float Z = 0.0;												/* Altitude (Unknown) */
	const float T0 = 288.15;							/* Temperatuer at zero altitude, ISA */
	float P = 0.0;												/* Measured Pressure */
	const float P0 = 101325.0;						/* Pressure at zero altitude, ISA */
	const float g = 9.80655;							/* Acceleration due to gravity */
	const float L = -0.0065;							/* Lapse Rate, ISA */
	const float R = 287.053;							/* Gas constant for air */
	const float Meters_to_Feet = 3.2808;	/* Convert meters to feet */
	
	/* Read Pressure */
	P = LPS25HB_Pressure_Read()*100.0;			/* Convert mbar to Pa */
	
	/* Calculate Altitude in feet */
	Z = (T0/L)*(pow((P/P0),((-L*R)/g))-1)*Meters_to_Feet;
	
	return(Z);
}

/**
  \fn					float QuadCopter_Altitude(void)
  \brief			Calculate the height of the quad copter based on initial reading
	\returns		float Altitude Difference: Altitude of of quadcopter
*/

float QuadCopter_Altitude(void){
	
	/* Local Variables */
	float Altitude_Difference = 0.0;
	
	/* Find the Current Pressure */
	Pressure.Current = ISK01A1_Get_Altitude();
	
	Altitude_Difference = Pressure.Current - Pressure.Initial;
	
	return(Altitude_Difference);
}
