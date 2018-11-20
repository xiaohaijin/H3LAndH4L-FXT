/***************************************************************************
 *
 * $Id: StMuDstMaker.cxx,v 1.128 2018/03/01 20:42:38 jdb Exp $
 * Author: Frank Laue, BNL, laue@bnl.gov
 *
 **************************************************************************/
#include "TRegexp.h"
#include "Stiostream.h"
#include "Stsstream.h"
#include "StChain.h"
#include "THack.h"
#include "StEvent.h"
#include "StTrack.h"
#include "StTrackNode.h"
#include "StRichSpectra.h"
#include "StDetectorState.h"
#include "StEventTypes.h"
#include "StRunInfo.h"
#include "StEventInfo.h"
#include "StDcaGeometry.h"
#include "StFgtCollection.h"
#include "StFgtStrip.h"
#include "StFgtHit.h"
#include "StEnumerations.h"
#include "StFgtConsts.h"
#include "StuRefMult.hh"
#include "StuProbabilityPidAlgorithm.h"

#include "StPhysicalHelixD.hh"
#include "StTimer.hh"
#include "StMatrixF.hh"

#include "StIOMaker.h"
#include "StTreeMaker.h"
#ifndef __NO_STRANGE_MUDST__
#include "StStrangeMuDstMaker.h"
#include "StStrangeEvMuDst.hh"
#include "StV0MuDst.hh"
#include "StV0Mc.hh"
#include "StXiMuDst.hh"
#include "StXiMc.hh"
#include "StKinkMuDst.hh"
#include "StKinkMc.hh"
#include "StStrangeCuts.hh"
#endif

#include "COMMON/StMuException.hh"
#include "COMMON/StMuEvent.h"
#include "COMMON/StMuPrimaryVertex.h"
#include "COMMON/StMuRpsCollection.h"
#include "COMMON/StMuMtdCollection.h"
#include "COMMON/StMuMtdRawHit.h"
#include "COMMON/StMuMtdHit.h"
#include "COMMON/StMuMtdHeader.h"
#include "COMMON/StMuTrack.h"
#include "COMMON/StMuDebug.h"
#include "COMMON/StMuCut.h"
#include "COMMON/StMuFilter.h"
#include "COMMON/StMuL3Filter.h"
#include "COMMON/StMuChainMaker.h"
#include "COMMON/StMuEmcCollection.h"
#include "COMMON/StMuEmcUtil.h"
#include "COMMON/StMuFmsCollection.h"
#include "COMMON/StMuFmsUtil.h"
#include "COMMON/StMuFmsHit.h"
#include "COMMON/StMuEpdHit.h"           // MALisa
#include "COMMON/StMuEpdHitCollection.h" // MALisa
#include "StEpdCollection.h"             // MALisa
#include "COMMON/StMuEpdUtil.h"          // MALisa
#include "COMMON/StMuFgtStrip.h"
#include "COMMON/StMuFgtCluster.h"
#include "COMMON/StMuFgtStripAssociation.h"
#include "COMMON/StMuFgtAdc.h"
#include "COMMON/StMuPmdCollection.h"
#include "COMMON/StMuPmdUtil.h"
#include "COMMON/StMuPmdHit.h"
#include "COMMON/StMuPmdCluster.h"
#include "COMMON/StMuTofHit.h"
#include "COMMON/StMuTofHitCollection.h"
#include "COMMON/StMuTofUtil.h"
/// dongx
#include "StBTofCollection.h"
#include "StBTofRawHit.h"
#include "StBTofHeader.h"
#include "COMMON/StMuBTofHit.h"
#include "COMMON/StMuBTofHitCollection.h"
#include "COMMON/StMuBTofUtil.h"
#include "COMMON/StMuPrimaryTrackCovariance.h"
#include "EZTREE/StMuEzTree.h"
#include "EZTREE/EztEventHeader.h"
#include "EZTREE/EztEmcRawData.h"
#include "EZTREE/EztTrigBlob.h"
#include "EZTREE/EztFpdBlob.h"

#include "StMuDstMaker.h"
#include "COMMON/StMuDst.h"

#include "TFile.h"
#include "TTree.h"
#include "TClass.h"
#include "TChain.h"
#include "TStreamerInfo.h"
#include "TClonesArray.h"
#include "TEventList.h"

#include "THack.h"
#include "COMMON/StMuMcVertex.h"
#include "COMMON/StMuMcTrack.h"
#include "StG2TrackVertexMap.h"

class StEpdHit; // MALisa

ClassImp(StMuDstMaker)

#if !(ST_NO_NAMESPACES)
using namespace units;
#endif

/**
   The default constructor as it is right now was written in order to run
   the StMuDstMaker during reconstruction in the bfc.
   Since the PID table that is needed for muDst production is not passed as
   an argument to the bfc, this default constructor
   sets a specific PID table. This table has to be updated when changing to
   a new production version.
   Also, the standard track and l3 track filters are set.
 */
StMuDstMaker::StMuDstMaker(const char *name)
    : StIOInterFace(name), mStEvent(0), mStMuDst(0),
#ifndef __NO_STRANGE_MUDST__
      mStStrangeMuDstMaker(0),
#endif
      mIOMaker(0), mTreeMaker(0), mIoMode(1), mIoNameMode((int)ioTreeMaker), mEventList(0), mTrackType(256),
      mReadTracks(1), mReadV0s(1), mReadXis(1), mReadKinks(1), mFinish(0), mTrackFilter(0), mL3TrackFilter(0),
      mCurrentFile(0), mChain(0), mTTree(0), mSplit(99), mCompression(9), mBufferSize(65536 * 4), mVtxList(100),
      mProbabilityPidAlgorithm(0), mEmcCollectionArray(0), mEmcCollection(0), mFmsCollection(0), mPmdCollectionArray(0),
      mPmdCollection(0) {
  assignArrays();

  mDirName = "./";
  mFileName = "";
  streamerOff();
  zeroArrays();
  if (mIoMode == ioRead) {
    openRead();
  }
  if (mIoMode == ioWrite) {
    mProbabilityPidAlgorithm = new StuProbabilityPidAlgorithm();
  }

  mEventCounter = 0;
  mStMuDst = new StMuDst();
  mEmcUtil = new StMuEmcUtil();
  mFmsUtil = new StMuFmsUtil();
  mPmdUtil = new StMuPmdUtil();
  mTofUtil = new StMuTofUtil();
  mBTofUtil = new StMuBTofUtil(); /// dongx
  mEpdUtil = new StMuEpdUtil();   /// MALisa
  mEzTree = new StMuEzTree();
  if (!mStMuDst || !mEmcUtil || !mFmsUtil || !mPmdUtil || !mTofUtil || !mBTofUtil || !mEpdUtil || !mEzTree) {
    throw StMuExceptionNullPointer(
        "StMuDstMaker:: constructor. Something went horribly wrong, cannot allocate pointers", __PRETTYF__);
  }

  createArrays();

  setProbabilityPidFile();
  StMuL3Filter *l3Filter = new StMuL3Filter();
  setL3TrackFilter(l3Filter);
  StMuFilter *filter = new StMuFilter();
  setTrackFilter(filter);
  FORCEDDEBUGMESSAGE("ATTENTION: use standard MuFilter");
  FORCEDDEBUGMESSAGE("ATTENTION: use standard l3 MuFilter");
}

/*!
 * This method assigns individual TCloneArrays location from one
 * big global one. Dirty init MUST follow the order in StMuArrays.
 * This allows for block initialization/zeroing without problems
 * or side effects such as the one cause by array boundary overwrite
 * if the first TClone in a list of declared variables is used.
 */
void StMuDstMaker::assignArrays() {
  mArrays = mAArrays + 0;
#ifndef __NO_STRANGE_MUDST__
  mStrangeArrays = mArrays + __NARRAYS__;
  mMCArrays = mStrangeArrays + __NSTRANGEARRAYS__;
#else
  mMCArrays = mArrays + __NARRAYS__;
#endif
  mEmcArrays = mMCArrays + __NMCARRAYS__;
  mPmdArrays = mEmcArrays + __NEMCARRAYS__;
  mFmsArrays = mPmdArrays + __NPMDARRAYS__;
  mTofArrays = mFmsArrays + __NFMSARRAYS__;
  mBTofArrays = mTofArrays + __NTOFARRAYS__;  /// dongx
  mEpdArrays = mBTofArrays + __NBTOFARRAYS__; /// MALisa
  mMtdArrays = mEpdArrays + __NEPDARRAYS__;   /// dongx
  mFgtArrays = mMtdArrays + __NMTDARRAYS__;
  mEztArrays = mFgtArrays + __NFGTARRAYS__;
}

void StMuDstMaker::clearArrays() {
  const int ezIndex = __NARRAYS__ +
#ifndef __NO_STRANGE_MUDST__
                      __NSTRANGEARRAYS__ +
#endif
                      __NMCARRAYS__ + __NEMCARRAYS__ + __NPMDARRAYS__ + __NFMSARRAYS__ + __NTOFARRAYS__ +
                      __NBTOFARRAYS__ + /// dongx
                      __NEPDARRAYS__ +  // MALisa
                      __NMTDARRAYS__ + __NFGTARRAYS__;

  for (int i = 0; i < ezIndex; i++) {
    mAArrays[i]->Clear();
    StMuArrays::arrayCounters[i] = 0;
  }
  // ezTree classes need Delete, because of TArrayS
  for (int i = ezIndex; i < __NALLARRAYS__; i++) {
    mAArrays[i]->Delete();
    StMuArrays::arrayCounters[i] = 0;
  }
}

