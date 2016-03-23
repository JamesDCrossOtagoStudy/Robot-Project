#pragma config(Sensor, S1,     leftTouch,      sensorEV3_Touch)
#pragma config(Sensor, S2,     rightTouch,     sensorEV3_Touch)
#pragma config(Sensor, S3,     colorSensor,    sensorEV3_Color)
#pragma config(Sensor, S4,     sonarSensor,    sensorEV3_Ultrasonic)
#pragma config(Motor,  motorB,          leftMotor,     tmotorEV3_Large, PIDControl, driveLeft, encoder)
#pragma config(Motor,  motorC,          rightMotor,    tmotorEV3_Large, PIDControl, driveRight, encoder)

int black;
int white;
int grey;

int black_grey; //Reflected light intensity Black->Grey->White
int grey_white; //thresholds at arrows black_grey and grey_white respectively

int prevColor;
int lostTile;

int leftCorrectDist;
int rightCorrectDist;

void calibrateTiles(){
	eraseDisplay();

	displayTextLine(6, "Put me on white");

	sleep(1000);

	int bPress = 0;//while loop break condition

	while(bPress == 0){//while loop halts the program until the bumper is pressed
		if(SensorValue(S1) == 1 || SensorValue(S2) == 1 ){ //checks for bumper press
			white = getColorReflected(S3); //white is set to current sensor reading
			displayTextLine(6, "white : %d", white );//displays white on screen
			bPress = 1;
		}
	}

	sleep(2000);

	eraseDisplay();
	displayTextLine(6, "Put me on black");
	bPress = 0;

	while(bPress == 0){
		if(SensorValue(S1) == 1 || SensorValue(S2) == 1 ){
			black = getColorReflected(S3);
			displayTextLine(6, "black : %d", black );
			bPress = 1;
		}
	}
	sleep(2000);

	eraseDisplay();

	displayTextLine(6, "Put me on grey");
	bPress = 0;
	while(bPress == 0){
		if(SensorValue(S1) == 1 || SensorValue(S2) == 1 ){
			grey = getColorReflected(S3);
			displayTextLine(6, "grey : %d", grey );
			bPress = 1;
		}
	}

	black_grey = (black + grey)/2;
	grey_white = (white + grey)/2;

	displayTextLine(5, "white : %d", white );
	displayTextLine(6, "black : %d", black );
	displayTextLine(7, "grey : %d", grey );
	displayTextLine(9, "black_grey : %d", black_grey );
	displayTextLine(10, "grey_white : %d", grey_white );

}

void moveForward(int distance, int speed){
	resetMotorEncoder(motorB);
	resetMotorEncoder(motorC);
	setMotorTarget(motorB, distance, speed);
	setMotorTarget(motorC, distance, speed);
	sleep(100 * distance / speed);
}

void moveForward(int speed){
	resetMotorEncoder(motorB);
	resetMotorEncoder(motorC);
	setMotorSpeed(motorB, speed);
	setMotorSpeed(motorC, speed);
}

void stopMotors(){
	setMotorSpeed(motorB, 0);
	setMotorSpeed(motorC, 0);
}

void turnLeft(int distance, int speed){
	resetMotorEncoder(motorB);
	resetMotorEncoder(motorC);
	setMotorSpeed(motorB, 0);
	setMotorTarget(motorC, distance, speed);
	sleep(100 * distance / speed);
}

void turnLeft(int speed){
	resetMotorEncoder(motorB);
	resetMotorEncoder(motorC);
	setMotorSpeed(motorB, 0);
	setMotorSpeed(motorC, speed);
}

void turnRight(int distance, int speed){
	resetMotorEncoder(motorB);
	resetMotorEncoder(motorC);
	setMotorTarget(motorB, distance, speed);
	setMotorSpeed(motorC, 0);
	sleep(100 * distance / speed);
}

void pivotLeft(int distance, int speed){
	resetMotorEncoder(motorB);
	resetMotorEncoder(motorC);
	setMotorTarget(motorB, distance, -speed);
	setMotorTarget(motorC, distance, speed);
	sleep(100 * distance / speed);
}

void pivotLeft(int speed){
	resetMotorEncoder(motorB);
	resetMotorEncoder(motorC);
	setMotorSpeed(motorB, -speed);
	setMotorSpeed(motorC, speed);
}

void pivotRight(int distance, int speed){
	resetMotorEncoder(motorB);
	resetMotorEncoder(motorC);
	setMotorTarget(motorB, distance, speed);
	setMotorTarget(motorC, distance, -speed);
	sleep(100 * distance / speed);
}
void pivotRight(int speed){
	resetMotorEncoder(motorB);
	resetMotorEncoder(motorC);
	setMotorSpeed(motorB, speed);
	setMotorSpeed(motorC, -speed);
}


void turnRight(int speed){
	resetMotorEncoder(motorB);
	resetMotorEncoder(motorC);
	setMotorSpeed(motorB, speed);
	setMotorSpeed(motorC, 0);
}

int tileColor(){
	int scannedValue;
	scannedValue = getColorReflected(S3);
	if(scannedValue < black_grey){
		return 1; //return 1 for BLACK
	}
	if(scannedValue < grey_white){
		return 3; //return 2 for GREY
		}else{
		return 2; //return 3 for WHITE
	}
}

