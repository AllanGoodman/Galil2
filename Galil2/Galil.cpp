#include "galil.h"
#include <bitset>

#define COMMAND_LENGTH 128
#define READBUF_LENGTH 1024

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
	Functions = new EmbeddedFunctions;
	Functions = Funcs;
	g = 0;
	setPoint = 0;
	
	for (int i = 0; i < READBUF_LENGTH; i++) {
		ReadBuffer[i] = { NULL };
	}
	
	for (int i = 0; i < 3; i++) {
		ControlParameters[i] = 0;
	}
	
	GReturn galilStatus;
	galilStatus = Functions->GOpen(address, &g);
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
	char command[COMMAND_LENGTH] = "";

	uint8_t bytes[2];
	bytes[0] = value >> 8; // high byte
	bytes[1] = value & 0x00FF; // low byte

	sprintf_s(command, "OP%d,%d;", bytes[1], bytes[0]);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
}

void Galil::DigitalByteOutput(bool bank, uint8_t value) {
	//print high or low byte (bank = 1 or 0 respectively)
	//Just use OP, first argument is low byte, 2nd arg is high byte
	char command[COMMAND_LENGTH] = "";
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
	char command[COMMAND_LENGTH] = "";
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
	//Just loop through digitalbitinput 8 times for 0-7 or 8-15
	uint8_t store = 0x00;
	int initial = 0;
	bool store_bit = 0;
	//Select the start position for the bit scanning according to the selected byte
	if (bank) {
		initial = 8;
	}
	else {
		initial = 0;
	}

	//Loop through the digital inputs, creating a byte using bitwise operations 
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
	char command[COMMAND_LENGTH] = "";
	sprintf_s(command, "MG @IN[%d];", bit);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);

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
	char command[COMMAND_LENGTH] = "";
	char floatbuf[4] = "";
	sprintf_s(command, "MG @AN[%d];", channel);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);

	//Load the value and 2 decimal places into floatbuf for conversion into a float
	for (int i = 0; i < 4; i++) {
		floatbuf[i] = ReadBuffer[i + 1];
	}

	return std::stof(floatbuf);
}

void Galil::AnalogOutput(uint8_t channel, double voltage) {
	char command[COMMAND_LENGTH];
	sprintf_s(command, "AO%d,%f;", channel, voltage);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
}

void Galil::AnalogInputRange(uint8_t channel, uint8_t range) {
	//arguments are channel, then the range (1 = +/- 5v, 2= +/- 10v, 3 = 0-5v, 4 = 0-10v)
	//e.g. AQ0,1 sets channel 0 to range +/-5v
	char command[COMMAND_LENGTH];
	sprintf_s(command, "AQ%d,%d;", channel, range);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
}



// ENCODER / CONTROL FUNCTIONS
void Galil::WriteEncoder() {
	//User WE, doesnt return bytes.
	//Only use channel 0 which is default as start.

	// WE0 sets channel 0 encoder to 0
	// WE10 sets channel 0 encoder to 10
	Functions->GCommand(g, "WE0,0;", ReadBuffer, sizeof(ReadBuffer), 0);

}

int Galil::ReadEncoder() {
	//USE QE, returns some bytes.
	//QE0 returns encoder value of channel 0
	Functions->GCommand(g, "QE0;", ReadBuffer, sizeof(ReadBuffer), 0);
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
	char info[READBUF_LENGTH];
	char ver[READBUF_LENGTH];
	galil.Functions->GInfo(galil.g, info, sizeof(info));
	galil.Functions->GVersion(ver, sizeof(ver));
	output << info << "\n\n";
	output << ver << "\n\n";
	return output;
}
