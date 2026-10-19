python3 << 'EOF'
import ROOT
ROOT.gSystem.Load("libFWCoreFWLite.so")
ROOT.gSystem.Load("libSimDataFormatsTrackingHit.so")
ROOT.gInterpreter.Declare('#include "SimDataFormats/TrackingHit/interface/PSimHit.h"')
ROOT.FWLiteEnabler.enable()

from DataFormats.FWLite import Events, Handle
import math

def decode_btl(detid):
    detector  = (detid >> 28) & 0xF
    subdet    = (detid >> 25) & 0x7
    zside     = (detid >> 24) & 0x1
    newformat = (detid >> 15) & 0x1
    if newformat:
        rod     = (detid >> 16) & 0x3F
        runit   = (detid >> 10) & 0x7
        dmodule = (detid >>  6) & 0xF
        smodule = (detid >>  5) & 0x1
        crystal = (detid >>  0) & 0x1F
        return f"NEW: zside={zside} rod={rod} runit={runit} dmod={dmodule} smod={smodule} crystal={crystal}"
    else:
        rod     = (detid >> 16) & 0x3F
        module  = (detid >> 10) & 0x3F
        modtype = (detid >>  8) & 0x3
        ru      = (detid >>  6) & 0x3
        crystal = (detid >>  0) & 0x3F
        return f"OLD: zside={zside} rod={rod} module={module} modtype={modtype} ru={ru} crystal={crystal}"

def decode_etl(detid):
    detector = (detid >> 28) & 0xF
    subdet   = (detid >> 25) & 0x7
    zside    = (detid >> 24) & 0x1
    disc     = (detid >> 18) & 0x3F
    sector   = (detid >> 15) & 0x7
    module   = (detid >> 10) & 0x1F
    ring     = (detid >>  6) & 0xF
    det      = (detid)       & 0x3F
    return f"zside={zside} disc={disc} sector={sector} module={module} ring={ring} det={det}"

events = Events("step2-fast.root")
handle = Handle("vector<PSimHit>")

for event in events:
    event.getByLabel("fastSimProducer", "FastTimerHitsBarrel", handle)
    btl = handle.product()
    event.getByLabel("fastSimProducer", "FastTimerHitsEndcap", handle)
    etl = handle.product()

    print(f"BTL hits: {btl.size()}, ETL hits: {etl.size()}")
    print()

    print("=== BTL hits ===")
    for i, hit in enumerate(btl):
        lx = hit.localPosition().x()
        ly = hit.localPosition().y()
        lz = hit.localPosition().z()
        r  = math.sqrt(lx**2 + ly**2)
        phi = math.degrees(math.atan2(ly, lx))
        detid = hit.detUnitId()
        print(f"  BTL hit {i}:")
        print(f"    detId={detid}  {decode_btl(detid) if detid != 0 else 'N/A'}")
        print(f"    localPos=({lx:.2f}, {ly:.2f}, {lz:.2f}) mm")
        print(f"    r={r:.2f}mm  phi={phi:.2f}deg")
        print(f"    tof={hit.timeOfFlight():.4f}ns  eloss={hit.energyLoss():.6f}GeV")
        print(f"    p={hit.pabs():.4f}GeV  pdg={hit.particleType()}")
        if i >= 9: break

    print()
    print("=== ETL hits ===")
    for i, hit in enumerate(etl):
        lx = hit.localPosition().x()
        ly = hit.localPosition().y()
        lz = hit.localPosition().z()
        detid = hit.detUnitId()
        print(f"  ETL hit {i}:")
        print(f"    detId={detid}  {decode_etl(detid) if detid != 0 else 'N/A'}")
        print(f"    localPos=({lx:.2f}, {ly:.2f}, {lz:.2f}) mm")
        print(f"    tof={hit.timeOfFlight():.4f}ns  eloss={hit.energyLoss():.6f}GeV")
        print(f"    p={hit.pabs():.4f}GeV  pdg={hit.particleType()}")
        if i >= 9: break
    break
EOF
