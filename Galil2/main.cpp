#include "Galil.h"
#include <bitset>
#include <conio.h>

int main() {
	EmbeddedFunctions* Funcs = new EmbeddedFunctions;
	Galil* galilTasks = new Galil(Funcs, "192.168.0.120 -d");
	
	//Task 9
	char ledChar;
	//Clear all outputs

	galilTasks->DigitalOutput(0);
	while (ledChar != 'q' || ledChar != 'Q') {
		ledChar = _getch();
		galilTasks->DigitalByteOutput(1, 0b00000001);
		switch (ledChar) {
		case '1': galilTasks->DigitalOutput(0b00000001); galilTasks->DigitalByteOutput(1, 0b00000001); break;
		case '2': galilTasks->DigitalOutput(0b00000010); galilTasks->DigitalByteOutput(1, 0b00000010); break;
		case '3': galilTasks->DigitalOutput(0b00000100); galilTasks->DigitalByteOutput(1, 0b00000100); break;
		case '4': galilTasks->DigitalOutput(0b00001000); galilTasks->DigitalByteOutput(1, 0b00001000); break;
		case '5': galilTasks->DigitalOutput(0b00010000); galilTasks->DigitalByteOutput(1, 0b00010000); break;
		case '6': galilTasks->DigitalOutput(0b00100000); galilTasks->DigitalByteOutput(1, 0b00100000); break;
		case '7': galilTasks->DigitalOutput(0b01000000); galilTasks->DigitalByteOutput(1, 0b01000000); break;
		case '8': galilTasks->DigitalOutput(0b10000000); galilTasks->DigitalByteOutput(1, 0b10000000); break;
		}

	}



	_getch();
	//Task 12
	//Stop motor
	char exitChar;
	while (exitChar != 'q' || exitChar != 'Q') {
		galilTasks->AnalogOutput(0, 0);

		//Reset encoder
		galilTasks->WriteEncoder();

		//Turn 1 rev
		galilTasks->setSetPoint(5000);
		galilTasks->setKp(1);
		galilTasks->setKi(0.0001);
		galilTasks->setKd(0.25);
		galilTasks->PositionControl(1,0);

		//Wait 200ms
		Sleep(200);

		//Return to start
		galilTasks->setSetPoint(0);
		galilTasks->PositionControl(1, 0);

		Sleep(200);

		exitChar = _getch();
	}
	
	return 0;
}