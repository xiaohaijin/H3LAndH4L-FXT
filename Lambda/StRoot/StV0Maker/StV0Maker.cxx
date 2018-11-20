#include "StV0Maker.h"
#include "StDcaService.h"
#include "SystemOfUnits.h"
#include "PhysicalConstants.h"
#include "StBTofUtil/tofPathLength.hh"
//#include "StBichsel/Bichsel.h"

#include <iostream>

#include "StMessMgr.h"
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuBTofPidTraits.h" //TOF Info.
#include "StMuDSTMaker/COMMON/StMuBTofHit.h"
#include "StBTofHeader.h"
#include "StStrangeMuDstMaker/StStrangeMuDstMaker.h"
#include "StStrangeMuDstMaker/StV0MuDst.hh"
//#include "StStrangeMuDstMaker/StStrangeEvMuDst.hh"

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "StDcaGeometry.h"

/////////////////////////////////////////////////////////
/// was added by xiaohai(begin)
/////////////////////////////////////////////////////////
//#define XIAOHAI

#ifdef XIAOHAI
#include "tofPathLength.hh"
#include "COMMON/StMuDstMaker.h"
#include "COMMON/StMuTrack.h"
#include "COMMON/StMuEvent.h"
#include "COMMON/StMuBTofPidTraits.h" //TOF Info.
#include "COMMON/StMuBTofHit.h"
#include "StStrangeMuDstMaker.h"
#include "StV0MuDst.hh"
#endif
/////////////////////////////////////////////////////////
/// was added by xiaohai(end)
////////////////////////////////////////////////////////

ClassImp(StV0Maker) // Macro for CINT compatibility

StV0Maker::StV0Maker(StMuDstMaker* maker, const char* name) : StMaker(name)
{
    // Initialize and/or zero all public/private data members here.

    mMuDstMaker = maker; // Pass MuDst pointer to DstAnlysisMaker Class member functions
    mV0Type = kLambda; // Lambda as default!

    mRotate = false;
    mSameSignPlus = false;
    mSameSignMinus = false;

    mDcaAlgoLong = false; // true;

    histogram_output = NULL; // Zero the Pointer to histogram output file
    v0tree_output = NULL; // Zero the Pointer to v0 tree output file
    mV0Tree = NULL; // Zero the Pointer to mV0Tree
    mHistogramOutputFileName = ""; // Histogram Output File Name will be set inside the "analysis".C macro
    mV0TreeOutputFileName = ""; // V0 Output File Name will be set inside the "analysis".C macro

    mEventsProcessed = 0; // Zero the Number of Events processed by the maker
    mTestNTrack = 0;
    mTestVZ = 0;
}

StV0Maker::~StV0Maker()
{
    // Destroy and/or zero out all public/private data members here.
}

void StV0Maker::initConst()
{

    // initialize the constant for different V0 types.

    // for Lambda:proton+pi- and AntiLambda: Antiproton+pi+
    mMassV0 = 1.115683; // mass of H3L  //GeV is unit.
    mMass1 = 0.938272081; // mass of He3    //GeV is unit.
    mMass2 = 0.13957061; // mass of pion      //GeV is unit.

    if (mV0Type == kLambda) {
        mCharge1 = 1; // He3 charge = 2.
        mChargeV0 = 0; // V0 charge,Lambda=0; Xi- = -1; H3L = 1.
        mCharge2 = -1; // pion minus charge = -1.
    }

    if (mV0Type == kAntiLambda) {
        mCharge1 = -1; // antiHe3 charge= -2.
        mChargeV0 = 0; // V0 charge,Lambda=0; antiXi- = 1; antiH3L = -1.
        mCharge2 = 1; // pion plus = 1.
    }

    // do not setup the cut values here. those are parameters

    // parameters for StDcaService.cxx
    kShiftConnect = 0.3;
    kShiftContain = 0.3;

    return;
}

void StV0Maker::initParam()
{
    // setup the cut values here. do not hard-code them in ::Make()

    cutAbsVertexZLeEq = 50.0; /*!< done */
    cutNHitsGr = 10; /*!< done */
    cutNHitsDedxGr = 0; /*!< done */
    cutPtGrEq = 0.1; /*!< done, this is not asymmetric. */

    cutAbsNSigma2Le = 3.0; /*!< done */

    cutDca1GrEq = 0.9; /*!< done */
    cutDca2GrEq = 0.9; /*!< done */

    cutDca1to2LeEq = 1.0;
    cutV0MassWidthLeEq = 0.01;
    cutDauPtArmLeEq = 0.3;
    cutAbsDausPtShoulderDiffLeEq = 1.2;
    cutDau1DecAngGr = 0.0;
    cutDau2DecAngGr = 0.0;
    cutV0rdotpGr = 0.0;
    cutDcaV0Le = 1.0;
    cutV0DecLenGrEq = 7.0;

    RotDegree = 180.0; // Rotate one dau to estimate background level.

    // initial dedx_dau1_th[11901]
    float temp1;
    ifstream f_dedx_th1;
    f_dedx_th1.open("/star/u/jhai/dedx/dedx_mean_proton.dat");
    for (int i = 0; i < 11901; i++) {
        f_dedx_th1 >> temp1 >> dedx_dau1_th[i];
    }

    f_dedx_th1.close();
    for (int i = 0; i < 11901; i++) {
        dedx_dau1_th[i] *= 1.e-6;
    }

    // initial dedx_dau2_th[11901]
    float temp2;
    ifstream f_dedx_th2;
    f_dedx_th2.open("/star/u/jhai/dedx/dedx_mean_pi.dat");
    for (int i = 0; i < 11901; i++) {
        f_dedx_th2 >> temp2 >> dedx_dau2_th[i];
    }

    f_dedx_th2.close();
    for (int i = 0; i < 11901; i++) {
        dedx_dau2_th[i] *= 1.e-6;
    }
    return;
}

