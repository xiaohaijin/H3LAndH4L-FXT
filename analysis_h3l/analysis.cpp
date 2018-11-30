// c++
#include <iomanip>
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <cstddef>
#include <vector>
#include <fstream>
#include <sstream>

// root
#include "TH3D.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TRandom.h"
#include "TMath.h"
#include "TSystem.h"
#include "TUnixSystem.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TROOT.h"
#include "TFile.h"
#include "TChain.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGaxis.h"
#include "TPaveStats.h"
#include "TObject.h"
#include "TClonesArray.h"
#include "TRefArray.h"
#include "TRef.h"
#include "TBits.h"

#include "boost/progress.hpp"

#include "V0PicoDst.h"

using namespace std;

const int BINS = 80;
const double lowValue = 2.96;
const double highValue = 3.04;

TH1D* v0Mass = new TH1D("v0Mass", "v0Mass", BINS, lowValue, highValue);
TH1D* v0Mass0 = new TH1D("v0Mass0", "v0Mass0", BINS, lowValue, highValue);
TH1D* v0Mass1 = new TH1D("v0Mass1", "v0Mass1", BINS, lowValue, highValue);
TH1D* v0Mass2 = new TH1D("v0Mass2", "v0Mass2", BINS, lowValue, highValue);
TH1D* v0Mass3 = new TH1D("v0Mass3", "v0Mass3", BINS, lowValue, highValue);
TH1D* v0Mass4 = new TH1D("v0Mass4", "v0Mass4", BINS, lowValue, highValue);
TH1D* v0Mass5 = new TH1D("v0Mass5", "v0Mass5", BINS, lowValue, highValue);
TH1D* v0Mass6 = new TH1D("v0Mass6", "v0Mass6", BINS, lowValue, highValue);
TH1D* v0Mass7 = new TH1D("v0Mass7", "v0Mass7", BINS, lowValue, highValue);
TH1D* v0Mass8 = new TH1D("v0Mass8", "v0Mass8", BINS, lowValue, highValue);
TH1D* v0Mass9 = new TH1D("v0Mass9", "v0Mass9", BINS, lowValue, highValue);

