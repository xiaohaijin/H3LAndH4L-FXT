/***************************************************************************
 *
 * $Id: StMuEvent.h,v 1.35 2017/04/17 20:40:56 smirnovd Exp $
 * Author: Frank Laue, BNL, laue@bnl.gov
 *
 ***************************************************************************/
#ifndef StMuEvent_h
#define StMuEvent_h

#include "TObject.h"
#include "TArrayI.h"
#include "COMMON/StMuL3EventSummary.h"
#include "COMMON/StMuEmcCollection.h"
#include "COMMON/StMuFmsCollection.h"
#include "COMMON/StMuTriggerIdCollection.h"

#include "StEventInfo.h"
#include "StRunInfo.h"
#include "StEventSummary.h"
#include "StVpdTriggerDetector.h"
#include "StMtdTriggerDetector.h"
#include "StCtbTriggerDetector.h"
#include "StZdcTriggerDetector.h"
#include "StBbcTriggerDetector.h"
#include "StEmcTriggerDetector.h"
#include "StFpdTriggerDetector.h"
#include "StFmsTriggerDetector.h"
#include "StFpdCollection.h"
#include "StL0Trigger.h"
#include "StTriggerIdCollection.h"
#include "StDetectorState.h"

#ifndef __NO_STRANGE_MUDST__
#include "StStrangeMuDst.hh"
#endif
//#include "StarClassLibrary/StThreeVectorD.hh"

class StEvent;
class StMuCut;
class StTofCollection; // calibrated vpd for TOF
class StTriggerData;

/**
   @class StMuEvent
   The StMuEvent class holds the event-wise information of the STAR's common muDst.
   Most of its data members are classes from the StEvent package.
   Please refer to the StEvent manual for information on these classes.
 */
class StMuEvent : public TObject {
public:
    StMuEvent();
    StMuEvent(const StEvent*);
    virtual ~StMuEvent();

    int eventId();
    int eventNumber();
    int runId();
    int runNumber();
    StRunInfo& runInfo();
    StEventInfo& eventInfo();
    StEventSummary& eventSummary();

    StVpdTriggerDetector& vpdTriggerDetector();
    StMtdTriggerDetector& mtdTriggerDetector();
    StCtbTriggerDetector& ctbTriggerDetector();
    StZdcTriggerDetector& zdcTriggerDetector();
    StBbcTriggerDetector& bbcTriggerDetector();
    StEmcTriggerDetector& emcTriggerDetector();
    StFpdTriggerDetector& fpdTriggerDetector();
    StFmsTriggerDetector& fmsTriggerDetector();
    StFpdCollection& fpdCollection();
    StL0Trigger& l0Trigger();
    // Special classes for the muDst
    StMuL3EventSummary& l3EventSummary();
    StMuTriggerIdCollection& triggerIdCollection();
    const StTriggerData* triggerData() const;

    /// Reference multiplicity of positive particles as defined in StEventUtilities/StuRefMult.hh for vertex vtx_id (-1
    /// is default index from StMuDst)
    unsigned short refMultPos(int vtx_id = -1);
    /// Reference multiplicity of negative particles as defined in StEventUtilities/StuRefMult.hh for vertex vtx_id (-1
    /// is default index from StMuDst)
    unsigned short refMultNeg(int vtx_id = -1);
    /// Reference multiplicity of charged particles as defined in StEventUtilities/StuRefMult.hh for vertex vtx_id (-1
    /// is default index from StMuDst)
    unsigned short refMult(int vtx_id = -1);
    /// Reference multiplicity of particles in the east FTPC as defined in StEventUtilities/StuFtpcRefMult.hh for vertex
    /// vtx_id (-1 is default index from StMuDst)
    unsigned short refMultFtpcEast(int vtx_id = -1);
    /// Reference multiplicity of particles in the west FTPC as defined in StEventUtilities/StuFtpcRefMult.hh for vertex
    /// vtx_id (-1 is default index from StMuDst)
    unsigned short refMultFtpcWest(int vtx_id = -1);
    /// Reference multiplicity of particles in the east+west FTPC as defined in StEventUtilities/StuFtpcRefMult.hh for
    /// vertex vtx_id (-1 is default index from StMuDst)
    unsigned short refMultFtpc(int vtx_id = -1);
    unsigned short grefmult(int vtx_id = -1);
    unsigned short btofTrayMultiplicity();
    float nearestVertexZ(int vtx_id = -1);

