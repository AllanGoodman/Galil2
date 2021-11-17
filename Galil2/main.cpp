#include "Galil.h"

//GCon g = 0;

int main() {

	EmbeddedFunctions* Funcs = new EmbeddedFunctions;
	Galil* myGalil = new Galil(Funcs, "192.168.0.120 -d");
	myGalil->DigitalOutput(6);
	std::cout << myGalil->AnalogInput(0) << std::endl;

	//char buf[1024];
	//char command[128] = "";
	//GOpen("192.168.0.120 -d", &g);

	//sprintf_s(command, "AO0,3;");

	//GCommand(g, command, buf, sizeof(buf), 0);
	//while (1);
	//if (g) { GClose(g); }
	system("PAUSE");
	return G_NO_ERROR;
}