int main(int argc, char* argv[])
{
    if (argc != 3)
        return 0;
    char* inputFile = argv[1];
    char* outputFile = argv[2];

    char FileList[512];

    ofstream output_data(outputFile);
    if (!output_data) {
        cerr << "parameter error !" << endl;
        return -1;
    }

    TChain* chain = new TChain("V0PicoDst");

    /// data list 的处理方式
    if ((static_cast<string>(inputFile)).find(".list") != string::npos) {
        ifstream input_data(inputFile);
        if (!input_data) {
            cerr << "parameter error !" << endl;
            return -1;
        }
        for (; input_data.good();) {
            input_data.getline(FileList, 512);
            if (input_data.good()) {
                /// test the file is ok.
                TFile* ftmp = new TFile(FileList);
                if (!ftmp || !(ftmp->IsOpen()) || !(ftmp->GetNkeys())) {
                    cout << "The root file " << FileList << " is broken." << endl;
                } else {
                    cout << FileList << endl;
                    chain->Add(FileList);
                }
                delete ftmp;
            }
        }
        input_data.close();
    } else if ((static_cast<string>(inputFile)).find(".root") != string::npos) {
        /// test the file is ok.
        TFile* ftmp = new TFile(inputFile);
        if (!ftmp || !(ftmp->IsOpen()) || !(ftmp->GetNkeys())) {
            cout << "The root file " << FileList << " is broken." << endl;
        } else {
            cout << inputFile << endl;
            chain->Add(inputFile);
        }
        delete ftmp;
    } else {
        cerr << "parameter is error, please check it." << endl;
        return -1;
    }

    /////////////////////////////////////////////////////////////
    /// file and tree (begin)
    ////////////////////////////////////////////////////////////
    TFile* saveFile = new TFile("result.root", "RECREATE");
    V0PicoDst* v0 = new V0PicoDst(chain);

    v0Mass->SetDirectory(saveFile);
    v0Mass0->SetDirectory(saveFile);
    v0Mass1->SetDirectory(saveFile);
    v0Mass2->SetDirectory(saveFile);
    v0Mass3->SetDirectory(saveFile);
    v0Mass4->SetDirectory(saveFile);
    v0Mass5->SetDirectory(saveFile);
    v0Mass6->SetDirectory(saveFile);
    v0Mass7->SetDirectory(saveFile);
    v0Mass8->SetDirectory(saveFile);
    v0Mass9->SetDirectory(saveFile);
    ////////////////////////////////////////////////////////////
    /// file and tree (end)
    ////////////////////////////////////////////////////////////

    long int entries = chain->GetEntries();
    if (entries >= 1.e+008) {
        return -1;
    }

    int tracks = 0;
    boost::progress_display pid(static_cast<unsigned long>(entries));

    for (unsigned l = 0; l != entries; ++l) {
        ++pid;

        chain->GetEntry(l);
        tracks = v0->nv0;

        if (v0->primvertexZ < 210 || v0->primvertexZ > 212) {
            break;
        }

        for (int t = 0; t != tracks; ++t) {

	  // eta cut
	  if(v0->dau1eta[t] > 0 || v0->dau2eta[t] > 0 || v0->dau1eta[t] < -2 || v0->dau2eta[t] < -2){
	    continue;
	  }
	  v0Mass9->Fill(static_cast<double>(v0->v0mass[t]));
	  

	  if (v0->dau1nhitsfit[t] <= 10 || v0->dau2nhitsfit[t] <= 10) {
	    continue;
	  }
	  v0Mass8->Fill(static_cast<double>(v0->v0mass[t]));

	    
	  if (v0->dau1nhitsdedx[t] <= 0 || v0->dau2nhitsdedx[t] <= 0) {
	    continue;
	  }
	  v0Mass7->Fill(static_cast<double>(v0->v0mass[t]));

	  
	  if (fabs(v0->dau2nsigma[t]) > 2.0f || v0->dau1Z[t] < -0.1 || v0->dau1Z[t] > 0.3) {
	    continue;
	  }
	  v0Mass6->Fill(static_cast<double>(v0->v0mass[t]));

	  
	  if (v0->dau1pt[t] > 2 || v0->dau1pt[t] < 1.f || v0->dau2pt[t] < 0.15f) {
	    continue;
	  }
	  v0Mass5->Fill(static_cast<double>(v0->v0mass[t]));

  
	  if (v0->dau1dca[t] < 0.0f || v0->dau1dca[t] > 1.0f) {
	    continue;
	  }
	  v0Mass4->Fill(static_cast<double>(v0->v0mass[t]));
	    
	  if (v0->dau2dca[t] < 1.5 || v0->dau2dca[t] > 15) {
	    continue;
	  }
	  v0Mass3->Fill(static_cast<double>(v0->v0mass[t]));

	  if (v0->dca1to2[t] > 0.7f) {
	    continue;
	  }
	  v0Mass2->Fill(static_cast<double>(v0->v0mass[t]));

	  if (v0->v0declen[t] < 4.8) {
	    continue;
	  }
	  v0Mass1->Fill(static_cast<double>(v0->v0mass[t]));

	  if(v0->v0rdotp[t] < 0){
	    continue;
	  }
	  v0Mass0->Fill(static_cast<double>(v0->v0mass[t]));

	  if (v0->v0dca[t] > 1.0f) {
	    continue;
	  }
	  v0Mass->Fill(static_cast<double>(v0->v0mass[t]));
        }
    }

    v0Mass9->Write();
    v0Mass8->Write();
    v0Mass7->Write();
    v0Mass6->Write();
    v0Mass5->Write();
    v0Mass4->Write();
    v0Mass3->Write();
    v0Mass2->Write();
    v0Mass1->Write();
    v0Mass0->Write();
    v0Mass->Write();
    output_data.close();
    delete chain;
    delete v0;
    delete saveFile;
    return 0;
}
