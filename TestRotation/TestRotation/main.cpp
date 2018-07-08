
#include <iostream>
#include "..\Includes\Thorlabs.MotionControl.Benchtop.StepperMotor.h"



int GetPosition(const char *serialNo, short channel)
{
	SBC_RequestPosition(serialNo, channel);
	Sleep(100);
	return SBC_GetPosition(serialNo, channel);
}


bool GoToPosition(const char *serialNo, short channel, int position)
{
	if (SBC_MoveToPosition(serialNo, channel, position) != 0)
	{
		return false;
	}
	int temp_position = GetPosition(serialNo, channel);
	while (temp_position != position)
	{
		std::cout << "temp position is " << temp_position << std::endl;
		Sleep(200);
	}
	return true;
}

int main(int argc, char * argv[])
{
	
	

	int serialNo = 40871779;

	//device unit
	int position = 3754629;
	
	int velocity = 37546290;
	int Acceleration = 18773140;
	// identify and access device
	char testSerialNo[16];
	sprintf_s(testSerialNo, "%d", serialNo);
	//SBC_StopImmediate(testSerialNo, 1);
	// stop polling
	//SBC_StopPolling(testSerialNo, 1);
	// close device
	//SBC_Close(testSerialNo);

	try
	{
		if (TLI_BuildDeviceList() == 0)
		{
			// get device list size 
			short n = TLI_GetDeviceListSize();
			std::cout << n << std::endl;
			// get BBD serial numbers
			char serialNos[100];
			TLI_GetDeviceListByTypeExt(serialNos, 100, 40);


			// output list of matching devices
			char *p = strtok(serialNos, ",");
			while (p != NULL)
			{
				TLI_DeviceInfo deviceInfo;
				// get device info from device
				TLI_GetDeviceInfo(p, &deviceInfo);
				// get strings from device info structure
				char desc[65];
				strncpy(desc, deviceInfo.description, 64);
				desc[64] = '\0';
				char serialNo[9];
				strncpy(serialNo, deviceInfo.serialNo, 8);
				serialNo[8] = '\0';
				// output
				printf("Found Device %s=%s : %s\r\n", p, serialNo, desc);
				p = strtok(NULL, ",");
			}

			// open device
			if (SBC_Open(testSerialNo) == 0)
			{
				// start the device polling at 200ms intervals
				SBC_StartPolling(testSerialNo, 1, 200);

				bool needHoming = SBC_CanMoveWithoutHomingFirst(testSerialNo, 1);
				std::cout << "Can this device be moved without Homing? " << needHoming << std::endl;



				//int tmp = SBC_SetRotationModes(testSerialNo, 1, RotationalUnlimited, Forwards);
				//std::cout << "set rotation: " << tmp << std::endl;

				double stepsPerRev;
				double gearBoxRatio;
				double pitch;

				SBC_GetMotorParamsExt(testSerialNo, 1, &stepsPerRev, &gearBoxRatio, &pitch);
				std::cout << "stepsPerRev, gearBoxRatio, pitch: " << stepsPerRev << " " << gearBoxRatio << " " << pitch << std::endl;

				int initPos = SBC_GetPosition(testSerialNo, 1);
				std::cout << "initPos: " << initPos << std::endl;
				
				SBC_SetMotorParamsExt(testSerialNo, 1, 409600.0, 66.0, 1.0);
				SBC_GetMotorParamsExt(testSerialNo,1,&stepsPerRev, &gearBoxRatio, &pitch);
				std::cout << "stepsPerRev, gearBoxRatio, pitch: " << stepsPerRev << " " << gearBoxRatio << " " << pitch << std::endl;
				
				initPos = SBC_GetPosition(testSerialNo, 1);
				std::cout << "initPos: " << initPos << std::endl;

				double rv;
				int getrv = SBC_GetRealValueFromDeviceUnit(testSerialNo, 1, 3754777 , &rv, 1);
				std::cout << "real value from device unit: " << rv << std::endl;
				std::cout << "getrv: " << getrv << std::endl;
				
				int du;
				int getdu = SBC_GetDeviceUnitFromRealValue(testSerialNo, 1, 1000.0, &du, 0);
				std::cout << "device unit for real value 1: " << du << std::endl;
				std::cout << "getdu: " << getdu << std::endl;

				Sleep(3000);
			
				// Home device
				SBC_ClearMessageQueue(testSerialNo, 1);
				SBC_Home(testSerialNo, 1);
				printf("Device %s homing\r\n", testSerialNo);

				// wait for completion
				WORD messageType;
				WORD messageId;
				DWORD messageData;
				SBC_WaitForMessage(testSerialNo, 1, &messageType, &messageId, &messageData);
				while (messageType != 3 || messageId != 1)
				{
					SBC_WaitForMessage(testSerialNo, 1, &messageType, &messageId, &messageData);
				}

				// set velocity if desired
				if (velocity > 0)
				{
					int currentVelocity, currentAcceleration;
					SBC_GetVelParams(testSerialNo, 1, &currentVelocity, &currentAcceleration);
					std::cout << "velocity and acceleration: " << currentVelocity << " " << currentAcceleration << std::endl;
					SBC_SetVelParams(testSerialNo, 1, velocity, Acceleration);
					SBC_GetVelParams(testSerialNo, 1, &currentVelocity, &currentAcceleration);
					std::cout << "velocity and acceleration: "<<currentVelocity<<" "<<currentAcceleration<<std::endl;
				}

			
				// move to position (channel 1)
				SBC_ClearMessageQueue(testSerialNo, 1);
				//SBC_RequestPosition(testSerialNo, 1);
				//Sleep(100);
				int positionget = SBC_GetPosition(testSerialNo, 1);
				std::cout << positionget << std::endl;
		
				//GoToPosition(testSerialNo, 1, position);
				bool suc = SBC_MoveToPosition(testSerialNo,1,position);
				
				//SBC_RequestPosition(testSerialNo, 1);
				//Sleep(100);
				int positionget2 = SBC_GetPosition(testSerialNo, 1);
				std::cout << positionget2 << std::endl;
				printf("Device %s moving\r\n", testSerialNo);


				// wait for completion
				SBC_WaitForMessage(testSerialNo, 1, &messageType, &messageId, &messageData);
				while (messageType != 3 || messageId != 1)
				{
					SBC_WaitForMessage(testSerialNo, 1, &messageType, &messageId, &messageData);
				}

				// get actual poaition
				int pos = SBC_GetPosition(testSerialNo, 1);
				printf("Device %s moved to %d\r\n", testSerialNo, pos);

				
				// stop polling
				SBC_StopPolling(testSerialNo, 1);
				// close device
				SBC_Close(testSerialNo);
			}
		}
	}
	catch (char * e)
	{
		printf("Error %s\r\n", e);
	}
	
	return 0;
}