void StV0Maker::initHisto()
{
    // Create Histograms
    // there is no better way to set QA histograms. do not use histogram arrays or vectors.
    // it is not useful. there are no need to operate all the histograms at the same time.

    const Int_t nbins = 100;

    // QA for events
    hNPrimVertex = new TH1F("PrimVertex", "Number of Primary Vertex", 10, 0.0, 10.0);
    hVertexZ = new TH1F("VertexZ", "Event Vertex Z Position", nbins * 20, -200.0, 200.0);
    hVertexZdiff = new TH1F("VertexZdiff", "Vertex Z Position(VPD-TPC)", nbins * 20, -200.0, 200.0);
    hNRefMult = new TH1F("RefMult", "Reference Multiplicity", nbins * 10, 0.0, 1000.0);
    hSelectNRefMult = new TH1F("SelectRefMult", "Reference Multiplicity of selected events", nbins * 10, 0.0, 1000.0);
    hPrimaryRefMult = new TH1F("PrimaryRefMult", "PrimaryRefMult", nbins * 10, 0.0, 1000.0);
    hMagneticField = new TH1F("MagneticField", "magneticField of selected events", nbins * 2, -10.0, 10.0);

    hnSigmaProton = new TH2F("nSigmaProton", "nSigmaProton", nbins * 20, -10.0, 10.0, nbins * 2, -10.0, 10.0);
    hnSigmaPion = new TH2F("nSigmaPion", "nSigmaPion", nbins * 20, -10.0, 10.0, nbins * 2, -10.0, 10.0);
    hnSigmaKaon = new TH2F("nSigmaKaon", "nSigmaKaon", nbins * 20, -10.0, 10.0, nbins * 2, -10.0, 10.0);

    hdEdxP = new TH2F("dEdxP", "dEdx vs. Rigidity", nbins * 20, -10.0, 10.0, nbins * 5, 0.0, 5e-5);
    hDcaP = new TH2F("DcaP", "Dca vs. Rigidity", nbins * 20, -10.0, 10.0, nbins * 20, 0.0, 20.0);
    hMassP = new TH2F("MassP", "Mass vs. Rigidity", nbins * 20, -10.0, 10.0, nbins * 160, 0.0, 16.0);
    hInvBetaP = new TH2F("InvBetaP", "InvBeta vs. Rigidity", nbins * 20, -10.0, 10.0, nbins * 5, 0.0, 5.0);

    hdau1dEdxP = new TH2F("dau1dEdxP", "dau1dEdx vs. Rigidity", nbins * 20, -10.0, 10.0, nbins * 5, 0.0, 5e-5);
    hdau2dEdxP = new TH2F("dau2dEdxP", "dau2dEdx vs. Rigidity", nbins * 20, -10.0, 10.0, nbins * 5, 0.0, 5e-5);

    hdau1ZP = new TH2F("dau1ZP", "dau1Z vs. Rigidity", nbins * 20, -10.0, 10.0, nbins, -0.5, 0.5);
    hdau2ZP = new TH2F("dau2ZP", "dau2Z vs. Rigidity", nbins * 20, -10.0, 10.0, nbins, -0.5, 0.5);

    hdau1MassP = new TH2F("dau1MassP", "dau1Mass vs. Rigidity", nbins * 20, -10.0, 10.0, nbins * 160, 0.0, 16.0);
    hdau2MassP = new TH2F("dau2MassP", "dau2Mass vs. Rigidity", nbins * 20, -10.0, 10.0, nbins * 160, 0.0, 16.0);

    hdau1InvBetaP = new TH2F("dau1InvBetaP", "dau1InvBeta vs. Rigidity", nbins * 20, -10.0, 10.0, nbins * 5, 0.0, 5.0);
    hdau2InvBetaP = new TH2F("dau2InvBetaP", "dau2InvBeta vs. Rigidity", nbins * 20, -10.0, 10.0, nbins * 5, 0.0, 5.0);

    hdau1DiffInvBetaP
        = new TH2F("dau1DiffInvBetaP", "dau1DiffInvBeta vs. Rigidity", nbins * 20, -10.0, 10.0, nbins * 20, -1.0, 1.0);
    hdau2DiffInvBetaP
        = new TH2F("dau2DiffInvBetaP", "dau2DiffInvBeta vs. Rigidity", nbins * 20, -10.0, 10.0, nbins * 20, -1.0, 1.0);

    // QA for V0's
    hInvMass
        = new TH1F("V0Mass", "V0 Inv. Mass", nbins * 2, mMassV0 - cutV0MassWidthLeEq, mMassV0 + cutV0MassWidthLeEq);

    return;
}

