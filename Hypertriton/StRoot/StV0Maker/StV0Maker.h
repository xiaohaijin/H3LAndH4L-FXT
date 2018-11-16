#ifndef StV0Maker_def
#define StV0Maker_def

#include "StMaker.h"
#include "TString.h"
#include "StPhysicalHelixD.hh"
#include "StThreeVectorD.hh"

class StMuDstMaker;
class StMuTrack;
class TFile;
class TTree;
class TH1F;
class TH2F;

#include "StV0Type.h"
#include "StV0Dst.h"

class StV0Maker : public StMaker {

private:
    // constants

    // Make MuDst pointer available to member functions
    StMuDstMaker* mMuDstMaker;

    // master switch for the analysis, v0 type
    StV0Type mV0Type;

    // switch for rotating the coordinates and momenta (in transverse direction) of one daughter.
    // for background estimation.
    bool mRotate;
    bool mSameSignPlus;
    bool mSameSignMinus;
    bool mDcaAlgoLong;

    // section of v0type related constants
    Float_t mMassV0;
    Float_t mMass1;
    Float_t mMass2;
    Int_t mChargeV0;
    Int_t mCharge1;
    Int_t mCharge2;

    // section of parameters for v0 analysis
    float cutAbsVertexZLeEq;
    int cutNHitsGr;
    int cutNHitsDedxGr;
    float cutPtGrEq;

    float cutAbsNSigma2Le;
    float cutDca1GrEq;
    float cutDca2GrEq;

    float cutDca1to2LeEq;
    float cutV0MassWidthLeEq;
    float cutDauPtArmLeEq;
    float cutAbsDausPtShoulderDiffLeEq;
    float cutDau1DecAngGr;
    float cutDau2DecAngGr;
    float cutV0rdotpGr;
    float cutDcaV0Le;
    float cutV0DecLenGrEq;
    float cutDau1Dau2Ang3DLe;
    float cutDau1Dau2DipAngDiffLe;

    double RotDegree;

    // histograms (mostly for QA purpose)
    TH1F* hNPrimVertex;
    TH1F* hVertexZ;
    TH1F* hVertexZdiff;
    TH1F* hNRefMult;
    TH1F* hSelectNRefMult;
    TH1F* hMagneticField;

    TH2F* hnSigmaProton;
    TH2F* hnSigmaPion;
    TH2F* hnSigmaKaon;
    TH2F* hdEdxP;
    TH2F* hDcaP;
    TH2F* hMassP;
    TH2F* hInvBetaP;
    TH2F* hdau1dEdxP;
    TH2F* hdau2dEdxP;
    TH2F* hdau1ZP;
    TH2F* hdau2ZP;
    TH2F* hdau1MassP;
    TH2F* hdau2MassP;
    TH2F* hdau1InvBetaP;
    TH2F* hdau2InvBetaP;
    TH2F* hdau1DiffInvBetaP;
    TH2F* hdau2DiffInvBetaP;

    TH1F* hInvMass;

    // dEdx information of tracks.
    double dedx_dau1_th[11901];
    double dedx_dau2_th[11901];

    // files related
    TString mHistogramOutputFileName; //  Name of the histogram output file
    TString mV0TreeOutputFileName; //  Name of the v0 tree output file
    TFile* histogram_output; //  Histograms outputfile pointer
    TFile* v0tree_output; //  V0 Tree outputfile pointer
    TTree* mV0Tree; // V0 Tree outputfile pointer
    StV0Dst mV0Dst; // V0 event (picoDst), to fill the tree

    // statistic information
    UInt_t mEventsProcessed; //  Number of Events read and processed

    // some diagnosing variables
    Float_t mTestVZ;
    UInt_t mTestNTrack;
    bool mPassEventCut;

    // temporary variables (put here for performance consideration)
    vector<StMuTrack*> mDauVec1;
    vector<StMuTrack*> mDauVec2;
    vector<double> mDauDcaVec1;
    vector<double> mDauDcaVec2;
    vector<double> mDauZVec1;
    vector<double> mDauZVec2;
    vector<double> mDauMassVec1;
    vector<double> mDauMassVec2;
    vector<double> mDauBetaVec1;
    vector<double> mDauBetaVec2;
    vector<double> mDauDiffInvBetaVec1;
    vector<double> mDauDiffInvBetaVec2;

    // private member functions
    void initParam(); // initialize parameters
    void initConst(); // initialize constants for v0type
    void initHisto(); // book histograms
    void initTree(); // book tree

protected:
    // I do not expect some class inherits this maker!

public:
    StV0Maker(StMuDstMaker* maker, const char* name); // Constructor
    virtual ~StV0Maker(); // Destructor

    Int_t Init(); // Initiliaze the analysis tools ... done once
    Int_t Make(); // The main analysis that is done on each event
    Int_t Finish(); // Finish the analysis, close files, and clean up.

    void setHistoFileName(TString name)
    {
        mHistogramOutputFileName = name;
    } // set the name of output file for histograms
    void setV0TreeFileName(TString name)
    {
        mV0TreeOutputFileName = name;
    } // set the name of output file for StV0Dst
    void setV0Type(StV0Type v0type)
    {
        mV0Type = v0type;
    } // set the v0 type: kLambda,kAntiLambda,kKs
    void setRotate(bool brot)
    {
        mRotate = brot;
    } // set rotation option
    void setSameSignPlus(bool brot)
    {
        mSameSignPlus = brot;
    } // set same sign plus
    void setSameSignMinus(bool brot)
    {
        mSameSignMinus = brot;
    } // set same sign minus

    const StV0Dst& getV0Dst() const
    {
        return mV0Dst;
    }
    bool passEventCut() const
    {
        return mPassEventCut;
    }
    // Rotate dau1 or dau2 tracks by certain degrees in the azimuthal plane with respect to the primary vertex.
    StPhysicalHelixD RotHelix(StPhysicalHelixD Helix, StThreeVectorD Pv, double degree, double magn, int charge);

    int index_for_p(float);

    ClassDef(StV0Maker, 1) // Macro for CINT compatability
};

#endif
