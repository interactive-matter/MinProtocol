#include <stdio.h>

#include "MinProtocol.h"


MinProtocol::MinProtocol(Stream & ccomms) {
	comms = &ccomms;
}