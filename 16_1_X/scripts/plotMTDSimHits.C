// plotMTDSimHits.C
// Usage: root -l -b -q 'plotMTDSimHits.C("step2-fast.root")'

#include <fstream>
#include <sstream>
#include <map>
#include <tuple>

#include "TFile.h"
#include "TCanvas.h"
#include "TPolyMarker3D.h"
#include "TEllipse.h"
#include "TLine.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TMath.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TMarker.h"

#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "FWCore/FWLite/interface/FWLiteEnabler.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"

// ----------------------------
// Load DetId → global position from testMTDinDDD.log
// ----------------------------
std::map<uint32_t, std::tuple<double,double,double>>
loadMTDPositions(const char* logfile)
{
    std::map<uint32_t, std::tuple<double,double,double>> posMap;
    std::ifstream f(logfile);
    if (!f.is_open()) {
        std::cerr << "Cannot open " << logfile << std::endl;
        return posMap;
    }
    std::string line;
    while (std::getline(f, line)) {
        std::istringstream ss(line);
        uint32_t detid;
        double x1, y1, z1, x2, y2, z2;
        if (!(ss >> detid >> x1 >> y1 >> z1 >> x2 >> y2 >> z2)) continue;
        double cx = (x1 + x2) / 2. / 10.;  // mm → cm
        double cy = (y1 + y2) / 2. / 10.;
        double cz = (z1 + z2) / 2. / 10.;
        posMap[detid] = {cx, cy, cz};
    }
    std::cout << "Loaded " << posMap.size() << " MTD DetId positions" << std::endl;
    return posMap;
}