void StV0Maker::initTree()
{
    // initialize the TTree for StV0Dst

    mV0Tree = new TTree("V0PicoDst", "V0PicoDst from StV0Maker");
    mV0Tree->SetDirectory(v0tree_output);

    // Events info.*******************************************************************************
    mV0Tree->Branch("magn", &mV0Dst.magn, "magn/F");
    mV0Tree->Branch("eventid", &mV0Dst.eventid, "eventid/I");
    mV0Tree->Branch("runid", &mV0Dst.runid, "runid/I");
    mV0Tree->Branch("nrefmult", &mV0Dst.nrefmult, "nrefmult/I");
    mV0Tree->Branch("primvertexX", &mV0Dst.primvertexX, "primvertexX/F");
    mV0Tree->Branch("primvertexY", &mV0Dst.primvertexY, "primvertexY/F");
    mV0Tree->Branch("primvertexZ", &mV0Dst.primvertexZ, "primvertexZ/F");
    mV0Tree->Branch("vpdVz", &mV0Dst.vpdVz, "vpdVz/F");
    mV0Tree->Branch("nv0", &mV0Dst.nv0, "nv0/I");

    // V0 info.***********************************************************************************
    mV0Tree->Branch("v0mass", mV0Dst.v0mass, "v0mass[nv0]/F");
    mV0Tree->Branch("v0rapidity", mV0Dst.v0rapidity, "v0rapidity[nv0]/F");
    mV0Tree->Branch("v0eta", mV0Dst.v0eta, "v0eta[nv0]/F");
    mV0Tree->Branch("v0x", mV0Dst.v0x, "v0x[nv0]/F");
    mV0Tree->Branch("v0y", mV0Dst.v0y, "v0y[nv0]/F");
    mV0Tree->Branch("v0z", mV0Dst.v0z, "v0z[nv0]/F");
    mV0Tree->Branch("v0px", mV0Dst.v0px, "v0px[nv0]/F");
    mV0Tree->Branch("v0py", mV0Dst.v0py, "v0py[nv0]/F");
    mV0Tree->Branch("v0pz", mV0Dst.v0pz, "v0pz[nv0]/F");
    mV0Tree->Branch("v0xpath", mV0Dst.v0xpath, "v0xpath[nv0]/F");
    mV0Tree->Branch("v0ypath", mV0Dst.v0ypath, "v0ypath[nv0]/F");
    mV0Tree->Branch("v0zpath", mV0Dst.v0zpath, "v0zpath[nv0]/F");
    mV0Tree->Branch("v0pxpath", mV0Dst.v0pxpath, "v0pxpath[nv0]/F");
    mV0Tree->Branch("v0pypath", mV0Dst.v0pypath, "v0pypath[nv0]/F");
    mV0Tree->Branch("v0pzpath", mV0Dst.v0pzpath, "v0pzpath[nv0]/F");
    mV0Tree->Branch("v0pt", mV0Dst.v0pt, "v0pt[nv0]/F");
    mV0Tree->Branch("v0declen", mV0Dst.v0declen, "v0declen[nv0]/F");
    mV0Tree->Branch("v0declenH", mV0Dst.v0declenH, "v0declenH[nv0]/F");
    mV0Tree->Branch("v0dca", mV0Dst.v0dca, "v0dca[nv0]/F");
    mV0Tree->Branch("dca1to2", mV0Dst.dca1to2, "dca1to2[nv0]/F");
    mV0Tree->Branch("v0rdotp", mV0Dst.v0rdotp, "v0rdotp[nv0]/F");
    mV0Tree->Branch("v0theta", mV0Dst.v0theta, "v0theta[nv0]/F");
    mV0Tree->Branch("v0sinth", mV0Dst.v0sinth, "v0sinth[nv0]/F");

    // dau1**************************************************************************************
    mV0Tree->Branch("dau1id", mV0Dst.dau1id, "dau1id[nv0]/I");
    mV0Tree->Branch("dau1dca", mV0Dst.dau1dca, "dau1dca[nv0]/F");
    mV0Tree->Branch("dau1mass", mV0Dst.dau1mass, "dau1mass[nv0]/F");
    mV0Tree->Branch("dau1nhitsfit", mV0Dst.dau1nhitsfit, "dau1nhitsfit[nv0]/I");
    mV0Tree->Branch("dau1nhitsdedx", mV0Dst.dau1nhitsdedx, "dau1nhitsdedx[nv0]/I");
    mV0Tree->Branch("dau1nhitsposs", mV0Dst.dau1nhitsposs, "dau1nhitsposs[nv0]/I");
    mV0Tree->Branch("dau1hftbits", mV0Dst.dau1hftbits, "dau1hftbits[nv0]/I");
    mV0Tree->Branch("dau1eta", mV0Dst.dau1eta, "dau1eta[nv0]/F");
    mV0Tree->Branch("dau1nsigma", mV0Dst.dau1nsigma, "dau1nsigma[nv0]/F");
    mV0Tree->Branch("dau1px", mV0Dst.dau1px, "dau1px[nv0]/F");
    mV0Tree->Branch("dau1py", mV0Dst.dau1py, "dau1py[nv0]/F");
    mV0Tree->Branch("dau1pz", mV0Dst.dau1pz, "dau1pz[nv0]/F");
    mV0Tree->Branch("dau1pt", mV0Dst.dau1pt, "dau1pt[nv0]/F");
    mV0Tree->Branch("dau1dEdx", mV0Dst.dau1dEdx, "dau1dEdx[nv0]/F");
    mV0Tree->Branch("dau1Z", mV0Dst.dau1Z, "dau1Z[nv0]/F");
    mV0Tree->Branch("dau1beta", mV0Dst.dau1beta, "dau1beta[nv0]/F");
    mV0Tree->Branch("dau1diffinvbeta", mV0Dst.dau1diffinvbeta, "dau1diffinvbeta[nv0]/F");
    mV0Tree->Branch("dau1toflocaly", mV0Dst.dau1toflocaly, "dau1toflocaly[nv0]/F");
    mV0Tree->Branch("dau1toflocalz", mV0Dst.dau1toflocalz, "dau1toflocalz[nv0]/F");

    // dau2***************************************************************************************
    mV0Tree->Branch("dau2id", mV0Dst.dau2id, "dau2id[nv0]/I");
    mV0Tree->Branch("dau2dca", mV0Dst.dau2dca, "dau2dca[nv0]/F");
    mV0Tree->Branch("dau2mass", mV0Dst.dau2mass, "dau2mass[nv0]/F");
    mV0Tree->Branch("dau2nhitsfit", mV0Dst.dau2nhitsfit, "dau2nhitsfit[nv0]/I");
    mV0Tree->Branch("dau2nhitsdedx", mV0Dst.dau2nhitsdedx, "dau2nhitsdedx[nv0]/I");
    mV0Tree->Branch("dau2nhitsposs", mV0Dst.dau2nhitsposs, "dau2nhitsposs[nv0]/I");
    mV0Tree->Branch("dau2hftbits", mV0Dst.dau2hftbits, "dau2hftbits[nv0]/I");
    mV0Tree->Branch("dau2eta", mV0Dst.dau2eta, "dau2eta[nv0]/F");
    mV0Tree->Branch("dau2nsigma", mV0Dst.dau2nsigma, "dau2nsigma[nv0]/F");
    mV0Tree->Branch("dau2px", mV0Dst.dau2px, "dau2px[nv0]/F");
    mV0Tree->Branch("dau2py", mV0Dst.dau2py, "dau2py[nv0]/F");
    mV0Tree->Branch("dau2pz", mV0Dst.dau2pz, "dau2pz[nv0]/F");
    mV0Tree->Branch("dau2pt", mV0Dst.dau2pt, "dau2pt[nv0]/F");
    mV0Tree->Branch("dau2dEdx", mV0Dst.dau2dEdx, "dau2dEdx[nv0]/F");
    mV0Tree->Branch("dau2Z", mV0Dst.dau2Z, "dau2Z[nv0]/F");
    mV0Tree->Branch("dau2beta", mV0Dst.dau2beta, "dau2beta[nv0]/F");
    mV0Tree->Branch("dau2diffinvbeta", mV0Dst.dau2diffinvbeta, "dau2diffinvbeta[nv0]/F");
    mV0Tree->Branch("dau2toflocaly", mV0Dst.dau2toflocaly, "dau2toflocaly[nv0]/F");
    mV0Tree->Branch("dau2toflocalz", mV0Dst.dau2toflocalz, "dau2toflocalz[nv0]/F");

    return;
}

