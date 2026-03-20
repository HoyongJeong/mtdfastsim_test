#include <iostream>
#include <vector>
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"

void readMTD(const char* filename = "step1.root") {
    // 1. CMSSW 라이브러리 로드 (매크로 실행 시 필수)
    gSystem->Load("libDataFormatsFWLite.so");
    FWLiteEnabler::enable();

    TFile* file = TFile::Open(filename);
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }

    fwlite::Event event(file);

    // 2. 이벤트 루프 (첫 번째 이벤트만 확인)
    int iEvent = 0;
    for (event.toBegin(); !event.atEnd(); ++event) {
        std::cout << "\n>>>> Processing Event: " << ++iEvent << " <<<<" << std::endl;

        // BTL(Barrel) Hits 읽기
        fwlite::Handle<std::vector<PSimHit>> btlHits;
        btlHits.getByLabel(event, "g4SimHits", "FastTimerHitsBarrel");

        if (btlHits.isValid()) {
            std::cout << " [BTL] Number of Hits: " << btlHits->size() << std::endl;
            for (size_t i = 0; i < btlHits->size(); ++i) {
                const auto& hit = (*btlHits)[i];
                printf("  Hit %3lu | PDG: %5d | Time: %6.3f ns | Eloss: %6.2f keV | Pos: (%5.2f, %5.2f)\n", 
                       i, hit.particleType(), hit.tof(), hit.energyLoss()*1e6, 
                       hit.localPosition().x(), hit.localPosition().y());
                
                if (i > 10) { std::cout << "  ... (only showing first 10 hits)" << std::endl; break; }
            }
        }

        // ETL(Endcap) Hits 읽기
        fwlite::Handle<std::vector<PSimHit>> etlHits;
        etlHits.getByLabel(event, "g4SimHits", "FastTimerHitsEndcap");

        if (etlHits.isValid()) {
            std::cout << " [ETL] Number of Hits: " << etlHits->size() << std::endl;
            for (size_t i = 0; i < etlHits->size(); ++i) {
                const auto& hit = (*etlHits)[i];
                printf("  Hit %3lu | PDG: %5d | Time: %6.3f ns | Eloss: %6.2f keV | Pos: (%5.2f, %5.2f)\n", 
                       i, hit.particleType(), hit.tof(), hit.energyLoss()*1e6, 
                       hit.localPosition().x(), hit.localPosition().y());
                
                if (i > 10) { std::cout << "  ... (only showing first 10 hits)" << std::endl; break; }
            }
        }

        // 테스트를 위해 첫 번째 이벤트만 보고 종료
        break; 
    }

    file->Close();
}
