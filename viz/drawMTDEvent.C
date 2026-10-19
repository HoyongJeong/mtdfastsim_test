#include <algorithm>
#include <cmath>
#include <vector>

#include "TCanvas.h"
#include "TFile.h"
#include "TLatex.h"
#include "TPad.h"
#include "TPolyLine.h"
#include "TStyle.h"
#include "TTree.h"

// ---- display settings (cm) ----
const double kBoxHalf = 2.0;      // half size of the cube drawn for a sim hit
const double kCmPerGeV = 50.;     // arrow length per GeV of |p|
const double kMaxLen = 340.;      // arrow length cap
const double kZRange = 400.;      // half range of the horizontal (z) axis
const double kDepth = 0.30;       // oblique depth factor for x (0 = pure y-z side view)
const double kDepthAngle = 30.;   // deg, direction in which the depth (x) axis is drawn
const double kAzimuth = 0.;       // deg, rotation of the view about the beam axis
const int kCanvasW = 1800;
const int kCanvasH =  500;

// (x, y, z) in CMS coordinates -> pad coordinates (sx horizontal = z, sy vertical)
void project(double x, double y, double z, double& sx, double& sy) {
	const double a = kAzimuth * M_PI / 180.;
	const double d = kDepthAngle * M_PI / 180.;
	const double xr = x * std::cos(a) - y * std::sin(a);   // depth coordinate
	const double yr = x * std::sin(a) + y * std::cos(a);   // vertical coordinate
	sx = z + kDepth * xr * std::cos(d);
	sy = yr + kDepth * xr * std::sin(d);
}

void line(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z, int color, int style = 1, int width = 1) {
	std::vector<double> sx(x.size()), sy(x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		project(x[i], y[i], z[i], sx[i], sy[i]);
	}
	auto* pl = new TPolyLine(x.size(), sx.data(), sy.data());
	pl->SetLineColor(color);
	pl->SetLineStyle(style);
	pl->SetLineWidth(width);
	pl->Draw();
}

void ring(double r, double z, int color) {
	const int n = 72;
	std::vector<double> x(n + 1), y(n + 1), zz(n + 1, z);
	for (int i = 0; i <= n; ++i) {
		x[i] = r * std::cos(2 * M_PI * i / n);
		y[i] = r * std::sin(2 * M_PI * i / n);
	}
	line(x, y, zz, color);
}

void box(double x, double y, double z, double h, int color) {
	// 12 edges of a cube as one continuous polyline
	const double c[8][3] = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
	                        {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1}};
	const int path[] = {0, 1, 2, 3, 0, 4, 5, 6, 7, 4, 5, 1, 2, 6, 7, 3};
	std::vector<double> bx, by, bz;
	for (int i : path) {
		bx.push_back(x + h * c[i][0]);
		by.push_back(y + h * c[i][1]);
		bz.push_back(z + h * c[i][2]);
	}
	line(bx, by, bz, color);
}

void drawDetector() {
	const int grey = kGray + 1;
	// beam axis
	line({0., 0.}, {0., 0.}, {-kZRange, kZRange}, kGray + 2, 2);
	// BTL: cylinder r = 115.8 cm, |z| < 260 cm
	for (double z : {-260., 260.}) {
		ring(115.8, z, grey);
	}
	for (int i = 0; i < 12; ++i) {
		const double a = 2 * M_PI * i / 12;
		line({115.8 * std::cos(a), 115.8 * std::cos(a)}, {115.8 * std::sin(a), 115.8 * std::sin(a)}, {-260., 260.}, grey, 3);
	}
	// ETL: discs r = 31 - 120 cm at |z| ~ 300 cm (rough positions, drawing only)
	for (double z : {-302.5, -300.1, 300.1, 302.5}) {
		ring(31., z, grey);
		ring(120., z, grey);
	}
}

