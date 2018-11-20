/***************************************************************************
 *
 * $Id: StMuDst.cxx,v 1.69 2018/02/27 04:11:57 jdb Exp $
 * Author: Frank Laue, BNL, laue@bnl.gov
 *
 ***************************************************************************/

#include <map>

#include "StMuDst.h"

#include "StContainers.h"
#include "StEventTypes.h"
#include "StTriggerData.h"
#include "StTriggerData2003.h"
#include "StTriggerData2004.h"
#include "StTriggerData2005.h"
#include "StTriggerData2007.h"
#include "StTriggerData2008.h"
#include "StTriggerData2009.h"

#include "COMMON/StMuDebug.h"
#include "COMMON/StMuDstMaker.h"
#include "COMMON/StMuEmcUtil.h"
#include "COMMON/StMuEvent.h"
#include "COMMON/StMuFmsUtil.h"
#include "COMMON/StMuMtdCollection.h"
#include "COMMON/StMuPmdUtil.h"
#include "COMMON/StMuPrimaryVertex.h"
#include "COMMON/StMuRpsCollection.h"
#include "COMMON/StMuTrack.h"
#include "StTimer.hh"
/// dongx
#include "COMMON/StMuBTofHit.h"
#include "COMMON/StMuEpdHit.h"            // MALisa
#include "COMMON/StMuEpdHitCollection.h"  // MALisa
#include "COMMON/StMuMtdHeader.h"
#include "COMMON/StMuMtdHit.h"
#include "StBTofCollection.h"
#include "StBTofHeader.h"
#include "StBTofPidTraits.h"
#include "StBTofRawHit.h"
#include "TClonesArray.h"
#include "TTree.h"
#ifndef __NO_STRANGE_MUDST__
#include "StKinkMuDst.hh"
#include "StV0MuDst.hh"
#include "StXiMuDst.hh"
#endif
TClonesArray **StMuDst::arrays = 0;
#ifndef __NO_STRANGE_MUDST__
TClonesArray **StMuDst::strangeArrays = 0;
#endif
#include "COMMON/StMuMcTrack.h"
#include "COMMON/StMuMcVertex.h"
TClonesArray **StMuDst::mcArrays = 0;
TClonesArray **StMuDst::emcArrays = 0;
TClonesArray **StMuDst::fmsArrays = 0;
TClonesArray **StMuDst::pmdArrays = 0;
TClonesArray **StMuDst::tofArrays = 0;
TClonesArray **StMuDst::btofArrays = 0;  /// dongx
TClonesArray **StMuDst::epdArrays = 0;   /// MALisa
TClonesArray **StMuDst::mtdArrays = 0;
TClonesArray **StMuDst::fgtArrays = 0;
TClonesArray *StMuDst::mMuEmcCollectionArray = 0;
StMuEmcCollection *StMuDst::mMuEmcCollection = 0;
StMuFmsCollection *StMuDst::mMuFmsCollection = 0;
TClonesArray *StMuDst::mMuPmdCollectionArray = 0;
StMuPmdCollection *StMuDst::mMuPmdCollection = 0;
StEmcCollection *StMuDst::mEmcCollection = 0;
StFmsCollection *StMuDst::mFmsCollection = 0;
TClonesArray **StMuDst::eztArrays = 0;

Int_t StMuDst::mCurrVertexId = -2;
TObjArray *StMuDst::mCurrPrimaryTracks = 0;

