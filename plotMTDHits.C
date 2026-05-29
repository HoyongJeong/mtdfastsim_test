// plotMTDHits.C
// Usage: root -l -b -q 'plotMTDHits.C("step2-fast.root")'

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TEllipse.h"
#include "TLine.h"
#include "TLatex.h"

// FWLite
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "FWCore/FWLite/interface/FWLiteEnabler.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"

void plotMTDHits(const char* filename = "step2-fast.root") {

    FWLiteEnabler::enable();

    // ----------------------------
    // Histograms
    // ----------------------------

    // BTL: r-z plane (실린더이므로 r은 일정, z vs phi)
    TH2F* hBTL_xy   = new TH2F("hBTL_xy",   "BTL hits (x-y);x [cm];y [cm]",
                                 300, -130., 130., 300, -130., 130.);
    TH2F* hBTL_rz   = new TH2F("hBTL_rz",   "BTL hits (r-z);z [cm];r [cm]",
                                 300, -270., 270., 100, 110., 122.);
    TH2F* hBTL_zphi = new TH2F("hBTL_zphi", "BTL hits (z-#phi);z [cm];#phi [rad]",
                                 300, -270., 270., 100, -3.15, 3.15);

    // ETL: x-y plane (디스크이므로 z는 일정)
    TH2F* hETL_xy_pos = new TH2F("hETL_xy_pos", "ETL hits z>0 (x-y);x [cm];y [cm]",
                                   300, -130., 130., 300, -130., 130.);
    TH2F* hETL_xy_neg = new TH2F("hETL_xy_neg", "ETL hits z<0 (x-y);x [cm];y [cm]",
                                   300, -130., 130., 300, -130., 130.);
    TH2F* hETL_rz     = new TH2F("hETL_rz",     "ETL hits (r-z);z [cm];r [cm]",
                                   100, 280., 310., 130, 25., 130.);

    // 3D: BTL + ETL 합쳐서
    TH3F* h3D = new TH3F("h3D", "MTD hits 3D;x [cm];y [cm];z [cm]",
                           60, -130., 130.,
                           60, -130., 130.,
                           60, -300., 300.);

    // ----------------------------
    // Event loop
    // ----------------------------
    fwlite::Event event(TFile::Open(filename));
    fwlite::Handle<edm::PSimHitContainer> hBTL, hETL;

    int nEvents = 0;
    for (event.toBegin(); !event.atEnd(); ++event) {
        nEvents++;

        hBTL.getByLabel(event, "mtdSimHitProducer", "FastSimMTDBarrel");
        hETL.getByLabel(event, "mtdSimHitProducer", "FastSimMTDEndcap");

        // BTL
        if (hBTL.isValid()) {
            for (const auto& hit : *hBTL.product()) {
                // PSimHit의 localPosition()은 local frame
                // 현재 구현에서는 global coords를 local로 넣었으므로 그대로 사용
                float x   = hit.localPosition().x();
                float y   = hit.localPosition().y();
                float z   = hit.localPosition().z();
                float r   = std::sqrt(x*x + y*y);
                float phi = std::atan2(y, x);

                hBTL_xy  ->Fill(x, y);
                hBTL_rz  ->Fill(z, r);
                hBTL_zphi->Fill(z, phi);
                h3D      ->Fill(x, y, z);
            }
        }

        // ETL
        if (hETL.isValid()) {
            for (const auto& hit : *hETL.product()) {
                float x = hit.localPosition().x();
                float y = hit.localPosition().y();
                float z = hit.localPosition().z();
                float r = std::sqrt(x*x + y*y);

                hETL_rz->Fill(z, r);
                h3D    ->Fill(x, y, z);

                if (z > 0) hETL_xy_pos->Fill(x, y);
                else       hETL_xy_neg->Fill(x, y);
            }
        }
    }

    std::cout << "Processed " << nEvents << " event(s)" << std::endl;

    // ----------------------------
    // Drawing
    // ----------------------------
    gStyle->SetOptStat(0);
    gStyle->SetPalette(kBird);

    // Canvas 1: BTL
    TCanvas* cBTL = new TCanvas("cBTL", "BTL Hits", 1400, 500);
    cBTL->Divide(3, 1);

    cBTL->cd(1);
    hBTL_xy->Draw("COLZ");
    // BTL 반경 원 표시
    TEllipse* eBTL = new TEllipse(0, 0, 116., 116.);
    eBTL->SetFillStyle(0);
    eBTL->SetLineColor(kRed);
    eBTL->SetLineWidth(2);
//    eBTL->Draw("same");

    cBTL->cd(2);
    hBTL_rz->Draw("COLZ");

    cBTL->cd(3);
    hBTL_zphi->Draw("COLZ");

    cBTL->SaveAs("btl_hits.png");

    // Canvas 2: ETL
    TCanvas* cETL = new TCanvas("cETL", "ETL Hits", 1200, 500);
    cETL->Divide(3, 1);

    cETL->cd(1);
    hETL_xy_pos->Draw("COLZ");
    TLatex* ltPos = new TLatex(-110, 115, "z > 0");
    ltPos->SetTextSize(0.05);
    ltPos->Draw();

    cETL->cd(2);
    hETL_xy_neg->Draw("COLZ");
    TLatex* ltNeg = new TLatex(-110, 115, "z < 0");
    ltNeg->SetTextSize(0.05);
    ltNeg->Draw();

    cETL->cd(3);
    hETL_rz->Draw("COLZ");

    cETL->SaveAs("etl_hits.png");

    // Canvas 3: 3D
    TCanvas* c3D = new TCanvas("c3D", "MTD Hits 3D", 800, 800);
    h3D->Draw("BOX2");
    c3D->SaveAs("mtd_hits_3d.png");

    // Canvas 4: r-z 통합 (BTL + ETL 같이)
    TCanvas* cRZ = new TCanvas("cRZ", "MTD r-z view", 900, 600);
    TH2F* hRZ = new TH2F("hRZ", "MTD hits r-z (all);|z| [cm];r [cm]",
                           320, 0., 320., 130, 0., 130.);
    hRZ->SetStats(0);

    // BTL를 |z|로 다시 채우기
    for (int bx = 1; bx <= hBTL_rz->GetNbinsX(); bx++) {
        for (int by = 1; by <= hBTL_rz->GetNbinsY(); by++) {
            double val = hBTL_rz->GetBinContent(bx, by);
            if (val > 0) {
                double z = std::abs(hBTL_rz->GetXaxis()->GetBinCenter(bx));
                double r = hBTL_rz->GetYaxis()->GetBinCenter(by);
                hRZ->Fill(z, r, val);
            }
        }
    }
    // ETL를 |z|로 다시 채우기
    for (int bx = 1; bx <= hETL_rz->GetNbinsX(); bx++) {
        for (int by = 1; by <= hETL_rz->GetNbinsY(); by++) {
            double val = hETL_rz->GetBinContent(bx, by);
            if (val > 0) {
                double z = std::abs(hETL_rz->GetXaxis()->GetBinCenter(bx));
                double r = hETL_rz->GetYaxis()->GetBinCenter(by);
                hRZ->Fill(z, r, val);
            }
        }
    }

    hRZ->Draw("COLZ");

    // BTL 영역 박스
    TLine* lBTL_r1 = new TLine(0,   116., 260., 116.);
    TLine* lBTL_r2 = new TLine(0,   116., 260., 116.);
    TLine* lBTL_z1 = new TLine(260., 110., 260., 122.);
    for (auto* l : {lBTL_r1, lBTL_z1}) {
        l->SetLineColor(kRed);
        l->SetLineWidth(2);
        l->SetLineStyle(2);
//        l->Draw();
    }

    // ETL 영역 선
    TLine* lETL = new TLine(290., 31., 290., 120.);
    lETL->SetLineColor(kBlue);
    lETL->SetLineWidth(2);
    lETL->SetLineStyle(2);
//    lETL->Draw();

    TLegend* leg = new TLegend(0.65, 0.75, 0.88, 0.88);
    leg->AddEntry(lBTL_r1, "BTL boundary", "l");
    leg->AddEntry(lETL,    "ETL boundary", "l");
//    leg->Draw();

    cRZ->SaveAs("mtd_hits_rz.png");

    std::cout << "Saved: btl_hits.png, etl_hits.png, mtd_hits_3d.png, mtd_hits_rz.png" << std::endl;
}
