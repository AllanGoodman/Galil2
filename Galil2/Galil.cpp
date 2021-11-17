#include "Galil.h"
#include <bitset>

Galil::Galil() {
	g = 0;
	setPoint = 0;
	Functions = new EmbeddedFunctions;

	for (int i = 0; i < 1024; i++) {
		ReadBuffer[i] = { NULL };
	}

	for (int i = 0; i < 3; i++) {
		ControlParameters[i] = 0;
	}
}

Galil::Galil(EmbeddedFunctions* Funcs, GCStringIn address) {
	Functions = Funcs;
	Functions = new EmbeddedFunctions;
	g = 0;
	setPoint = 0;
	
	for (int i = 0; i < 1024; i++) {
		ReadBuffer[i] = { NULL };
	}
	
	for (int i = 0; i < 3; i++) {
		ControlParameters[i] = 0;
	}
	
	GReturn galilStatus;
	galilStatus = Functions->GOpen(address, &g);
	if (g == G_NO_ERROR) {
		std::cout << "Connected!" << std::endl;
	}
	else {
		std::cout << "Failed to connect" << std::endl;
	}
}

Galil::~Galil() {
	if (g) {
		Functions->GClose(g);
		delete Functions;
	}
}



// DIGITAL OUTPUTS
void Galil::DigitalOutput(uint16_t value) {
	//USES OP.
	//OP1 makes digital inputs = 1
	//OP4 makes digital inputs = 4 etc
	char command[128] = "";
	sprintf_s(command, "OP%d;", value);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
}

void Galil::DigitalByteOutput(bool bank, uint8_t value) {
	//print high or low byte (bank = 1 or 0 respectively)
	//Just use OP, first argument is low byte, 2nd arg is high byte
	char command[128] = "";
	if (bank) {
		sprintf_s(command, "OP,%d;", value);
	}
	else {
		sprintf_s(command, "OP%d;", value);
	}
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
}

void Galil::DigitalBitOutput(bool val, uint8_t bit) {
	//USE SB for val = 1
	//USE CB for val = 0
	//SB1 sets digital bit 1 to 1 etc
	if (val) {
		char command[128] = "";
		sprintf_s(command, "SB%d;", bit);
		Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
	}
	else {
		char command[128] = "";
		sprintf_s(command, "CB%d;", bit);
		Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
	}
	//DOUBLE CHECK THIS!

	//Maybe just use OP, but print out 
	//Like if currently the digital output is 1100 (12) and we want to make bit 2 (3rd) bit 0, we send OP8 which makes it 1000
	//maybe use char type to keep track of the binary? int bits[16] = [0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0, 0, 1, 1, 0, 0]
	//can we convert this above to a single int?
}



// DIGITAL INPUTS
uint16_t Galil::DigitalInput() {
	//loop 16 times through digitalbitinput, getting all digital inputs.
	uint16_t store = 0x00;
	for (int i = 15; i > -1; i--) {
		bool store_bit = DigitalBitInput(i);
		if (store_bit == 1) {
			store = ((store << 1) | 0x1);
		}
		else {
			store <<= 1;
		}
	}
	return store;
}

uint8_t Galil::DigitalByteInput(bool bank) {
	//Return high or low byte (bank = 1 or 0 respectively)
	//Just loop through digitalbitinput 8 times for 7-0 or 15-8
	//std::cout << "bing" << std::endl;
	uint8_t store = 0x00;
	int initial = 0;
	if (bank) {
		initial = 15;
	}
	else {
		initial = 8;
	}
	for (int i = initial; i > initial-9; i--) {
		bool store_bit = DigitalBitInput(i);
		if (store_bit == 1) {
			store = ((store << 1) | 0x1);
		}
		else {
			store <<= 1;
		}
	}
	//std::cout << std::bitset<8>(store) << std::endl;
	return store;
}

bool Galil::DigitalBitInput(uint8_t bit) {
	//Read a bit from one of the digital inputs specified by bit.
	char command[128] = "";
	GSize returnedNum = 1;
	sprintf_s(command, "MG @IN[%d];", bit);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), &returnedNum);
	return (ReadBuffer[1]-48);
}

bool Galil::CheckSuccessfulWrite() {
	if (ReadBuffer == ":") {
		return 1;
	}
	else if (ReadBuffer == "?") {
		return 0;
	}
}


//Analog Functions
float Galil::AnalogInput(uint8_t channel) {
	//USE MG @AN[channel] to return the analog values!
	char command[128] = "";
	char floatbuf[4] = "";
	GSize returnedNum = 1;
	sprintf_s(command, "MG @AN[%d];", channel);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), &returnedNum);
	floatbuf[0] = ReadBuffer[1];
	floatbuf[1] = ReadBuffer[2];
	floatbuf[2] = ReadBuffer[3];
	floatbuf[3] = ReadBuffer[4];
	float returnable = std::stof(floatbuf);
	return returnable;
}

void Galil::AnalogOutput(uint8_t channel, double voltage) {
	char command[128];
	sprintf_s(command, "AO%d,%f;", channel, voltage);
	GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
}

void Galil::AnalogInputRange(uint8_t channel, uint8_t range) {
	//USE AQ

	//arguments are channel, then the range (1 = +/- 5v, 2= +/- 10v, 3 = 0-5v, 4 = 0-10v)
	//e.g. AQ0,1 sets channel 0 to range +/-5v
	char command[128];
	sprintf_s(command, "AQ%d,%d;", channel, range);
	GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
}



// ENCODER / CONTROL FUNCTIONS
void Galil::WriteEncoder() {
	//User WE, doesnt return bytes.
	//Only use channel 0 which is default as start.

	// WE0 sets channel 0 encoder to 0
	// WE10 sets channel 0 encoder to 10

	/*
	char command[128] = "";
	sprintf(command, "WE %d", bit);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0)
		*/

	char command[128];
	sprintf_s(command, "WE0,0");
	GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);

}

int Galil::ReadEncoder() {
	//USE QE, returns some bytes.
	//QE0 returns encoder value of channel 0
	char command[128];
	char intbuf[1] = "";
	sprintf_s(command, "QE0");
	GSize returnedNum = 1;
	GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), &returnedNum);
	intbuf[0] = ReadBuffer[1];
	int returnable = atoi(intbuf);
	return returnable;
}

void Galil::setSetPoint(int s) {
	setPoint = s;
}

void Galil::setKp(double gain) {
	ControlParameters[0] = gain;
}

void Galil::setKi(double gain) {
	ControlParameters[1] = gain;
}

void Galil::setKd(double gain) {
	ControlParameters[2] = gain;
}

/*

No need to implement, defined in GalilControl.lib

void Galil::PositionControl(bool debug, int Motorchannel) {
	
}

void Galil::SpeedControl(bool debug, int Motorchannel) {

}*/
