float fullCourtTargetSpeed = 39;
float fullCourtStartSpeed = 66;
float skillsTargetSpeed = 28;
float skillsStartSpeed = 49;
float shortTargetSpeed = 25.5;
float shortStartSpeed = 39;
float targetSpeed = 0;
float startSpeed = 0;
float flywheelKp = 16.5;
float lastHalfVelocity = 0;
float velocity = 0, power = 0;
bool flywheelRunning = false;
bool calibrated = false;

void calibrate()
{
	calibrated = false;
	time1[t2] = 0;
	bool going = true;
	setFlywheel(fullCourtStartSpeed);
	wait1Msec(5000);
	while(going)
	{
		//Wait for time, use timers instead of waits to account for processing time
		while(time1[T1] < timeDifference){}
		time1[T1] = 0;
		//Calculate average velocity over both encoders
		velocity = (SensorValue[enc] + SensorValue[encB]) /2;
		SensorValue[enc] = 0;
		SensorValue[encB] = 0;
		//Set flywheel to last calibrated value
		setFlywheel(fullCourtStartSpeed);
		//Wait for flywheel to get up to speed
		if(time1[T2] > 2000)
		{
			//If it is close enough to the right speed, exit
			if(abs(velocity - fullCourtTargetSpeed) < 2)
			{
				going = false;
				setFlywheel(0);
			}
			//If it is going too slow, increase start speed
			else if(velocity < fullCourtTargetSpeed)
				fullCourtStartSpeed++;
			//If it is going too fast, decrease start speed
			else if(velocity > fullCourtTargetSpeed)
				fullCourtStartSpeed--;
			time1[T2] = 0;
		}
	}
}

task flywheelControl()
{

	//Run feeder backwards to keep balls away from flywheel when starting up
	flywheelRunning = true;
	cLock = true;
	motor[conveyor] = -127;
	wait1Msec(20);
	motor[conveyor] = 0;
	Sensorvalue[enc] = 0;

	while(true){
		//Wait for time, use timers instead of waits to account for processing time
		while(time1[T1] < timeDifference/2){}
		time1[T1] = 0;

		//Calculate half time velocity, Velocity is encoder displacement in half of timeDifference
		float currentHalfVelocity =  (float)(SensorValue[enc] + SensorValue[encB]) /2
		velocity = currentHalfVelocity + lastHalfVelocity;
		lastHalfVelocity = currentHalfVelocity;
		SensorValue[enc] = 0;
		SensorValue[encB] = 0;
		//Calculate PID values
		difference = targetSpeed - velocity;

		//When the flywheel is running faster than it should, reduce power proportionally from the calibrated value
		if(difference < 0)
		{
			power = startSpeed + difference * flywheelKp;
		}
		//When it is shooting short range and almost at speed, reduce power from full speed to stop over shooting
		else if(difference < 2 && targetSpeed == shortTargetSpeed)
		{
			power = 90;
		}
		//When it is shooting at mid range and almost at speed, reduce power from full speed to stop over shooting
		{
			power = 100;
		}
		//Set full speed forwards if it is going slower than it should
		else if(difference > 0)
		{
			power = 127;
		}
		//It is running at the correct speed, set the power to the calibrated value
		else
		{
			power = startSpeed;
		}
		if(power < 0)
			power = 0;
		setFlywheel(power);
	}
}
