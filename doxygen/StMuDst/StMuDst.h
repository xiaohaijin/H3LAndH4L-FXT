/***************************************************************************
 *
 * $Id: StMuDst.h,v 1.54 2018/02/27 04:11:57 jdb Exp $
 * Author: Frank Laue, BNL, laue@bnl.gov
 *
 ***************************************************************************/

#ifndef StMuDst_h
#define StMuDst_h

#include "TClonesArray.h"
#include "TObject.h"

class StMuDstMaker;
class StMuEvent;
class StMuPrimaryVertex;
class StMuTrack;
class StRichSpectra;
class StDetectorState;
class StL3AlgorithmInfo;
#ifndef __NO_STRANGE_MUDST__
class StStrangeEvMuDst;
class StV0MuDst;
class StXiMuDst;
class StKinkMuDst;
class StV0Mc;
class StXiMc;
class StKinkMc;
class StStrangeAssoc;
class TCut;
#endif
class StMuEmcCollection;
class StMuFmsCollection;
class StMuPmdCollection;

class StEvent;
class StTriggerData;
class StTrack;
class StTrackGeometry;
class StEmcCollection;
class StFmsCollection;

class StMtdCollection;

class StMuTofHit;
class StTofData;
// run 5 - dongx
class StTofRawData;
// dongx
class StBTofCollection;
class StMuBTofHit;
class StBTofRawHit;
class StBTofHeader;
class StMuEpdHitCollection;  // MALisa
class StMuEpdHit;            // MALisa
class EztEventHeader;
class EztTrigBlob;
class EztFpdBlob;
class EztEmcRawData;

class StDcaGeometry;
class StMuPrimaryTrackCovariance;

class StMuRpsCollection;
class StMuMtdCollection;

class StMuMtdHit;
class StMuMtdRawHit;
class StMuMtdHeader;

#include "StPhysicalHelixD.hh"

#include "COMMON/StMuArrays.h"
#include "COMMON/StMuException.hh"
#include "TObject.h"

#define ARRAY(NAME)                  \
  static TClonesArray *(NAME)##s() { \
    return tca_##NAME##s;            \
  }
