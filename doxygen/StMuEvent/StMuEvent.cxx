
/***************************************************************************
 *
 * $Id: StMuEvent.cxx,v 1.29 2017/04/17 20:40:56 smirnovd Exp $
 * Author: Frank Laue, BNL, laue@bnl.gov
 *
 ***************************************************************************/

#include <string.h>
#include "StEvent.h"
#include "StEventTypes.h"
#include "StEventSummary.h"
#include "StEventInfo.h"
#include "StDetectorState.h"
#include "StIstHitCollection.h"
#include "StSsdHitCollection.h"
#include "StPxlHitCollection.h"
#include "StPxlSectorHitCollection.h"
#include "StPxlLadderHitCollection.h"

#include "StuRefMult.hh"
#include "StuFtpcRefMult.hh"

#include "SystemOfUnits.h"
#include "StTimer.hh"

#include "COMMON/StMuException.hh"
#include "StMuEvent.h"
#include "COMMON/StMuTrack.h"
#include "COMMON/StMuL3EventSummary.h"
#include "COMMON/StMuCut.h"
#include "COMMON/StMuDebug.h"
#include "COMMON/StMuDst.h"
#include "COMMON/StMuPrimaryVertex.h"
#include "COMMON/StMuBTofHit.h"

#include "TClonesArray.h"
#include "TObject.h"
#include "TClass.h"