Int_t StV0Maker::Init()
{

    // setup the constants according to mV0Type
    initConst();

    // initialize parameters (cuts)
    initParam();

    // Create Histogram output file
    if (mHistogramOutputFileName == "") {
        // CAUTION: ALWAYS USE { } HERE!!! LOG_XXX is a if()xxx macro!!!
        LOG_ERROR << "StV0Maker: Please specify the histrogram output file !!!!!" << endm;
        exit(-1);
    } else {
        histogram_output = new TFile(mHistogramOutputFileName, "recreate");
    }

    // Book histograms
    initHisto();

    // Create V0 Tree output file
    if (mV0TreeOutputFileName == "") {
        LOG_WARN << "StV0Maker: The V0 tree output file is not specified! output is smeared!" << endm;
    } else {
        v0tree_output = new TFile(mV0TreeOutputFileName, "recreate");

        // Create V0 Tree
        initTree();
    }

    // Clear daughter vectors.
    mDauVec1.clear();
    mDauVec2.clear();
    mDauDcaVec1.clear();
    mDauDcaVec2.clear();
    mDauZVec1.clear();
    mDauZVec2.clear();
    mDauMassVec1.clear();
    mDauMassVec2.clear();
    mDauBetaVec1.clear();
    mDauBetaVec2.clear();
    mDauDiffInvBetaVec1.clear();
    mDauDiffInvBetaVec2.clear();
    return kStOK;
}