#define OBJECT(TYPE, FUNC)                                            \
  static TYPE FUNC##(unsigned int i = 0) {                            \
    if (FUNC##s() && (i < (unsigned int)FUNC##s()->GetEntriesFast())) \
      return (##TYPE##)FUNC##s()->UncheckedAt(i);                     \
    return 0;                                                         \
  }

#define DO(TYPE, NAME) ARRAY(NAME) OBJECT(TYPE, NAME)

/*!
 *  @class StMuDst
 *  Top class of the 'dataformat'. This class exists only in memory and is not written/read to/from disk.
 *  However, this class is used to hold the pointers to all the TClonesArrays that have been read from disk.
 *  The class is used to navigate within a 'physics' event (to access tracks, detector info, etc).
 */
class StMuDst : public TObject {
 public:
  /// constructor
  StMuDst();
  /// set the pointers to the TClonesArrays
  static void set(StMuDstMaker *maker);
  /// set the pointers to the TClonesArrays dongx
  static void set(TClonesArray ** /* Arrays */,
#ifndef __NO_STRANGE_MUDST__
                  TClonesArray ** /* StrangeArrays */,
#endif
                  TClonesArray **mc_ptca = 0, TClonesArray **emc_ptca = 0, TClonesArray **fms_ptca = 0,
                  TClonesArray **pmd_ptca = 0, TClonesArray **tof_ptca = 0, TClonesArray **btof_ptca = 0,
                  TClonesArray **epd_col = 0,  // MALisa
                  TClonesArray **mtd_ptca = 0, TClonesArray **fgt_ptca = 0, TClonesArray **ezt_ptca = 0,
                  TClonesArray *emc_tca = 0, StMuEmcCollection *emc_col = 0, StMuFmsCollection *fms_col = 0,
                  TClonesArray *pmd_tca = 0, StMuPmdCollection *pmd_col = 0);

  /*! set pointer to current StEmcCollection */
  static void setEmcCollection(StEmcCollection *emc_coll) {
    mEmcCollection = emc_coll;
  }

  /*!  */
  static void setFmsCollection(StFmsCollection *fms_coll) {
    mFmsCollection = fms_coll;
  }

  /*! resets the pointers to the TClonesArrays to 0 */
  static void unset();

  /*!
   * checks and if necessary corrects the indecies of elements pointing to each other
   * (e.g., a primary track's index to the corresponding global track)
   */
  static void fixTrackIndices(TClonesArray *primary, TClonesArray *global);

  /*!
   * checks and if necessary corrects the indecies of elements pointing to each other (e.g., a primary track's index to
   * the corresponding global track)
   */
  void fixTrackIndices();

  /*!
   * fills gloabl track's mIndex2Global with the index to the respective primary track
   */
  static void fixTrackIndicesG(int mult = 1);

  /*!
   * creates a StEvent from the StMuDst (this) and returns a pointer to it. (This function is not yet finished)
   */
  StEvent *createStEvent();

  /*!
   * helper function to create a StTrackGeometry
   */
  static StTrackGeometry *trackGeometry(int q, StPhysicalHelixD *h);

  /*!
   * creates a StTrack from an StMuTrack and return pointer to it
   */
  static StTrack *createStTrack(const StMuTrack *);

  /*!
   * \author dongx
   */
  static void fixTofTrackIndices(TClonesArray *btofHit, TClonesArray *primary, TClonesArray *global);
  static void fixMtdTrackIndices(TClonesArray *mtdHit, TClonesArray *primary, TClonesArray *global);

  /*!  */
  void fixTofTrackIndices();
  void fixMtdTrackIndices();

  void setMtdArray(StMtdCollection *mtd_coll);

 protected:
  /*!
   * array of TClonesArrays
   */
  static TClonesArray **arrays;
#ifndef __NO_STRANGE_MUDST__
  /*!
   * array of TClonesArrays for the stuff inherited from the StStrangeMuDst
   */
  static TClonesArray **strangeArrays;
#endif
  static TClonesArray **mcArrays;

  /*!
   * array of TClonesArrays for the stuff inherited from the Emc
   */
  static TClonesArray **emcArrays;

  /*!
   * array of TClonesArrays for the stuff inherited from the Fms
   */
  static TClonesArray **fmsArrays;

  /*!
   * array of TClonesArrays for the stuff inherited from the Pmd
   */
  static TClonesArray **pmdArrays;

  /*!
   * array of TClonesArrays for the stuff inherited from the TOF
   */
  static TClonesArray **tofArrays;

  /*!
   *  array of TClonesArrays for the stuff inherited from the BTOF
   *  \author dongx
   */
  static TClonesArray **btofArrays;

  /*!
   * array of TClonesArrays for Epd
   */
  static TClonesArray **epdArrays;

  /*!
   * array of TClonesArrays for the stuff inherited from the Mtd
   */
  static TClonesArray **mtdArrays;

  /*!
   * array of TClonesArrays for the stuff inherited from the Fgt
   */
  static TClonesArray **fgtArrays;

  /*!
   * pointer to array with MuEmcCollection (for backward compatible mode)
   */
  static TClonesArray *mMuEmcCollectionArray;

  /*!
   * pointer to EmcCollection (manages the EmcArrays)
   */
  static StMuEmcCollection *mMuEmcCollection;

  /*!
   * pointer to array with MuPmdCollection (for backward compatible mode)
   */
  static TClonesArray *mMuPmdCollectionArray;

  /*!
   * pointer to FmsCollection (manages the FmsArrays)
   */
  static StMuFmsCollection *mMuFmsCollection;

  /*!
   * pointer to PmdCollection (manages the PmdArrays)
   */
  static StMuPmdCollection *mMuPmdCollection;

  /*!
   * pointer to EmcCollecion (for Emc clusterfinding etc)
   */
  static StEmcCollection *mEmcCollection;

  /*!
   * pointer to FmsCollecion (for Fms clusterfinding etc)
   */
  static StFmsCollection *mFmsCollection;

  /*!
   * array of TClonesArrays for the stuff inherited from the EZT (ezTree)
   */
  static TClonesArray **eztArrays;

  /*!
   * Index number of current primary vertex
   */
  static Int_t mCurrVertexId;

  /*!
   * Temporary array to collect tracks from currect primary vertex
   */
  static TObjArray *mCurrPrimaryTracks;

  /*!
   * Helper function to collect tracks for the current prim vertex
   */
  static void collectVertexTracks();

 public:
  /*!
   * \brief setVertexIndex.
   *  Set the index number of the current primary vertex (used by both primaryTracks() funtions and for
   * StMuEvent::refMult() \param vtx_id
   */
  static void setVertexIndex(Int_t vtx_id);

  /*!
   * Get the index number of the current primary vertex
   */
  static Int_t currentVertexIndex() {
    return mCurrVertexId;
  }

  /*!
   * returns pointer to the n-th TClonesArray
   */
  static TClonesArray *array(int type) {
    return arrays[type];
  }
#ifndef __NO_STRANGE_MUDST__
  /*!
   * returns pointer to the n-th TClonesArray from the strangeness arrays
   */
  static TClonesArray *strangeArray(int type) {
    return strangeArrays[type];
  }
#endif

  /*!
   * \brief mcArray
   * \param type
   * \return
   */
  static TClonesArray *mcArray(int type) {
    return mcArrays[type];
  }

  /*!
   * returns pointer to the n-th TClonesArray from the emc arrays
   */
  static TClonesArray *emcArray(int type) {
    return emcArrays[type];
  }

  /*!
   * returns pointer to the n-th TClonesArray from the fms arrays
   */
  static TClonesArray *fmsArray(int type) {
    return fmsArrays[type];
  }

  /*!
   * returns pointer to the n-th TClonesArray from the pmd arrays
   */
  static TClonesArray *pmdArray(int type) {
    return pmdArrays[type];
  }

  /*!
   * returns pointer to the n-th TClonesArray from the tof arrays
   */
  static TClonesArray *tofArray(int type) {
    return tofArrays[type];
  }

  /*!
   * returns pointer to the n-th TClonesArray from the btof arrays
   * \author dongx
   */
  static TClonesArray *btofArray(int type) {
    return btofArrays[type];
  }

  /*!
   * returns pointer to the n-th TClonesArray from the mtd arrays
   */
  static TClonesArray *mtdArray(int type) {
    return mtdArrays[type];
  }

  /*!
   * returns pointer to the n-th TClonesArray from the fgt arrays
   */
  static TClonesArray *fgtArray(int type) {
    return fgtArrays[type];
  }

  /*!
   * returns pointer to the n-th TClonesArray from the ezt arrays
   */
  static TClonesArray *eztArray(int type) {
    return eztArrays[type];
  }

  /*!
   * returns pointer to the EpdHitCollection
   */
  static TClonesArray *epdHits() {
    return epdArrays[muEpdHit];
  }

  /*!
   * returns pointer to the primary vertex list
   */
  static TClonesArray *primaryVertices() {
    return arrays[muPrimaryVertex];
  }

  /*!
   * returns pointer to a list of tracks belonging to the selected primary vertex
   */
  static TObjArray *primaryTracks() {
    return mCurrPrimaryTracks;
  }

  /*!
   * returns pointer to the global tracks list
   */
  static TObjArray *globalTracks() {
    return arrays[muGlobal];
  }

  /*!
   * returns pointer to the other tracks list (all tracks that are not flagged as primary of global)
   */
  static TClonesArray *otherTracks() {
    return arrays[muOther];
  }

  /*!
   * returns pointer to the l3Tracks list
   */
  static TClonesArray *l3Tracks() {
    return arrays[muL3];
  }

  /*!
   * returns pointer to the list of rich spectra
   */
  static TClonesArray *richSpectra() {
    return arrays[muRich];
  }

  /*!
   * returns pointer to the list of detector states
   */
  static TClonesArray *detectorStates() {
    return arrays[muState];
  }

  /*!
   * returns pointer to list of accepted l3 algorithms
   */
  static TClonesArray *l3AlgoAccept() {
    return arrays[muAccept];
  }

  /*!
   * returns pointer to list rejected l3 algorithms
   */
  static TClonesArray *l3AlgoReject() {
    return arrays[muReject];
  }

  /*!
   * \brief covGlobTrack
   * \return
   */
  static TClonesArray *covGlobTrack() {
    return arrays[muCovGlobTrack];
  }

  /*!
   * \brief covPrimTrack
   * \return
   */
  static TClonesArray *covPrimTrack() {
    return arrays[muCovPrimTrack];
  }

  /*!
   * returns pointer to current StMuEvent (class holding the event wise information, e.g. event number, run number)
   */
  static StMuEvent *event() {
    return (StMuEvent *)arrays[muEvent]->UncheckedAt(0);
  }

  /*!
   * return pointer to current primary vertex
   */
  static StMuPrimaryVertex *primaryVertex() {
    return (StMuPrimaryVertex *)arrays[muPrimaryVertex]->UncheckedAt(mCurrVertexId);
  }

  /*!
   * return pointer to i-th primary vertex
   */
  static StMuPrimaryVertex *primaryVertex(int i) {
    return (StMuPrimaryVertex *)arrays[muPrimaryVertex]->UncheckedAt(i);
  }

  /*!
   * return pointer to i-th primary track
   */
  static StMuTrack *primaryTracks(int i) {
    return (StMuTrack *)mCurrPrimaryTracks->UncheckedAt(i);
  }

  /*!
   * return pointer to i-th global track
   */
  static StMuTrack *globalTracks(int i) {
    return (StMuTrack *)arrays[muGlobal]->UncheckedAt(i);
  }

  /*!
   * return pointer to i-th other track  (track that is not flagged as primary of global)
   */
  static StMuTrack *otherTracks(int i) {
    return (StMuTrack *)arrays[muOther]->UncheckedAt(i);
  }

  /*!
   * return pointer to i-th l3 track
   */
  static StMuTrack *l3Tracks(int i) {
    return (StMuTrack *)arrays[muL3]->UncheckedAt(i);
  }

  /*!
   * returns pointer to i-th StRichSpectra
   */
  static StRichSpectra *richSpectra(int i) {
    return (StRichSpectra *)arrays[muRich]->UncheckedAt(i);
  }

  /*!
   * returns pointer to i-th StDetectorState
   */
  static StDetectorState *detectorStates(int i) {
    return (StDetectorState *)arrays[muState]->UncheckedAt(i);
  }

  /*!
   * returns pointer to i-th accepted StL3AlgorithmInfo
   */
  static StL3AlgorithmInfo *l3AlgoAccept(int i) {
    return (StL3AlgorithmInfo *)arrays[muAccept]->UncheckedAt(i);
  }

  /*!
   * returns pointer to i-th rejected StL3AlgorithmInfo
   */
  static StL3AlgorithmInfo *l3AlgoReject(int i) {
    return (StL3AlgorithmInfo *)arrays[muReject]->UncheckedAt(i);
  }

  /*!
   * returns pp2pp infomation
   */
  static StMuRpsCollection *RpsCollection() {
    return (StMuRpsCollection *)arrays[mupp2pp]->UncheckedAt(0);
  }

  /*!
   * \brief MtdCollection
   * \return
   */
  static StMuMtdCollection *MtdCollection() {
    return (StMuMtdCollection *)arrays[muMtd]->UncheckedAt(0);
  }

  /*!
   * \brief covGlobTracks
   * \param i
   * \return
   */
  static StDcaGeometry *covGlobTracks(int i) {
    return (StDcaGeometry *)arrays[muCovGlobTrack]->UncheckedAt(i);
  }

  /*!
   * \brief covPrimTracks
   * \param i
   * \return
   */
  static StMuPrimaryTrackCovariance *covPrimTracks(int i) {
    return (StMuPrimaryTrackCovariance *)arrays[muCovPrimTrack]->UncheckedAt(i);
  }

#ifndef __NO_STRANGE_MUDST__
  /*!
   * returns pointer to current StStrangeEvMuDst (class holding the event wise information, e.g. event number, run
   * number)
   */
  static StStrangeEvMuDst *strangeEvent() {
    return (StStrangeEvMuDst *)strangeArrays[smuEv]->UncheckedAt(0);
  }

  /*!
   * returns pointer to MC version of current StStrangeEvMuDst
   */
  static StStrangeEvMuDst *strangeEventMc() {
    return (StStrangeEvMuDst *)strangeArrays[smuEvMc]->UncheckedAt(0);
  }

  /*!
   * returns pointer to the v0 list
   */
  static TClonesArray *v0s() {
    return strangeArrays[smuV0];
  }

  /*!
   * returns pointer to the mc v0 list
   */
  static TClonesArray *v0sMc() {
    return strangeArrays[smuV0Mc];
  }

  /*!
   * returns pointer to the v0 association list
   */
  static TClonesArray *v0Assoc() {
    return strangeArrays[smuV0Assoc];
  }

  /*!
   * returns pointer to the xi list
   */
  static TClonesArray *xis() {
    return strangeArrays[smuXi];
  }

  /*!
   * returns pointer to the mc xi list
   */
  static TClonesArray *xisMc() {
    return strangeArrays[smuXiMc];
  }

  /*!
   * returns pointer to the xi association list
   */
  static TClonesArray *xiAssoc() {
    return strangeArrays[smuXiAssoc];
  }

  /*!
   * returns pointer to the kink list
   */
  static TClonesArray *kinks() {
    return strangeArrays[smuKink];
  }

  /*!
   * returns pointer to the mc kink list
   */
  static TClonesArray *kinksMc() {
    return strangeArrays[smuKinkMc];
  }

  /*!
   * returns pointer to the kink association list
   */
  static TClonesArray *kinkAssoc() {
    return strangeArrays[smuKinkAssoc];
  }

  /*!
   * returns pointer to the list of strangeCuts
   */
  static TClonesArray *strangeCuts() {
    return strangeArrays[smuCut];
  }

  /*!
   * returns pointer to the i-th v0
   */
  static StV0MuDst *v0s(int i) {
    return (StV0MuDst *)strangeArrays[smuV0]->UncheckedAt(i);
  }

  /*!
   * \brief v0sMc
   * \param i
   * \return
   */
  static StV0Mc *v0sMc(int i) {
    return (StV0Mc *)strangeArrays[smuV0Mc]->UncheckedAt(i);
  }

  /*!
   * \brief v0Assoc
   * \param i
   * \return
   */
  static StStrangeAssoc *v0Assoc(int i) {
    return (StStrangeAssoc *)strangeArrays[smuV0Assoc]->UncheckedAt(i);
  }

  /*!
   * returns pointer to the i-th xi
   */
  static StXiMuDst *xis(int i) {
    return (StXiMuDst *)(void *)strangeArrays[smuXi]->UncheckedAt(i);
  }

  /*!
   * \brief xisMc
   * \param i
   * \return
   */
  static StXiMc *xisMc(int i) {
    return (StXiMc *)strangeArrays[smuXiMc]->UncheckedAt(i);
  }

  /*!
   * \brief xiAssoc
   * \param i
   * \return
   */
  static StStrangeAssoc *xiAssoc(int i) {
    return (StStrangeAssoc *)strangeArrays[smuXiAssoc]->UncheckedAt(i);
  }

  /*!
   * returns pointer to the i-th kink
   */
  static StKinkMuDst *kinks(int i) {
    return (StKinkMuDst *)(void *)strangeArrays[smuKink]->UncheckedAt(i);
  }

  /*!
   * \brief kinksMc
   * \param i
   * \return
   */
  static StKinkMc *kinksMc(int i) {
    return (StKinkMc *)strangeArrays[smuKinkMc]->UncheckedAt(i);
  }

  /*!
   * \brief kinkAssoc
   * \param i
   * \return
   */
  static StStrangeAssoc *kinkAssoc(int i) {
    return (StStrangeAssoc *)strangeArrays[smuKinkAssoc]->UncheckedAt(i);
  }

  /*!
   * returns pointer to the i-th stranneCut (of type TCut)
   */
  static TCut *strangeCuts(int i) {
    return (TCut *)strangeArrays[smuCut]->UncheckedAt(i);
  }
#endif
  /*!
   * returns pointer to current StMuEmcCollection
   */
  static StMuEmcCollection *muEmcCollection() {
    if (mMuEmcCollectionArray)
      return (StMuEmcCollection *)mMuEmcCollectionArray->UncheckedAt(0);
    else
      return mMuEmcCollection;
  }

  /*!
   * returns pointer to current StMuFmsCollection
   */
  static StMuFmsCollection *muFmsCollection() {
    return mMuFmsCollection;
  }

  /*!
   * returns pointer to current StMuPmdCollection
   */
  static StMuPmdCollection *pmdCollection() {
    if (mMuPmdCollectionArray)
      return (StMuPmdCollection *)mMuPmdCollectionArray->UncheckedAt(0);
    else
      return mMuPmdCollection;
  }

  /*!
   * returns pointer to current StEmcCollection
   */
  static StEmcCollection *emcCollection() {
    return mEmcCollection;
  }

  /*!
   * returns pointer to current StFmsCollection
   */
  static StFmsCollection *fmsCollection() {
    return mFmsCollection;
  }

  /*!
   * returns pointer to the i-th muTofHit
   */
  static StMuTofHit *tofHit(int i) {
    return (StMuTofHit *)tofArrays[muTofHit]->UncheckedAt(i);
  }

  /*!
   * returns pointer to the i-th tofData
   */
  static StTofData *tofData(int i) {
    return (StTofData *)tofArrays[muTofData]->UncheckedAt(i);
  }

  /*!
   * returns pointer to the i-th tofRawData
   */
  static StTofRawData *tofRawData(int i) {
    return (StTofRawData *)tofArrays[muTofRawData]->UncheckedAt(i);
  }

  /*!
   * returns pointer to the i-th muBTofHit
   */
  static StMuBTofHit *btofHit(int i) {
    return (StMuBTofHit *)btofArrays[muBTofHit]->UncheckedAt(i);
  }

  /*!
   * returns pointer to the i-th btofRawHit
   * \author dongx
   */
  static StBTofRawHit *btofRawHit(int i) {
    return (StBTofRawHit *)btofArrays[muBTofRawHit]->UncheckedAt(i);
  }

  /*!
   * returns pointer to the btofHeader
   * \author dongx
   */
  static StBTofHeader *btofHeader() {
    return (StBTofHeader *)btofArrays[muBTofHeader]->UncheckedAt(0);
  }

  /*!
   * \brief epdHit
   * \param i
   * \return
   */
  static StMuEpdHit *epdHit(int i) {
    return (StMuEpdHit *)epdArrays[muEpdHit]->UncheckedAt(i);
  }

  /*!
   * \brief mtdHit
   * \param i
   * \return
   */
  static StMuMtdHit *mtdHit(int i) {
    return (StMuMtdHit *)mtdArrays[muMTDHit]->UncheckedAt(i);
  }

  /*!
   * \brief mtdRawHit
   * \param i
   * \return
   */
  static StMuMtdRawHit *mtdRawHit(int i) {
    return (StMuMtdRawHit *)mtdArrays[muMTDRawHit]->UncheckedAt(i);
  }

  /*!
   * \brief mtdHeader
   * \return
   */
  static StMuMtdHeader *mtdHeader() {
    return (StMuMtdHeader *)mtdArrays[muMTDHeader]->UncheckedAt(0);
  }

  /*!
   * returns pointer to eztHeader
   */
  static EztEventHeader *eztHeader() {
    return (EztEventHeader *)eztArrays[muEztHead]->UncheckedAt(0);
  }

  /*!
   * returns pointer to eztTrig
   */
  static EztTrigBlob *eztTrig() {
    return (EztTrigBlob *)eztArrays[muEztTrig]->UncheckedAt(0);
  }

  /*!
   * returns pointer to eztFpd
   */
  static EztFpdBlob *eztFpd() {
    return (EztFpdBlob *)eztArrays[muEztFpd]->UncheckedAt(0);
  }

  /*!
   * returns pointer to ETOW
   */
  static EztEmcRawData *eztETow() {
    return (EztEmcRawData *)eztArrays[muEztETow]->UncheckedAt(0);
  }

  /*!
   * returns pointer to eztESmd +pre/post
   */
  static EztEmcRawData *eztESmd() {
    return (EztEmcRawData *)eztArrays[muEztESmd]->UncheckedAt(0);
  }

  /*!
   * \brief numberOfPrimaryVertices
   * \return
   */
  static unsigned int numberOfPrimaryVertices() {
    return arrays[muPrimaryVertex]->GetEntriesFast();
  }

  /*!
   * \brief numberOfPrimaryTracks
   * \return
   */
  static unsigned int numberOfPrimaryTracks() {
    return mCurrPrimaryTracks->GetEntriesFast();
  }

  /*!
   * \brief numberOfGlobalTracks
   * \return
   */
  static unsigned int numberOfGlobalTracks() {
    return arrays[muGlobal]->GetEntriesFast();
  }

  /*!
   * \brief numberOfOtherTracks
   * \return
   */
  static unsigned int numberOfOtherTracks() {
    return arrays[muOther]->GetEntriesFast();
  }

  /*!
   * \brief numberOfL3Tracks
   * \return
   */
  static unsigned int numberOfL3Tracks() {
    return arrays[muL3]->GetEntriesFast();
  }

  /*!
   * \brief numberOfRichSpectras
   * \return
   */
  static unsigned int numberOfRichSpectras() {
    return arrays[muRich]->GetEntriesFast();
  }

  /*!
   * \brief numberOfDetectorStates
   * \return
   */
  static unsigned int numberOfDetectorStates() {
    return arrays[muState]->GetEntriesFast();
  }

  /*!
   * \brief numberOfL3AlgoAccepts
   * \return
   */
  static unsigned int numberOfL3AlgoAccepts() {
    return arrays[muAccept]->GetEntriesFast();
  }

  /*!
   * \brief numberOfL3AlgoRejects
   * \return
   */
  static unsigned int numberOfL3AlgoRejects() {
    return arrays[muReject]->GetEntriesFast();
  }

  /*!
   * \brief numberOfCovGlobTracks
   * \return
   */
  static unsigned int numberOfCovGlobTracks() {
    return arrays[muCovGlobTrack]->GetEntriesFast();
  }

  /*!
   * \brief numberOfCovPrimTracks
   * \return
   */
  static unsigned int numberOfCovPrimTracks() {
    return arrays[muCovPrimTrack]->GetEntriesFast();
  }
#ifndef __NO_STRANGE_MUDST__
  /*!
   * \brief numberOfV0s
   * \return
   */
  static unsigned int numberOfV0s() {
    return strangeArrays[smuV0]->GetEntriesFast();
  }

  /*!
   * \brief numberOfV0sMc
   * \return
   */
  static unsigned int numberOfV0sMc() {
    return strangeArrays[smuV0Mc]->GetEntriesFast();
  }

  /*!
   * \brief numberOfV0Assoc
   * \return
   */
  static unsigned int numberOfV0Assoc() {
    return strangeArrays[smuV0Assoc]->GetEntriesFast();
  }

  /*!
   * \brief numberOfXis
   * \return
   */
  static unsigned int numberOfXis() {
    return strangeArrays[smuXi]->GetEntriesFast();
  }

  /*!
   * \brief numberOfXisMc
   * \return
   */
  static unsigned int numberOfXisMc() {
    return strangeArrays[smuXiMc]->GetEntriesFast();
  }

  /*!
   * \brief numberOfXiAssoc
   * \return
   */
  static unsigned int numberOfXiAssoc() {
    return strangeArrays[smuXiAssoc]->GetEntriesFast();
  }

  /*!
   * \brief numberOfKinks
   * \return
   */
  static unsigned int numberOfKinks() {
    return strangeArrays[smuKink]->GetEntriesFast();
  }

  /*!
   * \brief numberOfKinksMc
   * \return
   */
  static unsigned int numberOfKinksMc() {
    return strangeArrays[smuKinkMc]->GetEntriesFast();
  }

  /*!
   * \brief numberOfKinkAssoc
   * \return
   */
  static unsigned int numberOfKinkAssoc() {
    return strangeArrays[smuKinkAssoc]->GetEntriesFast();
  }

  /*!
   * \brief numberOfStrangeCuts
   * \return
   */
  static unsigned int numberOfStrangeCuts() {
    return strangeArrays[smuCut]->GetEntriesFast();
  }
#endif
  /*!
   * tofr
   */
  static unsigned int numberOfTofHit() {
    return tofArrays[muTofHit]->GetEntriesFast();
  }

  /*!
   * \brief numberOfTofData
   * \return
   */
  static unsigned int numberOfTofData() {
    return tofArrays[muTofData]->GetEntriesFast();
  }

  /*!
   * \brief numberOfTofRawData
   * \return
   */
  static unsigned int numberOfTofRawData() {
    return tofArrays[muTofRawData]->GetEntriesFast();
  }

  /*!
   * \brief numberOfBTofHit
   * \author dongx
   * \return
   */
  static unsigned int numberOfBTofHit() {
    return btofArrays[muBTofHit]->GetEntriesFast();
  }

  /*!
   * \brief numberOfBTofRawHit
   * \return
   */
  static unsigned int numberOfBTofRawHit() {
    return btofArrays[muBTofRawHit]->GetEntriesFast();
  }

  /*!
   * \brief numberOfEpdHit
   * \return
   */
  static unsigned int numberOfEpdHit() {
    return epdArrays[muEpdHit]->GetEntriesFast();
  }

  /*!
   * \brief numberOfMTDHit
   * \return
   */
  static unsigned int numberOfMTDHit() {
    return mtdArrays[muMTDHit]->GetEntriesFast();
  }

  /*!
   * \brief numberOfBMTDRawHit
   * \return
   */
  static unsigned int numberOfBMTDRawHit() {
    return mtdArrays[muMTDRawHit]->GetEntriesFast();
  }

  /*!
   * \brief GetNPrimaryVertex
   * \return
   */
  static unsigned int GetNPrimaryVertex() {
    return numberOfPrimaryVertices();
  }

  /*!
   * \brief GetNPrimaryTrack
   * \return
   */
  static unsigned int GetNPrimaryTrack() {
    return numberOfPrimaryTracks();
  }

  /*!
   * \brief GetNGlobalTrack
   * \return
   */
  static unsigned int GetNGlobalTrack() {
    return numberOfGlobalTracks();
  }

  /*!
   * \brief GetNOtherTrack
   * \return
   */
  static unsigned int GetNOtherTrack() {
    return numberOfOtherTracks();
  }

  /*!
   * \brief GetNL3Track
   * \return
   */
  static unsigned int GetNL3Track() {
    return numberOfL3Tracks();
  }
  static unsigned int GetNRichSpectra() {
    return numberOfRichSpectras();
  }
  static unsigned int GetNDetectorState() {
    return numberOfDetectorStates();
  }
  static unsigned int GetNL3AlgoAccept() {
    return numberOfL3AlgoAccepts();
  }
  static unsigned int GetNL3AlgoReject() {
    return numberOfL3AlgoRejects();
  }
#ifndef __NO_STRANGE_MUDST__
  static unsigned int GetNV0() {
    return numberOfV0s();
  }
  static unsigned int GetNV0Mc() {
    return numberOfV0sMc();
  }
  static unsigned int GetNV0Assoc() {
    return numberOfV0Assoc();
  }
  static unsigned int GetNXi() {
    return numberOfXis();
  }
  static unsigned int GetNXiMc() {
    return numberOfXisMc();
  }
  static unsigned int GetNXiAssoc() {
    return numberOfXiAssoc();
  }
  static unsigned int GetNKink() {
    return numberOfKinks();
  }
  static unsigned int GetNKinkMc() {
    return numberOfKinksMc();
  }
  static unsigned int GetNKinkAssoc() {
    return numberOfKinkAssoc();
  }
  static unsigned int GetNStrangeCut() {
    return numberOfStrangeCuts();
  }
#endif
  static unsigned int GetNTofHit() {
    return numberOfTofHit();
  }
  static unsigned int GetNTofData() {
    return numberOfTofData();
  }
  // run 5 - dongx
  static unsigned int GetNTofRawData() {
    return numberOfTofRawData();
  }
  // dongx
  static unsigned int GetNBTofHit() {
    return numberOfBTofHit();
  }
  static unsigned int GetNBTofRawHit() {
    return numberOfBTofRawHit();
  }

  static unsigned int GetNEpdHit() {
    return numberOfEpdHit();
  }

  static unsigned int GetNMTDHit() {
    return numberOfMTDHit();
  }
  static unsigned int GetNMTDRawHit() {
    return numberOfBMTDRawHit();
  }

  virtual void Print(Option_t *option = "") const;  ///< Print basic event info
  static void printPrimaryTracks();
  static void printGlobalTracks();
  static void printVertices();

  friend class StMuDstMaker;
  friend class StMuIOMaker;

  // Increment this by 1 every time the class structure is changed
  ClassDef(StMuDst, 4)
};

#endif
