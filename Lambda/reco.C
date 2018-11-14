// topologically reconstruct Lambda(using StV0Maker)

#include "StRoot/StV0Maker/StV0Type.h"

void reco(TString InputFileList, Int_t nFiles = 1000, Int_t nEvents = 0, TString OutputDir = "output/", TString JobIdName = "testrun");

void reco(TString InputFileList, Int_t nFiles, Int_t nEvents, TString OutputDir, TString JobIdName) {

  //	Load libraries
  gROOT->Macro("loadMuDst.C");
  gSystem->Load("StV0Maker");

  //	List of member links in the chain

  StChain* chain = new StChain;
  StMuDstMaker* muDstMaker = new StMuDstMaker(0, 0, "", InputFileList, "MuDst", nFiles);

  //	Turn off everything but Primary tracks in order to speed up the analysis and eliminate IO
  muDstMaker->SetStatus("*", 1);  // Turn off all branches
                                  //	muDstMaker -> SetStatus("MuEvent",1);// Turn on the Event data (esp. Event number)
                                  //	muDstMaker -> SetStatus("GlobalTracks",1);// Turn on the global track data
                                  //	muDstMaker -> SetStatus("BTofHeader",1);// Turn on the global track data
  muDstMaker->SetDebug(0);        // Turn off Debug information

  //***********************************************************************************
  //	use StV0Maker to get the v0 candidates
  StV0Maker* rawsig = new StV0Maker(muDstMaker, "v0makerfirst_signal");
  //	Miscellaneous things we need before starting the chain
  rawsig->setHistoFileName(OutputDir + JobIdName + ".la.histo.root");     // Name the output file for histograms
  rawsig->setV0TreeFileName(OutputDir + JobIdName + ".la.picodst.root");  // V0 candidate tree file for further cuts.
  rawsig->setV0Type(kLambda);  // set V0 type. once a time! do not try to mess things up for the time being.
  rawsig->setRotate(false);
  rawsig->SetDebug(0);

  //  use StV0Maker to get the anti-v0 candidates
  StV0Maker* rawantisig = new StV0Maker(muDstMaker, "v0makerfirst_antisignal");
  rawantisig->setHistoFileName(OutputDir + JobIdName + ".ala.histo.root");
  rawantisig->setV0TreeFileName(OutputDir + JobIdName + ".ala.picodst.root");
  rawantisig->setV0Type(kAntiLambda);
  rawantisig->setRotate(false);
  rawantisig->SetDebug(0);

  //	use Rotate method to get the background information.
//  StV0Maker* rawbg = new StV0Maker(muDstMaker, "v0makerfirst_background");
//  rawbg->setHistoFileName(OutputDir + JobIdName + ".labg.histo.root");
//  rawbg->setV0TreeFileName(OutputDir + JobIdName + ".labg.picodst.root");
//  rawbg->setV0Type(kLambda);
//  rawbg->setRotate(true);
//  rawbg->SetDebug(0);

  //  use Rotate method to get the anti-background information.
//  StV0Maker* rawantibg = new StV0Maker(muDstMaker, "v0makerfirst_antibackground");
//  TString Name = JobIdName;
//  Name.Append(".alabg.histo.root");
//  rawantibg->setHistoFileName(OutputDir + JobIdName + ".alabg.histo.root");
//  rawantibg->setV0TreeFileName(OutputDir + JobIdName + ".alabg.picodst.root");
//  rawantibg->setV0Type(kAntiLambda);
//  rawantibg->setRotate(true);
//  rawantibg->SetDebug(0);
  //***********************************************************************************

  if (nEvents == 0) nEvents = 10000000;  // Take all events in nFiles if nEvents = 0
                                         //	Loop over the links in the chain
  Int_t iInit = chain->Init();
  if (iInit) chain->Fatal(iInit, "on init");
  Int_t totalE = muDstMaker->chain()->GetEntries();

  //	chain -> EventLoop(1,nEvents);//will output lots of useless debugging info.
  Int_t istat = 0, i = 1;
  cout << " Total entries = " << totalE << endl;
  if (nEvents > totalE) nEvents = totalE;

  while (i <= nEvents && istat != 2) {

    if (i % 1000 == 0) cout << endl << "== Event " << i << " start ==" << endl;
    // cout << endl << "== Event " << i << " start ==" << endl;

    chain->Clear();
    istat = chain->Make(i);
    // cout << endl << "== Event " << i << " finish =="<< endl;
    if (istat == 2) cout << "Last  event processed. Status = " << istat << endl;
    if (istat == 3) cout << "Error event processed. Status = " << istat << endl;

    i++;
  }

  if (nEvents > 1) chain->Finish();

  // Cleanup
  delete chain;
}