    /// Currently not filled properly.
    double reactionPlane(unsigned short);
    void setReactionPlane(unsigned short, double v);
    /// Currently not filled properly.
    double reactionPlanePtWgt(unsigned short);
    void setReactionPlanePtWgt(unsigned short, double v);
    double magneticField();
    double zdcAdcAttentuatedSumWest();
    double zdcAdcAttentuatedSumEast();
    double ctbMultiplicity();
    ///    The StMuDst is supposed to be structured in 'physical events'.  Therefore there is only 1 primary vertex per
    /// mu event.
    StThreeVectorF primaryVertexPosition(int vtx_id = -1) const;
    StThreeVectorF primaryVertexErrors(int vtx_id = -1) const;
    TArrayI& L2Result(); // Raw L2Result[] array

    // Calibrated VPD info from StTofCollection in StEvent
    unsigned int numberOfVpdEastHits();
    unsigned int numberOfVpdWestHits();
    float vpdTstart();
    float vpdTdiff();
    float vpdVz();

    unsigned int numberOfPxlInnerHits();
    unsigned int numberOfPxlOuterHits();
    unsigned int numberOfIstHits();
    unsigned int numberOfSsdHits();

protected:
    void clear();
    void fill(const StEvent*);

    // classes that we just takes from StEvent
    StRunInfo mRunInfo;
    StEventInfo mEventInfo;
    StEventSummary mEventSummary;
    StVpdTriggerDetector mVpdTriggerDetector;
    StMtdTriggerDetector mMtdTriggerDetector;
    StCtbTriggerDetector mCtbTriggerDetector;
    StZdcTriggerDetector mZdcTriggerDetector;
    StBbcTriggerDetector mBbcTriggerDetector;
    StEmcTriggerDetector mEmcTriggerDetector;
    StFpdTriggerDetector mFpdTriggerDetector;
    StFmsTriggerDetector mFmsTriggerDetector;
    StFpdCollection mFpdCollection;
    StL0Trigger mL0Trigger;
    // special classes from MuDst
    StMuL3EventSummary mL3EventSummary;
    StMuTriggerIdCollection mTriggerIdCollection;
    StTriggerData* mTriggerData;

    UShort_t mRefMultPos;
    UShort_t mRefMultNeg;
    UShort_t mRefMultFtpcEast;
    UShort_t mRefMultFtpcWest;
    Float_t mReactionPlane[2];
    Float_t mReactionPlanePtWgt[2];

    UShort_t mNHitsHFT[4];

    StThreeVectorF mPrimaryVertexError;

    TArrayI mL2Result; // Raw L2 info

    UInt_t mVpdEast; // VPD East Hit pattern
    UInt_t mVpdWest; // VPD West Hit pattern
    Float_t mVpdTstart; // VPD start time (calibrated)
    Float_t mVpdTdiff; // VPD time difference (calibrated)
    Float_t mVpdVz; // VPD vertex z

    friend class StMuDst;
    friend class StMuDstMaker;
    friend class StMuMomentumShiftMaker;
    friend class StMuL3EventSummary;
    ClassDef(StMuEvent, 14)
};

