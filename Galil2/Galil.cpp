#include "Galil.h"

int main() {

}


Galil::Galil() {

}

Galil::Galil(EmbeddedFunctions* Funcs, GCStringIn address) {

}

Galil::~Galil() {

}



// DIGITAL OUTPUTS
void Galil::DigitalOutput(uint16_t value) {

}

void Galil::DigitalByteOutput(bool bank, uint8_t value) {

}

void Galil::DigitalBitOutput(bool val, uint8_t bit) {

}



// DIGITAL INPUTS
uint16_t Galil::DigitalInput() {

}

uint8_t Galil::DigitalByteInput(bool bank) {

}

bool Galil::DigitalBitInput(uint8_t bit) {

}

bool Galil::CheckSuccessfulWrite() {

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

}

void Galil::setKp(double gain) {

}

void Galil::setKi(double gain) {

}

void Galil::setKd(double gain) {

}

void Galil::PositionControl(bool debug, int Motorchannel) {

}

void Galil::SpeedControl(bool debug, int Motorchannel) {

}
