#include "Galil.h"

int main() {

}


Galil::Galil() {

}

Galil::Galil(EmbeddedFunctions* Funcs, GCStringIn address) {
	Functions = Funcs;
}

Galil::~Galil() {

}



// DIGITAL OUTPUTS
void Galil::DigitalOutput(uint16_t value) {
	
}

void Galil::DigitalByteOutput(bool bank, uint8_t value) {
	//
}

void Galil::DigitalBitOutput(bool val, uint8_t bit) {

}



// DIGITAL INPUTS
uint16_t Galil::DigitalInput() {

}

uint8_t Galil::DigitalByteInput(bool bank) {

}

bool Galil::DigitalBitInput(uint8_t bit) {
	//Read a bit from one of the digital inputs specified by bit.
}

bool Galil::CheckSuccessfulWrite() {
	if (success) {
		return 1;
	}
	else {
		return 0;
	}
}

float Galil::AnalogInput(uint8_t channel) {

}

void Galil::AnalogOutput(uint8_t channel, double voltage) {

}

void Galil::AnalogInputRange(uint8_t channel, uint8_t range) {

}



// ENCODER / CONTROL FUNCTIONS
void Galil::WriteEncoder() {

}

int Galil::ReadEncoder() {

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
