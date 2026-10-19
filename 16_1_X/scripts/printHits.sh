python3 << 'EOF'
import ROOT
ROOT.gSystem.Load("libFWCoreFWLite.so")
ROOT.FWLiteEnabler.enable()

from DataFormats.FWLite import Events, Handle

events = Events("step2-fast.root")
handle = Handle("vector<PSimHit>")

total_btl = 0
total_etl = 0

for event in events:
    event.getByLabel("fastSimProducer", "FastSimMTDBarrel", handle)
    btl = handle.product()
    event.getByLabel("fastSimProducer", "FastSimMTDEndcap", handle)
    etl = handle.product()

    total_btl += btl.size()
    total_etl += etl.size()

    print(f"BTL hits: {btl.size()}, ETL hits: {etl.size()}")

    for i, hit in enumerate(btl):
        print(f"  BTL hit {i}: "
              f"x={hit.localPosition().x():.2f} "
              f"y={hit.localPosition().y():.2f} "
              f"z={hit.localPosition().z():.2f} "
              f"eloss={hit.energyLoss():.6f} "
              f"tof={hit.timeOfFlight():.3f} ns "
              f"p={hit.pabs():.3f} GeV "
              f"pdg={hit.particleType()}")

    for i, hit in enumerate(etl):
        print(f"  ETL hit {i}: "
              f"x={hit.localPosition().x():.2f} "
              f"y={hit.localPosition().y():.2f} "
              f"z={hit.localPosition().z():.2f} "
              f"eloss={hit.energyLoss():.6f} "
              f"tof={hit.timeOfFlight():.3f} ns "
              f"p={hit.pabs():.3f} GeV "
              f"pdg={hit.particleType()}")

print(f"\nTotal: BTL={total_btl}, ETL={total_etl}")
EOF
