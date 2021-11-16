#include "Galil.h"

int main() {

	EmbeddedFunctions Funcs;
	Galil* myGalil = new Galil(&Funcs, "192.168.0.120 -d");
	GOpen("192.168.0.120 -d", &myGalil.g);

	if (g != 0) {

	}

}