inline int StMuEvent::eventId()
{
    return mEventInfo.id();
}
inline int StMuEvent::eventNumber()
{
    return mEventInfo.id();
}
inline int StMuEvent::runId()
{
    return mEventInfo.runId();
}
inline int StMuEvent::runNumber()
{
    return mEventInfo.runId();
}
inline StRunInfo& StMuEvent::runInfo()
{
    return mRunInfo;
}
inline StEventInfo& StMuEvent::eventInfo()
{
    return mEventInfo;
}
inline StEventSummary& StMuEvent::eventSummary()
{
    return mEventSummary;
}
inline StVpdTriggerDetector& StMuEvent::vpdTriggerDetector()
{
    return mVpdTriggerDetector;
}
inline StMtdTriggerDetector& StMuEvent::mtdTriggerDetector()
{
    return mMtdTriggerDetector;
}
inline StCtbTriggerDetector& StMuEvent::ctbTriggerDetector()
{
    return mCtbTriggerDetector;
}
inline StZdcTriggerDetector& StMuEvent::zdcTriggerDetector()
{
    return mZdcTriggerDetector;
}
inline StBbcTriggerDetector& StMuEvent::bbcTriggerDetector()
{
    return mBbcTriggerDetector;
}
inline StEmcTriggerDetector& StMuEvent::emcTriggerDetector()
{
    return mEmcTriggerDetector;
}
inline StFpdTriggerDetector& StMuEvent::fpdTriggerDetector()
{
    return mFpdTriggerDetector;
}
inline StFmsTriggerDetector& StMuEvent::fmsTriggerDetector()
{
    return mFmsTriggerDetector;
}
inline StFpdCollection& StMuEvent::fpdCollection()
{
    return mFpdCollection;
}
inline StL0Trigger& StMuEvent::l0Trigger()
{
    return mL0Trigger;
}
// special classes for muDst
inline StMuL3EventSummary& StMuEvent::l3EventSummary()
{
    return mL3EventSummary;
}
inline StMuTriggerIdCollection& StMuEvent::triggerIdCollection()
{
    return mTriggerIdCollection;
}
// inline const StTriggerData* StMuEvent::triggerData() const { if(mTriggerData!=0) {mTriggerData->setDebug(0); return
// mTriggerData; } else return 0; }
inline const StTriggerData* StMuEvent::triggerData() const
{
    return mTriggerData;
}
inline double StMuEvent::reactionPlane(unsigned short s)
{
    return (s == 0) ? mReactionPlane[0] : mReactionPlane[1];
}
inline void StMuEvent::setReactionPlane(unsigned short s, double v)
{
    (s == 0) ? mReactionPlane[0] = v : mReactionPlane[1] = v;
}
inline double StMuEvent::reactionPlanePtWgt(unsigned short s)
{
    return (s == 0) ? mReactionPlanePtWgt[0] : mReactionPlanePtWgt[1];
}
inline void StMuEvent::setReactionPlanePtWgt(unsigned short s, double v)
{
    (s == 0) ? mReactionPlanePtWgt[0] = v : mReactionPlanePtWgt[1] = v;
}
inline double StMuEvent::magneticField()
{
    return mEventSummary.magneticField();
}
inline double StMuEvent::zdcAdcAttentuatedSumWest()
{
    return mZdcTriggerDetector.adc(10);
}
inline double StMuEvent::zdcAdcAttentuatedSumEast()
{
    return mZdcTriggerDetector.adc(13);
}
inline double StMuEvent::ctbMultiplicity()
{
    double ctb = 0;
    for (unsigned int slat = 0; slat < mCtbTriggerDetector.numberOfSlats(); slat++) {
        for (unsigned int tray = 0; tray < mCtbTriggerDetector.numberOfTrays(); tray++) {
            ctb += mCtbTriggerDetector.mips(tray, slat, 0);
        }
    }
    return ctb;
}
inline TArrayI& StMuEvent::L2Result()
{
    return mL2Result;
}
inline unsigned int StMuEvent::numberOfVpdEastHits()
{
    unsigned int num = 0;
    for (int i = 0; i < 32; i++) {
        num += mVpdEast >> i & 1;
    }
    return num;
}
inline unsigned int StMuEvent::numberOfVpdWestHits()
{
    unsigned int num = 0;
    for (int i = 0; i < 32; i++) {
        num += mVpdWest >> i & 1;
    }
    return num;
}
inline float StMuEvent::vpdTstart()
{
    return mVpdTstart;
}
inline float StMuEvent::vpdTdiff()
{
    return mVpdTdiff;
}
inline float StMuEvent::vpdVz()
{
    return mVpdVz;
}
inline unsigned int StMuEvent::numberOfPxlInnerHits()
{
    return mNHitsHFT[0];
}
inline unsigned int StMuEvent::numberOfPxlOuterHits()
{
    return mNHitsHFT[1];
}
inline unsigned int StMuEvent::numberOfIstHits()
{
    return mNHitsHFT[2];
}
inline unsigned int StMuEvent::numberOfSsdHits()
{
    return mNHitsHFT[3];
}

#endif
