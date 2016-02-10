/*
 *  inverse_kinematics_sim.cpp
 *
 *
 *  Created by Matt Bunting on 10/18/12.
 *  Copyright 2012 12 Cent Dwarf. All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <Hexapod.h>
#include <sixaxis.h>
#include <MBmath.h>
//#include <bmatrix.h>
//#include <portmidi.h>
//#include <porttime.h>

typedef enum { walking, quaternion, headbob, fistpump } State;

void printUsage()
{
	printf("Usage:\n");
	printf("  ./inverse_kinematics_sim s c h\n");
	printf("\n");
	printf("  s:    Simulation enable, t for true, f for false (true hexapod control)\n");
	printf("  c:    Sixaxis enable, t for true, f for false \n");
	printf("  h:    Head bobbing enable, t for true, f for false \n");
	printf("\n");
}

//PmStream * midi;
//    char line[80];
 //   PmEvent buffer[5]; 

//void process_midi(PtTimestamp timestamp, void *userData)/
//{
// buffer[0].timestamp = timestamp+0;
 //       buffer[0].message = Pm_Message(0x90, 60, 100);
 //       buffer[1].timestamp = timestamp+1000;
 //       buffer[1].message = Pm_Message(0x90, 60, 0);
 //      buffer[2].timestamp = timestamp+1000;
 //       buffer[2].message = Pm_Message(0x90, 64, 100);
 //       buffer[3].timestamp = timestamp+2000;
  //      buffer[3].message = Pm_Message(0x90, 64, 0);
 //Pm_Write(midi, buffer, 4);
//} 

int main(int argc, char* argv[])
{
    Hexapod hex( HexapodLeg::DMITRI );                // hexapod class to operate hexapod
    bool terminate = false;     //to exit main loop
	MBnode rootNode;
	hex.build(&rootNode);
	rootNode.printStructure();
	
	bool readyToSwitchState = false;

	// Simulation enabler:
	bool simEnable;
	bool sixaxisEnable;
	bool headbobEnable;

	State state = walking;
	double headBobMagnitude = 0;
	double walkingMagnitude = 1;
	double quaternionMagnitude = 0;
	double fistPumpMagnitude = 0;

	// Determine the input commands:
	if ((argc > 3) && (argc < 5)) {
		// Simulation enabler:
		if (*argv[1] == 't') {
			simEnable = true;
		} else if (*argv[1] == 'f') {
			simEnable = false;
		} else {
			printUsage();
			return 1;
		}

		// Sixaxis enabler:
		if (*argv[2] == 't') {
			sixaxisEnable = true;
		} else if (*argv[2] == 'f') {
			sixaxisEnable = false;
		} else {
			printUsage();
			return 1;
		}

		// headbob enabler:
		if (*argv[3] == 't') {
			headbobEnable = true;
		} else if (*argv[3] == 'f') {
			headbobEnable = false;
		} else {
			printUsage();
			return 1;
		}
	} else {
		printUsage();
		return 1;
	}

	// MIDI stuff:
	//PortMidiStream *stream;
	//PmDeviceID inputDevice = 0;
	//long bufferSize = 64;
	//PmTimeProcPtr  	time_proc;

//	Pt_Start(2000, &process_midi, 0); 
	//Pm_Initialize();
	//PmError pmError = Pm_OpenInput 	( 	&stream, inputDevice, 0, bufferSize, 0, 0 );
	//if(pmError == pmNoError) {
//		printf("Woot! Openned a MIDI device!\n");
	//} else {
	//	printf("BUMMER! no MIDI support...");
	//}

	// Wifi stuff:
	AppInterface hexapodSocketServer;
        //hexapodSocketServer.initialize();
        hexapodSocketServer.setPort( 8888 );
        hexapodSocketServer.start(  );

	// Sixaxis things:
	Sixaxis six;					// Call the sixaxis controller structure
    double PStimer = 0;             // timer for held down time of PS button (see below)
	//std::ofstream error_out("sixaxiserror.log");	// Output goes into an error log

	if (sixaxisEnable) {
		six.initialize();
		//SIXINIT(&six, error_out);   // Need to initialize sixaxis structure
		six.Btc = .4;    // button LPF time constant
		six.Atc = .4;
	}

    Time time, timeTest;                  // This keeps track of time, see main loop below
    double tint = 0;            // integrates dtime and gets reset
    Time beatCounter;           // Handles beat timing
    double currentFrequency = 128. /(60. * 2. * 16.);    // The current estimated frequency from input
    double currentPhase = 0;    // current phase offset, used for syncronization
    double dtimeBuffer[10];     // stores the last 10 button push dtimes
	double timeBuffer[10];  // stores the actual time the beat was measured
	
   // bool enoughBeatSamples = false; // is true when samples are filled
    int totalBeatsSampled = 0;       // overall number of samples taken (used for intialization)
	bool lastButtonState = 0;
	bool timeToSample = false;

//    IplImage* frame;             // frame to store simulation image
        if (simEnable) {
	//  frame = cvCreateImage( cvSize(640, 480), IPL_DEPTH_8U, 3);
	 // cvNamedWindow("Hexapod Simulation", CV_WINDOW_AUTOSIZE);
	}
    //char key = 0;

    // Start hexapod:
    hex.WD_enabled = false; // BUT FIRST!  disable all walking dynamics
	//hex.BG_enabled = false;
	hex.bodyheight = 100;
	if (simEnable) {
		hex.powerUpNonBlock();	// This initializes the motors and gets it to stand up.  starts KINEMATICS thread
	} else {
    	hex.powerUpNonBlock();              
	}
	


	if (headbobEnable) {
//		printf("Press enter on the downbeat:");
	//	char dummy;
//		scanf("%c", &dummy);
	}

    time.initialize();
	timeTest.initialize();
	double desiredLoopTime = 1.0/200.0;
	double sleepTime = 0;
	double loopTime = 0;
	double sleepTimeOffset = 0;

	Vector footSave = hex.footinit[1];
	Vector bodyAngularVelocity(3);
	bool needToSaveFoot = true;

    while(!terminate){

		// This ensures a desireable thread update rate:
		timeTest.update();
		loopTime = timeTest.dTime();
		sleepTime = desiredLoopTime - loopTime + sleepTimeOffset;
		if (sleepTime > 0) {
			usleep( sleepTime * 1000000.0 );
		}
		timeTest.reset();
		sleepTimeOffset += 0.001 * (desiredLoopTime - 1.0/time.frequency());
		if ((fabs(sleepTimeOffset) >= 10000000000) || (sleepTimeOffset != sleepTimeOffset)) {
			sleepTimeOffset = 0;
		}



        // Perform timing information update:
		time.update();  // updated the timer, updates running time and dtime since last call
        tint += time.dTime();
		if(tint >=3){   // repeat every 3 seconds
            printf("Controller frequency: %3.4f, Running time: %3.4f\n",time.frequency(),time.runningTime());
            tint -= 3;
        }

		if (hex.walking()) {
			readyToSwitchState = true;
		}


		// This example demonstrates the possibility of keeping the head stationary while modifying the body angles:
		Vector angles(3), setAngles(3);
		static Vector lastAngles(3), lastAngles2(3);
		double bodyheight = 0;
		
//if(hex.walking())
//{
//state = quaternion;
//}
	
		switch(state)
		{
			case walking:
				walkingMagnitude += 2*time.dTime();
				break;
			
			case headbob:
				headBobMagnitude += 2*time.dTime();
				break;
			
			case quaternion:
				quaternionMagnitude += 2*time.dTime();
				break;
			
			case fistpump:
				if( hex.WD_enabled == false )
				{
					fistPumpMagnitude += 2*time.dTime();
				}
				break;
			
			default:
				break;
		}
		walkingMagnitude	-= time.dTime();
		headBobMagnitude	-= time.dTime();
		quaternionMagnitude	-= time.dTime();
		fistPumpMagnitude	-= time.dTime();
		
		if(walkingMagnitude < 0)
			walkingMagnitude = 0;
		if(walkingMagnitude > 1)
			walkingMagnitude = 1;
		if(headBobMagnitude < 0)
			headBobMagnitude = 0;
		if(headBobMagnitude > 1)
			headBobMagnitude = 1;
		if(quaternionMagnitude < 0)
			quaternionMagnitude = 0;
		if(quaternionMagnitude > 1)
			quaternionMagnitude = 1;
		if(fistPumpMagnitude < 0)
			fistPumpMagnitude = 0;
		if(fistPumpMagnitude > 1)
			fistPumpMagnitude = 1;
		
		
		
		
		//angles(0) = MOGI_PI/16.0 * (1.0 + sin(3.2*time.runningtime));
		if (sixaxisEnable) {
			six.update();
			//SIXAXIS(&six);
			six.Aalpha = exp(-time.dTime()/six.Atc);	// Calculate the alpha for the accelerometer LPF (low-pass filter)
			six.Balpha = exp(-time.dTime()/six.Btc);	// Calculate the alpha for the Buttons LPF

			if(BTN_cross(six))
				state = walking;
			if(BTN_square(six))
				state = headbob;
			if(BTN_circle(six))
				state = quaternion;
			if(BTN_triangle(six))
				state = fistpump;

			setAngles(0) =  ((26.506 - 3.7601)/2.0 + .6*(26.506 + 3.7601) * six.Yl );// - .25 * six.pitch +;
			if (setAngles(0) > 26.506) {
				setAngles(0) = 26.506;
			} else if(angles(0) < -3.0) {
				setAngles(0) = -3.0;
			}
			setAngles(0) *= MOGI_PI/180.0;

			setAngles(1) = 1.0*six.Xl * 20.0; // + .25 * six.roll;
			if (setAngles(1) > 20.0) {
				setAngles(1) = 20.0;
			} else if(angles(1) < -20.0) {
				setAngles(1) = -20.0;
			}
			setAngles(1) *= MOGI_PI/180.0;

			setAngles(2) = 13.0 * six.Xr * MOGI_PI/180.0;

			bodyheight = (six.L2 - six.R2 + six.Yr) * 50;

			if (bodyheight > 50) {
				bodyheight = 50;
			} else if(bodyheight < -50) {
				bodyheight = -50;
			}

			angles = six.Balpha * setAngles + (1-six.Balpha) * lastAngles;
			lastAngles = angles;

			// Now run synchronization stuff:
			//if(BTN_R1(six)) 
			//{
				if( lastButtonState == BTN_R1(six) )
				{
					// do nothing
				} else if ( (lastButtonState == 0) && (BTN_R1(six) == 1) ) {
					timeToSample = true;
				} else if ( (lastButtonState == 1) && (BTN_R1(six) == 0) ) {
					// do nothing
				}
				lastButtonState = BTN_R1(six);
			//}

			if(timeToSample)
			{
				timeToSample = false;
				if(totalBeatsSampled == 0)
				{
					//totalBeatsSampled++;
					beatCounter.initialize();
			      } else {
			      		beatCounter.update();
					if(4/currentFrequency > beatCounter.dTime())
					{
				      		for( int i = 9; i > 0; i--)
							{
								dtimeBuffer[i] = dtimeBuffer[i-1];
								timeBuffer[i] = timeBuffer[i-1];
				      		}
				      		dtimeBuffer[0] = beatCounter.dTime(); 
							timeBuffer[0] = time.runningTime();
				      
				     		if(totalBeatsSampled > 11) { 
								// update frequency:
								currentFrequency = 0;
								double w = 1;
								for( int i = 0; i < 10; i ++) {
									//currentFrequency += ((9.5 - (double)i)/10.0)/(5) * dtimeBuffer[i];	
									currentFrequency += ((w/2.0 - (w/9.0)*(double)i + 5)/50) * dtimeBuffer[i];
								}
								currentFrequency = 1/(currentFrequency * 16);
	
								// update phase:
								currentPhase = 0;
								double tempOffset = timeBuffer[4];
								for( int i = 0; i < 10; i ++) {
									//currentPhase += (9.5 - (double)i)/(50) * (fmodf(timeBuffer[i]*MOGI_PI*currentFrequency - MOGI_PI, 2*MOGI_PI));	
									currentPhase +=    ((w/2.0 - (w/9.0)*(double)i + 5)/50) * fmodf((timeBuffer[i]-tempOffset)*MOGI_PI*currentFrequency + 16*MOGI_PI, 16*2*MOGI_PI);
								}
								currentPhase = fmodf(tempOffset * MOGI_PI * currentFrequency + currentPhase, 16*2*MOGI_PI) - 16*MOGI_PI;
				      		}
					}
				      
				}
				totalBeatsSampled++;
			}

		} else {
			if (readyToSwitchState) {


			if((hexapodSocketServer.slider1() <= 0.5) && (hexapodSocketServer.slider2() <= 0.5))
				state = walking;
			if((hexapodSocketServer.slider1() > 0.5) && (hexapodSocketServer.slider2() <= 0.5))
				state = headbob;
			if((hexapodSocketServer.slider1() <= 0.5) && (hexapodSocketServer.slider2() > 0.5))
				state = quaternion;
			if((hexapodSocketServer.slider1() > 0.5) && (hexapodSocketServer.slider2() > 0.5))
				state = fistpump;
			}

			setAngles(0) =  ((26.506 - 3.7601)/2.0 + .6*(26.506 + 3.7601) * hexapodSocketServer.yLeft() );// - .25 * six.pitch +;
			if (setAngles(0) > 26.506) {
				setAngles(0) = 26.506;
			} else if(angles(0) < -3.0) {
				setAngles(0) = -3.0;
			}
			setAngles(0) *= MOGI_PI/180.0;

			setAngles(1) = 1.0*hexapodSocketServer.xLeft() * 20.0; // + .25 * six.roll;
			if (setAngles(1) > 20.0) {
				setAngles(1) = 20.0;
			} else if(angles(1) < -20.0) {
				setAngles(1) = -20.0;
			}
			setAngles(1) *= MOGI_PI/180.0;

			setAngles(2) = 13.0 * hexapodSocketServer.xRight() * MOGI_PI/180.0;



			double alphaForThis = .001;//exp(-0.4/time.dTime());
			angles = alphaForThis * setAngles + (1.0-alphaForThis) * lastAngles;
			lastAngles = angles;
			angles = alphaForThis * angles + (1.0-alphaForThis) * lastAngles2;
			lastAngles2 = angles;


			bodyheight = alphaForThis*(hexapodSocketServer.yRight()) * 50 + (1.0-alphaForThis) * bodyheight;

			if (bodyheight > 50) {
				bodyheight = 50;
			} else if(bodyheight < -50) {
				bodyheight = -50;
			}

			angles(0) =  ((26.506 - 3.7601)/2.0 + .5 *(26.506 - 3.7601)*sin(27.0/40.0 * MOGI_PI*time.runningTime())) * MOGI_PI/180.0;
			angles(1) = 0.9 * 20.0 * sin(30.0/40.0*MOGI_PI*time.runningTime()) * MOGI_PI/180.0;
			angles(2) = .6 * 13.0 * sin(24.0/40.0*MOGI_PI*time.runningTime()) * MOGI_PI/180.0;

		}
		
		/////////////////////////////////////////////////////////////
		// Walking control
		/////////////////////////////////////////////////////////////
		if(walkingMagnitude == 1)
		{
			hex.leglifttime = 1.0/(currentFrequency * 8.0 * 3);
			hex.WD_enabled = true;
			needToSaveFoot = true;

			if (sixaxisEnable) {


			hex.bodyVelocity(0) = six.Xl * 50;
			hex.bodyVelocity(1) = six.Yl * -50;
			bodyAngularVelocity(2) = six.Xr * 15.0 * MOGI_PI / 180.;
			} else {
				hex.bodyVelocity(0) = hexapodSocketServer.xLeft() * 50;
				hex.bodyVelocity(1) = hexapodSocketServer.yLeft() * 50;
				bodyAngularVelocity(2) = hexapodSocketServer.xRight() * 15.0 * MOGI_PI / 180.;
			}
			
		} else if (walkingMagnitude == 0) {
			double totalHeight = 0;
			hex.bodyVelocity *= 0;
			bodyAngularVelocity *= 0;
			for(int i = 0; i < 6; i ++)
			{
				totalHeight += hex.foot[i](2);
			}
			if(totalHeight == 0 )
			{
				hex.WD_enabled = false;
				if(needToSaveFoot)
				{
					needToSaveFoot = false;
					footSave = hex.foot[1];
				}
			}
			
		}

		// With the desired head angles, now apply motions to the body:
		MBmatrix xRot(3,3), yRot(3,3), zRot(3,3), totalRotation;
		xRot.makeXRotation( angles(0) );
		yRot.makeYRotation( angles(1) );
		zRot.makeZRotation( angles(2) );
		totalRotation = xRot * zRot * yRot;

		Vector headQuaternion;
		headQuaternion = totalRotation.quaternion();

		Vector bodyAngleAdjust = headQuaternion.xyzAnglesFromQuaternion();

		// Now that the body is rotated, need to shift the body so that the head is stationary
		MBmatrix camOrigin(3,1);

		camOrigin(0,0) = 0.0;
		camOrigin(1,0) = 133.48;
		camOrigin(1,0) = 133.48;
		camOrigin(2,0) = 23.6;

		xRot.makeXRotation( -bodyAngleAdjust(0) );
		yRot.makeYRotation( -bodyAngleAdjust(1) );
		zRot.makeZRotation( -bodyAngleAdjust(2) );

		MBmatrix camRotate = xRot*yRot*zRot*camOrigin;

		MBmatrix camOffset = camOrigin - camRotate;
		Vector bodyOffset(3);
		bodyOffset(0) = camOffset(0,0);
		bodyOffset(1) = camOffset(1,0);
		bodyOffset(2) = camOffset(2,0) + bodyheight;

		angles *= 180.0/MOGI_PI * quaternionMagnitude;
		bodyOffset *= quaternionMagnitude;
		if(bodyAngleAdjust(0) > MOGI_PI)
			bodyAngleAdjust(0) -= 2*MOGI_PI;
		if(bodyAngleAdjust(0) < -MOGI_PI)
			bodyAngleAdjust(0) += 2*MOGI_PI;
		bodyAngleAdjust *= quaternionMagnitude;
		//printf("bodyAngleAdjust(0) = %3f\n", bodyAngleAdjust(0));

		double timePass;
			
		double bpm = 128;
			double bFreq = bpm / (60 * 2);
			//double timeComponent = time.runningtime*MOGI_PI*bFreq;
			static double globalOffset = 0;
			double timeComponent = (time.runningTime())*MOGI_PI*currentFrequency - globalOffset;
			if(timeComponent > (2*MOGI_PI))
				globalOffset += 2 * MOGI_PI;
			timeComponent -= currentPhase;
			timePass = timeComponent;

		if (headbobEnable) {
			

			timeComponent *= 16.0;

			double tempangle = 13.5 * 2./sqrt(3)*( sin(timeComponent/2.) + sin(3.*timeComponent/2.)/6. + sin(5.*timeComponent/2.)/60. );
			double tempangle2 = 4.5/bFreq*cos(2*timeComponent) + 3.5*(1 - 2*pow(sin(timeComponent), 8) + .2*cos(2*timeComponent) + .1*sin(timeComponent +.2));
			
			angles(2) += headBobMagnitude*tempangle;
			angles(0) += headBobMagnitude*tempangle2;

			bodyOffset(0) +=headBobMagnitude*.75*tempangle;

			bodyAngleAdjust(0) -= headBobMagnitude*(2.0/bFreq*cos(2*timeComponent) + tempangle2/6.)* MOGI_PI/180.0;
			bodyAngleAdjust(2) -= headBobMagnitude*-.70*tempangle * MOGI_PI/180.;

		}
		
		
		/////////////////////////////////////////////////
		// Fist Pump control
		/////////////////////////////////////////////////
		if(hex.WD_enabled == false)
		{
			hex.beginModifying();
			{
				hex.foot[1](2) = 220;
			//		footHeight += time.dtime * 2.0*corkscrew_height/time_to_quit * ;
		
			//          hex.foot[1](2) = initHeight + footHeight;
				hex.foot[1](0) = 40 + 40*cos(timePass*32.0);
				hex.foot[1](1) = 40 + 40*sin(timePass*32.0);
			
				hex.foot[1] *= fistPumpMagnitude;
				hex.foot[1] += footSave;
			}
			hex.endModifying();
		}

		//camRotate.print_stats();

		// Now modify the hexapod structure for control:
        hex.beginModifying();
        {
			if (hex.walking())
			{
				// Set the head angles:
				//angles *= 180.0/MOGI_PI;
				hex.headMechanism.setAngles( angles );
				//hex.roll  = angles(1) * 180.0/MOGI_PI;
				//hex.yaw   = angles(2) * 180.0/MOGI_PI;
				

				// Set the body angles opposite to the head angles
				//hex.dbtheta2 = -1.0 * bodyAngleAdjust;
				hex.setBodyAngles( -1.0 * bodyAngleAdjust );

				//hex.setBodyOrientation()

				// Set the body offset:
				//hex.dbody2(0) = camOffset(0,0);
				//hex.dbody2(1) = camOffset(1,0);
				//hex.dbody2(2) = camOffset(2,0) + bodyheight;
				hex.setBodyLocation( bodyOffset );
				//printf("quaternionMagnitude = %.3f\n", quaternionMagnitude);
				//hex.setBodyAngularVelocity( bodyAngularVelocity ); // broken when changing to quaternions
			}
		hexapodSocketServer.setBody( hex.getBodyVector());
		double coxa[6], femur[6], tibia[6];
			double coxaOffset[6];
			coxaOffset[1] = 40.01 * MOGI_PI/180.0;
			coxaOffset[2] = MOGI_PI - coxaOffset[1];
			coxaOffset[3] = MOGI_PI;
			coxaOffset[4] = MOGI_PI + coxaOffset[1];
			coxaOffset[5] = 2.0*MOGI_PI - coxaOffset[1];

                for (int i = 0; i < 6; i++) {
					coxa[i] = hex.legAngles[i](0) + coxaOffset[i];
                        femur[i] = hex.legAngles[i](1);
                        tibia[i] = hex.legAngles[i](2) + atan(91.55/177.06);
					coxa[i]*= 180.0/MOGI_PI;
					femur[i]*= 180.0/MOGI_PI;
					tibia[i]*= 180.0/MOGI_PI;
                }
                hexapodSocketServer.setLegAngles( coxa, femur, tibia );
		hexapodSocketServer.setHeadAngles( hex.headMechanism.getAngles() * MOGI_PI/180.0 );
        }
        hex.endModifying();

        // Plot cerrent hexapod state:
/*		if (simEnable)
		{
			cvZero(frame);  // Erase frame
			plotHex(    &hex,       // hex class
                    frame,      // frame for the plot
                    320,        // x center of sim in image
                    240,        // y center of sim in image
                    55 + 20*(sin(time.runningtime/3)),   // pitch angle
                    5*time.runningtime,		// yaw angle
					1100. + 200*sin(time.runningtime/2),		// camera distance
					.7);			// camera scale

			char printBuff[64];
			sprintf( printBuff, "Freq: %.2f  Phase:  %.3f  Samples: %d  timeC: %.2f", currentFrequency, currentPhase, totalBeatsSampled, timePass);
			CvFont font;
			cvInitFont( &font, CV_FONT_HERSHEY_PLAIN, 1,
                 			1, 0,
                 			1, 8 );
			cvPutText( frame, printBuff, cvPoint(20, 20), &font, cvScalar(255,255,255,0));

			switch (state)
			{
				case walking:
					sprintf( printBuff, "State: Walking");
					break;
		
				case quaternion:
					sprintf( printBuff, "State: Quaternion");
					break;

				case headbob:
					sprintf( printBuff, "State: Head Bob");
					break;

				case fistpump:
					sprintf( printBuff, "State: Fist Pump");
					break;

				default:
					break;
			}
			cvPutText( frame, printBuff, cvPoint(20, 40), &font, cvScalar(255,255,255,0));

			cvCircle( frame, cvPoint(27, 60), 7, cvScalar( 0, 0, 255 * (int)( fmodf(timePass * 16.0, 2*MOGI_PI) < (.2*MOGI_PI)), 0), 3, 8, 0 );

			cvShowImage("Hexapod Simulation", frame);
			key = cvWaitKey(10);
			if (key == 'q') {
				terminate = true;
			}
		} else {
*/			if (sixaxisEnable) {
				// This sequence handles the PS button, if held down for 3 seconds, terminate loop.
				if(six.B3 == 1) PStimer += time.dTime(); //hex->terminate = 1;
				else PStimer = 0;
				if(PStimer >= 3) terminate = true;		// after three seconds, terminate everything
			} else {
				// Terminate after 30 seconds
				//if(time.runningtime>120)
				//	terminate = true;
			}
//		}

	}

	if (simEnable)
	{
//		cvReleaseImage(&frame);
	//	cvDestroyAllWindows();
	}

    printf("Main Loop finished!  Shutting down hexapod...");
	hexapodSocketServer.stop();
    hex.powerDown();
	printf("Done.\n");

	return(0);
}