void StMuDstMaker::zeroArrays() {
  memset(mAArrays, 0, sizeof(void *) * __NALLARRAYS__);
  memset(mStatusArrays, (char)1, sizeof(mStatusArrays)); // default all ON
  // ezt arrays switched off
  memset(&mStatusArrays[__NARRAYS__ +
#ifndef __NO_STRANGE_MUDST__
                        __NSTRANGEARRAYS__ +
#endif
                        __NMCARRAYS__ + __NEMCARRAYS__ + __NPMDARRAYS__ + __NFMSARRAYS__ + __NTOFARRAYS__ +
                        __NBTOFARRAYS__  /// dongx
                        + __NEPDARRAYS__ // MALisa
                        + __NMTDARRAYS__ + __NFGTARRAYS__],
         (char)0, __NEZTARRAYS__);
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/*!
  Selecting SetBranchStatus for particular MuDst branches
  Special names:
   MuEventAll - all branches related to StMuEvent
   StrangeAll - all branches related to StrangeMuDst
   EmcAll     - all branches related to Emc
   PmdAll     - all branches related to Pmd
   TofAll     - all branches related to Tof
   BTofAll    - all branches related to BTof  /// dongx
   MTDAll     - all branches related to MTD
   FgtAll     - all branches related to Fgt

  By default all branches of MuDst are read. If user wants to read only some of
  them, then:
   SetStatus("*",0)           // all branches off
   SetStatus("MuEventAll",1)  // all standard MuEvent branches ON
   SetStatus("StrangeAll",1)  // all standard Strange branches ON
   SetStatus("EmcAll"    ,1)  // all standard Emc     branches ON
   SetStatus("PmdAll"    ,1)  // all standard Pmd     branches ON
   SetStatus("TofAll"    ,1)  // all standard Tof     branches ON
   SetStatus("BTofAll"   ,1)  // all standard BTof    branches ON  /// dongx
   SetStatus("EpdAll"    ,1)  // all standard Epd     branches ON  /// MALisa
   SetStatus("MTDAll"    ,1)  // all standard Mtd     branches ON
   SetStatus("FgtAll"    ,1)  // all standard Fgt     branches ON

   SetStatus("XiAssoc"    ,1) // Strange branch "XiAssoc" is ON

  Names of branches look StMuArrays::arrayTypes[]
*/
void StMuDstMaker::SetStatus(const char *arrType, int status) {
#ifndef __NO_STRANGE_MUDST__
  static const char *specNames[] = { "MuEventAll", "StrangeAll", "MCAll",  "EmcAll", "PmdAll", "FMSAll", "TofAll",
                                     "BTofAll",    "EpdAll",     "MTDAll", "FgtAll", "EztAll", 0 }; /// dongx, MALisa
#else
  static const char *specNames[] = { "MuEventAll", "MCAll",  "EmcAll", "PmdAll", "FMSAll", "TofAll",
                                     "BTofAll",    "EpdAll", "MTDAll", "FgtAll", "EztAll", 0 }; /// dongx, MALisa
#endif
  static const int specIndex[] = { 0,                  __NARRAYS__,
#ifndef __NO_STRANGE_MUDST__
                                   __NSTRANGEARRAYS__,
#endif
                                   __NMCARRAYS__,      __NEMCARRAYS__,  __NPMDARRAYS__, __NFMSARRAYS__,
                                   __NTOFARRAYS__,     __NBTOFARRAYS__, __NEPDARRAYS__, __NMTDARRAYS__,
                                   __NFGTARRAYS__,     __NEZTARRAYS__,  -1 };

  // jdb fixed with new implementation,
  // this method was broken for several years
  bool found = false;
  int species_start = 0;
  int num = 0;
  for (int i = 0; specNames[i]; i++) {
    species_start += specIndex[i];
    num = specIndex[i + 1];
    int cm = strcmp(arrType, specNames[i]);
    if (0 == cm) {
      found = true;
      break; // matches
    }
  }

  // found a species name
  if (found) {
    char *sta = mStatusArrays + species_start;
    memset(sta, status, num);
    LOG_INFO << "StMuDstMaker::SetStatus \"" << arrType << "\" to status=" << status << " array indices ("
             << species_start << " -> " << species_start + num << ")" << endm;
    if (mIoMode == ioRead)
      setBranchAddresses(mChain);
    return;
  }

  if (strncmp(arrType, "Strange", 7) != 0 && strncmp(arrType, "St", 2) == 0)
    arrType += 2; // Ignore first "St"

  TRegexp re(arrType, 1);
  for (int i = 0; i < __NALLARRAYS__; i++) {
    Ssiz_t len;
    if (re.Index(StMuArrays::arrayNames[i], &len) < 0)
      continue;
    LOG_INFO << "StMuDstMaker::SetStatus " << status << " to " << StMuArrays::arrayNames[i] << endm;
    mStatusArrays[i] = status;
  }
  if (mIoMode == ioRead)
    setBranchAddresses(mChain);
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
StMuDstMaker::StMuDstMaker(int mode, int nameMode, const char *dirName, const char *fileName, const char *filter,
                           int maxFiles, const char *name)
    : StIOInterFace(name), mStEvent(0), mStMuDst(0),
#ifndef __NO_STRANGE_MUDST__
      mStStrangeMuDstMaker(0),
#endif
      mIOMaker(0), mTreeMaker(0), mIoMode(mode), mIoNameMode(nameMode), mDirName(dirName), mFileName(fileName),
      mFilter(filter), mMaxFiles(maxFiles), mEventList(0), mTrackType(256), mReadTracks(1), mReadV0s(1), mReadXis(1),
      mReadKinks(1), mFinish(0), mTrackFilter(0), mL3TrackFilter(0), mCurrentFile(0), mSplit(99), mCompression(9),
      mBufferSize(65536 * 4), mProbabilityPidAlgorithm(0), mEmcCollectionArray(0), mEmcCollection(0), mFmsCollection(0),
      mPmdCollectionArray(0), mPmdCollection(0) {
  assignArrays();
  streamerOff();
  zeroArrays();
  createArrays();
  if (mIoMode == ioRead)
    openRead();
  if (mIoMode == ioWrite)
    mProbabilityPidAlgorithm = new StuProbabilityPidAlgorithm();

  setProbabilityPidFile();

  mEventCounter = 0;
  mStMuDst = new StMuDst();
  mEmcUtil = new StMuEmcUtil();
  mFmsUtil = new StMuFmsUtil();
  mPmdUtil = new StMuPmdUtil();
  mTofUtil = new StMuTofUtil();
  mBTofUtil = new StMuBTofUtil(); /// dongx
  mEpdUtil = new StMuEpdUtil();   // jdb, fix rt 3338
  mEzTree = new StMuEzTree();
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/*StMuDstMaker::StMuDstMaker(int mode, int nameMode, const char* dirName, int year) : StIOInterFace("MuDst"),
  mStEvent(0), mStMuDst(0), mStStrangeMuDstMaker(0),
  mIOMaker(0), mTreeMaker(0),
  mIoMode(mode), mIoNameMode(nameMode),
  mDirName(dirName), mFileName(""), mFilter("."),
  mMaxFiles(10), mEventList(0),
  mTrackType(256), mReadTracks(1),
  mReadV0s(1), mReadXis(1), mReadKinks(1), mFinish(0),
  mTrackFilter(0), mL3TrackFilter(0), mCurrentFile(0),
  mSplit(99), mCompression(9), mBufferSize(65536*4),mStTriggerYear(year),
  mProbabilityPidAlgorithm(0), mEmcCollectionArray(0), mEmcCollection(0),
mFmsCollection(0), mPmdCollectionArray(0), mPmdCollection(0)
{
  assignArrays();
  streamerOff();
  zeroArrays();
  createArrays();
  if (mIoMode==ioRead) openRead();
  if (mIoMode==ioWrite) mProbabilityPidAlgorithm = new StuProbabilityPidAlgorithm();

  setProbabilityPidFile();

  mEventCounter=0;
  mStMuDst = new StMuDst();
  mEmcUtil = new StMuEmcUtil();
  mFmsUtil = new StMuFmsUtil();
  mPmdUtil = new StMuPmdUtil();
  mTofUtil = new StMuTofUtil();
  mBTofUtil= new StMuBTofUtil();  /// dongx
  mEzTree  = new StMuEzTree();
}*/
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
StMuDstMaker::~StMuDstMaker() {
  DEBUGMESSAGE1("");
  // clear(999);
  if (mStMuDst && mStMuDst->TestBit(kCanDelete))
    SafeDelete(mStMuDst);
  SafeDelete(mTofUtil);
  SafeDelete(mBTofUtil); /// dongx
  SafeDelete(mEpdUtil);  /// MALisa
  DEBUGMESSAGE3("after arrays");
  saveDelete(mProbabilityPidAlgorithm);
  saveDelete(mTrackFilter);
  saveDelete(mL3TrackFilter);
  DEBUGMESSAGE3("after filter");
  if (mIoMode == ioWrite)
    closeWrite();
  if (mIoMode == ioRead)
    closeRead();
  DEBUGMESSAGE3("after close");
  saveDelete(mChain);
  // VP  saveDelete(mTTree);
  SafeDelete(mEmcCollectionArray);
  SafeDelete(mPmdCollectionArray);
  DEBUGMESSAGE3("out");
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/** Switch of the TObject part of some streamers, so that only the
    datTClonesamenbers of the derived classes are written to disk, but not
    the data members of the base class TObject
*/
void StMuDstMaker::streamerOff() {
  StMuEvent::Class()->IgnoreTObjectStreamer();
  StMuL3EventSummary::Class()->IgnoreTObjectStreamer();
#ifndef __NO_STRANGE_MUDST__
  StStrangeMuDst::Class()->IgnoreTObjectStreamer();
  StV0MuDst::Class()->IgnoreTObjectStreamer();
  StXiMuDst::Class()->IgnoreTObjectStreamer();
  StKinkMuDst::Class()->IgnoreTObjectStreamer();
  StV0Mc::Class()->IgnoreTObjectStreamer();
  StXiMc::Class()->IgnoreTObjectStreamer();
  StKinkMc::Class()->IgnoreTObjectStreamer();
#endif
  StMuMcVertex::Class()->IgnoreTObjectStreamer();
  StMuMcTrack::Class()->IgnoreTObjectStreamer();
  StMuTrack::Class()->IgnoreTObjectStreamer();
  StMuPrimaryVertex::Class()->IgnoreTObjectStreamer();
  //  StDcaGeometry::Class()->IgnoreTObjectStreamer();
  StMuPrimaryTrackCovariance::Class()->IgnoreTObjectStreamer();
  StMuHelix::Class()->IgnoreTObjectStreamer();
  StMuEmcHit::Class()->IgnoreTObjectStreamer();
  StMuEmcTowerData::Class()->IgnoreTObjectStreamer();
  StMuPmdHit::Class()->IgnoreTObjectStreamer();
  StMuPmdCluster::Class()->IgnoreTObjectStreamer();
  EztEventHeader::Class()->IgnoreTObjectStreamer();
  EztTrigBlob::Class()->IgnoreTObjectStreamer();
  EztFpdBlob::Class()->IgnoreTObjectStreamer();
  EztEmcRawData::Class()->IgnoreTObjectStreamer();
  StMuFgtStrip::Class()->IgnoreTObjectStreamer();
  StMuFgtCluster::Class()->IgnoreTObjectStreamer();
  StMuFgtStripAssociation::Class()->IgnoreTObjectStreamer();
  StMuFgtAdc::Class()->IgnoreTObjectStreamer();
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::createArrays() {
  /// all stuff
  for (int i = 0; i < __NALLARRAYS__; i++) {
    DEBUGVALUE2(mAArrays[i]);
    clonesArray(mAArrays[i], StMuArrays::arrayTypes[i], StMuArrays::arraySizes[i], StMuArrays::arrayCounters[i]);
    DEBUGVALUE2(mAArrays[i]);
  }
  mStMuDst->set(this);
  // commented to include tof again (subhasis)
  // mStMuDst->set(mArrays,mStrangeArrays,mEmcArrays,mPmdArrays);
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
TClonesArray *StMuDstMaker::clonesArray(TClonesArray *&p, const char *type, int size, int &counter) {
  DEBUGMESSAGE2("");
  if (p)
    return p;
  DEBUGVALUE2(type);
  p = new TClonesArray(type, size);
  counter = 0;
  return p;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/**
   The Init() routine is used to establish contact to other makers. As it is STAR
   habit (but really really bad coding) we identify the
   other makers by names (instead of passing pointers). Here, the names are
   hard-wired because they have to be identical to the names
   the bfc is assining to the makers. Do not alter these names unless you know
   what you are doing.
*/
int StMuDstMaker::Init() {
  DEBUGMESSAGE2("");
  mIOMaker = (StIOMaker *)GetMaker("IOMaker");
  mTreeMaker = (StTreeMaker *)GetMaker("outputStream");
#ifndef __NO_STRANGE_MUDST__
  mStStrangeMuDstMaker = (StStrangeMuDstMaker *)GetMaker("strangeMuDst");
#endif
  TDataSet *muDstSet = AddObj(mStMuDst, ".const"); ///< added for Valeri to be able to pick it up in other makers
  if (muDstSet)
    muDstSet->SetName("muDst"); ///< added for Valeri to be able to pick it up in other makers

  return 0;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::Clear(const char *) {
  DEBUGMESSAGE2("");
  // We need to clear the StMuFmsCluster array even when reading events, in
  // order to clear the TRefArrays of hits and photons it holds. Otherwise
  // the references from the previous event may be re-used.
  const int fmsClusterIndex = __NARRAYS__ +
#ifndef __NO_STRANGE_MUDST__
                              __NSTRANGEARRAYS__ +
#endif
                              // FMS arrays follow PMD arrays. Hits = 0th, clusters = 1st, photons = 2nd
                              __NMCARRAYS__ + __NEMCARRAYS__ + __NPMDARRAYS__ + 1;
  mAArrays[fmsClusterIndex]->Clear("C"); // "C" calls StMuFmsCluster::Clear
  if (mIoMode == ioRead)
    return;
  clearArrays();
  if (mStMuDst->event())
    mStMuDst->event()->fmsTriggerDetector().clearFlag(); // Clear flag for every event

  DEBUGMESSAGE3("out");
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/**
   Depending on ioMode, calling Make() will initiate the StMuDstMaker to read or
   write the next event. After the Make() function has finished,
   a call to muDst() will return a pointer to an object od type StMuDst. This object
   will hold the current event if the io was successful, or return a null pointer.
*/
int StMuDstMaker::Make() {

  DEBUGMESSAGE2("");
  int returnStarCode = kStOK;
  StTimer timer;
  timer.start();
  if (mIoMode == ioWrite)
    returnStarCode = MakeWrite();
  else if (mIoMode == ioRead)
    returnStarCode = MakeRead();
  DEBUGVALUE2(timer.elapsedTime());
  return returnStarCode;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
Int_t StMuDstMaker::MakeRead(const StUKey &RunEvent) { return MakeRead(); }
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
Int_t StMuDstMaker::MakeRead() {
  int returnStarCode = kStOK;
  if (mIoMode == ioRead) {
    try {
      read();
    }
    catch (StMuExceptionEOF e) {
      e.print();
      returnStarCode = kStEOF;
    }
    catch (StMuException e) {
      e.print();
      returnStarCode = kStERR;
    }
  }
  return returnStarCode;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
Int_t StMuDstMaker::MakeWrite() {
  int returnStarCode = kStOK;
  if (mIoMode == ioWrite) {
    try {
      write();
    }
    catch (StMuExceptionEOF e) {
      e.print();
      returnStarCode = kStEOF;
    }
    catch (StMuException e) {
      e.print();
      returnStarCode = kStERR;
    }
  }
  return returnStarCode;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::fill() {
  DEBUGMESSAGE2("");
  mStEvent = (StEvent *)GetInputDS("StEvent");
  if (!mStEvent) {
    DEBUGMESSAGE2("no StEvent");
    throw StMuExceptionNullPointer("no StEvent", __PRETTYF__);
  }
  /// once per event the pid algorithm has to be set up
  /// we make it static for the StMuTrack, because all tracks use the same instance
  if (mProbabilityPidAlgorithm)
    SafeDelete(mProbabilityPidAlgorithm);
  mProbabilityPidAlgorithm = new StuProbabilityPidAlgorithm(*mStEvent);
  StMuTrack::setProbabilityPidAlgorithm(mProbabilityPidAlgorithm);
  StMuTrack::setProbabilityPidCentrality(uncorrectedNumberOfNegativePrimaries(*mStEvent));
  try {
    fillTrees(mStEvent);
  }
  catch (StMuException e) {
    e.print();
    throw e;
  }
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::write() {
  DEBUGMESSAGE2("");
  try {
    fill();
  }
  catch (StMuException e) {
    e.print();
    return;
  }

  string ioMakerFileName;
  string theFileName("/dev/null");
  DEBUGVALUE2(mIoNameMode);
  switch (mIoNameMode) {
  case ioFix:
    DEBUGMESSAGE2("===> ioFix\n");
    theFileName = buildFileName(mDirName + "/", basename(mFileName), ".MuDst.root");
    break;
  case ioIOMaker:
    DEBUGMESSAGE2("===> ioIOMaker\n");
    ioMakerFileName = string(mIOMaker->GetFile());
    DEBUGVALUE2(ioMakerFileName);
    theFileName = buildFileName(mDirName + "/", basename(ioMakerFileName), ".MuDst.root");
    break;
  case ioTreeMaker:
    //    ioMakerFileName = mTreeMaker->GetTree()->GetBaseName();
    ioMakerFileName = mTreeMaker->GetTree()->GetBaseName();
    theFileName = buildFileName(dirname(ioMakerFileName), basename(ioMakerFileName), ".MuDst.root");
    break;
  default:
    DEBUGMESSAGE("do not know where to get the filename from");
  }

  DEBUGVALUE2(theFileName.c_str());

  if (theFileName != mCurrentFileName) {
    closeWrite();
    openWrite(theFileName);
    mCurrentFileName = theFileName;
  }

  DEBUGMESSAGE2("now fill tree");
  mTTree->Fill();
  THack::IsTreeWritable(mTTree);
  DEBUGMESSAGE2("tree filled");

  return;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
int StMuDstMaker::Finish() {
  DEBUGMESSAGE2("");
  if (mFinish) {
    for (int i = 0; i < 10; i++) {
      LOG_INFO << "why are you calling the Finish() again  ???????" << endl;
      LOG_INFO << "are you the stupid chain destructor ???????????" << endl;
    }
  } else {
    if (mIoMode == ioWrite)
      closeWrite();
    if (mIoMode == ioRead)
      closeRead();
    mFinish = true;
  }
  DEBUGMESSAGE3("out");
  return 0;
}
//-----------------------------------------------------------------------
const char *StMuDstMaker::GetFile() const {
  if (mIoMode == ioWrite)
    return mCurrentFileName.c_str();
  if (mIoMode == ioRead && mChain && mChain->GetFile())
    return mChain->GetFile()->GetName();
  return 0;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::setBranchAddresses() { setBranchAddresses(mChain); }

void StMuDstMaker::setBranchAddresses(TChain *chain) {
  // all stuff
  if (!chain)
    return;
  chain->SetBranchStatus("*", 0);
  TString ts;
  Int_t emc_oldformat = 0;
  Int_t pmd_oldformat = 0;
  chain->BranchRef(); // Activate autoloading of TRef-referenced objects
  for (int i = 0; i < __NALLARRAYS__; i++) {
    if (mStatusArrays[i] == 0)
      continue;
    const char *bname = StMuArrays::arrayNames[i];
    TBranch *tb = chain->GetBranch(bname);
    if (!tb) {
#ifndef __NO_STRANGE_MUDST__
      if (i >= __NARRAYS__ + __NSTRANGEARRAYS__ + __NMCARRAYS__ &&
          i < __NARRAYS__ + __NSTRANGEARRAYS__ + __NMCARRAYS__ + __NEMCARRAYS__) {
        emc_oldformat = 1;
        continue;
      }

      if (i >= __NARRAYS__ + __NSTRANGEARRAYS__ + __NMCARRAYS__ + __NEMCARRAYS__ &&
          i < __NARRAYS__ + __NSTRANGEARRAYS__ + __NMCARRAYS__ + __NEMCARRAYS__ + __NPMDARRAYS__) {
        pmd_oldformat = 1;
        continue;
      }
#else
      if (i >= __NARRAYS__ + __NMCARRAYS__ && i < __NARRAYS__ + __NMCARRAYS__ + __NEMCARRAYS__) {
        emc_oldformat = 1;
        continue;
      }

      if (i >= __NARRAYS__ + __NMCARRAYS__ + __NEMCARRAYS__ &&
          i < __NARRAYS__ + __NMCARRAYS__ + __NEMCARRAYS__ + __NPMDARRAYS__) {
        pmd_oldformat = 1;
        continue;
      }
#endif
      Warning("setBranchAddresses", "Branch name %s does not exist", bname);
      continue;
    }
    ts = bname;
    ts += "*";
    chain->SetBranchStatus(ts, 1);
    if (strstr("MuEvent", bname) && mChain->GetBranch("MuEvent.mQA.fX")) {
      // Need to manually switch off Q-vector branches to avoid root warnings
      // Note: the Q-vectors are only present in SL07b
      mChain->SetBranchStatus("MuEvent.mQA*", 0);
      mChain->SetBranchStatus("MuEvent.mQB*", 0);
      mChain->SetBranchStatus("MuEvent.mQNegEastA*", 0);
      mChain->SetBranchStatus("MuEvent.mQNegEastB*", 0);
      mChain->SetBranchStatus("MuEvent.mQPosEastA*", 0);
      mChain->SetBranchStatus("MuEvent.mQPosEastB*", 0);
      mChain->SetBranchStatus("MuEvent.mQNegWestA*", 0);
      mChain->SetBranchStatus("MuEvent.mQNegWestB*", 0);
      mChain->SetBranchStatus("MuEvent.mQPosWestA*", 0);
      mChain->SetBranchStatus("MuEvent.mQPosWestB*", 0);
    }
    chain->SetBranchAddress(bname, mAArrays + i);
    assert(tb->GetAddress() == (char *)(mAArrays + i));
  }
  if (emc_oldformat) {
    TBranch *branch = chain->GetBranch("EmcCollection");
    if (branch) {
      Warning("setBranchAddresses", "Using backward compatibility mode for EMC");
      if (!mEmcCollectionArray) {
        mEmcCollectionArray = new TClonesArray("StMuEmcCollection", 1);
      }
      chain->SetBranchStatus("EmcCollection*", 1);
      chain->SetBranchAddress("EmcCollection", &mEmcCollectionArray);
      StMuEmcHit::Class()->IgnoreTObjectStreamer(0);
      mStMuDst->set(this);
    }
  } else if (!mEmcCollection) {
    mEmcCollection = new StMuEmcCollection();
    connectEmcCollection();
    mStMuDst->set(this);
  }

  if (!mFmsCollection) {
    mFmsCollection = new StMuFmsCollection();
    connectFmsCollection();
    mStMuDst->set(this);
  }

  if (pmd_oldformat) {
    TBranch *branch = chain->GetBranch("PmdCollection");
    if (branch) {
      Warning("setBranchAddresses", "Using backward compatibility mode for PMD");
      if (!mPmdCollectionArray) {
        mPmdCollectionArray = new TClonesArray("StMuPmdCollection", 1);
      }
      chain->SetBranchStatus("PmdCollection*", 1);
      chain->SetBranchAddress("PmdCollection", &mPmdCollectionArray);
      StMuPmdCluster::Class()->IgnoreTObjectStreamer(0);
      mStMuDst->set(this);
    }
  } else if (!mPmdCollection) {
    mPmdCollection = new StMuPmdCollection();
    connectPmdCollection();
    mStMuDst->set(this);
  }
  mTTree = mChain->GetTree();
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
int StMuDstMaker::openRead() {
  DEBUGVALUE2(mDirName.c_str());
  DEBUGVALUE2(mFileName.c_str());
  DEBUGVALUE2(mFilter.c_str());

  StMuChainMaker chainMaker("MuDst");
  mChain = chainMaker.make(mDirName, mFileName, mFilter, mMaxFiles);

  if (mChain) {
    DEBUGVALUE3(mChain);
    setBranchAddresses(mChain);

    mStMuDst->set(this);
  }

  return 0;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::read() {
  if (!mChain) {
    DEBUGMESSAGE2("ATTENTION: No StMuChain ... results won't be exciting (nothing to do)");
    throw StMuExceptionNullPointer("No input files", __PRETTYF__);
    return;
  }

  DEBUGMESSAGE2("");
  if (mChain->GetCurrentFile()) {
    DEBUGVALUE2(mChain->GetCurrentFile()->GetName());
  }

  if (!mEventList) {
    int bytes = mChain->GetEntry(mEventCounter++);
    while (bytes <= 0) {
      DEBUGVALUE3(mEventCounter);
      if (mEventCounter >= mChain->GetEntriesFast())
        throw StMuExceptionEOF("end of input", __PRETTYF__);
      bytes = mChain->GetEntry(mEventCounter++);
      DEBUGVALUE3(bytes);
    }
  } else {
    int bytes = mChain->GetEntry(mEventList->GetEntry(mEventCounter++));
    while (bytes <= 0) {
      DEBUGVALUE3(mEventCounter);
      if (mEventCounter >= mEventList->GetN())
        throw StMuExceptionEOF("end of event list", __PRETTYF__);
      bytes = mChain->GetEntry(mEventList->GetEntry(mEventCounter++));
      DEBUGVALUE3(bytes);
    }
  }
  if (GetDebug() > 1)
    printArrays();
  mStMuDst->set(this);
  fillHddr();
  mStMuDst->setVertexIndex(0);
  mStMuDst->collectVertexTracks(); // Make temp list of tracks for current prim vtx
  return;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::closeRead() {
  DEBUGMESSAGE2("");
  if (mChain)
    mChain->Delete();
  mChain = 0;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::openWrite(string fileName) {
  DEBUGVALUE2(fileName.c_str());
  // creat a Picoevent and and output file
  DEBUGMESSAGE2("now create file");
  mCurrentFile = new TFile(fileName.c_str(), "RECREATE", "StMuDst");

  if (mCurrentFile->IsZombie())
    throw StMuExceptionNullPointer("no file openend", __PRETTYF__);

  mCurrentFile->SetCompressionLevel(mCompression);

  // Create a ROOT Tree and one superbranch
  DEBUGMESSAGE2("now create trees and branches");

  int bufsize = mBufferSize;
  if (mSplit)
    bufsize /= 4;
  //  all stuff
  mTTree = new TTree("MuDst", "StMuDst", mSplit);
  mTTree->BranchRef(); // Activate autoloading of TRef-referenced objects
#if ROOT_VERSION_CODE < ROOT_VERSION(5, 26, 0)
  Long64_t MAXLONG = 100000000000LL; // 100 GB
  LOG_INFO << "Tree size MAX will be " << (float)MAXLONG / 1000 / 1000 / 1000 << " GB " << endm;
  mTTree->SetMaxTreeSize(MAXLONG); // limited to 1.9 GB  - set to maximum
#endif
  // mTTree->SetAutoSave(1000000);  // autosave when 1 Mbyte written
  DEBUGMESSAGE2("all arrays");
  for (int i = 0; i < __NALLARRAYS__; i++) {
    if (mStatusArrays[i] == 0)
      continue;
    mTTree->Branch(StMuArrays::arrayNames[i], &mAArrays[i], bufsize, mSplit);
  }
  mCurrentFileName = fileName;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::closeWrite() {
  DEBUGMESSAGE(__PRETTYF__);
  if (mTTree && mCurrentFile) {
    LOG_INFO << " ##### " << __PRETTYF__ << endm;
    LOG_INFO << " ##### File=" << mCurrentFile->GetName() << " ";
    LOG_INFO << " NumberOfEvents= " << mTTree->GetEntries() << " ";
    LOG_INFO << " ##### " << endm;
  }
  // if (mTTree) mTTree->AutoSave();

  if (mCurrentFile) {
    mCurrentFile->Write();
    mCurrentFile->Close();
  }
  mTTree = 0;
  mCurrentFile = 0;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::fillTrees(StEvent *ev, StMuCut *cut) {

  DEBUGMESSAGE2("");
  try {
    fillMC();
  }
  catch (StMuException e) {
    e.print();
    throw e;
  }

  try {
    fillEvent(ev);
    fillL3AlgorithmInfo(ev);
    fillDetectorStates(ev);
    fillEmc(ev);
    fillPmd(ev);
    fillFms(ev);
    fillTof(ev);
    fillBTof(ev);
    fillEpd(ev); // MALisa
    fillMtd(ev);
    fillFgt(ev);
    fillEzt(ev);
  }
  catch (StMuException e) {
    e.print();
    throw e;
  }

  try {
    fillVertices(ev);
  }
  catch (StMuException e) {
    e.print();
    throw e;
  }

  try {
    fillpp2pp(ev);
  }
  catch (StMuException e) {
    e.print();
    throw e;
  }

  try {
    fillTracks(ev, mTrackFilter);
  }
  catch (StMuException e) {
    e.print();
    throw e;
  }

  try {
    fillL3Tracks(ev, mL3TrackFilter);
  }
  catch (StMuException e) {
    e.print();
    throw e;
  }

#ifndef __NO_STRANGE_MUDST__
  if (mStStrangeMuDstMaker) {
    try {
      fillStrange(mStStrangeMuDstMaker);
    }
    catch (StMuException e) {
      e.print();
      throw e;
    }
  }
#endif

  // catch(StMuException e) {
  //  e.print();
  //  throw e;
  //}

  mStMuDst->set(this);
  mStMuDst->fixTofTrackIndices();
  mStMuDst->fixMtdTrackIndices();
  mStMuDst->fixTrackIndicesG(mStMuDst->numberOfPrimaryVertices());
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::fillEvent(StEvent *ev, StMuCut *cut) {
  DEBUGMESSAGE2("");
  StMuEvent *typeOfEvent = 0;
  if (!ev)
    throw StMuExceptionNullPointer("no StEvent", __PRETTYF__);
  StTimer timer;
  timer.start();
  if (!cut || cut->pass(ev)) {
    DEBUGMESSAGE3("");
    addType(mArrays[muEvent], ev, typeOfEvent);
  }
  timer.stop();
  DEBUGVALUE2(timer.elapsedTime());
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::fillEmc(StEvent *ev) {
  DEBUGMESSAGE2("");
  StEmcCollection *emccol = (StEmcCollection *)ev->emcCollection();
  if (!emccol)
    return; // throw StMuExceptionNullPointer("no StEmcCollection",__PRETTYF__);
  StTimer timer;
  timer.start();

  TClonesArray *tca = mEmcArrays[muEmcTow];
  new ((*tca)[0]) StMuEmcTowerData();
  if (!mEmcCollection) {
    mEmcCollection = new StMuEmcCollection();
    connectEmcCollection();
    mStMuDst->set(this);
  }
  mEmcUtil->fillMuEmc(mEmcCollection, emccol);

  timer.stop();
  DEBUGVALUE2(timer.elapsedTime());
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::fillFms(StEvent *ev) {
  DEBUGMESSAGE2("");
  StFmsCollection *fmscol = (StFmsCollection *)ev->fmsCollection();
  if (!fmscol)
    return; // throw StMuExceptionNullPointer("no StFmsCollection",__PRETTYF__);
  StTimer timer;
  timer.start();

  if (!mFmsCollection) {
    mFmsCollection = new StMuFmsCollection();
    connectFmsCollection();
    mStMuDst->set(this);
  }
  LOG_DEBUG << "StMuDSTMaker filling StMuFmsCollection from StEvent" << endm;
  mFmsUtil->fillMuFms(mFmsCollection, fmscol);

  timer.stop();
  DEBUGVALUE2(timer.elapsedTime());
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::fillPmd(StEvent *ev) {
  DEBUGMESSAGE2("");
  StPhmdCollection *phmdColl = (StPhmdCollection *)ev->phmdCollection();
  if (!phmdColl)
    return; // throw StMuExceptionNullPointer("no StPhmdCollection",__PRETTYF__);
  StTimer timer;
  timer.start();

  if (!mPmdCollection) {
    mPmdCollection = new StMuPmdCollection();
    connectPmdCollection();
    mStMuDst->set(this);
  }
  mPmdUtil->fillMuPmd(phmdColl, mPmdCollection);

  timer.stop();
  DEBUGVALUE2(timer.elapsedTime());
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

void StMuDstMaker::fillTof(StEvent *ev) {
  DEBUGMESSAGE2("");
  StTofCollection *tofcol = ev->tofCollection();
  // run 5 - dongx
  if (!ev || !tofcol || (!tofcol->dataPresent() && !tofcol->rawdataPresent()))
    return; // throw StMuExceptionNullPointer("no StTofDataCollection",__PRETTYF__);
  StTimer timer;
  timer.start();

  // fill tofHit
  StMuTofHitCollection muTofHitColl;
  mTofUtil->fillMuTofHit(&muTofHitColl, tofcol);
  for (size_t i = 0; i < muTofHitColl.size(); i++) {
    StMuTofHit *tofMuHit = (StMuTofHit *)muTofHitColl.getHit(i);
    addType(mTofArrays[muTofHit], *tofMuHit);
  }

  // fill tofData
  StSPtrVecTofData &tofData = tofcol->tofData();
  for (size_t i = 0; i < tofData.size(); i++) {
    addType(mTofArrays[muTofData], *tofData[i]);
  }

  // run 5 - dongx
  // fill tofRawData
  StSPtrVecTofRawData &tofRawData = tofcol->tofRawData();
  for (size_t i = 0; i < tofRawData.size(); i++) {
    addType(mTofArrays[muTofRawData], *tofRawData[i]);
  }

  timer.stop();
  DEBUGVALUE2(timer.elapsedTime());
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/// dongx
void StMuDstMaker::fillBTof(StEvent *ev) {
  DEBUGMESSAGE2("");
  StBTofCollection *btofcol = ev->btofCollection();
  if (!ev || !btofcol || !btofcol->rawHitsPresent())
    return; // throw StMuExceptionNullPointer("no StBTofRawHitCollection",__PRETTYF__);
  StTimer timer;
  timer.start();

  // fill btofHit
  StMuBTofHitCollection muBTofHitColl;
  mBTofUtil->fillMuBTofHit(&muBTofHitColl, btofcol);
  for (size_t i = 0; i < muBTofHitColl.size(); i++) {
    StMuBTofHit *btofMuHit = (StMuBTofHit *)muBTofHitColl.getHit(i);
    addType(mBTofArrays[muBTofHit], *btofMuHit);
  }

  // fill btofRawHit
  StSPtrVecBTofRawHit &btofRawHits = btofcol->tofRawHits();
  for (size_t i = 0; i < btofRawHits.size(); i++) {
    addType(mBTofArrays[muBTofRawHit], *btofRawHits[i]);
  }

  // fill btofHeader
  StBTofHeader *btofHeader = btofcol->tofHeader();
  addType(mBTofArrays[muBTofHeader], *btofHeader);

  timer.stop();
  DEBUGVALUE2(timer.elapsedTime());
}
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
void StMuDstMaker::fillEpd(StEvent *ev) { // MALisa
  DEBUGMESSAGE2("");
  StEpdCollection *epdcol = (StEpdCollection *)ev->epdCollection();
  if (!epdcol)
    return; // throw StMuExceptionNullPointer("no StEpdCollection"__PRETTYF__);
  StTimer timer;
  timer.start();

  // fill epdHit
  StMuEpdHitCollection muEpdHitColl;
  mEpdUtil->fillMuEpdHit(&muEpdHitColl, epdcol);
  for (size_t i = 0; i < muEpdHitColl.size(); i++) {
    StMuEpdHit *epdMuHit = (StMuEpdHit *)muEpdHitColl.getHit(i);
    addType(mEpdArrays[muEpdHit], *epdMuHit);
  }
  timer.stop();
  DEBUGVALUE2(timer.elapsedTime());
  /*
  if (!mMuEpdHitCollection){
    mMuEpdHitCollection = new StMuEpdHitCollection();
    mStMuDst->set(this);
  }
  LOG_DEBUG << "StMuDSTMaker filling StMuEpdHitCollection from StEvent" << endm;
  mMuEpdHitCollection->Fill(epdHitCollection);

  cout << "\n\n\n\n I just filled the muEpdHitCollection and it has " << mMuEpdHitCollection->nHits() << "
  hits\n\n\n\n";
  */
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::fillFgt(StEvent *ev) {

  const StFgtCollection *fgtCollPtr = 0;
  if (ev)
    fgtCollPtr = ev->fgtCollection();

  if (fgtCollPtr) {
    // assert existance of the arrays
    assert(mFgtArrays);
    assert(mFgtArrays[muFgtStrips]);
    assert(mFgtArrays[muFgtClusters]);
    assert(mFgtArrays[muFgtStripAssociations]);

    // need pointer types to enforce conversions in StMuDstMaker::addType
    StMuFgtCluster *clusterClassType = 0;

    // need a map to keep track of the index of each strip.
    // Key = geoId, value = idx in muFgtStrips TClonesArray
    std::map<Int_t, Int_t> stripGeoIdIdxMap;
    std::map<Int_t, Int_t>::iterator stripGeoIdIdxIter;

    // iterate over discs
    for (UShort_t discIdx = 0; discIdx < kFgtNumDiscs; ++discIdx) {

      // copy strips
      const StSPtrVecFgtStrip &stripVec = fgtCollPtr->mStripCollection[discIdx].getStripVec();
      for (const_StFgtStripIterator stripIter = stripVec.begin(); stripIter != stripVec.end(); ++stripIter) {
        Short_t stripType = (*stripIter)->getClusterSeedType();

        // save only the strips with a certain type
        if (stripType == kFgtSeedType1 || stripType == kFgtSeedType2 || stripType == kFgtSeedType3 ||
            stripType == kFgtSeedType4 || stripType == kFgtSeedType5 || stripType == kFgtSeedTypeMax ||
            stripType == kFgtClusterPart || stripType == kFgtNextToDeadGuy || stripType == kFgtClusterEndUp ||
            stripType == kFgtClusterEndDown || stripType == kFgtStripShared || stripType == kFgtClusterTooBig ||
            stripType == kFgtClusterSeedInSeaOfNoise || stripType == kFgtNextToCluster || stripType == kFgtKeepStrip) {

          // make the strip using the "casting" constructor
          StMuFgtStrip fgtStrip(*(*stripIter));

          // Set the range for the time bins to save.
          // Note: for now, use the obvious.  Update the
          // following two lines later.`1
          Int_t nTBstart = 0;
          Int_t nTBend = nTBstart + fgtCollPtr->getNumTimeBins();

          // to do: comment about ensuring tb in same range as StFgtStrip class in StEvent

          for (Int_t tb = nTBstart; tb < nTBend; ++tb) {
            // make an StMuFgtAdc object
            StMuFgtAdc fgtAdc((*stripIter)->getAdc(tb), tb);

            Int_t adcIdx = addType(mFgtArrays[muFgtAdcs], fgtAdc);

            // just the first time
            if (tb == nTBstart) {

              // set the strip to know where the first ADC is and how many there are
              fgtStrip.setAdcInfo(adcIdx, nTBend - nTBstart);
            };
          };

          // add strip to the TClonesArray
          Int_t idx = addType(mFgtArrays[muFgtStrips], fgtStrip);

          // add to the map
          stripGeoIdIdxMap[(*stripIter)->getGeoId()] = idx;
        };
      };

      // faster to declare iterator outside of the loop
      stripWeightMap_t::const_iterator wIter;

      // copy clusters
      const StSPtrVecFgtHit &hitVec = fgtCollPtr->mHitCollection[discIdx].getHitVec();
      for (const_StFgtHitIterator hitIter = hitVec.begin(); hitIter != hitVec.end(); ++hitIter) {
        if ((*hitIter)->charge() > 0) {
          // add the cluster to the array
          Int_t clusIdx = addType(mFgtArrays[muFgtClusters], *(*hitIter), clusterClassType);

          // get the map of associations
          const stripWeightMap_t &wMap = (*hitIter)->getStripWeightMap();

          // iterate over associated strips and add the associations
          Bool_t isFirst = 1;
          for (wIter = wMap.begin(); wIter != wMap.end(); ++wIter) {

            Int_t geoId = wIter->first->getGeoId(); // first is a StFgtStrip, which knows its own geoId
            Float_t weight = wIter->second;         // second is the weight

            // determine the stripIdx
            stripGeoIdIdxIter = stripGeoIdIdxMap.find(geoId);

            // make sure the strip was saved
            assert(stripGeoIdIdxIter != stripGeoIdIdxMap.end());

            Int_t stripIdx = stripGeoIdIdxIter->second;

            // make the association
            StMuFgtStripAssociation association(clusIdx, stripIdx, weight);

            // add it to the array
            Int_t associationIdx = addType(mFgtArrays[muFgtStripAssociations], association);

            if (isFirst) {
              // change the flag state
              isFirst = 0;

              // set the index in the cluster
              StMuFgtCluster *clusPtr = (StMuFgtCluster *)mFgtArrays[muFgtClusters]->UncheckedAt(clusIdx);
              clusPtr->setFirstStripAssociationIndex(associationIdx);
            };
          };
        };
      };
    };
  };
};

void StMuDstMaker::fillMtd(StEvent *ev) {
  DEBUGMESSAGE2("");
  StTimer timer;
  timer.start();

  //	StMuMtdCollection *typeOfMtd=0;

  const StMtdCollection *mtd = ev->mtdCollection();
  if (!mtd)
    return;
  StMuMtdCollection mMTD(*mtd);

  for (size_t i = 0; i < (size_t)mMTD.hitsPresent(); i++) {
    StMuMtdHit *mtdHit = (StMuMtdHit *)mMTD.MtdHit(i);
    addType(mMtdArrays[muMTDHit], *mtdHit);
  }

  for (size_t i = 0; i < (size_t)mMTD.rawHitsPresent(); i++) {
    StMtdRawHit *mtdHit = (StMtdRawHit *)mMTD.RawMtdHit(i);
    addType(mMtdArrays[muMTDRawHit], *mtdHit);
  }
  StMuMtdHeader *mtdHead = mMTD.mtdHeader();
  if (mtdHead)
    addType(mMtdArrays[muMTDHeader], *mtdHead);

  // if (mtd) addType( mArrays[muMtd], *mtd, typeOfMtd );
  timer.stop();
  DEBUGVALUE2(timer.elapsedTime());
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::fillEzt(StEvent *ev) {
  if (ev == 0)
    return;
  char *eztArrayStatus =
      &mStatusArrays[__NARRAYS__ +
#ifndef __NO_STRANGE_MUDST__
                     __NSTRANGEARRAYS__ +
#endif
                     __NMCARRAYS__ + __NEMCARRAYS__ + __NPMDARRAYS__ + __NFMSARRAYS__ + __NTOFARRAYS__ +
                     __NBTOFARRAYS__ + __NEPDARRAYS__ + __NMTDARRAYS__ + __NFGTARRAYS__]; /// dongx, MALisa
  if (eztArrayStatus[muEztHead]) {
    EztEventHeader *header = mEzTree->copyHeader(ev);
    addType(mEztArrays[muEztHead], *header);
  }

  if (eztArrayStatus[muEztTrig]) {
    EztTrigBlob *trig = mEzTree->copyTrig(ev);
    if (trig)
      addType(mEztArrays[muEztTrig], *trig);
  }

  if (eztArrayStatus[muEztFpd]) {
    EztFpdBlob *fpd = mEzTree->copyFpd(ev);
    addType(mEztArrays[muEztFpd], *fpd);
  }

  if (eztArrayStatus[muEztETow] || eztArrayStatus[muEztESmd]) {
    StEmcCollection *emcCol = (StEmcCollection *)ev->emcCollection();
    if (emcCol == 0) {
      gMessMgr->Message("", "W") << GetName()
                                 << "::fillEzt(), missing StEmcCollection, EEMC raw data NOT saved in muDst" << endm;
    } else { //........... EMC-Collection in StEvent exist
      StEmcRawData *eeRaw = emcCol->eemcRawData();
      if (eeRaw) {
        if (eztArrayStatus[muEztETow]) {
          EztEmcRawData *ETow = mEzTree->copyETow(eeRaw);
          addType(mEztArrays[muEztETow], *ETow);
        }

        if (eztArrayStatus[muEztESmd]) {
          EztEmcRawData *ESmd = mEzTree->copyESmd(eeRaw);
          addType(mEztArrays[muEztESmd], *ESmd);
        }
      }
    }
  }
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::fillL3AlgorithmInfo(StEvent *ev) {
  DEBUGMESSAGE2("");
  if (!ev->l3Trigger())
    return;
  if (!ev->l3Trigger()->l3EventSummary())
    return;

  StTimer timer;
  timer.start();
  StL3EventSummary *l3 = ev->l3Trigger()->l3EventSummary();
  int n = l3->numberOfAlgorithms();
  for (int i = 0; i < n; i++) {
    if (l3->algorithms()[i]->accept())
      addType(mArrays[muAccept], *l3->algorithms()[i]);
    else
      addType(mArrays[muReject], *l3->algorithms()[i]);
  }
  timer.stop();
  DEBUGVALUE2(timer.elapsedTime());
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::fillVertices(StEvent *ev) {
  DEBUGMESSAGE2("");
  StTimer timer;
  timer.start();

  StMuPrimaryVertex *typeOfVertex = 0;
  Int_t n_vtx = ev->numberOfPrimaryVertices();
  DEBUGVALUE2(n_vtx);
  mVtxList.Clear();
  for (Int_t i_vtx = 0; i_vtx < n_vtx; i_vtx++) {
    const StPrimaryVertex *vtx = ev->primaryVertex(i_vtx);
    addType(mArrays[muPrimaryVertex], vtx, typeOfVertex);
    mVtxList.AddAtAndExpand(ev->primaryVertex(i_vtx), i_vtx);
  }
  timer.stop();
  DEBUGVALUE2(timer.elapsedTime());
}

void StMuDstMaker::fillpp2pp(StEvent *ev) {
  DEBUGMESSAGE2("");
  StTimer timer;
  timer.start();

  StMuRpsCollection *typeOfRps = 0;

  const StRpsCollection *rps = ev->rpsCollection();
  if (rps)
    addType(mArrays[mupp2pp], *rps, typeOfRps);
  timer.stop();
  DEBUGVALUE2(timer.elapsedTime());
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::fillTracks(StEvent *ev, StMuCut *cut) {
  DEBUGMESSAGE2("");
  StTimer timer;
  timer.start();

  StSPtrVecTrackNode &nodes = ev->trackNodes();
  DEBUGVALUE2(nodes.size());
  for (StSPtrVecTrackNodeConstIterator iter = nodes.begin(); iter != nodes.end(); iter++) {
    addTrackNode(ev, *iter, cut, mArrays[muGlobal], mArrays[muPrimary], mArrays[muOther], mArrays[muCovGlobTrack],
                 mArrays[muCovPrimTrack], false);
  }
  timer.stop();
  DEBUGVALUE2(timer.elapsedTime());
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::fillL3Tracks(StEvent *ev, StMuCut *cut) {
  DEBUGMESSAGE2("");
  if (!ev->l3Trigger())
    return;

  StTimer timer;
  timer.start();
  StSPtrVecTrackNode &nodes = ev->l3Trigger()->trackNodes();
  DEBUGVALUE2(nodes.size());
  for (StSPtrVecTrackNodeConstIterator iter = nodes.begin(); iter != nodes.end(); iter++) {
    addTrackNode(ev, *iter, cut, mArrays[muL3], 0, 0, 0, 0, true);
  }
  timer.stop();
  DEBUGVALUE2(timer.elapsedTime());
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::fillDetectorStates(StEvent *ev) {
  DEBUGMESSAGE2("");
  StTimer timer;
  timer.start();
  for (int i = 0; i < StMuArrays::arraySizes[muState]; i++) {
    StDetectorState *state = ev->detectorState((StDetectorId)i);
    if (state) {
      addType(mArrays[muState], *state);
    }
  }
  timer.stop();
  DEBUGVALUE2(timer.elapsedTime());
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::addTrackNode(const StEvent *ev, const StTrackNode *node, StMuCut *cut, TClonesArray *gTCA,
                                TClonesArray *pTCA, TClonesArray *oTCA, TClonesArray *covgTCA, TClonesArray *covpTCA,
                                bool l3) {
  DEBUGMESSAGE3("");
  const StTrack *tr = 0;

  /// do global track
  int index2Global = -1;
  if (gTCA) {
    const StTrack *pr_tr = node->track(primary);
    const StVertex *vtx = 0;
    if (pr_tr)
      vtx = pr_tr->vertex();
    if (vtx == 0)
      vtx = ev->primaryVertex();

    tr = node->track(global);
    if (tr && !tr->bad())
      index2Global = addTrack(gTCA, ev, tr, vtx, cut, -1, l3, covgTCA, covpTCA);
  }
  /// do primary track track
  if (pTCA) {
    tr = node->track(primary);
    if (tr && !tr->bad())
      addTrack(pTCA, ev, tr, tr->vertex(), cut, index2Global, l3, covgTCA, covpTCA);
  }
  /// all other tracks
  if (oTCA) {
    size_t nEntries = node->entries();
    for (size_t j = 0; j < nEntries; j++) { /// loop over all tracks in tracknode
      tr = node->track(j);
      if (tr && !tr->bad() && (tr->type() != global) && (tr->type() != primary)) { /// exclude global and primary tracks
        addTrack(oTCA, ev, tr, tr->vertex(), cut, index2Global, l3);
      }
    }
  }
}
//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------
int StMuDstMaker::addTrack(TClonesArray *tca, const StEvent *event, const StTrack *track, const StVertex *vtx,
                           StMuCut *cut, int index2Global, bool l3, TClonesArray *covgTCA, TClonesArray *covpTCA) {
  DEBUGMESSAGE3("");
  StRichSpectra typeOfStRichSpectra;
  int index = -1;
  int index2RichSpectra = -1;
  /// if (!tca || !track) return index; /// I made sure that the array anf the track is there
  int counter = tca->GetEntries();
  try {
    if (cut && !cut->pass(track))
      throw StMuExceptionBadValue("failed track cut", __PRETTYF__);
    // add StRichSpectra if StRichPidTraits are found
    // we have to do this more elegant
    StRichSpectra *rich = richSpectra(track);
    if (rich) {
      index2RichSpectra = addType(mArrays[muRich], *rich);
    }
    StMuTrack *muTrack =
        new ((*tca)[counter]) StMuTrack(event, track, vtx, index2Global, index2RichSpectra, l3, &mVtxList);
    if (track->type() == primary) {
      if (covpTCA) {
        Int_t countCOVPTCA = covpTCA->GetEntries();
#if 0
	const StMatrixF covMatrix = track->fitTraits().covariantMatrix();
	new((*covpTCA)[countCOVPTCA]) StMuPrimaryTrackCovariance(covMatrix);
#else
        //	cout << track->fitTraits().covariantMatrix() << endl;
        const Float_t *cov = track->fitTraits().covariance();
        new ((*covpTCA)[countCOVPTCA]) StMuPrimaryTrackCovariance(cov);
#endif
        muTrack->setIndex2Cov(countCOVPTCA);
      }
    } else {
      if (track->type() == global) {
        if (covgTCA) {
          Int_t countCOVGTCA = covgTCA->GetEntries();
          const StDcaGeometry *dcaGeometry = ((StGlobalTrack *)track)->dcaGeometry();
          if (dcaGeometry) {
            new ((*covgTCA)[countCOVGTCA]) StDcaGeometry(*dcaGeometry);
            muTrack->setIndex2Cov(countCOVGTCA);
          }
        }
      }
    }
    index = counter;
  }
  catch (StMuException e) {
    IFDEBUG3(e.print());
  }
  return index; /// return index to self if newly created, else return -1;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
StRichSpectra *StMuDstMaker::richSpectra(const StTrack *track) {
  DEBUGMESSAGE3("");
  const StPtrVecTrackPidTraits &traits = track->pidTraits(kRichId);
  for (StPtrVecTrackPidTraitsConstIterator traitIter = traits.begin(); traitIter != traits.end(); ++traitIter) {
    StRichPidTraits *pid = dynamic_cast<StRichPidTraits *>(*traitIter);
    if (pid)
      return pid->getRichSpectra();
  }
  return 0;
}
#ifndef __NO_STRANGE_MUDST__
void StMuDstMaker::fillStrange(StStrangeMuDstMaker *maker) {
  DEBUGMESSAGE2("");
  /// now fill the strangeness stuff
  if (!maker)
    throw StMuExceptionNullPointer("no StrangeMuDstMaker", __PRETTYF__);

  StStrangeEvMuDst *ev = 0;
  StV0MuDst *v0 = 0;
  StStrangeAssoc *assoc = 0;
  StXiMuDst *xi = 0;
  StKinkMuDst *kink = 0;
  StV0Mc *v0Mc = 0;
  StXiMc *xiMc = 0;
  StKinkMc *kinkMc = 0;
  TCut *strangeCut = 0;

  addType(maker->GetEvClonesArray(), mStrangeArrays[0], ev);
  addType(maker->GetEvMcArray(), mStrangeArrays[1], ev);

  addType(maker->GetV0ClonesArray(), mStrangeArrays[2], v0);
  addType(maker->GetV0McArray(), mStrangeArrays[3], v0Mc);
  addType(maker->GetV0AssocArray(), mStrangeArrays[4], assoc);

  addType(maker->GetXiClonesArray(), mStrangeArrays[5], xi);
  addType(maker->GetXiMcArray(), mStrangeArrays[6], xiMc);
  addType(maker->GetXiAssocArray(), mStrangeArrays[7], assoc);

  addType(maker->GetKinkClonesArray(), mStrangeArrays[8], kink);
  addType(maker->GetKinkMcArray(), mStrangeArrays[9], kinkMc);
  addType(maker->GetKinkAssocArray(), mStrangeArrays[10], assoc);

  addType(maker->GetCutsArray(), mStrangeArrays[11], strangeCut);
}
#endif
//-----------------------------------------------------------------------
void StMuDstMaker::fillMC() {
  St_g2t_track *g2t_track = (St_g2t_track *)GetDataSet("geant/g2t_track");
  if (!g2t_track)
    return;
  St_g2t_vertex *g2t_vertex = (St_g2t_vertex *)GetDataSet("geant/g2t_vertex");
  if (!g2t_vertex)
    return;
  StG2TrackVertexMap::instance(g2t_track, g2t_vertex);
  mStEvent->setIdTruth();
  StMuMcVertex *mcvx = 0;
  StMuMcTrack *mctr = 0;
  g2t_vertex_st *vertex = g2t_vertex->GetTable();
  UInt_t NV = g2t_vertex->GetNRows();
  for (UInt_t i = 0; i < NV; i++)
    addType(mMCArrays[MCVertex], vertex[i], mcvx);
  g2t_track_st *track = g2t_track->GetTable();
  UInt_t NT = g2t_track->GetNRows();
  for (UInt_t i = 0; i < NT; i++) {
    if (track[i].pt <= 1e-3)
      track[i].pt = -999;
    addType(mMCArrays[MCTrack], track[i], mctr);
  }
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
template <class T> int StMuDstMaker::addType(TClonesArray *tcaFrom, TClonesArray *&tcaTo, T *t) {
  if (tcaFrom && tcaTo) {
    int n = tcaFrom->GetEntries();
    int counter = tcaTo->GetEntries();
    for (int i = 0; i < n; i++) {
      // old     new((*tcaTo)[counter++]) T( (T&)*tcaFrom->UncheckedAt(i) );
      new ((*tcaTo)[counter++]) T(*(T *)(void *)tcaFrom->UncheckedAt(i));
    }
  }
  return 0;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
// int StMuDstMaker::addType(TClonesArray* tcaTo , StMuEmcCollection *t) {
//   int counter =-1;
//   if (tcaTo) {
//     counter = tcaTo->GetEntries();

//     new((*tcaTo)[counter]) StMuEmcCollection();
//   }
//   return counter;
// }
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
template <class T> int StMuDstMaker::addType(TClonesArray *tcaTo, T &t) {
  int counter = -1;
  if (tcaTo) {
    counter = tcaTo->GetEntries();
    new ((*tcaTo)[counter]) T(t);
  }
  return counter;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
template <class T, class U> int StMuDstMaker::addType(TClonesArray *tcaTo, U &u, T *t) {
  int counter = -1;
  if (tcaTo) {
    counter = tcaTo->GetEntries();
    DEBUGMESSAGE2("");
    new ((*tcaTo)[counter]) T(u);
  }
  return counter;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
string StMuDstMaker::buildFileName(string dir, string fileName, string extention) {
  DEBUGMESSAGE3(dir.c_str());
  DEBUGMESSAGE3(fileName.c_str());
  DEBUGMESSAGE3(extention.c_str());
  fileName = dir + fileName + extention;
  while (fileName.find("//") != string::npos) {
    int pos = fileName.find("//");
    fileName.erase(pos, 1);
  }
  return fileName;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
string StMuDstMaker::basename(string s) {
  DEBUGVALUE3(s.c_str());
  string name(s);
  DEBUGVALUE3(name.c_str());
  size_t pos;
  pos = name.find_last_of("/");
  if (pos != string::npos)
    name.erase(0, pos);
  DEBUGVALUE3(name.c_str());
  pos = name.find_last_of(".");
  if (pos != string::npos)
    name.erase(pos, name.length() - pos);
  DEBUGVALUE3(name.c_str());
  pos = name.find_last_of(".");
  if (pos != string::npos)
    name.erase(pos, name.length() - pos);
  DEBUGVALUE3(name.c_str());
  return name;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
string StMuDstMaker::dirname(string s) {
  string name(s);
  DEBUGVALUE3(name.c_str());
  size_t pos;
  pos = name.find_last_of("/");

  if (pos != string::npos)
    name.erase(pos, name.length());
  if (name == s)
    name = ".";

  name = name + "/";
  DEBUGVALUE3(name);
  return name;
}

void StMuDstMaker::setProbabilityPidFile(const char *file) {
  ostrstream flnm;

  if (!file) {
    const char *PIDtable = "PIDTableP01gl.root";

    flnm << getenv("STAR") << "/StarDb/dEdxModel/" << PIDtable << ends;
    FORCEDDEBUGMESSAGE("ATTENTION: pid table hardwired to " << flnm.str());

  } else {
    flnm << file << ends;
    FORCEDDEBUGMESSAGE("Using pid table to user value " << flnm.str());
  }

  if (mProbabilityPidAlgorithm)
    mProbabilityPidAlgorithm->readParametersFromFile(flnm.str());
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDstMaker::printArrays() {
  // all stuff
  TClonesArray *tcl;
  for (int i = 0; i < __NALLARRAYS__; i++) {
    if (mStatusArrays[i] == 0)
      continue;
    tcl = mAArrays[i];
    printf(" Array %s\t = %s::%s(%d)\n", StMuArrays::arrayNames[i], tcl->ClassName(), tcl->GetName(),
           tcl->GetEntriesFast());
  }
}
//-----------------------------------------------------------------------
void StMuDstMaker::fillHddr() {
  StMuEvent *me = mStMuDst->event();
  if (me == 0)
    return;
  StEventInfo &ei = me->eventInfo();
  StRunInfo &ri = me->runInfo();
  StEvtHddr *hd = GetEvtHddr();

  hd->SetRunNumber(ei.runId());
  hd->SetEventType(ei.type().Data());
  hd->SetTriggerMask(ei.triggerMask());
  // hd->SetInputTriggerMask(???);
  hd->SetBunchCrossingNumber(ei.bunchCrossingNumber(0), ei.bunchCrossingNumber(1));
  hd->SetCenterOfMassEnergy(ri.centerOfMassEnergy());
  // hd->SetBImpact  (float b)  	;
  // hd->SetPhiImpact(float p)  	;
  // hd->SetPhImpact (float p)  	;
  // hd->SetAEast(int a)		;
  // hd->SetZEast(int z)		;
  // hd->SetAWest(int a)		;
  // hd->SetZWest(int z)		;
  // hd->SetLuminosity(float lu)	;
  hd->SetGMTime((unsigned int)ei.time());
  hd->SetProdDateTime(ri.productionTime());
  // hd->SetIventNumber(int iv)	;
  hd->SetEventSize(ei.eventSize());
  hd->SetEventNumber(ei.id());
  // hd->SetGenerType(int g);
}

//-----------------------------------------------------------------------
void StMuDstMaker::connectEmcCollection() {
  mEmcCollection->setTowerData((StMuEmcTowerData *)(*(mEmcArrays[muEmcTow]))[0]);
  mEmcCollection->setPrsArray(2, mEmcArrays[muEmcPrs]);
  mEmcCollection->setSmdArray(3, mEmcArrays[muEmcSmde]);
  mEmcCollection->setSmdArray(4, mEmcArrays[muEmcSmdp]);
  mEmcCollection->setPrsArray(6, mEmcArrays[muEEmcPrs]);
  mEmcCollection->setSmdArray(7, mEmcArrays[muEEmcSmdu]);
  mEmcCollection->setSmdArray(8, mEmcArrays[muEEmcSmdv]);
}
//-----------------------------------------------------------------------
void StMuDstMaker::connectFmsCollection() {
  mFmsCollection->setFmsHitArray(mFmsArrays[muFmsHit]);
  mFmsCollection->setFmsClusterArray(mFmsArrays[muFmsCluster]);
  mFmsCollection->setFmsPointArray(mFmsArrays[muFmsPoint]);
  mFmsCollection->setFmsInfoArray(mFmsArrays[muFmsInfo]);
}
//-----------------------------------------------------------------------
void StMuDstMaker::connectPmdCollection() {
  mPmdCollection->setPmdHitArray(mPmdArrays[muPmdHit]);
  mPmdCollection->setCpvHitArray(mPmdArrays[muCpvHit]);
  mPmdCollection->setPmdClusterArray(mPmdArrays[muPmdCluster]);
  mPmdCollection->setCpvClusterArray(mPmdArrays[muCpvCluster]);
}
