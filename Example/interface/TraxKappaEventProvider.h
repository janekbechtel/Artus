
#pragma once

#include "Artus/KappaAnalysis/interface/KappaEventProviderBase.h"

#include "TraxTypes.h"

/*
 * Will load the corresponding ntuple from a root file
 * The memory locations are passed to ROOT one time, in the
 * WireEvent() method call.
 */

class TraxKappaEventProvider: public KappaEventProviderBase<TraxTypes> {
public:
	TraxKappaEventProvider(FileInterface2 & fi, InputTypeEnum inpType);

};