void plotMTDSimHits(const char* filename  = "step2-fast.root",
                    const char* logfile   = "testMTDinDDD.log")
{
    FWLiteEnabler::enable();

    // load position map
    auto posMap = loadMTDPositions(logfile);

    // ----------------------------
    // Geometry constants
    // ----------------------------
    const double btlR    = 115.8;
    const double btlZmax = 260.0;
    const double etlZmin = 299.5;
    const double etlZmax = 303.1;
    const double etlRmin =  31.0;
    const double etlRmax = 120.0;

    // ----------------------------
    // Histograms
    // ----------------------------
    TH2F* hRZ  = new TH2F("hRZ",  "MTD SimHits r-z;z [cm];r [cm]",
                            640, -320., 320., 130, 0., 130.);
    TH2F* hBTL = new TH2F("hBTL", "BTL SimHits x-y;x [cm];y [cm]",
                            260, -130., 130., 260, -130., 130.);
    TH2F* hETL_pos = new TH2F("hETL_pos", "ETL SimHits x-y (z>0);x [cm];y [cm]",
                                260, -130., 130., 260, -130., 130.);
    TH2F* hETL_neg = new TH2F("hETL_neg", "ETL SimHits x-y (z<0);x [cm];y [cm]",
                                260, -130., 130., 260, -130., 130.);

    // track trajectory storage
//    std::vector<std::tuple<double,double,double,double,double,double>> tracks_rz;
    std::vector<std::tuple<double,double,double,double>> tracks_rz;

    // (vz, vr, hz, hr) and (vx, vy, hx, hy)
    std::vector<std::tuple<double,double,double,double>> tracks_xy;

    // ----------------------------
    // Event loop
    // ----------------------------
    fwlite::Event ev(TFile::Open(filename));
    fwlite::Handle<edm::SimTrackContainer>  hTrk;
    fwlite::Handle<edm::SimVertexContainer> hVtx;
    fwlite::Handle<edm::PSimHitContainer>   hBTLh, hETLh;

    for (ev.toBegin(); !ev.atEnd(); ++ev)
    {
        hTrk .getByLabel(ev, "fastSimProducer", "");
        hVtx .getByLabel(ev, "fastSimProducer", "");
        hBTLh.getByLabel(ev, "fastSimProducer", "FastSimMTDBarrel");
        hETLh.getByLabel(ev, "fastSimProducer", "FastSimMTDEndcap");

        const auto& simTracks   = *hTrk .product();
        const auto& simVertices = *hVtx .product();

        // SimTrack trajectories
        for (const auto& trk : simTracks)
        {
            if (trk.charge() == 0) continue;
            unsigned int vi = trk.vertIndex();
            if (vi >= simVertices.size()) continue;

            const auto& vtx = simVertices[vi];
            double vx = vtx.position().X();
            double vy = vtx.position().Y();
            double vz = vtx.position().Z();
            double vr = std::sqrt(vx*vx + vy*vy);

            double px = trk.momentum().X();
            double py = trk.momentum().Y();
            double pz = trk.momentum().Z();

            // propagate straight line to BTL radius
            double a = px*px + py*py;
            double b = 2.*(vx*px + vy*py);
            double c = vx*vx + vy*vy - btlR*btlR;
            double disc = b*b - 4.*a*c;
            if (disc < 0 || a == 0) continue;
            double t = (-b + std::sqrt(disc)) / (2.*a);
            if (t <= 0) continue;

            double hx = vx + px*t;
            double hy = vy + py*t;
            double hz = vz + pz*t;
            double hr = std::sqrt(hx*hx + hy*hy);

			tracks_rz.push_back(std::make_tuple(vz, vr, hz, hr));
			tracks_xy.push_back(std::make_tuple(vx, vy, hx, hy));
        }

        // BTL hits — use local position (global coords stored as local)
        if (hBTLh.isValid())
        {
            for (const auto& hit : *hBTLh.product())
            {
                if (hit.detUnitId() == 0) continue;
                double x = hit.localPosition().x();
                double y = hit.localPosition().y();
                double z = hit.localPosition().z();
                double r = std::sqrt(x*x + y*y);
                hRZ ->Fill(z, r);
                hBTL->Fill(x, y);
            }
        }

        // ETL hits — use posMap for global position
        if (hETLh.isValid())
        {
            for (const auto& hit : *hETLh.product())
            {
                uint32_t detid = hit.detUnitId();
                if (detid == 0) continue;

                // ETL DetId matches directly in log
                auto it = posMap.find(detid);
                if (it == posMap.end()) continue;
				double gx = std::get<0>(it->second);
				double gy = std::get<1>(it->second);
				double gz = std::get<2>(it->second);

//                auto [gx, gy, gz] = it->second;
                double gr = std::sqrt(gx*gx + gy*gy);

                hRZ->Fill(gz, gr);
                if (gz > 0) hETL_pos->Fill(gx, gy);
                else        hETL_neg->Fill(gx, gy);
            }
        }
    }

    // ----------------------------
    // Drawing
    // ----------------------------
    gStyle->SetOptStat(0);
    gStyle->SetPalette(kBird);

    // ── r-z view ──────────────────────────────────
    TCanvas* cRZ = new TCanvas("cRZ", "MTD SimHits r-z", 1200, 600);
    cRZ->SetGrid();
    hRZ->Draw("COLZ");

    // BTL boundary
    auto drawLine = [](double x1, double y1, double x2, double y2,
                       int col, int style) {
        TLine* l = new TLine(x1, y1, x2, y2);
        l->SetLineColor(col); l->SetLineWidth(2); l->SetLineStyle(style);
        l->Draw(); return l;
    };

    TLine* lBTL = drawLine(-btlZmax, btlR,  btlZmax, btlR,  kRed,  2);
    drawLine(-btlZmax, 110., -btlZmax, 122., kRed, 2);
    drawLine( btlZmax, 110.,  btlZmax, 122., kRed, 2);

    // ETL boundaries
    TLine* lETL = nullptr;
    for (double z : {etlZmin, etlZmax, -etlZmin, -etlZmax})
    {
        lETL = drawLine(z, etlRmin, z, etlRmax, kBlue, 2);
    }

    // SimTrack trajectories
    TLine* lTrk = nullptr;
//    for (const auto& [vz, vr, hz, hr] : tracks_rz)
	for (const auto& t : tracks_rz)
    {
	    lTrk = new TLine(std::get<0>(t), std::get<1>(t),
                     std::get<2>(t), std::get<3>(t));
    lTrk->SetLineColor(kGreen+2); lTrk->SetLineWidth(1);
    lTrk->Draw();
    }

    TLegend* legRZ = new TLegend(0.70, 0.75, 0.88, 0.88);
    if (lBTL) legRZ->AddEntry(lBTL, "BTL boundary", "l");
    if (lETL) legRZ->AddEntry(lETL, "ETL boundary", "l");
    if (lTrk) legRZ->AddEntry(lTrk, "SimTrack",     "l");
    legRZ->Draw();

    cRZ->SaveAs("mtd_rz.png");

    // ── BTL x-y view ──────────────────────────────
    TCanvas* cBTL = new TCanvas("cBTL", "BTL x-y", 800, 800);
    cBTL->SetGrid();
    hBTL->Draw("COLZ");

    TEllipse* eBTL = new TEllipse(0., 0., btlR, btlR);
    eBTL->SetFillStyle(0); eBTL->SetLineColor(kRed);
    eBTL->SetLineWidth(2); eBTL->SetLineStyle(2);
    eBTL->Draw("same");

//    for (const auto& [vx, vy, hx, hy] : tracks_xy)
	for (const auto& t : tracks_xy)
    {
    TLine* l = new TLine(std::get<0>(t), std::get<1>(t),
                         std::get<2>(t), std::get<3>(t));
    l->SetLineColor(kGreen+2); l->SetLineWidth(1);
    l->Draw();
    }

    cBTL->SaveAs("mtd_btl_xy.png");

    // ── ETL x-y views ─────────────────────────────
    TCanvas* cETL = new TCanvas("cETL", "ETL x-y", 1600, 800);
    cETL->Divide(2, 1);

    auto drawETL = [&](TVirtualPad* pad, TH2F* h, const char* title) {
        pad->cd(); pad->SetGrid();
        h->SetTitle(title);
        h->Draw("COLZ");
        for (auto* e : {new TEllipse(0.,0.,etlRmin,etlRmin),
                        new TEllipse(0.,0.,etlRmax,etlRmax)}) {
            e->SetFillStyle(0); e->SetLineColor(kBlue);
            e->SetLineWidth(2); e->SetLineStyle(2);
            e->Draw("same");
        }
    };

    drawETL(cETL->cd(1), hETL_pos, "ETL SimHits x-y (z>0);x [cm];y [cm]");
    drawETL(cETL->cd(2), hETL_neg, "ETL SimHits x-y (z<0);x [cm];y [cm]");

    cETL->SaveAs("mtd_etl_xy.png");

    std::cout << "Saved: mtd_rz.png, mtd_btl_xy.png, mtd_etl_xy.png" << std::endl;
}