void drawOne(const char* fname, const char* title, int entry) {
	TFile* f = TFile::Open(fname);
	if (!f || f->IsZombie()) {
		printf("cannot open %s\n", fname);
		return;
	}
	auto* t = static_cast<TTree*>(f->Get("dump/events"));
	if (!t) {
		printf("tree dump/events not found in %s\n", fname);
		return;
	}
	if (entry < 0 || entry >= t->GetEntries()) {
		printf("entry %d out of range (0-%lld) in %s\n", entry, t->GetEntries() - 1, fname);
		return;
	}
	// note: the dumper writes gPdg, gQ, hSub as int and the others as float
	std::vector<int> *gPdg = nullptr, *gQ = nullptr, *hSub = nullptr;
	std::vector<float> *gPx = nullptr, *gPy = nullptr, *gPz = nullptr;
	std::vector<float> *gVx = nullptr, *gVy = nullptr, *gVz = nullptr;
	std::vector<float> *hX = nullptr, *hY = nullptr, *hZ = nullptr;
	t->SetBranchAddress("gPdg", &gPdg);
	t->SetBranchAddress("gQ", &gQ);
	t->SetBranchAddress("gPx", &gPx);
	t->SetBranchAddress("gPy", &gPy);
	t->SetBranchAddress("gPz", &gPz);
	t->SetBranchAddress("gVx", &gVx);
	t->SetBranchAddress("gVy", &gVy);
	t->SetBranchAddress("gVz", &gVz);
	t->SetBranchAddress("hSub", &hSub);
	t->SetBranchAddress("hX", &hX);
	t->SetBranchAddress("hY", &hY);
	t->SetBranchAddress("hZ", &hZ);
	t->GetEntry(entry);

	gPad->SetFillColor(kBlack);
	gPad->SetBorderMode(0);
	gPad->SetMargin(0., 0., 0., 0.);
	const double aspect = static_cast<double>(kCanvasH) / (kCanvasW / 2.);   // pad height / pad width
	gPad->Range(-kZRange, -kZRange * aspect, kZRange, kZRange * aspect);

	drawDetector();

	// primary particles: initial momentum direction, Geant4 colors
	for (size_t i = 0; i < gPdg->size(); ++i) {
		const double px = (*gPx)[i], py = (*gPy)[i], pz = (*gPz)[i];
		const double p = std::sqrt(px * px + py * py + pz * pz);
		if (p <= 0.)
			continue;
		const double len = std::min(kMaxLen, kCmPerGeV * p);
		const int q = (*gQ)[i];
		const int color = (q < 0) ? kRed : (q > 0 ? kBlue + 1 : kGreen);
		const double x0 = (*gVx)[i], y0 = (*gVy)[i], z0 = (*gVz)[i];
		line({x0, x0 + len * px / p}, {y0, y0 + len * py / p}, {z0, z0 + len * pz / p}, color, 1, 1);
	}

	// MTD sim hits as small cubes
	int nBtl = 0, nEtl = 0;
	for (size_t i = 0; i < hSub->size(); ++i) {
		const bool btl = ((*hSub)[i] == 1);
		box((*hX)[i], (*hY)[i], (*hZ)[i], kBoxHalf, btl ? kCyan : kOrange);
		(btl ? nBtl : nEtl)++;
	}

	// labels
	auto* label = new TLatex(0.015, 0.90, Form("%s  BTL hits: %d, ETL hits: %d", title, nBtl, nEtl));
	label->SetNDC();
	label->SetTextColor(kWhite);
	label->SetTextSize(0.07);
	label->Draw();

	auto* zlabel = new TLatex(0.975, 0.50, "z");
	zlabel->SetNDC();
	zlabel->SetTextAlign(32);
	zlabel->SetTextColor(kGray + 1);
	zlabel->SetTextSize(0.07);
	zlabel->Draw();
}

void drawMTDEvent(const char* fullFile = "hits_full.root", const char* fastFile = "hits_fast.root", int entry = 0) {
	gStyle->SetCanvasColor(kBlack);
	auto* c = new TCanvas("c", "MTD sim hits", kCanvasW, kCanvasH);
	c->SetFillColor(kBlack);
	c->Divide(2, 1, 0., 0.);
	c->cd(1);
	drawOne(fullFile, "FullSim", entry);
	c->cd(2);
	drawOne(fastFile, "FastSim", entry);
	c->SaveAs(Form("mtd_event_%d.png", entry));
}
