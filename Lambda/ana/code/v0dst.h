//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Feb 24 14:09:07 20500 by ROOT version 5.34/30
// from TTree V0PicoDst/V0PicoDst from StV0Maker
// found on file: D7FDB926601B42B8C9770DD00051831A_0.la.picodst.root
//////////////////////////////////////////////////////////

#ifndef v0dst_h
#define v0dst_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class v0dst {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Float_t         magn;
   Int_t           eventid;
   Int_t           runid;
   Int_t           nrefmult;
   Float_t         primvertexX;
   Float_t         primvertexY;
   Float_t         primvertexZ;
   Int_t           nv0;
   Float_t         v0mass[4000];   //[nv0]
   Float_t         v0rapidity[4000];   //[nv0]
   Float_t         v0eta[4000];   //[nv0]
   Float_t         v0x[4000];   //[nv0]
   Float_t         v0y[4000];   //[nv0]
   Float_t         v0z[4000];   //[nv0]
   Float_t         v0px[4000];   //[nv0]
   Float_t         v0py[4000];   //[nv0]
   Float_t         v0pz[4000];   //[nv0]
   Float_t         v0xpath[4000];   //[nv0]
   Float_t         v0ypath[4000];   //[nv0]
   Float_t         v0zpath[4000];   //[nv0]
   Float_t         v0pxpath[4000];   //[nv0]
   Float_t         v0pypath[4000];   //[nv0]
   Float_t         v0pzpath[4000];   //[nv0]
   Float_t         v0pt[4000];   //[nv0]
   Float_t         v0declen[4000];   //[nv0]
   Float_t         v0dca[4000];   //[nv0]
   Float_t         dca1to2[4000];   //[nv0]
   Float_t         v0rdotp[4000];   //[nv0]
   Float_t         v0theta[4000];   //[nv0]
   Float_t         v0sinth[4000];   //[nv0]
   Int_t           dau1id[4000];   //[nv0]
   Float_t         dau1dca[4000];   //[nv0]
   Float_t         dau1mass[4000];   //[nv0]
   Int_t           dau1nhitsfit[4000];   //[nv0]
   Int_t           dau1nhitsdedx[4000];   //[nv0]
   Int_t           dau1nhitsposs[4000];   //[nv0]
   Float_t         dau1eta[4000];   //[nv0]
   Float_t         dau1nsigma[4000];   //[nv0]
   Float_t         dau1px[4000];   //[nv0]
   Float_t         dau1py[4000];   //[nv0]
   Float_t         dau1pz[4000];   //[nv0]
   Float_t         dau1pt[4000];   //[nv0]
   Float_t         dau1dEdx[4000];   //[nv0]
   Float_t         dau1Z[4000];   //[nv0]
   Float_t         dau1beta[4000];   //[nv0]
   Float_t         dau1toflocaly[4000];   //[nv0]
   Int_t           dau2id[4000];   //[nv0]
   Float_t         dau2dca[4000];   //[nv0]
   Float_t         dau2mass[4000];   //[nv0]
   Int_t           dau2nhitsfit[4000];   //[nv0]
   Int_t           dau2nhitsdedx[4000];   //[nv0]
   Int_t           dau2nhitsposs[4000];   //[nv0]
   Float_t         dau2eta[4000];   //[nv0]
   Float_t         dau2nsigma[4000];   //[nv0]
   Float_t         dau2px[4000];   //[nv0]
   Float_t         dau2py[4000];   //[nv0]
   Float_t         dau2pz[4000];   //[nv0]
   Float_t         dau2pt[4000];   //[nv0]
   Float_t         dau2dEdx[4000];   //[nv0]
   Float_t         dau2Z[4000];   //[nv0]
   Float_t         dau2beta[4000];   //[nv0]
   Float_t         dau2toflocaly[4000];   //[nv0]

   // List of branches
   TBranch        *b_magn;   //!
   TBranch        *b_eventid;   //!
   TBranch        *b_runid;   //!
   TBranch        *b_nrefmult;   //!
   TBranch        *b_primvertexX;   //!
   TBranch        *b_primvertexY;   //!
   TBranch        *b_primvertexZ;   //!
   TBranch        *b_nv0;   //!
   TBranch        *b_v0mass;   //!
   TBranch        *b_v0rapidity;   //!
   TBranch        *b_v0eta;   //!
   TBranch        *b_v0x;   //!
   TBranch        *b_v0y;   //!
   TBranch        *b_v0z;   //!
   TBranch        *b_v0px;   //!
   TBranch        *b_v0py;   //!
   TBranch        *b_v0pz;   //!
   TBranch        *b_v0xpath;   //!
   TBranch        *b_v0ypath;   //!
   TBranch        *b_v0zpath;   //!
   TBranch        *b_v0pxpath;   //!
   TBranch        *b_v0pypath;   //!
   TBranch        *b_v0pzpath;   //!
   TBranch        *b_v0pt;   //!
   TBranch        *b_v0declen;   //!
   TBranch        *b_v0dca;   //!
   TBranch        *b_dca1to2;   //!
   TBranch        *b_v0rdotp;   //!
   TBranch        *b_v0theta;   //!
   TBranch        *b_v0sinth;   //!
   TBranch        *b_dau1id;   //!
   TBranch        *b_dau1dca;   //!
   TBranch        *b_dau1mass;   //!
   TBranch        *b_dau1nhitsfit;   //!
   TBranch        *b_dau1nhitsdedx;   //!
   TBranch        *b_dau1nhitsposs;   //!
   TBranch        *b_dau1eta;   //!
   TBranch        *b_dau1nsigma;   //!
   TBranch        *b_dau1px;   //!
   TBranch        *b_dau1py;   //!
   TBranch        *b_dau1pz;   //!
   TBranch        *b_dau1pt;   //!
   TBranch        *b_dau1dEdx;   //!
   TBranch        *b_dau1Z;   //!
   TBranch        *b_dau1beta;   //!
   TBranch        *b_dau1toflocaly;   //!
   TBranch        *b_dau2id;   //!
   TBranch        *b_dau2dca;   //!
   TBranch        *b_dau2mass;   //!
   TBranch        *b_dau2nhitsfit;   //!
   TBranch        *b_dau2nhitsdedx;   //!
   TBranch        *b_dau2nhitsposs;   //!
   TBranch        *b_dau2eta;   //!
   TBranch        *b_dau2nsigma;   //!
   TBranch        *b_dau2px;   //!
   TBranch        *b_dau2py;   //!
   TBranch        *b_dau2pz;   //!
   TBranch        *b_dau2pt;   //!
   TBranch        *b_dau2dEdx;   //!
   TBranch        *b_dau2Z;   //!
   TBranch        *b_dau2beta;   //!
   TBranch        *b_dau2toflocaly;   //!

   v0dst(TTree *tree=0);
   virtual ~v0dst();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef v0dst_cxx
