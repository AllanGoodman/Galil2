#include "galil.h"
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
	//Functions = new EmbeddedFunctions;
	Functions = Funcs;
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
	/*
	if (g == G_NO_ERROR) {
		std::cout << "Connected!" << std::endl;
	}
	else {
		std::cout << "Failed to connect" << std::endl;
	}*/
}

Galil::~Galil() {
	if (g) {
		Functions->GClose(g);
	}
	delete Functions;
}



// DIGITAL OUTPUTS
void Galil::DigitalOutput(uint16_t value) {
	//USES OP.
	//OP1 makes digital inputs = 1
	//OP4 makes digital inputs = 4 etc
	//Each channel is restricted to 255, so we swap to channel 1 if value > 255.
	char command[128] = "";
	if (value < 256) {
		sprintf_s(command, "OP%d;", value);
	}
	else {
		sprintf_s(command, "OP255,%d;", (value - 255));
	}
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
	char command[128] = "";
	if (val) {
		sprintf_s(command, "SB%d;", bit);
	}
	else {	
		sprintf_s(command, "CB%d;", bit);
	}
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
}



// DIGITAL INPUTS
uint16_t Galil::DigitalInput() {
	//loop 16 times through digitalbitinput, getting all digital inputs.
	uint16_t store = 0x00;
	for (int i = 0; i < 16; i++) {
		bool store_bit = DigitalBitInput(i);
		if (store_bit == 1) {
			store = ((store >> 1) | 0x80);
		}
		else {
			store >>= 1;
		}
	}
	return store;
}

uint8_t Galil::DigitalByteInput(bool bank) {
	//Return high or low byte (bank = 1 or 0 respectively)
	//Just loop through digitalbitinput 8 times for 0-7 or 8-15
	uint8_t store = 0x00;
	int initial = 0;
	bool store_bit = 0;
	if (bank) {
		initial = 8;
	}
	else {
		initial = 0;
	}
	for (int i = (0 + initial); i < (8 + initial); i++) {
		store_bit = DigitalBitInput(i);
		if (store_bit == 1) {
			store = ((store >> 1) | 0x80);
		}
		else {
			store >>= 1;
		}
	}
	return store;
}

bool Galil::DigitalBitInput(uint8_t bit) {
	//Read a bit from one of the digital inputs specified by bit.
	char command[128] = "";
	GSize returnedNum = 1;
	sprintf_s(command, "MG @IN[%d];", bit);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), &returnedNum);

	//In order to return an bool value, we must subtract 48 to turn ASCII code into the appropriate value.
	return (ReadBuffer[1]-48);
}

bool Galil::CheckSuccessfulWrite() {
	bool success = false;
	if (ReadBuffer[0] == ':') {
		std::cout << "Successful Write!" << std::endl;
		success = true;
	}
	else if (ReadBuffer[0] == '?') {
		std::cout << "Unsuccessful Write!" << std::endl;
	}
	else {
		std::cout << "Unknown ReadBuffer contents." << std::endl;
	}
	return success;
}


//Analog Functions
float Galil::AnalogInput(uint8_t channel) {
	//USE MG @AN[channel] to return the analog values!
	char command[128] = "";
	char floatbuf[4] = "";
	GSize returnedNum = 1;
	sprintf_s(command, "MG @AN[%d];", channel);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), &returnedNum);

	for (int i = 0; i < 4; i++) {
		floatbuf[i] = ReadBuffer[i + 1];
	}

	return std::stof(floatbuf);
}

void Galil::AnalogOutput(uint8_t channel, double voltage) {
	char command[128];
	sprintf_s(command, "AO%d,%f;", channel, voltage);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
}

void Galil::AnalogInputRange(uint8_t channel, uint8_t range) {
	//USE AQ

	//arguments are channel, then the range (1 = +/- 5v, 2= +/- 10v, 3 = 0-5v, 4 = 0-10v)
	//e.g. AQ0,1 sets channel 0 to range +/-5v
	char command[128];
	sprintf_s(command, "AQ%d,%d;", channel, range);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
}



// ENCODER / CONTROL FUNCTIONS
void Galil::WriteEncoder() {
	//User WE, doesnt return bytes.
	//Only use channel 0 which is default as start.

	// WE0 sets channel 0 encoder to 0
	// WE10 sets channel 0 encoder to 10

	char command[128];
	sprintf_s(command, "WE0,0");
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);

}

int Galil::ReadEncoder() {
	//USE QE, returns some bytes.
	//QE0 returns encoder value of channel 0
	char command[128];
	char intbuf[1] = "";
	sprintf_s(command, "QE0");
	GSize returnedNum = 1;
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), &returnedNum);
	intbuf[0] = ReadBuffer[1];
	return atoi(ReadBuffer);
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


std::ostream& operator<<(std::ostream& output, Galil& galil) {
	char info[1024];
	char ver[1024];
	galil.Functions->GInfo(galil.g, info, sizeof(info));
	galil.Functions->GVersion(ver, sizeof(ver));
	output << info << "\n\n";
	output << ver << "\n\n";
	return output;
}