StMuDst::StMuDst() {
  DEBUGMESSAGE("");
  /* no-op */
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDst::unset() {
  arrays = 0;
#ifndef __NO_STRANGE_MUDST__
  strangeArrays = 0;
#endif
  mcArrays = 0;
  emcArrays = 0;
  fmsArrays = 0;
  pmdArrays = 0;
  tofArrays = 0;
  btofArrays = 0;  // dongx
  mtdArrays = 0;   // dongx
  epdArrays = 0;   // MALisa
  fgtArrays = 0;
  mMuEmcCollectionArray = 0;
  mMuEmcCollection = 0;
  mMuFmsCollection = 0;
  mMuPmdCollectionArray = 0;
  mMuPmdCollection = 0;
  mEmcCollection = 0;
  mFmsCollection = 0;
  eztArrays = 0;
  mtdArrays = 0;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDst::set(StMuDstMaker *maker) {
  DEBUGMESSAGE2("");
  if (!maker) {
    DEBUGVALUE(maker);
    return;
  }
  arrays = maker->mArrays;
#ifndef __NO_STRANGE_MUDST__
  strangeArrays = maker->mStrangeArrays;
#endif
  mcArrays = maker->mMCArrays;
  emcArrays = maker->mEmcArrays;
  fmsArrays = maker->mFmsArrays;
  pmdArrays = maker->mPmdArrays;
  tofArrays = maker->mTofArrays;
  btofArrays = maker->mBTofArrays;  // dongx
  epdArrays = maker->mEpdArrays;    // MALisa
  mtdArrays = maker->mMtdArrays;
  fgtArrays = maker->mFgtArrays;

  mMuEmcCollectionArray = maker->mEmcCollectionArray;
  mMuEmcCollection = maker->mEmcCollection;
  mMuFmsCollection = maker->mFmsCollection;
  mMuPmdCollectionArray = maker->mPmdCollectionArray;
  mMuPmdCollection = maker->mPmdCollection;
  eztArrays = maker->mEztArrays;

#ifndef __NO_STRANGE_MUDST__
  StStrangeEvMuDst *ev = strangeEvent();
  int nV0s = v0s()->GetEntriesFast();
  for (int i = 0; i < nV0s; i++)
    v0s(i)->SetEvent(ev);  // set the pointer to the StStrangeEvMuDst which is not read from disk
  int nXis = xis()->GetEntriesFast();
  for (int i = 0; i < nXis; i++)
    xis(i)->SetEvent(ev);  // set the pointer to the StStrangeEvMuDst which is not read from disk
//  int nKinks = kinks()->GetEntriesFast(); for (int i=0;i<nKinks; i++) kinks(i)->SetEvent(ev);
#endif
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDst::set(TClonesArray **theArrays,
#ifndef __NO_STRANGE_MUDST__
                  TClonesArray **theStrangeArrays,
#endif
                  TClonesArray **theMCArrays, TClonesArray **theEmcArrays, TClonesArray **theFmsArrays,
                  TClonesArray **thePmdArrays, TClonesArray **theTofArrays, TClonesArray **theBTofArrays,  // dongx
                  TClonesArray **theEpdArrays,                                                             // MALisa
                  TClonesArray **theMTDArrays, TClonesArray **theFgtArrays, TClonesArray **theEztArrays,
                  TClonesArray *emc_arr, StMuEmcCollection *emc, StMuFmsCollection *fms, TClonesArray *pmd_arr,
                  StMuPmdCollection *pmd) {
  // I don't understand why this method is still needed,
  // but cannot comile dictionary  when it is removed
  DEBUGMESSAGE2("");
  arrays = theArrays;
#ifndef __NO_STRANGE_MUDST__
  strangeArrays = theStrangeArrays;
#endif
  mcArrays = theMCArrays;
  emcArrays = theEmcArrays;
  fmsArrays = theFmsArrays;
  fgtArrays = theFgtArrays;
  pmdArrays = thePmdArrays;
  tofArrays = theTofArrays;
  btofArrays = theBTofArrays;  // dongx
  epdArrays = theEpdArrays;    // MALisa
  mMuEmcCollectionArray = emc_arr;
  mMuEmcCollection = emc;
  mMuFmsCollection = fms;
  mMuPmdCollectionArray = pmd_arr;
  mMuPmdCollection = pmd;
  eztArrays = theEztArrays;
  mtdArrays = theMTDArrays;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDst::collectVertexTracks() {
  if (mCurrPrimaryTracks == 0)
    mCurrPrimaryTracks = new TObjArray();
  Int_t n_track = arrays[muPrimary]->GetEntriesFast();
  mCurrPrimaryTracks->Clear();
  for (Int_t i_track = 0; i_track < n_track; i_track++) {
    if (((StMuTrack *)arrays[muPrimary]->UncheckedAt(i_track))->vertexIndex() == mCurrVertexId)
      mCurrPrimaryTracks->AddLast(arrays[muPrimary]->UncheckedAt(i_track));
  }
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDst::setVertexIndex(Int_t vtx_id) {
  if (mCurrVertexId == vtx_id)
    return;
  mCurrVertexId = vtx_id;
  collectVertexTracks();
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDst::fixTrackIndices() {
  /// global and primary tracks share the same id, so we can fix the
  /// index2Global up in case they got out of order (e.g. by removing
  /// a track from the TClonesArrays
  fixTrackIndices(arrays[muPrimary], arrays[muGlobal]);
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDst::fixTrackIndices(TClonesArray *primary, TClonesArray *global) {
  /// NOTE: this method does not work for productions with FTPC from SL04d
  ///       up to SL05g, because StFtpcTrackToStEvent generates duplicate
  ///       track keys
  ///
  /// global and primary tracks share the same id, so we can fix the
  /// index2Global up in case they got out of order (e.g. by removing
  /// a track from the TClonesArrays)

  if (!(global && primary))
    return;
  DEBUGMESSAGE1("");
  StTimer timer;
  timer.start();

  static int warningPrinted = 0;
  if (!warningPrinted) {
    LOG_WARN << "WARNING: You are using " << __PRETTY_FUNCTION__
             << " which does not work properly "
                " for productions with FTPC >= SL04d and <= SL05g"
             << endm;
    warningPrinted = 1;
  }
  int nGlobals = global->GetEntriesFast();
  int nPrimaries = primary->GetEntriesFast();
  // map to keep track of index numbers, key is track->id(), value is index of track in MuDst
  map<short, unsigned short> globalIndex;

  for (int i = 0; i < nGlobals; i++) {
    StMuTrack *g = (StMuTrack *)global->UncheckedAt(i);
    if (g) {
      globalIndex[g->id()] = i + 1;
      globalTracks(i)->setIndex2Global(i);
    }
  }
  // set the indices for the primary tracks
  DEBUGVALUE2(primary->GetEntriesFast());
  for (int i = 0; i < nPrimaries; i++) {
    StMuTrack *p = (StMuTrack *)primary->UncheckedAt(i);
    if (p) {
      if (globalIndex[p->id()])
        p->setIndex2Global(globalIndex[p->id()] - 1);
      else
        p->setIndex2Global(-1);
    }
  }
  DEBUGVALUE2(timer.elapsedTime());
}

void StMuDst::fixTrackIndicesG(int mult) {
  /// Match global track index to primary track
  // mult = 0 means there is just a single vertex in the event, mult>0 means there are multiple...

  if (mult == 0) {
    if (!(fabs(event()->primaryVertexPosition().x()) < 1.e-5 && fabs(event()->primaryVertexPosition().y()) < 1.e-5 &&
          fabs(event()->primaryVertexPosition().z()) < 1.e-5)) {
      int startpos = 0;
      int tid, pid;
      if (!globalTracks())
        return;
      for (int i = 0; i < globalTracks()->GetEntriesFast(); i++) {
        tid = globalTracks(i)->id();
        globalTracks(i)->setIndex2Global(-2);
        if (!primaryTracks())
          return;
        for (int j = startpos; j < primaryTracks()->GetEntriesFast(); j++) {
          pid = primaryTracks(j)->id();
          if (pid == tid) {
            globalTracks(i)->setIndex2Global(j);
            if (j == startpos)
              startpos++;
            break;
          } else if (pid > tid)
            break;
        }
      }
    }
    return;
  }
  // New MuDsts with multiple vertices....
  if (!primaryVertices())
    return;
  const int Nvert = primaryVertices()->GetEntriesFast();
  if (!Nvert)
    return;
  int curVer = currentVertexIndex();
  int startpos[Nvert];
  for (int i = 0; i < Nvert; i++) startpos[i] = 0;
  int tid, pid;
  if (!globalTracks())
    return;

  for (int i = 0; i < globalTracks()->GetEntriesFast(); i++) {
    tid = globalTracks(i)->id();
    globalTracks(i)->setIndex2Global(-2);
    globalTracks(i)->setVertexIndex(-2);
    // Scan through vertices
    for (int j = 0; j < Nvert; j++) {
      if (globalTracks(i)->index2Global() >= 0)
        break;
      setVertexIndex(j);
      if (!primaryTracks())
        continue;
      for (int k = startpos[j]; k < primaryTracks()->GetEntriesFast(); k++) {
        pid = primaryTracks(k)->id();
        if (pid == tid) {
          globalTracks(i)->setIndex2Global(k);
          globalTracks(i)->setVertexIndex(j);
          if (k == startpos[j])
            startpos[j]++;
          break;
        } else if (pid > tid)
          break;
      }
    }
  }
  setVertexIndex(curVer);
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDst::fixTofTrackIndices() {
  /// global and primary tracks share the same id, so we can fix the
  /// index2Global up in case they got out of order (e.g. by removing
  /// a track from the TClonesArrays
  fixTofTrackIndices(btofArrays[muBTofHit], arrays[muPrimary], arrays[muGlobal]);
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDst::fixTofTrackIndices(TClonesArray *btofHit, TClonesArray *primary, TClonesArray *global) {
  if (!(primary && global && btofHit))
    return;
  DEBUGMESSAGE1("");
  StTimer timer;
  timer.start();

  int nPrimarys = primary->GetEntriesFast();
  int nGlobals = global->GetEntriesFast();
  int nBTofHits = btofHit->GetEntriesFast();
  // map to keep track of index numbers, key is track->id(), value is index of track in MuDst
  map<short, unsigned short> tofIndex;
  map<short, unsigned short> globalIndex;
  map<short, unsigned short> primaryIndex;

  for (int i = 0; i < nBTofHits; i++) {
    StMuBTofHit *t = (StMuBTofHit *)btofHit->UncheckedAt(i);
    if (t) {
      tofIndex[t->associatedTrackId()] = i + 1;  // starting from 1
    }
  }

  for (int i = 0; i < nGlobals; i++) {
    StMuTrack *g = (StMuTrack *)global->UncheckedAt(i);
    if (g) {
      globalIndex[g->id()] = i + 1;

      if (tofIndex[g->id()])
        g->setIndex2BTofHit(tofIndex[g->id()] - 1);
      else
        g->setIndex2BTofHit(-1);
    }
  }
  for (int i = 0; i < nPrimarys; i++) {
    StMuTrack *p = (StMuTrack *)primary->UncheckedAt(i);
    if (p) {
      primaryIndex[p->id()] = i + 1;

      if (tofIndex[p->id()])
        p->setIndex2BTofHit(tofIndex[p->id()] - 1);
      else
        p->setIndex2BTofHit(-1);
    }
  }

  /// set the indices for BTofHits
  for (int i = 0; i < nBTofHits; i++) {
    StMuBTofHit *t = (StMuBTofHit *)btofHit->UncheckedAt(i);
    if (t) {
      if (globalIndex[t->associatedTrackId()])
        t->setIndex2Global(globalIndex[t->associatedTrackId()] - 1);
      else
        t->setIndex2Global(-1);

      if (primaryIndex[t->associatedTrackId()])
        t->setIndex2Primary(primaryIndex[t->associatedTrackId()] - 1);
      else
        t->setIndex2Primary(-1);
    }
  }

  DEBUGVALUE2(timer.elapsedTime());
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDst::fixMtdTrackIndices() {
  /// global and primary tracks share the same id, so we can fix the
  /// index2Global up in case they got out of order (e.g. by removing
  /// a track from the TClonesArrays
  fixMtdTrackIndices(mtdArrays[muMTDHit], arrays[muPrimary], arrays[muGlobal]);
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDst::fixMtdTrackIndices(TClonesArray *mtdHit, TClonesArray *primary, TClonesArray *global) {
  if (!(primary && global && mtdHit))
    return;
  DEBUGMESSAGE1("");
  StTimer timer;
  timer.start();

  int nPrimarys = primary->GetEntriesFast();
  int nGlobals = global->GetEntriesFast();
  int nMtdHits = mtdHit->GetEntriesFast();
  // map to keep track of index numbers, key is track->id(), value is index of track in MuDst
  map<short, unsigned short> mtdIndex;
  map<short, unsigned short> globalIndex;
  map<short, unsigned short> primaryIndex;

  for (int i = 0; i < nMtdHits; i++) {
    StMuMtdHit *t = (StMuMtdHit *)mtdHit->UncheckedAt(i);
    if (t) {
      mtdIndex[t->associatedTrackKey()] = i + 1;  // starting from 1
    }
  }

  for (int i = 0; i < nGlobals; i++) {
    StMuTrack *g = (StMuTrack *)global->UncheckedAt(i);
    if (g) {
      globalIndex[g->id()] = i + 1;

      if (mtdIndex[g->id()])
        g->setIndex2MtdHit(mtdIndex[g->id()] - 1);
      else
        g->setIndex2MtdHit(-1);
    }
  }
  for (int i = 0; i < nPrimarys; i++) {
    StMuTrack *p = (StMuTrack *)primary->UncheckedAt(i);
    if (p) {
      primaryIndex[p->id()] = i + 1;

      if (mtdIndex[p->id()])
        p->setIndex2MtdHit(mtdIndex[p->id()] - 1);
      else
        p->setIndex2MtdHit(-1);
    }
  }

  /// set the indices for MtdHits
  for (int i = 0; i < nMtdHits; i++) {
    StMuMtdHit *t = (StMuMtdHit *)mtdHit->UncheckedAt(i);
    if (t) {
      if (globalIndex[t->associatedTrackKey()])
        t->setIndex2Global(globalIndex[t->associatedTrackKey()] - 1);
      else
        t->setIndex2Global(-1);

      if (primaryIndex[t->associatedTrackKey()])
        t->setIndex2Primary(primaryIndex[t->associatedTrackKey()] - 1);
      else
        t->setIndex2Primary(-1);
    }
  }

  DEBUGVALUE2(timer.elapsedTime());
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDst::setMtdArray(StMtdCollection *mtd_coll) {
  /// reset MTD hit array and header when running StMtdHitMaker on muDst
  /// in afterburner mode

  mtdArrays[muMTDHit]->Clear();
  StMuMtdCollection mMTD(*mtd_coll);
  for (size_t i = 0; i < (size_t)mMTD.hitsPresent(); i++) {
    StMuMtdHit *mtdHit = (StMuMtdHit *)mMTD.MtdHit(i);
    new ((*mtdArrays[muMTDHit])[i]) StMuMtdHit(*mtdHit);
  }

  StMuMtdHeader *mtdHead = mMTD.mtdHeader();
  if (mtdHead) {
    mtdArrays[muMTDHeader]->Clear();
    new ((*mtdArrays[muMTDHeader])[0]) StMuMtdHeader(*mtdHead);
  }
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
StEvent *StMuDst::createStEvent() {
  DEBUGMESSAGE1("");
  StTimer timer;
  timer.start();

  StMuEvent *mu = event();
  if (!mu)
    return NULL;
  StEvent *ev = new StEvent();

  ev->setInfo(new StEventInfo(mu->eventInfo()));
  ev->setRunInfo(new StRunInfo(mu->runInfo()));
  ev->setSummary(new StEventSummary(mu->eventSummary()));
  //   ev->setSoftwareMonitor(SoftwareMonitor*);
  //   ev->setTpcHitCollection(StTpcHitCollection*);
  //   ev->setFtpcHitCollection(StFtpcHitCollection*);
  //   ev->setSvtHitCollection(StSvtHitCollection*);
  //   ev->setSsdHitCollection(StSsdHitCollection*);
  //   ev->setEmcCollection(StEmcCollection*);
  //   ev->setRichCollection(StRichCollection*);
  //   ev->setTofCollection(StTofCollection*);
  // ev->setTofCollection( new StTofCollection() );
  StTriggerDetectorCollection *trg_coll = new StTriggerDetectorCollection();
  trg_coll->vpd() = mu->vpdTriggerDetector();
  trg_coll->bbc() = mu->bbcTriggerDetector();
  trg_coll->ctb() = mu->ctbTriggerDetector();
  trg_coll->emc() = mu->emcTriggerDetector();
  trg_coll->fpd() = mu->fpdTriggerDetector();
  trg_coll->zdc() = mu->zdcTriggerDetector();
  ev->setTriggerDetectorCollection(trg_coll);

  ev->setFpdCollection(new StFpdCollection(mu->fpdCollection()));
  // ev->setTriggerDetectorCollection(muStTriggerDetectorCollection*); <<< WE DON'T WANT THAT
  ev->setL0Trigger(new StL0Trigger(mu->l0Trigger()));
  //   ev->setL1Trigger ( new StL0Trigger(mu->l0Trigger()) );
  ev->setL3Trigger(new StL3Trigger());

  StPrimaryVertex *vp = new StPrimaryVertex();
  ev->addPrimaryVertex(vp);
  vp->setPosition(mu->eventSummary().primaryVertexPosition());

  int nGlobals = arrays[muGlobal]->GetEntriesFast();

  StSPtrVecTrackNode &trackNodes = ev->trackNodes();
  TArrayI global_indices(nGlobals);  // Temporary array to keep track of index numbers on trackNodes

  // add global tracks to tracknodes
  for (int i = 0; i < nGlobals; i++) {
    if (globalTracks(i)) {
      StTrackNode *node = new StTrackNode();
      node->addTrack(createStTrack(globalTracks(i)));
      trackNodes.push_back(node);
      global_indices[i] = trackNodes.size() - 1;
    } else {
      global_indices[i] = -1;
    }
  }

  /// add primary tracks and primary vertex
  ///
  /// This only uses the deafult vertex and tracks in case
  /// of multiple primary vertixes.

  TObjArray *prim_tracks = primaryTracks();

  int nPrimaries = prim_tracks->GetEntriesFast();
  for (int i = 0; i < nPrimaries; i++)
    if (primaryTracks(i)) {
      StTrack *t = createStTrack((StMuTrack *)prim_tracks->At(i));
      Int_t global_idx = primaryTracks(i)->index2Global();
      if (global_idx >= 0 && global_indices[global_idx] >= 0)
        trackNodes[global_indices[global_idx]]->addTrack(t);
      else {
        StTrackNode *node = new StTrackNode();
        node->addTrack(t);
        trackNodes.push_back(node);
      }
      vp->addDaughter(t);
    }

  /// do the same excercise for the l3 tracks
  /// we do this later
  /// we do this later
  /// we do this later

  // add detector states
  int nStates = arrays[muState]->GetEntriesFast();
  for (int i = 0; i < nStates; i++) {
    StDetectorState *det = new StDetectorState(*detectorStates(i));
    ev->addDetectorState(det);
  }

  // now get the EMC stuff and put it in the StEvent
  static StMuEmcUtil *mEmcUtil = new StMuEmcUtil();
  StMuEmcCollection *emc = muEmcCollection();
  if (emc) {  // transform to StEvent format and fill it
    StEmcCollection *EMC = mEmcUtil->getEmc(emc);
    if (EMC)
      ev->setEmcCollection(EMC);
  }
  // now get the FMS stuff and put it in the StEvent
  static StMuFmsUtil *mFmsUtil = new StMuFmsUtil();
  StMuFmsCollection *fms = muFmsCollection();
  if (fms) {  // transform to StEvent format and fill it
    StFmsCollection *FMS = mFmsUtil->getFms(fms);
    if (FMS)
      ev->setFmsCollection(FMS);
  }
  // now get the PMD stuff and put it in the StEvent
  static StMuPmdUtil *mPmdUtil = new StMuPmdUtil();
  StMuPmdCollection *pmd = pmdCollection();
  if (pmd) {  // transform to StEvent format and fill it
    StPhmdCollection *PMD = mPmdUtil->getPmd(pmd);
    if (PMD)
      ev->setPhmdCollection(PMD);
  }

  // now get tof (after fix from Xin)
  StTofCollection *tofcoll = new StTofCollection();
  ev->setTofCollection(tofcoll);
  int nTofData = tofArrays[muTofData]->GetEntriesFast();
  for (int i = 0; i < nTofData; i++) {
    StTofData *aData;
    if (tofData(i)) {
      unsigned short id = tofData(i)->dataIndex();
      unsigned short adc = tofData(i)->adc();
      unsigned short tdc = tofData(i)->tdc();
      short tc = tofData(i)->tc();
      unsigned short sc = tofData(i)->sc();
      // run 5 - dongx
      aData = new StTofData(id, adc, tdc, tc, sc, 0, 0);
    } else {
      aData = new StTofData(0, 0, 0, 0, 0, 0, 0);
    }
    tofcoll->addData(aData);
  }
  // run 5 - dongx
  int nTofRawData = tofArrays[muTofRawData]->GetEntriesFast();
  for (int i = 0; i < nTofRawData; i++) {
    StTofRawData *aRawData;
    if (tofRawData(i)) {
      unsigned short tray = tofRawData(i)->tray();
      unsigned short leteFlag = tofRawData(i)->leteFlag();
      unsigned short channel = tofRawData(i)->channel();
      unsigned int tdc = tofRawData(i)->tdc();
      unsigned int triggertime = tofRawData(i)->triggertime();
      unsigned short quality = tofRawData(i)->quality();
      aRawData = new StTofRawData(leteFlag, tray, channel, tdc, triggertime, quality);
    } else {
      aRawData = new StTofRawData(0, 0, 0, 0, 0, 0);
    }
    tofcoll->addRawData(aRawData);
  }

  // now create, fill the StBTofCollection - dongx
  StBTofCollection *btofcoll = new StBTofCollection();
  ev->setBTofCollection(btofcoll);
  int nBTofRawHits = btofArrays[muBTofRawHit]->GetEntriesFast();
  for (int i = 0; i < nBTofRawHits; i++) {
    StBTofRawHit *aRawHit;
    if (btofRawHit(i)) {
      aRawHit = new StBTofRawHit(*(btofRawHit(i)));
    } else {
      aRawHit = new StBTofRawHit();
    }
    btofcoll->addRawHit(aRawHit);
  }
  if (btofHeader())
    btofcoll->setHeader(new StBTofHeader(*(btofHeader())));
  // now create, fill and add new StTriggerIdCollection to the StEvent
  StTriggerIdCollection *triggerIdCollection = new StTriggerIdCollection();
  StTriggerId triggerId;
  triggerId = mu->triggerIdCollection().l1();
  if (!StMuTriggerIdCollection::isEmpty(triggerId))
    triggerIdCollection->setL1(new StTriggerId(triggerId));
  triggerId = mu->triggerIdCollection().l2();
  if (!StMuTriggerIdCollection::isEmpty(triggerId))
    triggerIdCollection->setL2(new StTriggerId(triggerId));
  triggerId = mu->triggerIdCollection().l3();
  if (!StMuTriggerIdCollection::isEmpty(triggerId))
    triggerIdCollection->setL3(new StTriggerId(triggerId));
  triggerId = mu->triggerIdCollection().nominal();
  if (!StMuTriggerIdCollection::isEmpty(triggerId))
    triggerIdCollection->setNominal(new StTriggerId(triggerId));
  ev->setTriggerIdCollection(triggerIdCollection);

  DEBUGVALUE2(timer.elapsedTime());
  return ev;
}

#include "StarClassLibrary/PhysicalConstants.h"
#include "StarClassLibrary/SystemOfUnits.h"
StTrackGeometry *StMuDst::trackGeometry(int q, StPhysicalHelixD *h) {
  static StPhysicalHelixD nullHelix;
  StHelixModel *model = 0;
  if (nullHelix == *h)
    return 0;
  if (fabs(h->curvature()) > 100)
    return 0;
  if (fabs(h->origin().x()) > 1000)
    return 0;
  if (fabs(h->origin().y()) > 1000)
    return 0;
  if (fabs(h->origin().z()) > 1000)
    return 0;

  model = new StHelixModel(q, h->phase() + h->h() * pi / 2, h->curvature(), h->dipAngle(), h->origin(),
                           h->momentumAt(0, event()->runInfo().magneticField() * kilogauss), h->h());
  return model;
}

StTrack *StMuDst::createStTrack(const StMuTrack *track) {
  StTrack *t = 0;
  StTrackGeometry *tg;
  if (track->bad())
    return 0;

  if (track->type() == primary)
    t = new StPrimaryTrack();
  if (track->type() == global)
    t = new StGlobalTrack();
  assert(t);
  t->setFlag(track->flag());
  t->setFlagExtension(track->flagExtension());
  t->setKey(track->id());

  StPhysicalHelixD helix;
  helix = track->helix();
  tg = trackGeometry(track->charge(), &helix);
  if (tg)
    t->setGeometry(tg);
  helix = track->outerHelix();
  tg = trackGeometry(track->charge(), &helix);
  if (tg)
    t->setOuterGeometry(tg);

  t->setIdTruth(track->idTruth(), track->qaTruth());
  t->setIdParentVx(track->idParentVx());
  t->setLength(track->length());
  t->setImpactParameter(track->dca().mag());
  t->addPidTraits(new StDedxPidTraits(kTpcId, kTruncatedMeanId, track->nHitsDedx(), track->dEdx(), 0));
  Float_t a[2], b[15];
  a[0] = track->chi2();
  a[1] = 0;
  memset(b, 0, 15 * sizeof(Float_t));

  StTrackFitTraits traits(0, 0, a, b);
  traits.setNumberOfFitPoints(track->nHitsFit(kTpcId), kTpcId);
  traits.setNumberOfFitPoints(track->nHitsFit(kFtpcEastId), kFtpcEastId);
  traits.setNumberOfFitPoints(track->nHitsFit(kFtpcWestId), kFtpcWestId);
  traits.setNumberOfFitPoints(track->nHitsFit(kSvtId), kSvtId);
  traits.setNumberOfFitPoints(track->nHitsFit(kSsdId), kSsdId);
  // Set flag for primary tracks, but not if data is old-style
  // Old style data has +1 for vertex built-in for Ftpc (not sure about Tpc)
  if (track->type() == primary && track->mNHitsFitTpc != 255)
    traits.setPrimaryVertexUsedInFit(kTRUE);
  t->setFitTraits(traits);

  t->setNumberOfPossiblePoints(track->nHitsPoss(kTpcId), kTpcId);
  t->setNumberOfPossiblePoints(track->nHitsPoss(kFtpcEastId), kFtpcEastId);
  t->setNumberOfPossiblePoints(track->nHitsPoss(kFtpcWestId), kFtpcWestId);
  t->setNumberOfPossiblePoints(track->nHitsPoss(kSvtId), kSvtId);
  t->setNumberOfPossiblePoints(track->nHitsPoss(kSsdId), kSsdId);
  t->setNumberOfPossiblePoints(track->nHitsPoss(kSstId), kSstId);
  t->setNumberOfPossiblePoints(track->nHitsPoss(kPxlId), kPxlId);
  t->setNumberOfPossiblePoints(track->nHitsPoss(kIstId), kIstId);

  // set the topology map
  t->setTopologyMap(track->topologyMap());

  // set the btofPidTraits - dongx
  t->addPidTraits(track->btofPidTraits().createBTofPidTraits());

  return t;
}

void StMuDst::Print(Option_t *option) const {
  StMuEvent *event = 0;
  if ((event = StMuDst::event())) {
    cout << "++++++++++++++ MuDst run " << event->runId() << " event " << event->eventId() << " ++++++++++++++" << endl;
    cout << endl << "primary vertex pos " << event->primaryVertexPosition() << endl;
  } else
    cout << "No event structure (StMuEvent) found!" << endl;

  cout << numberOfPrimaryVertices() << " vertices reconstructed" << endl;
  cout << numberOfPrimaryTracks() << " primary tracks, ";
  if (mCurrVertexId != 0)
    cout << "( note vtx_id " << mCurrVertexId << " ) ";
  cout << numberOfGlobalTracks() << " global " << endl;

#ifndef __NO_STRANGE_MUDST__
  cout << numberOfV0s() << " V0s, " << numberOfXis() << " Xis " << numberOfKinks() << " kinks" << endl;
#endif
  cout << endl;
  if (muEmcCollection())
    cout << "EMC data present" << endl;
  else
    cout << "No EMC data present" << endl;
  cout << endl;
  if (muFmsCollection())
    cout << "FMS data present" << endl;
  else
    cout << "No FMS data present" << endl;
  if (pmdCollection())
    cout << "PMD data present" << endl;
  else
    cout << "No PMD data present" << endl;

  if (numberOfTofHit())
    cout << "TOF data present" << endl;
  else
    cout << "No TOF data present" << endl;
  cout << endl;
}

void StMuDst::printVertices() {
  if (numberOfPrimaryVertices() == 0) {
    cout << "No vertices stored (for older data, check StMuEvent)" << endl;
    return;
  }
  cout << endl;
  cout << "+++++++++ vertex list ( " << numberOfPrimaryVertices() << " entries )" << endl << endl;
  for (UInt_t i_vtx = 0; i_vtx < numberOfPrimaryVertices(); i_vtx++) {
    cout << "+++ Vertex " << i_vtx << endl;
    primaryVertex(i_vtx)->Print();
    cout << endl;
  }
}

void StMuDst::printPrimaryTracks() {
  if (numberOfPrimaryTracks() == 0) {
    cout << "No primary tracks found!" << endl;
    return;
  }
  cout << endl;
  cout << "+++++++++ PRIMARY track list ( " << numberOfPrimaryTracks() << " entries )" << endl << endl;
  for (UInt_t i_trk = 0; i_trk < numberOfPrimaryTracks(); i_trk++) {
    cout << "+++ Primary track " << i_trk << endl;
    primaryTracks(i_trk)->Print();
    cout << endl;
  }
}

void StMuDst::printGlobalTracks() {
  if (numberOfGlobalTracks() == 0) {
    cout << "No global tracks found!" << endl;
    return;
  }
  cout << endl;
  cout << "+++++++++ GLOBAL track list ( " << numberOfGlobalTracks() << " entries )" << endl << endl;
  for (UInt_t i_trk = 0; i_trk < numberOfGlobalTracks(); i_trk++) {
    cout << "+++ Global track " << i_trk << endl;
    globalTracks(i_trk)->Print();
    cout << endl;
  }
}

ClassImp(StMuDst)
