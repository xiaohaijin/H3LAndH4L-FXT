//c++
#include <iomanip>
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <cstddef>
#include <vector>
#include <fstream>
#include <sstream>

//root
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

int main(int argc, char *argv[])
{
  if (argc != 3) return 0;
  char *inputFile = argv[1];
  char *outputFile = argv[2];

  char FileList[512];

  ofstream output_data(outputFile);
  if (!output_data)
    {
      cerr << "parameter error !" << endl;
      return -1;
    }

  TChain *chain = new TChain("V0PicoDst");

  /// data list 的处理方式                                                         
  if ((static_cast<string>(inputFile)).find(".list") != string::npos)
    {
      ifstream input_data(inputFile);
      if (!input_data)
        {
          cerr << "parameter error !" << endl;
          return -1;
        }
      for (;input_data.good();)
        {
          input_data.getline(FileList,512);
          if  ( input_data.good() )
            {
              /// test the file is ok.                           
              TFile *ftmp = new TFile(FileList);
              if(!ftmp || !(ftmp->IsOpen()) || !(ftmp->GetNkeys()))
                {
                  cout << "The root file " << FileList << " is broken." << endl;
                }
              else
                {
                  cout << FileList << endl;
                  chain->Add(FileList);
                }
              delete ftmp;
            }
        }
      input_data.close();
    }
  else if((static_cast<string>(inputFile)).find(".root") != string::npos)
    {
      /// test the file is ok.                        
      TFile *ftmp = new TFile(inputFile);
      if(!ftmp || !(ftmp->IsOpen()) || !(ftmp->GetNkeys()))
        {
          cout << "The root file " << FileList << " is broken." << endl;
        }
      else
        {
          cout << inputFile << endl;
          chain->Add(inputFile);
        }
      delete ftmp;
    }
  else
    {
      cerr << "parameter is error, please check it." << endl;
      return -1;
    }


  TFile *saveFile = new TFile("result.root", "RECREATE");
  V0PicoDst *v0 = new V0PicoDst(chain);
  unsigned entries = chain->GetEntries();
  if(entries >= 1.e+008) return -1;

  unsigned int tracks = 0;
  boost::progress_display pid(entries);

  for (unsigned l = 0; l != entries; ++l)
    {
      ++pid;

      chain->GetEntry(l);
      tracks = v0->nv0;
      for (unsigned t = 0; t != tracks; ++t)
        {

        }
    }

  output_data.close();
  delete chain;
  delete v0;
  delete saveFile;
  return 0;
}