ClassImp(StMuEvent)

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
StMuEvent::StMuEvent()
    : mTriggerData(0)
    , mPrimaryVertexError(-999., -999., -999)
{
    DEBUGMESSAGE("");
    int n = (char*)mReactionPlanePtWgt - (char*)&mRefMultPos + sizeof(mReactionPlanePtWgt);
    memset(&mRefMultPos, 0, n);
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
StMuEvent::StMuEvent(const StEvent* event) : mPrimaryVertexError(-999., -999., -999.)
{
    try
    {
        fill(event);
    }
    catch (StMuException e)
    {
        throw e;
    }
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
StMuEvent::~StMuEvent()
{
    DEBUGMESSAGE("");
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuEvent::clear()
{
    DEBUGMESSAGE1("");
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuEvent::fill(const StEvent* event)
{
    DEBUGMESSAGE("");
    if (!event)
        throw StMuExceptionNullPointer("no StEvent", __PRETTYF__);
    if (!event->info())
        throw StMuExceptionNullPointer("no event info", __PRETTYF__);
    if (!event->runInfo())
        throw StMuExceptionNullPointer("no run info", __PRETTYF__);
    if (!event->summary())
        throw StMuExceptionNullPointer("no event summary", __PRETTYF__);
    //  if (event->numberOfPrimaryVertices() != 1 ) throw StMuExceptionBadValue("!= 1 primary vertex");

    /// classes that we just copy from StEvent
    mRunInfo = *event->runInfo();
    mEventInfo = *event->info();
    mEventSummary = *event->summary();
    const StPrimaryVertex* p_vtx = event->primaryVertex();
    if (p_vtx) {
        mPrimaryVertexError = p_vtx->positionError();
    }

    if (!event->triggerDetectorCollection()) {
        DEBUGVALUE2(event->type());
        DEBUGVALUE2(event->info()->time());
        DEBUGMESSAGE2("no trigger detector collection, creating dummy");
    } else {
        mVpdTriggerDetector = event->triggerDetectorCollection()->vpd();
        mMtdTriggerDetector = event->triggerDetectorCollection()->mtd();
        mCtbTriggerDetector = event->triggerDetectorCollection()->ctb();
        mZdcTriggerDetector = event->triggerDetectorCollection()->zdc();
        mBbcTriggerDetector = event->triggerDetectorCollection()->bbc();
        mEmcTriggerDetector = event->triggerDetectorCollection()->emc();
        mFpdTriggerDetector = event->triggerDetectorCollection()->fpd();
        mFmsTriggerDetector = event->triggerDetectorCollection()->fms();
    }

    if (event->fpdCollection())
        mFpdCollection = *event->fpdCollection();
    if (event->l0Trigger())
        mL0Trigger = *event->l0Trigger();
    mL3EventSummary.fill(event);

    mTriggerIdCollection.fill(event->triggerIdCollection()); // pointer check done inside

    mRefMultPos = uncorrectedNumberOfPositivePrimaries(*event);
    mRefMultNeg = uncorrectedNumberOfNegativePrimaries(*event);
    mRefMultFtpcEast = uncorrectedNumberOfFtpcEastPrimaries(*event);
    mRefMultFtpcWest = uncorrectedNumberOfFtpcWestPrimaries(*event);

    if (event->triggerData()) // MC has no triggerData
        mL2Result.Set(event->triggerData()->l2ResultLength(), (const Int_t*)event->triggerData()->l2Result());

    // calibrated vpd for TOF - X.Dong
    mVpdEast = mVpdWest = 0;
    mVpdTstart = mVpdTdiff = 0.;
    if (event->tofCollection()) {
        mVpdEast = event->tofCollection()->vpdEast();
        mVpdWest = event->tofCollection()->vpdWest();
        mVpdTstart = event->tofCollection()->tstart();
        mVpdTdiff = event->tofCollection()->tdiff();
        mVpdVz = event->tofCollection()->vzVpd();
    }
    // trigger data
    mTriggerData = const_cast<StTriggerData*>(event->triggerData());
    if (mTriggerData != 0)
        mTriggerData->setDebug(0);

    // HFT hits per layer - dongx
    for (int i = 0; i < 4; i++)
        mNHitsHFT[i] = 0;
    const StPxlHitCollection* PxlHitCollection = event->pxlHitCollection();
    if (PxlHitCollection) {
        UInt_t numberOfSectors = PxlHitCollection->numberOfSectors();
        for (UInt_t i = 0; i < numberOfSectors; i++) {
            const StPxlSectorHitCollection* PxlSectorHitCollection = PxlHitCollection->sector(i);
            if (!PxlSectorHitCollection)
                continue;

            UInt_t numberOfLadders = PxlSectorHitCollection->numberOfLadders();
            for (UInt_t j = 0; j < numberOfLadders; j++) {
                const StPxlLadderHitCollection* PxlLadderHitCollection = PxlSectorHitCollection->ladder(j);
                if (!PxlLadderHitCollection)
                    continue;

                if (j == 0)
                    mNHitsHFT[0] += PxlLadderHitCollection->numberOfHits();
                else
                    mNHitsHFT[1] += PxlLadderHitCollection->numberOfHits();
            }
        }
    } // PXL hits

    const StIstHitCollection* IstHitCollection = event->istHitCollection();
    if (IstHitCollection)
        mNHitsHFT[2] += IstHitCollection->numberOfHits();

    const StSsdHitCollection* SsdHitCollection = event->ssdHitCollection();
    if (SsdHitCollection)
        mNHitsHFT[3] += SsdHitCollection->numberOfHits();
    //
}

unsigned short StMuEvent::refMultPos(int vtx_id)
{
    // Check old format (no StMuPrimaryVertex stored)
    if (StMuDst::numberOfPrimaryVertices() == 0 && (vtx_id == 0 || vtx_id == -1))
        return mRefMultPos;
    if (vtx_id == -1)
        vtx_id = StMuDst::currentVertexIndex();
    if (StMuDst::primaryVertex(vtx_id))
        return StMuDst::primaryVertex(vtx_id)->refMultPos();
    return 0;
}

unsigned short StMuEvent::refMultNeg(int vtx_id)
{
    // Check old format (no StMuPrimaryVertex stored)
    if (StMuDst::numberOfPrimaryVertices() == 0 && (vtx_id == 0 || vtx_id == -1))
        return mRefMultNeg;
    if (vtx_id == -1)
        vtx_id = StMuDst::currentVertexIndex();
    if (StMuDst::primaryVertex(vtx_id))
        return StMuDst::primaryVertex(vtx_id)->refMultNeg();
    return 0;
}

unsigned short StMuEvent::refMult(int vtx_id)
{
    return refMultPos(vtx_id) + refMultNeg(vtx_id);
}

unsigned short StMuEvent::refMultFtpcEast(int vtx_id)
{
    // Check old format (no StMuPrimaryVertex stored)
    if (StMuDst::numberOfPrimaryVertices() == 0 && (vtx_id == 0 || vtx_id == -1))
        return mRefMultFtpcEast;
    if (vtx_id == -1)
        vtx_id = StMuDst::currentVertexIndex();
    if (StMuDst::primaryVertex(vtx_id))
        return StMuDst::primaryVertex(vtx_id)->refMultFtpcEast();
    return 0;
}

unsigned short StMuEvent::refMultFtpcWest(int vtx_id)
{
    // Check old format (no StMuPrimaryVertex stored)
    if (StMuDst::numberOfPrimaryVertices() == 0 && (vtx_id == 0 || vtx_id == -1))
        return mRefMultFtpcWest;
    if (vtx_id == -1)
        vtx_id = StMuDst::currentVertexIndex();
    if (StMuDst::primaryVertex(vtx_id))
        return StMuDst::primaryVertex(vtx_id)->refMultFtpcWest();
    return 0;
}

unsigned short StMuEvent::refMultFtpc(int vtx_id)
{
    return refMultFtpcEast(vtx_id) + refMultFtpcWest(vtx_id);
}

StThreeVectorF StMuEvent::primaryVertexPosition(int vtx_id) const
{
    StThreeVectorF vz(-999, -999, -999);
    // Check old format (no StMuPrimaryVertex stored)
    if (StMuDst::numberOfPrimaryVertices() == 0 && (vtx_id == 0 || vtx_id == -1)) {
        if (fabs(mEventSummary.primaryVertexPosition().x()) < 1.e-5
            && fabs(mEventSummary.primaryVertexPosition().y()) < 1.e-5
            && fabs(mEventSummary.primaryVertexPosition().z()) < 1.e-5)
            return vz;
        else
            return mEventSummary.primaryVertexPosition();
    }
    if (vtx_id == -1)
        vtx_id = StMuDst::currentVertexIndex();
    if (StMuDst::primaryVertex(vtx_id))
        return StMuDst::primaryVertex(vtx_id)->position();
    return vz;
}
StThreeVectorF StMuEvent::primaryVertexErrors(int vtx_id) const
{
    StThreeVectorF vz(-999, -999, -999);
    // Check old format (no StMuPrimaryVertex stored)
    if (StMuDst::numberOfPrimaryVertices() == 0 && (vtx_id == 0 || vtx_id == -1)) {
        if (fabs(mEventSummary.primaryVertexPosition().x()) < 1.e-5
            && fabs(mEventSummary.primaryVertexPosition().y()) < 1.e-5
            && fabs(mEventSummary.primaryVertexPosition().z()) < 1.e-5)
            return vz;
        else
            return mPrimaryVertexError;
    }
    if (vtx_id == -1)
        vtx_id = StMuDst::currentVertexIndex();
    if (StMuDst::primaryVertex(vtx_id))
        return StMuDst::primaryVertex(vtx_id)->posError();
    return vz;
}

unsigned short StMuEvent::grefmult(int vtx_id)
{
    unsigned short grefmult = 0;
    StMuTrack* glob;
    // For old MuDsts where there was one vertex per event
    if (StMuDst::numberOfPrimaryVertices() == 0 && (vtx_id == 0 || vtx_id == -1)) {
        if (!(fabs(mEventSummary.primaryVertexPosition().x()) < 1.e-5
              && fabs(mEventSummary.primaryVertexPosition().y()) < 1.e-5
              && fabs(mEventSummary.primaryVertexPosition().z()) < 1.e-5)) {
            for (int i = 0; i < StMuDst::globalTracks()->GetEntriesFast(); i++) {
                glob = StMuDst::globalTracks(i);
                if (fabs(glob->eta()) < 0.5 && fabs(glob->dcaGlobal().mag()) < 3 && glob->nHitsFit(kTpcId) >= 10)
                    grefmult++;
            }
            return grefmult;
        } else
            return 0;
    }

    if (vtx_id == -1)
        vtx_id = StMuDst::currentVertexIndex();

    if (StMuDst::primaryVertex(vtx_id)) {
        for (int i = 0; i < StMuDst::globalTracks()->GetEntriesFast(); i++) {
            glob = StMuDst::globalTracks(i);
            if (fabs(glob->eta()) < 0.5 && fabs(glob->dcaGlobal(vtx_id).mag()) < 3 && glob->nHitsFit(kTpcId) >= 10)
                grefmult++;
        }
        return grefmult;
    } else
        return 0;
}

unsigned short StMuEvent::btofTrayMultiplicity()
{

    unsigned short btofmult = (unsigned short)StMuDst::numberOfBTofHit();
    for (unsigned int i = 0; i < StMuDst::numberOfBTofHit(); i++)
        if (StMuDst::btofHit(i)->tray() > 120)
            btofmult--;
    return btofmult;
}

float StMuEvent::nearestVertexZ(int vtx_id)
{

    float dz = 999.0;
    // For old MuDsts where there was one vertex per event
    if (StMuDst::numberOfPrimaryVertices() == 0)
        return dz;
    const int Nvert = StMuDst::primaryVertices()->GetEntriesFast();
    if (Nvert < 2)
        return dz;

    if (vtx_id == -1)
        vtx_id = StMuDst::currentVertexIndex();
    float z = primaryVertexPosition(vtx_id).z();
    for (int i = 0; i < Nvert; i++) {
        if (vtx_id != i) {
            if (fabs(z - StMuDst::primaryVertex(i)->position().z()) < dz)
                dz = fabs(z - StMuDst::primaryVertex(i)->position().z());
        }
    }
    return dz;
}
