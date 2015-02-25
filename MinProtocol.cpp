#include <stdio.h>
#include <MinbProtocol.h>


MinProtocol:MinProtocol(Stream & comms) {
	this->comms = comms;
}