v0dst::v0dst(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("D7FDB926601B42B8C9770DD00051831A_0.la.picodst.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("D7FDB926601B42B8C9770DD00051831A_0.la.picodst.root");
      }
      f->GetObject("V0PicoDst",tree);

   }
   Init(tree);
}

v0dst::~v0dst()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t v0dst::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t v0dst::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void v0dst::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("magn", &magn, &b_magn);
   fChain->SetBranchAddress("eventid", &eventid, &b_eventid);
   fChain->SetBranchAddress("runid", &runid, &b_runid);
   fChain->SetBranchAddress("nrefmult", &nrefmult, &b_nrefmult);
   fChain->SetBranchAddress("primvertexX", &primvertexX, &b_primvertexX);
   fChain->SetBranchAddress("primvertexY", &primvertexY, &b_primvertexY);
   fChain->SetBranchAddress("primvertexZ", &primvertexZ, &b_primvertexZ);
   fChain->SetBranchAddress("nv0", &nv0, &b_nv0);
   fChain->SetBranchAddress("v0mass", v0mass, &b_v0mass);
   fChain->SetBranchAddress("v0rapidity", v0rapidity, &b_v0rapidity);
   fChain->SetBranchAddress("v0eta", v0eta, &b_v0eta);
   fChain->SetBranchAddress("v0x", v0x, &b_v0x);
   fChain->SetBranchAddress("v0y", v0y, &b_v0y);
   fChain->SetBranchAddress("v0z", v0z, &b_v0z);
   fChain->SetBranchAddress("v0px", v0px, &b_v0px);
   fChain->SetBranchAddress("v0py", v0py, &b_v0py);
   fChain->SetBranchAddress("v0pz", v0pz, &b_v0pz);
   fChain->SetBranchAddress("v0xpath", v0xpath, &b_v0xpath);
   fChain->SetBranchAddress("v0ypath", v0ypath, &b_v0ypath);
   fChain->SetBranchAddress("v0zpath", v0zpath, &b_v0zpath);
   fChain->SetBranchAddress("v0pxpath", v0pxpath, &b_v0pxpath);
   fChain->SetBranchAddress("v0pypath", v0pypath, &b_v0pypath);
   fChain->SetBranchAddress("v0pzpath", v0pzpath, &b_v0pzpath);
   fChain->SetBranchAddress("v0pt", v0pt, &b_v0pt);
   fChain->SetBranchAddress("v0declen", v0declen, &b_v0declen);
   fChain->SetBranchAddress("v0dca", v0dca, &b_v0dca);
   fChain->SetBranchAddress("dca1to2", dca1to2, &b_dca1to2);
   fChain->SetBranchAddress("v0rdotp", v0rdotp, &b_v0rdotp);
   fChain->SetBranchAddress("v0theta", v0theta, &b_v0theta);
   fChain->SetBranchAddress("v0sinth", v0sinth, &b_v0sinth);
   fChain->SetBranchAddress("dau1id", dau1id, &b_dau1id);
   fChain->SetBranchAddress("dau1dca", dau1dca, &b_dau1dca);
   fChain->SetBranchAddress("dau1mass", dau1mass, &b_dau1mass);
   fChain->SetBranchAddress("dau1nhitsfit", dau1nhitsfit, &b_dau1nhitsfit);
   fChain->SetBranchAddress("dau1nhitsdedx", dau1nhitsdedx, &b_dau1nhitsdedx);
   fChain->SetBranchAddress("dau1nhitsposs", dau1nhitsposs, &b_dau1nhitsposs);
   fChain->SetBranchAddress("dau1eta", dau1eta, &b_dau1eta);
   fChain->SetBranchAddress("dau1nsigma", dau1nsigma, &b_dau1nsigma);
   fChain->SetBranchAddress("dau1px", dau1px, &b_dau1px);
   fChain->SetBranchAddress("dau1py", dau1py, &b_dau1py);
   fChain->SetBranchAddress("dau1pz", dau1pz, &b_dau1pz);
   fChain->SetBranchAddress("dau1pt", dau1pt, &b_dau1pt);
   fChain->SetBranchAddress("dau1dEdx", dau1dEdx, &b_dau1dEdx);
   fChain->SetBranchAddress("dau1Z", dau1Z, &b_dau1Z);
   fChain->SetBranchAddress("dau1beta", dau1beta, &b_dau1beta);
   fChain->SetBranchAddress("dau1toflocaly", dau1toflocaly, &b_dau1toflocaly);
   fChain->SetBranchAddress("dau2id", dau2id, &b_dau2id);
   fChain->SetBranchAddress("dau2dca", dau2dca, &b_dau2dca);
   fChain->SetBranchAddress("dau2mass", dau2mass, &b_dau2mass);
   fChain->SetBranchAddress("dau2nhitsfit", dau2nhitsfit, &b_dau2nhitsfit);
   fChain->SetBranchAddress("dau2nhitsdedx", dau2nhitsdedx, &b_dau2nhitsdedx);
   fChain->SetBranchAddress("dau2nhitsposs", dau2nhitsposs, &b_dau2nhitsposs);
   fChain->SetBranchAddress("dau2eta", dau2eta, &b_dau2eta);
   fChain->SetBranchAddress("dau2nsigma", dau2nsigma, &b_dau2nsigma);
   fChain->SetBranchAddress("dau2px", dau2px, &b_dau2px);
   fChain->SetBranchAddress("dau2py", dau2py, &b_dau2py);
   fChain->SetBranchAddress("dau2pz", dau2pz, &b_dau2pz);
   fChain->SetBranchAddress("dau2pt", dau2pt, &b_dau2pt);
   fChain->SetBranchAddress("dau2dEdx", dau2dEdx, &b_dau2dEdx);
   fChain->SetBranchAddress("dau2Z", dau2Z, &b_dau2Z);
   fChain->SetBranchAddress("dau2beta", dau2beta, &b_dau2beta);
   fChain->SetBranchAddress("dau2toflocaly", dau2toflocaly, &b_dau2toflocaly);
   Notify();
}

Bool_t v0dst::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void v0dst::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t v0dst::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef v0dst_cxx