Int_t StV0Maker::Make()
{
    // Do each event

    if (GetDebug())
        LOG_QA << "in StV0Maker::Make" << endm;
    // Do some cleaning here, used for StV0Maker or other subsequent makers

    mV0Dst.nv0 = 0;
    mPassEventCut = false;

    // Get 'event' data
    StMuEvent* muEvent = mMuDstMaker->muDst()->event();

    if (!muEvent)
        return kStOK;

    hNPrimVertex->Fill(mMuDstMaker->muDst()->numberOfPrimaryVertices());

    StThreeVectorF pv = muEvent->primaryVertexPosition();

    //	Cut on the number of vertices in the event.  On old tapes, no-vertex gets reported as VtxPosition=(0,0,0).
    //	Skip events that do not have a primary vertex. the small value '1e-5' is ok for hard coding.
    if ((fabs(pv.x()) < 1e-5) && (fabs(pv.y()) < 1e-5) && (fabs(pv.z()) < 1e-5))
        return kStOK;
    //	if(pv.perp() > 2.0) return kStOK ;

    //	if(GetDebug()) LOG_QA<<"in StV0Maker::Make : has pv"<<endm;

    //	triggerid, skip events that do not comply with the required trigger id.
    //	select mb and central trigger

    //////////////////////////////////////////////////////////////
    //     This was comment by xiaohai.
    //////////////////////////////////////////////////////////////

    if ((!muEvent->triggerIdCollection().nominal().isTrigger(1)))
        return kStOK;

    //	Fill some QA plots
    hVertexZ->Fill(muEvent->primaryVertexPosition().z()); // Make histogram of the vertex Z distribution
    //  Float_t temprefMult = muEvent->refMult();
    Float_t temprefMult = muEvent->grefmult(); // for Run14.
    //  hNRefMult->Fill(temprefMult);

    //	cut on vertexZ, not necessary for run4 central trigger data
    ///////////////////////////////////////////////
    // This was modified by xiaohai.
    ///////////////////////////////////////////////
    // if (fabs(muEvent->primaryVertexPosition().z()) > cutAbsVertexZLeEq) return kStOK;
    if (muEvent->primaryVertexPosition().z() > 212 || muEvent->primaryVertexPosition().z() < 210) {
        return kStOK;
    }
    mV0Dst.primvertexX = muEvent->primaryVertexPosition().x();
    mV0Dst.primvertexY = muEvent->primaryVertexPosition().y();
    mV0Dst.primvertexZ = muEvent->primaryVertexPosition().z();

    //	cut on centrality or reference multiplicity.
    if (temprefMult) {
    } // TODO: need to check whether this is the same as in old code. the old code might ignore the case of pile-up.

    mPassEventCut = true;

    float mvpdVz = -999.0;
    StBTofHeader* mBTofHeader = mMuDstMaker->muDst()->btofHeader();
    if (mBTofHeader)
        mvpdVz = mBTofHeader->vpdVz();

    mV0Dst.nrefmult = temprefMult;
    mV0Dst.eventid = muEvent->eventId();
    mV0Dst.runid = muEvent->runInfo().runId();
    mV0Dst.vpdVz = mvpdVz;

    hVertexZdiff->Fill(mvpdVz - muEvent->primaryVertexPosition().z());

    //	Do 'event' analysis based on event data
    //	Record some information...

    hSelectNRefMult->Fill(temprefMult); // this is an ESSENTIAL histogram to record the total number of events for
    // certain centrality. always make sure
    // it is filled AFTER event selection!

    Double_t magn = muEvent->runInfo().magneticField();
    //	Double_t magn = muEvent->magneticField();	//checked! the same as above

    mV0Dst.magn = magn;

    hMagneticField->Fill(magn);

    //  Get 'track' data, make cuts on tracks, do physics analysis, histogram results.
    //  Create a TObject array containing the global tracks or the primary tracks.
    TObjArray* tracks = mMuDstMaker->muDst()->globalTracks();
    //	TObjArray* tracks = mMuDstMaker->muDst()->primaryTracks();
    TObjArrayIter GetTracks(tracks); // Create an iterator to step through the tracks
    hNRefMult->Fill(tracks->GetEntries());
    hPrimaryRefMult->Fill(mMuDstMaker->muDst()->primaryTracks()->GetEntries());
    StMuTrack* track = 0; // Pointer to a track

    //	Main loop for Iterating over tracks
    while ((track = (StMuTrack*)GetTracks.Next())) {
        short flag = track->flag();
        unsigned short nHitsDedx = track->nHitsDedx(); // added dEdx Hits to restrict the background!
        unsigned short nHitsFit = track->nHitsFit(kTpcId);
        short charge = track->charge();
        double nsigmapion = track->nSigmaPion();
        double nsigmaproton = track->nSigmaProton();
        double nsigmakaon = track->nSigmaKaon();
        double dedx = track->dEdx();
        int trackid = track->id();
        float tracketa = track->eta();
        StMuBTofPidTraits tofPid = (StMuBTofPidTraits)track->btofPidTraits();

        StPhysicalHelixD helix = track->helix(); // inner helix. good for dca to PV.

        //   if (track->index2Cov() < 0) continue;
        //   StDcaGeometry* dcaG = mMuDstMaker->muDst()->covGlobTracks(track->index2Cov());
        //   if (!dcaG) {
        //     cout << "No dca Geometry for this track !!!" << endl;
        //     continue;
        //   }
        //   StPhysicalHelixD helix = dcaG->helix();

        StThreeVectorD p = helix.momentum(magn * kilogauss); // momentum at origin
        StThreeVectorD origin = helix.origin(); // origin of helix
        double pathlength = helix.pathLength(
            pv, false); // do scan periods.NOTE:the default is false.this is not necessary for tracks with pt>0.15GeV/c

        StThreeVectorF dca = helix.at(pathlength) - pv;
        float tofbeta = -999.0;
        float tofmass = -999.0;

        if (tofPid.beta() > 0.0) {
            tofbeta = tofPid.beta();
            if (tofbeta < 1e-4 && dca.mag() < 3.0) {
                StThreeVectorF btofHitPos = tofPid.position();
                StThreeVectorF pVtx = muEvent->primaryVertexPosition();
                float L = tofPathLength(&pVtx, &btofHitPos, helix.curvature());
                float tof = tofPid.timeOfFlight();
                if (tof > 0)
                    tofbeta = L / (tof * (C_C_LIGHT / 1.e9));
                else
                    tofbeta = -999.0;
            }
        }

        if (tofbeta > 0.0)
            tofmass = p.mag2() * (1.0 / (tofbeta * tofbeta) - 1.0); //(GeV/c^2)

        // cut on tracks
        if (nHitsFit <= cutNHitsGr)
            continue;
        if (nHitsDedx <= cutNHitsDedxGr)
            continue;
        if (flag <= 0 || flag >= 1000)
            continue;
        // if(track->bad())continue;
        if (abs(charge) != 1)
            continue; // Let's comment out this line for checking

        /*! This was modified by xiaohai. */
        // if (fabs(track->eta()) > 1.0) continue;  // 1.0 to 1.5 to include more He3, pion should keep (-1.,1.0)
        //    if (track->eta() < -1.8 || track->eta() > 0.2) {
        //     continue;
        //  }

        if (Float_t(nHitsFit) / Float_t(track->nHitsPoss()) < 0.52)
            continue;
        if (Float_t(nHitsFit) / Float_t(track->nHitsPoss()) > 1.02)
            continue;
        if (p.perp() < cutPtGrEq)
            continue; // should be larger. like 0.15 or 0.2

        hnSigmaProton->Fill(p.mag() * charge, nsigmaproton);
        hnSigmaPion->Fill(p.mag() * charge, nsigmapion);
        hnSigmaKaon->Fill(p.mag() * charge, nsigmakaon);

        hdEdxP->Fill(p.mag() * charge, dedx); // dedx vs. rigidity.
        if (tofbeta > 0.0)
            hInvBetaP->Fill(p.mag() * charge, 1.0 / tofbeta); // beta vs. ragidity
        hMassP->Fill(p.mag() * charge, tofmass); // mass vs. rigidity.
        hDcaP->Fill(p.mag() * charge, dca.mag()); // Dca vs. rigidity.

        Double_t dau1Z = TMath::Log(dedx / dedx_dau1_th[index_for_p(abs(mCharge1) * p.mag())]); // He3 charge = 2. build
        // in mom p/q,should be
        // corrected here!
        Double_t dau2Z = TMath::Log(dedx / dedx_dau2_th[index_for_p(abs(mCharge2) * p.mag())]); // He3 charge = 2. build
        // in mom p/q,should be
        // corrected here!

        // record the dau1 information.
        if (charge == mCharge1 / abs(mCharge1) && fabs(nsigmaproton) < cutAbsNSigma2Le) {
            // 1st select, will re-select in v0 analysis
            //			if(mRotate)
            //			{
            //				StPhysicalHelixD helixtmp = RotHelix(helix,pv,RotDegree,magn,charge);
            //				helix = helixtmp;
            //
            //				pathlength = helix.pathLength(pv,true);
            //				dca = helix.at(pathlength)-pv;
            //			}

            if (dca.mag() < cutDca1GrEq)
                continue;

            StThreeVectorD dau1p = abs(mCharge1) * p;
            double dau1E = sqrt(dau1p.mag2() + mMass1 * mMass1);
            double dau1diffinvbeta = -999.0;

            if (tofbeta > 0.0)
                dau1diffinvbeta = 1.0 / tofbeta - dau1E / dau1p.mag();

            hdau1dEdxP->Fill(p.mag() * charge, dedx);
            hdau1ZP->Fill(p.mag() * charge, dau1Z);
            hdau1MassP->Fill(p.mag() * charge, tofmass);
            hdau1InvBetaP->Fill(p.mag() * charge, 1.0 / tofbeta);
            hdau1DiffInvBetaP->Fill(p.mag() * charge, dau1diffinvbeta);

            mDauVec1.push_back(track);
            mDauDcaVec1.push_back(dca.mag());
            mDauZVec1.push_back(dau1Z);
            mDauMassVec1.push_back(tofmass);
            mDauBetaVec1.push_back(tofbeta);
            mDauDiffInvBetaVec1.push_back(dau1diffinvbeta);
        }

        // record the dau2 information.
        if (charge == mCharge2 / abs(mCharge2) && fabs(nsigmapion) < cutAbsNSigma2Le) {
            //			if(mRotate)
            //			{
            //				StPhysicalHelixD helixtmp = RotHelix(helix,pv,RotDegree,magn,charge);
            //				helix = helixtmp;
            //
            //				pathlength = helix.pathLength(pv,true);
            //				dca = helix.at(pathlength)-pv;
            //			}

            if (dca.mag() < cutDca2GrEq)
                continue;

            StThreeVectorD dau2p = abs(mCharge2) * p;
            double dau2E = sqrt(dau2p.mag2() + mMass2 * mMass2);
            double dau2diffinvbeta = -999.0;

            if (tofbeta > 0.0)
                dau2diffinvbeta = 1.0 / tofbeta - dau2E / dau2p.mag();

            hdau2dEdxP->Fill(p.mag() * charge, dedx);
            hdau2ZP->Fill(p.mag() * charge, dau2Z);
            hdau2MassP->Fill(p.mag() * charge, tofmass);
            hdau2InvBetaP->Fill(p.mag() * charge, 1.0 / tofbeta);
            hdau2DiffInvBetaP->Fill(p.mag() * charge, dau2diffinvbeta);

            mDauVec2.push_back(track);
            mDauDcaVec2.push_back(dca.mag());
            mDauZVec2.push_back(dau2Z);
            mDauMassVec2.push_back(tofmass);
            mDauBetaVec2.push_back(tofbeta);
            mDauDiffInvBetaVec2.push_back(dau2diffinvbeta);
        }
    }

    assert(mDauVec1.size() == mDauDcaVec1.size() && mDauVec2.size() == mDauDcaVec2.size());
    assert(mDauVec1.size() == mDauZVec1.size() && mDauVec2.size() == mDauZVec2.size());
    assert(mDauVec1.size() == mDauMassVec1.size() && mDauVec2.size() == mDauMassVec2.size());
    assert(mDauVec1.size() == mDauBetaVec1.size() && mDauVec2.size() == mDauBetaVec2.size());
    assert(mDauVec1.size() == mDauDiffInvBetaVec1.size() && mDauVec2.size() == mDauDiffInvBetaVec2.size());

    // reconstruct V0
    int nV0 = 0;

    for (unsigned int i = 0; i < mDauVec1.size(); i++) {
        // get dau1 track info. here
        // cut them before in track selection
        StMuTrack* track1 = mDauVec1[i];
        double dca1 = mDauDcaVec1[i];

        //    if (track1->index2Cov() < 0) continue;
        //    StDcaGeometry* dcaG1 = mMuDstMaker->muDst()->covGlobTracks(track1->index2Cov());
        //    if (!dcaG1) {
        //      cout << "No dca Geometry for this track !!!" << endl;
        //      continue;
        //    }
        //    StPhysicalHelixD helix1 = dcaG1->helix();

        StPhysicalHelixD helix1 = track1->helix(); // inner helix. good for dca to PV.

        //		 if(mRotate)
        //		 {
        //			 StPhysicalHelixD helixtmp = RotHelix(helix1,pv,RotDegree,magn,track1->charge());
        //			 helix1 = helixtmp;
        //		 }

        for (unsigned int j = 0; j < mDauVec2.size(); j++) {
            // get dau2 track info. here
            StMuTrack* track2 = mDauVec2[j];
            double dca2 = mDauDcaVec2[j];

            //  if (track2->index2Cov() < 0) continue;
            //  StDcaGeometry* dcaG2 = mMuDstMaker->muDst()->covGlobTracks(track2->index2Cov());
            //  if (!dcaG2) {
            //    cout << "No dca Geometry for this track !!!" << endl;
            //    continue;
            //  }
            //  StPhysicalHelixD helix2 = dcaG2->helix();

            StPhysicalHelixD helix2 = track2->helix(); // inner helix. good for dca to PV.

            if (track2->id() == track1->id())
                continue; // for same sign

            if (mRotate) {
                StPhysicalHelixD helixtmp = RotHelix(helix2, pv, RotDegree, magn, track2->charge());
                helix2 = helixtmp;
            }

            StThreeVectorF xv0, op1, op2;
            StThreeVectorF ox1, ox2;
            double dca1to2;

            if (!mDcaAlgoLong) {
                pair<double, double> tmps = helix1.pathLengths(helix2);

                ox1 = helix1.at(tmps.first);
                ox2 = helix2.at(tmps.second);

                dca1to2 = (ox1 - ox2).mag();
                xv0 = (ox1 + ox2) / 2.;
                op1 = helix1.momentumAt(tmps.first, magn * kilogauss);
                op2 = helix2.momentumAt(tmps.second, magn * kilogauss);
            }

            // kStHelixDca = true;
            // kMinimize = true;

            // StHelix method above is VERY SLOW. use it only for checking the consistency of long's code
            if (mDcaAlgoLong)
                dca1to2 = closestDistance(helix1, helix2, magn, pv, xv0, op1, op2);

            op1 = abs(mCharge1) * op1;
            op2 = abs(mCharge2) * op2;

            if (dca1to2 > cutDca1to2LeEq)
                continue;

            double oe1 = sqrt(op1.mag2() + mMass1 * mMass1);
            double oe2 = sqrt(op2.mag2() + mMass2 * mMass2);
            double v0mass = sqrt(mMass1 * mMass1 + mMass2 * mMass2 + 2. * oe1 * oe2 - 2. * op1.dot(op2));

            //		 if(fabs(v0mass-mMassV0) > cutV0MassWidthLeEq)continue;

            StThreeVectorD pv0 = op1 + op2;
            StThreeVectorD xv0toPV = xv0 - pv;

            // cut on decay length
            double v0decaylength = xv0toPV.mag();
            //		 if(v0decaylength < cutV0DecLenGrEq )continue;

            // pthead, ptarm cut
            // double pthead1 = op1.dot(pv0)/pv0.mag();
            // double pthead2 = op2.dot(pv0)/pv0.mag();
            // double ptarm   = sqrt(op1.mag2()-pthead1*pthead1);

            // r dot p for v0. cut on it. should be larger than 0.
            double rdotp = xv0toPV.dot(pv0);

            // if(rdotp<cutV0rdotpGr)continue;

            // calculate v0 to PV dca. v0 carry no charge. straight line. cut on dca.

            StPhysicalHelixD helixv0;
            if (mChargeV0 == 0) {
                StPhysicalHelixD tmhelixv0(pv0 / pv0.perp() * 100.0, xv0, magn * kilogauss, mCharge1 / abs(mCharge1));
                helixv0 = tmhelixv0;
            }
            if (mChargeV0 != 0) {
                StPhysicalHelixD tmhelixv0(pv0 / abs(mChargeV0), xv0, magn * kilogauss, mChargeV0 / abs(mChargeV0));
                helixv0 = tmhelixv0;
            }

            Double_t pathlengthv0 = helixv0.pathLength(pv, false);
            StThreeVectorF dcav0tmp = helixv0.at(pathlengthv0) - pv;
            Double_t dcav0toPV = dcav0tmp.mag();

            // cut on v0 dca.
            if (dcav0toPV >= cutDcaV0Le)
                continue;
            if (fabs(pathlengthv0) < cutV0DecLenGrEq)
                continue;

            StThreeVectorD tmpv0;
            if (mChargeV0 == 0) {
                StThreeVectorD tmtmpv0 = pv0;
                tmpv0 = tmtmpv0;
            }
            if (mChargeV0 != 0) {
                StThreeVectorD tmtmpv0 = abs(mChargeV0) * helixv0.momentumAt(pathlengthv0, magn * kilogauss);
                tmpv0 = tmtmpv0;
            }

            StThreeVectorD tmxv0 = helixv0.at(pathlengthv0);

            // cut on sinth, or theta
            double sinth = (xv0toPV.cross(pv0)).mag() / xv0toPV.mag() / pv0.mag();
            double theta = atan2(sinth, rdotp / xv0toPV.mag() / pv0.mag()); // theta range: from 0 to pi

            StMuBTofPidTraits tofPid1 = (StMuBTofPidTraits)track1->btofPidTraits();
            StMuBTofPidTraits tofPid2 = (StMuBTofPidTraits)track2->btofPidTraits();

            mV0Dst.v0mass[nV0] = v0mass;
            mV0Dst.v0pt[nV0] = pv0.perp();
            mV0Dst.v0rapidity[nV0]
                = log((sqrt(v0mass * v0mass + pv0.mag2()) + pv0.z()) / sqrt(v0mass * v0mass + pv0.perp2()));
            mV0Dst.v0eta[nV0] = 0.5 * log((pv0.mag() + pv0.z()) / (pv0.mag() - pv0.z()));
            mV0Dst.v0x[nV0] = xv0.x();
            mV0Dst.v0y[nV0] = xv0.y();
            mV0Dst.v0z[nV0] = xv0.z();
            mV0Dst.v0px[nV0] = pv0.x();
            mV0Dst.v0py[nV0] = pv0.y();
            mV0Dst.v0pz[nV0] = pv0.z();
            mV0Dst.v0xpath[nV0] = tmxv0.x();
            mV0Dst.v0ypath[nV0] = tmxv0.y();
            mV0Dst.v0zpath[nV0] = tmxv0.z();
            mV0Dst.v0pxpath[nV0] = tmpv0.x();
            mV0Dst.v0pypath[nV0] = tmpv0.y();
            mV0Dst.v0pzpath[nV0] = tmpv0.z();
            mV0Dst.v0declen[nV0] = v0decaylength;
            mV0Dst.v0declenH[nV0] = fabs(pathlengthv0);
            mV0Dst.v0dca[nV0] = dcav0toPV;
            mV0Dst.v0rdotp[nV0] = rdotp;
            mV0Dst.v0sinth[nV0] = sinth;
            mV0Dst.v0theta[nV0] = theta;
            mV0Dst.dca1to2[nV0] = dca1to2;

            mV0Dst.dau1id[nV0] = track1->id();
            mV0Dst.dau1dca[nV0] = dca1;
            mV0Dst.dau1nhitsfit[nV0] = track1->nHitsFit(kTpcId);
            mV0Dst.dau1nhitsdedx[nV0] = track1->nHitsDedx();
            mV0Dst.dau1nhitsposs[nV0] = track1->nHitsPoss();
            mV0Dst.dau1eta[nV0] = track1->eta();
            mV0Dst.dau1nsigma[nV0] = track1->nSigmaProton();
            mV0Dst.dau1px[nV0] = op1.x();
            mV0Dst.dau1py[nV0] = op1.y();
            mV0Dst.dau1pz[nV0] = op1.z();
            mV0Dst.dau1pt[nV0] = op1.perp();
            mV0Dst.dau1dEdx[nV0] = track1->dEdx();
            mV0Dst.dau1Z[nV0] = mDauZVec1[i];
            mV0Dst.dau1hftbits[nV0] = track1->topologyMap().data(0);
            mV0Dst.dau1mass[nV0] = mDauMassVec1[i];
            mV0Dst.dau1diffinvbeta[nV0] = mDauDiffInvBetaVec1[i];
            mV0Dst.dau1beta[nV0] = mDauBetaVec1[i];
            mV0Dst.dau1toflocaly[nV0] = tofPid1.yLocal();
            mV0Dst.dau1toflocalz[nV0] = tofPid1.zLocal();

            mV0Dst.dau2id[nV0] = track2->id();
            mV0Dst.dau2dca[nV0] = dca2;
            mV0Dst.dau2nhitsfit[nV0] = track2->nHitsFit(kTpcId);
            mV0Dst.dau2nhitsdedx[nV0] = track2->nHitsDedx();
            mV0Dst.dau2nhitsposs[nV0] = track2->nHitsPoss();
            mV0Dst.dau2eta[nV0] = track2->eta();
            mV0Dst.dau2nsigma[nV0] = track2->nSigmaPion();
            mV0Dst.dau2pt[nV0] = op2.perp();
            mV0Dst.dau2px[nV0] = op2.x();
            mV0Dst.dau2py[nV0] = op2.y();
            mV0Dst.dau2pz[nV0] = op2.z();
            mV0Dst.dau2dEdx[nV0] = track2->dEdx();
            mV0Dst.dau2Z[nV0] = mDauZVec2[j];
            mV0Dst.dau2hftbits[nV0] = track2->topologyMap().data(0);
            mV0Dst.dau2mass[nV0] = mDauMassVec2[j];
            mV0Dst.dau2diffinvbeta[nV0] = mDauDiffInvBetaVec2[j];
            mV0Dst.dau2beta[nV0] = mDauBetaVec2[j];
            mV0Dst.dau2toflocaly[nV0] = tofPid2.yLocal();
            mV0Dst.dau2toflocalz[nV0] = tofPid2.zLocal();
            nV0++;

            hInvMass->Fill(v0mass);
            // mV0Vec.push_back(v0data);
        }
    }

    // cout<<"v0 loop done:: "<<nV0<<endl;

    mV0Dst.nv0 = nV0;

    if (nV0 > 0 && mV0Tree)
        mV0Tree->Fill();

    // dump v0 vector into a TTree
    mEventsProcessed++;
    mDauVec1.clear();
    mDauVec2.clear();
    mDauDcaVec1.clear();
    mDauDcaVec2.clear();
    mDauZVec1.clear();
    mDauZVec2.clear();
    mDauMassVec1.clear();
    mDauMassVec2.clear();
    mDauBetaVec1.clear();
    mDauBetaVec2.clear();
    mDauDiffInvBetaVec1.clear();
    mDauDiffInvBetaVec2.clear();
    // mV0Vec.clear();

    return kStOK;
}

