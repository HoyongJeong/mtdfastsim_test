#define MAXHITS 20

#include <iostream>
#include <vector>
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"

void readMTD(const char* filename = "step1.root", bool isFull = true)
{
	//------------------------------------------------------------------------------
	// Load CMSSW lib
	//------------------------------------------------------------------------------
	gSystem -> Load("libDataFormatsFWLite.so");
	FWLiteEnabler::enable();


	//------------------------------------------------------------------------------
	// Open ROOT file
	//------------------------------------------------------------------------------
	TFile* file = TFile::Open(filename);
	if ( ! file || file -> IsZombie() )
	{
		std::cerr << "Error: Cannot open file " << filename << std::endl;
		return;
	}

	fwlite::Event event(file);



	//------------------------------------------------------------------------------
	// Event loop (Check first event only)
	//------------------------------------------------------------------------------
	int iEvent = 0;
	for ( event . toBegin(); ! event . atEnd(); ++event )
	{
		std::cout << "Processing Event: " << ++iEvent << "..." << std::endl;

		//----------------------------------------------------------
		// Read BTL hits
		//----------------------------------------------------------
		fwlite::Handle<std::vector<PSimHit>> btlHits;
		if ( isFull ) btlHits . getByLabel(event, "g4SimHits"        , "FastTimerHitsBarrel"         );
		else          btlHits . getByLabel(event, "mtdSimHitProducer", "FastTimerHitsBarrelByFastsim");

		if ( btlHits . isValid() )
		{
			std::cout << " [BTL] Number of Hits: " << btlHits -> size() << std::endl;
			for ( size_t i = 0; i < btlHits -> size(); ++i )
			{
				const auto& hit = (*btlHits)[i];
				if ( hit . detUnitId() == 1661347213 )
				printf("  Hit %3lu | PDG: %5d | Time: %6.3f ns | Eloss: %6.2f keV | Pos: (%5.2f, %5.2f) | DetId: %d\n", 
					   i, hit . particleType(), hit . tof(), hit . energyLoss() * 1.e6, 
					   hit . localPosition() . x(), hit . localPosition() . y(), hit . detUnitId());

//				if ( i > MAXHITS )
//				{
//					std::cout << "  ... (only showing first " << MAXHITS << " hits)" << std::endl;
//					break;
//				}
			}
		}

		//----------------------------------------------------------
		// Real ETL hits
		//----------------------------------------------------------
		fwlite::Handle<std::vector<PSimHit>> etlHits;
		if ( isFull) etlHits . getByLabel(event, "g4SimHits"        , "FastTimerHitsEndcap"         );
		else         etlHits . getByLabel(event, "mtdSimHitProducer", "FastTimerHitsEndcapByFastsim");

		if ( etlHits . isValid() )
		{
			std::cout << " [ETL] Number of Hits: " << etlHits -> size() << std::endl;
			for ( size_t i = 0; i < etlHits -> size(); ++i )
			{
				const auto& hit = (*etlHits)[i];
				printf("  Hit %3lu | PDG: %5d | Time: %6.3f ns | Eloss: %6.2f keV | Pos: (%5.2f, %5.2f) | DetId: %d\n", 
					   i, hit . particleType(), hit . tof(), hit . energyLoss() * 1.e6, 
					   hit . localPosition() . x(), hit . localPosition() . y(), hit . detUnitId());
				
				if ( i > MAXHITS )
				{
					std::cout << "  ... (only showing first " << MAXHITS << " hits)" << std::endl;
					break;
				}
			}
		}

        break; 
    }

    file -> Close();
}
