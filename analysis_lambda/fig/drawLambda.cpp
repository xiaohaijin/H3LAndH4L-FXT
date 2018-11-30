#include "TAxis.h"
#include "TArrow.h"
#include "TAttFill.h"
#include "TBranch.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TColor.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TF2.h"
#include "TFile.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH2D.h"
#include "TH3.h"
#include "TH3D.h"
#include "TLatex.h"
#include "TLeaf.h"
#include "TLegend.h"
#include "TLine.h"
#include "TMarker.h"
#include "TMath.h"
#include "TNamed.h"
#include "TPad.h"
#include "TProfile.h"
#include "TRandom.h"
#include "TSpectrum.h"
#include "TStyle.h"
#include "TTree.h"

using namespace std;

int drawLambda()
{

  TCanvas *canvas = new TCanvas("canvas", "canvas", 0, 0, 800, 600);
  canvas->SetTopMargin(0.07);
  canvas->SetRightMargin(0.07);
  canvas->SetBottomMargin(0.07);
  canvas->SetLeftMargin(0.10);

  TFile *file = new TFile("../result.root");
  TH1D *v0Mass = static_cast<TH1D*>(file->Get("v0Mass"));
  v0Mass->SetTitle("#Lambda and #bar{#Lambda}");
  v0Mass->Draw("E");
  
  return 0;
}