Int_t StV0Maker::Finish()
{
    // Do once at the end the analysis

    // Write histograms to disk, output miscellaneous other information
    if (histogram_output != NULL)
        histogram_output->Write(); // Write all histograms to disk
    if (v0tree_output != NULL)
        v0tree_output->Write(); // Write all histograms to disk

    cout << "Total Events Processed in StV0Maker " << mEventsProcessed << endl;

    return kStOK;
}

StPhysicalHelixD StV0Maker::RotHelix(StPhysicalHelixD Helix, StThreeVectorD Pv, double degree, double magn, int charge)
{
    double SIN = sin((degree / 180.0) * TMath::Pi()) * 1000.0;
    double COS = cos((degree / 180.0) * TMath::Pi()) * 1000.0;

    int SINTMP = int(SIN);
    int COSTMP = int(COS);

    SIN = SINTMP / 1000.0;
    COS = COSTMP / 1000.0;

    StThreeVectorD ptmp = Helix.momentum(magn * kilogauss);
    StThreeVectorD xtmp = Helix.origin();

    double ptmpx = ptmp.x() * COS - ptmp.y() * SIN;
    double ptmpy = ptmp.y() * COS + ptmp.x() * SIN;

    double xtmpx = (xtmp.x() - Pv.x()) * COS - (xtmp.y() - Pv.y()) * SIN + Pv.x();
    double xtmpy = (xtmp.y() - Pv.y()) * COS + (xtmp.x() - Pv.x()) * SIN + Pv.y();

    ptmp.setX(ptmpx);
    ptmp.setY(ptmpy);
    xtmp.setX(xtmpx);
    xtmp.setY(xtmpy);

    StPhysicalHelixD helixtmp(ptmp, xtmp, magn * kilogauss, charge);

    return helixtmp;
}

int StV0Maker::index_for_p(float tp)
{
    int i = int((tp - 0.1) / 0.001 + 0.5);
    if (i < 0)
        return 0;
    else if (i > 11900)
        return 11900;
    else
        return i;
    return 0;
}