void finalMethod(){

int masterPower = 30;
	int slavePower = 30;


	int error = 0;
	//float error = 0;

	//float kp = 0.2; -- float didnt work to good
	int kp = 6;//initial value was 5, adjusted to get a better straight line
	//changed to flaoting point numbers as ev3 does have floating point (original code was for a different system that only supported integers

	resetMotorEncoder(motorB); //sets both motor 'encoders' (distance recorders) to zero
	resetMotorEncoder(motorC);


	//Repeat ten times a second.
	while(getUSDistance(S4)> 10)// changed to 10 is probably safer
	//while(getTouchValue (S1) || getTouchValue (S2))
	{
		//Set the motor powers to their respective variables.
		setMotorSpeed(motorB, masterPower);
		setMotorSpeed(motorC, slavePower);



		error = getMotorEncoder(motorB)- getMotorEncoder(motorC);



		slavePower += error / kp;
		//slavePower = error * kp;

		//Reset the encoders every loop so we have a fresh value to use to calculate the error.
		resetMotorEncoder(motorB);
		resetMotorEncoder(motorC);



	}
	while(!getTouchValue (S1) || !getTouchValue (S2) ){
		sleep(50);

	}

	setMotorSpeed(motorB, 100);
	setMotorSpeed(motorC, 100);

	sleep(3000);

	setMotorSpeed(motorB, 0);
	setMotorSpeed(motorC, 0);
}

task main()
{
	calibrateTiles();
	pivotLeft(180, 30);
	moveForward(180,30);
	moveForward(30);
	while(tileColor() != 1){ //REMAIN IN LOOP UNTIL BLACK REACHED
		sleep(50);
	}
	while(tileColor() == 1){ //REMAIN IN LOOP WHILST ON BLACK
		sleep(50);
	}
	stopMotors(); //STOP MOTORS WHEN GROUT REACHED

	moveForward(30);
	while(tileColor() == 1){
		sleep(10);
	}
	stopMotors();
	int blackwidth = getMotorEncoder(motorC); //$%^&*( CHECK THIS IS NOT A PROBLEM WITH THE ENCODER WIPING PART OF MOVEMETHODS
	moveForward(blackwidth/2, 30);
	sleep(500);
	pivotRight(180, 30);
	playSound(soundBeepBeep);
	int tileTotal = 28;
	int tileCount = 0;
	prevColor = 1; //ITS ON BLACK at this point

	while(tileCount < tileTotal){
		switch(prevColor)
		{
		case 1:
			while(tileColor() == 1){
				sleep(50);
			}
			break;

		case 2:
			while(tileColor() == 2){
				sleep (50);
			}
			break;

		case 3:
			//error correction begins
			sleep(100);
			if(tileColor() == 3){ //check that we're truly on grey
				pivotLeft(10);
				while(tileColor() == 3){
					sleep(10);
				}
				stopMotors();
				leftCorrectDist = getMotorEncoder(motorC);
				pivotRight(leftCorrectDist);

				if(tileColor() == 3){
					pivotRight(10);
					while(tileColor() == 3){
						sleep(10);
					}
				}

				rightCorrectDist = getMotorEncoder(motorB);
				displayTextLine(6, "ERROR CORRECTION DISTANCES", black );
				displayTextLine(7, "LEFT : %d", leftCorrectDist );
				displayTextLine(8, "RIGHT : %d", rightCorrectDist );

				sleep(500);

				if(leftCorrectDist < rightCorrectDist){
					pivotLeft(rightCorrectDist);
					pivotLeft(leftCorrectDist);
					displayTextLine(9, "ERROR CORRECTED WENT LEFT" );
					}else{
					sleep(50);
					displayTextLine(9, "ERROR CORRECTED WENT RIGHT" );
				}
				//error correction ends
				break;

			default:
				displayTextLine(6, "Unexpected error at switch 1.1");
				break;
			}

			if(tileColor() == 1){
				switch(prevColor) //begin switch 2.1
				{


				case 1:
					sleep(20);
					break;

				case 2:
					prevColor = 1;
					tileCount++;
					playSound(soundBlip);
					displayTextLine(6, "Tiles Counted : %d", tileCount );
					break;

				case 3:
					prevColor = 1;
					displayTextLine(5, "Error corrected 'back to black'");
					break;

				default:
					displayTextLine(6, "Unexpected error at switch 2.1");
					break;

				} //End of Switch 2.1

				}else{
				if(tileColor() == 2){
					switch(prevColor){ //begin switch 2.2
					case 1:
						prevColor = 2;
						tileCount++;
						playSound(soundBlip);
						displayTextLine(6, "Tiles Counted : %d", tileCount );
						break;

					case 2:
						sleep(20);
						break;

					case 3:
						prevColor = 2;
						displayTextLine(5, "Error corrected 'back from the white'");
						break;

					default:
						displayTextLine(6, "Unexpected error at switch 2.2");
						break;
					}//end of switch 2.2

					}else{
					switch(prevColor){ //begin switch 2.3

					case 1:
						lostTile = 1;
						prevColor = 3;
						break;

					case 2:
						lostTile = 2;
						prevColor = 3;
						break;

					case 3:
						displayTextLine(10, "HELP WE'RE STILL LOST");
						break;

					default:
						displayTextLine(6, "Unexpected error at switch 2.3");
						break;

					}// end of switch 2.3

				}
			}
		}
	}
	finalMethod();

}
