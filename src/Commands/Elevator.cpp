// RobotBuilder Version: 1.5
//
// This file was generated by RobotBuilder. It contains sections of
// code that are automatically generated and assigned by robotbuilder.
// These sections will be updated in the future when you export to
// C++ from RobotBuilder. Do not put any code or make any change in
// the blocks indicating autogenerated code or it will be lost on an
// update. Deleting the comments indicating the section will prevent
// it from being updated in the future.



#include "Elevator.h"

Elevator::Elevator() {
	// Add Commands here:
	// e.g. AddSequential(new Command1());
	//      AddSequential(new Command2());
	// these will run in order.

	// To run multiple commands at the same time,
	// use AddParallel()
	// e.g. AddParallel(new Command1());
	//      AddSequential(new Command2());
	// Command1 and Command2 will run in parallel.

	// A command group will require all of the subsystems that each member
	// would require.
	// e.g. if Command1 requires chassis, and Command2 requires arm,
	// a CommandGroup containing them would require both the chassis and the
	// arm.
}

// Properly move both elevator shafts to the proper position
// Making sure to slow down at the necessary points
// Use the right side for determining everything and
// at the same time, just make sure the left side match the right

int DriveElevator(float DesiredHeight)
{
   float CurrentHeightLeft;
   float CurrentHeightRight;
   bool LeftDone = false;
   bool RightDone = false;


   CurrentHeightLeft = GetHeight(LEFT);   // Returns height in inches.
   CurrentHeightRight = GetHeight(RIGHT);

   // See if we are in error - Sides are too far out of level with eachother (maybe one side is stuck)
   if (!InRange(CurrentHeightLeft,CurrentHeightRight,MAX_POS_ERROR))
   {
	   StopElevator(BOTH);   // stop the drive motors
	   return (OUT_OF_LEVEL);
   }

   // See if we are done on right side - meaning at the right position
   if (InRange(CurrentHeightRight,DesiredHeight,AT_POSITION))
   {
	   StopElevator(RIGHT);   // stop the drive motors
	   RightDone = true;
   }
   else  // not in range so continue to move
   {
	   if (CurrentHeightRight > DesiredHeight)
		   ElevatorMoveDown(RIGHT,ElevatorSpeed(CurrentHeightRight,CurrentHeightLeft,DesiredHeight));
	   else
		   ElevatorMoveUp(RIGHT,ElevatorSpeed(CurrentHeightRight,CurrentHeightLeft,DesiredHeight));
   }

   // See if we are done on Left side - meaning at the right position
   if (InRange(CurrentHeightLeft,DesiredHeight,AT_POSITION))
   {
	   StopElevator(LEFT);   // stop the drive motors
	   LeftDone = true;
   }
   else  // not in range so continue to move
   {
	   if (CurrentHeightLeft > DesiredHeight)
		   ElevatorMoveDown(LEFT,ElevatorSpeed(CurrentHeightLeft,CurrentHeightRight,DesiredHeight));
	   else
		   ElevatorMoveUp(LEFT,ElevatorSpeed(CurrentHeightLeft,CurrentHeightRight,DesiredHeight));
   }
   if (LeftDone && RightDone)
      return(POSITION_ACQUIRED);

   return(ELEVATOR_MOVING);
}

/**************************************************
*
*  Drives the elevator shaft down at the request speed
*  RETURNS Nothing
****************************************************/
void ElevatorMoveDown(SIDE Side, float Speed)
{

    // Note if the library speed command does not take normalized then we will need a gain factor
  if (Side == RIGHT)
      RightMotorSpeed(Speed);   // need the library for sensor read
  else
      LeftMotorSpeed(Speed);  // need the library for sensor read

}
/**************************************************
*
*  Drives the elevator shaft UP at the request speed
*  RETURNS Nothing
****************************************************/
void ElevatorMoveUp(SIDE Side, float Speed)
{
   ElevatorMoveDown(Side, -Speed);  // down is opposite of up so just run down with opposite speed...
}
/**************************************************
*
*  Stops the desired side of the elevator
*  RETURNS Nothing
****************************************************/
void StopElevator(SIDE Side)   // stop the drive motors
{
  if (Side == BOTH)  // Reading
  {
      LeftMotorOff();  // need the library for sensor read
      RightMotorOff();  // need the library for sensor read
  }
  else if (Side == RIGHT)
  {
      RightMotorOff();
  }
  else
      LeftMotorOff();  // need the library for sensor read
}
/**************************************************
*
*  Computes the height of the elevator shaft from the
*  Encoder
*  Basically use an equation to calc side - may need
*  to use a table if not linear enough
*
*  RETURNS inches
****************************************************/
float GetHeight(SIDE Side)
{
  if (Side == RIGHT)  // Reading
  {
      return(ReadRightSensor() * RIGHT_SCALE_INCH_PER;  // need the library for sensor read
  }
      return(ReadLeftSensor() * LEFT_SCALE_INCH_PER;    // need the library for sensor read
}

/**************************************************
*
*  ElevatorSpeed allows max speed when not near any place were
*  a tote/container will touch something.  However when it is near
*  then we will slow down.  Additionally, if one side is lagging
*  behind then we will slow the side that is further along.
*  Values are normalized so 1 is max
*  Note - This is only called if the calling routine has determined we are
*  not at the final spot.
*
****************************************************/
float ElevatorSpeed(float DriveSide, float OtherSide, float TargetLocation)
{
	int i;
	float Speed = (float)1.0;  // Normalized numbers - 1 is max

    if (InRange(DriveSide,TargetLocation,SPEED_ZONE))  // If near the final destination, slow down
    {
        // May need an equation here for ramp to final location
        Speed = Speed * SPEED_REDUCTION;
    }
    else 	// See if we are near a 'speed zone', meaning a place where we might contact something
    {
        for (i=0;i<SPEED_TABLE_SIZE;i++)
        {
            if (InRange(SPEED_TABLE[i],DriveSide,SPEED_ZONE))
            {
                Speed = Speed * SPEED_REDUCTION;
                break;
            }
        }
    }
	// If both sides are approx same height then done
	if (InRange(DriveSide,OtherSide,SIDES_SIMILAR))
		return(Speed);
	// Oops side are getting out of alignment so slow down if this side is closest to
	// the destination to allow the other side to catch up.
	// So - if higher than the other side and lower than target or
	//      if lower than the other side and higher than target
	if ((DriveSide > OtherSide &&
		 DriveSide < TargetLocation) ||
		(DriveSide < OtherSide &&
		 DriveSide > TargetLocation))
	{
	    Speed = Speed * SPEED_REDUCTION;
	}
	return(Speed);

}


// Probably a library function exists for this...
// Compare 2 values and see if they are within 'Range' of eachother
bool InRange(float Val1, float Val2, float Range)
{
	float tmp;
// can make this smaller if an abs function exists
	if (Val1 > Val2)
	{
		if (Val1 - Val2 <= Range)
			return(true);
	}
	else if (Val2 - Val1 <= Range)

		return(true);
	return(false);
}