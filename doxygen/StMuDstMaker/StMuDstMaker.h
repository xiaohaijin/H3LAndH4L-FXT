/***************************************************************************
 *
 * $Id: StMuDstMaker.h,v 1.62 2018/02/27 04:15:02 jdb Exp $
 * Author: Frank Laue, BNL, laue@bnl.gov
 ***************************************************************************/
#ifndef StMuDstMaker_hh
#define StMuDstMaker_hh

#include <string>

#ifndef ST_NO_NAMESPACES
using namespace std;
#endif

#include "StMaker.h"
#include "StChain.h"
#include "StIOInterFace.h"
#include "St_DataSetIter.h"

#include "StMuArrays.h"
#include "StMuFilter.h"
#include "StMuL3Filter.h"

///////////////////////////////////////////
///  was added by xiaohai(begin)
//////////////////////////////////////////
#include "COMMON/StMuArrays.h"
#include "COMMON/StMuFilter.h"
#include "COMMON/StMuL3Filter.h"
//////////////////////////////////////////
///  was added by xiaohai(end)
//////////////////////////////////////////

class StMuEvent;
class StMuDst;
class StMuCut;

class StEvent;
class StTrackNode;
class StTrack;
class StVertex;
class StRichSpectra;
class StDetectorState;
class StL3AlgorithmInfo;

class StuProbabilityPidAlgorithm;

class StIOMaker;
class StTreeMaker;
#ifndef __NO_STRANGE_MUDST__
/// strangeness group stuff
class StStrangeEvMuDst;
class StStrangeMuDstMaker;
class StV0MuDst;
class StV0Mc;
class StXiMuDst;
class StXiMc;
class StKinkMuDst;
class StKinkMc;
class StStrangeAssoc;
class StStrangeCuts;
#endif
/// emc stuff
#include "COMMON/StMuEmcCollection.h"
class StMuEmcUtil;

/// fms stuff
#include "COMMON/StMuFmsCollection.h"
class StMuFmsUtil;

#include "COMMON/StMuEpdHitCollection.h" // MALisa
class StMuEpdUtil;
/// PMD by Supriya Das
#include "COMMON/StMuPmdCollection.h"
class StMuPmdUtil;

// tof stuff
class StMuTofHit;
class StMuTofHitCollection;
class StTofCollection;
class StTofDataCollection;
// run 5 - dongx
class StTofRawDataCollection;
class StMuTofUtil;
// new btof
class StBTofCollection;
class StBTofRawHitCollection;
class StMuBTofHit;
class StMuBTofHitCollection;
class StMuBTofUtil;

// ezTree stuff
class StMuEzTree;

class TFile;
class TTree;
class TChain;
class TClonesArray;
class TEventList;

class StMuRpsCollection;
class StMuMtdCollection;

class StMuMtdHit;
class StMuMtdRawHit;
class StMuMtdHeader;

/*!
 * \brief The StMuDstMaker Class to create and read STAR's common micro dst
 *
 * This class is a true maker in the STAR sense. It inherits from "StMaker" and implements
 * the functions "int Init()", "void Clear()", "int Make()", and "int Finish()" in order to
 * run as part of an "StChain". Please refer to the STAR Computing Web pages in case you do
 * not know what "StMaker" and "StChain" mean.
 */
class StMuDstMaker : public StIOInterFace {
public:
  /// Default constructor
  StMuDstMaker(const char *name = "MuDst");
  /// Constructor
  StMuDstMaker(int mode, int nameMode, const char *dirName = "./", const char *fileName = "", const char *filter = ".",
               int maxfiles = 10, const char *name = "MuDst");

  ~StMuDstMaker();

  virtual int Init();
  virtual void Clear(Option_t *option = "");
  virtual int Make();

  virtual Int_t MakeRead(const StUKey &RunEvent);
  virtual Int_t MakeRead();
  virtual Int_t MakeWrite();

  virtual int Finish();

  void printArrays();
  void SetStatus(const char *arrType, int status);

  /*!
   * \brief SetEventList
   *
   * Set event list for reading only preselected events (generate list using chain->Draw())
   *
   * \param e
   */
  void SetEventList(TEventList *e) { mEventList = e; }

  /*!
   * \brief setTrackFilter
   *
   * Set the track filter used for all tracks (except the L3 tracks) when creating muDsts from StEvent and writting to
   * disk
   *
   * \param c
   */
  void setTrackFilter(StMuCut *c);
  StMuFilter *trackFilter();

  /*!
   * \brief setL3TrackFilter
   *
   * Set the track filter used for L3 tracks when creating muDsts from StEvent and write to disk.
   *
   * \param c
   */
  void setL3TrackFilter(StMuCut *c);

  StMuL3Filter *l3TrackFilter();

  /*!
   * \brief setProbabilityPidFill Set the file from where the PID probability tables should be read.
   *
   * Set the file from where the PID probability tables should be read.
   * These tables might change from production version to production version.
   * It is the reposibility of who ever creates muDsts to make sure the right tables
   * are used. So far, Aihong was providing these files. Thanks, Aihong.
   * Note that this method hard code the file name if the argument is not give.
   * The default is currently
   * getenv("STAR") << "/StarDb/dEdxModel/" << PIDtable
   * where PIDtable=PIDTableP01gl.root . This will later change with possibly a database approach.
   *
   * \param file
   */
  void setProbabilityPidFile(const char *file = NULL);

  /*!
   * \brief muDst
   *
   * Returns pointer to the StMuDst object, the transient class that holds all the TCloneArrays and has access
   * functions to the tracks, v0s, etc.
   *
   * \return null pointer if no StMuDst available.
   */
  StMuDst *muDst();

  /*!
   * \brief chain In read mode, returns pointer to the chain of .MuDst.root files that where selected.
   * \return
   */
  TChain *chain();

  /*!
   * \brief tree Returns pointer to the current TTree, the top level io structure that holds the event, track, v0,
   * ect.
   * \return
   */
  TTree *tree();

  /*!
   * \brief GetFile returns name of current input or output file, depending on mode(GetFileName does the same, see
   * StIOInterface)
   * \return
   */
  virtual const char *GetFile() const;

  /*!
   * \brief setSplit Sets the split level for the file and all branches, Please refer to the ROOT manual
   */
  void setSplit(int = 99);

  /*!
   * \brief setBufferSize Sets the buffer size for all branches.
   */
  void setBufferSize(int = 65536 * 4);

  /*!
   * \brief setCompression Sets the compression level for the file and all branches. 0 means no compression, 9 is the
   * higher compresion level
   * \param comp
   */
  void setCompression(int comp = 9);

  /*!
   * \brief setStTriggerYear Set for StTriggerData, Default is 2009
   */
  void setStTriggerYear(int);

  /*!
   * \brief muEmcUtil
   * \return pointer to StMuEmcUtil
   */
  StMuEmcUtil *muEmcUtil() { return mEmcUtil; }

  /*!
   * \brief muFmsUtil
   * \return  pointer to StMuFmsUtiln
   */
  StMuFmsUtil *muFmsUtil() { return mFmsUtil; }

  /*!
   * \brief muPmdUtil
   * \return pointer to StMuPmdUtil
   */
  StMuPmdUtil *muPmdUtil() { return mPmdUtil; }

  /*!
   * \brief GetCVS
   * \return version tag
   */
  virtual const char *GetCVS() const {

    static const char cvs[] =
        "Tag $Name: SL18c $ $Id: StMuDstMaker.h,v 1.62 2018/02/27 04:15:02 jdb Exp $ built " __DATE__ " " __TIME__;
    return cvs;
  }

protected:
  /*!
   * routine to set up connection between mEmcCollection and Emc arrays
   */
  void connectEmcCollection();
  void connectFmsCollection();
  void connectPmdCollection();
  enum ioMode {
    ioRead,
    ioWrite
  };

  /*!
   * Specifies the way the output file name is contructed when creating muDsts.
   * ioFix = use filename specified in when calling the constructor, right in the
   * same output file for all input files.
   * ioIOMaker = create one output file per input file, derive output filename
   * from current input file of the StIOMaker.
   * ioTreeMaker = create one output file per input file, derive output filename
   * from current input file of the StTreeMaker.
   */
  enum ioNameMode {
    ioFix = 0,
    ioIOMaker,
    ioTreeMaker
  };

  StEvent *mStEvent;
  StMuDst *mStMuDst;
#ifndef __NO_STRANGE_MUDST__
  StStrangeMuDstMaker *mStStrangeMuDstMaker;
#endif
  StIOMaker *mIOMaker;
  StTreeMaker *mTreeMaker;
  StMuEmcUtil *mEmcUtil;
  StMuFmsUtil *mFmsUtil;
  StMuPmdUtil *mPmdUtil;
  StMuTofUtil *mTofUtil;
  /// dongx
  StMuBTofUtil *mBTofUtil;
  StMuEpdUtil *mEpdUtil; // MALisa
  StMuEzTree *mEzTree;

  int mIoMode;
  int mIoNameMode;
  string mDirName;
  string mFileName;
  string mFilter;
  int mMaxFiles;

  TEventList *mEventList;

  unsigned int mTrackType;
  bool mReadTracks;
  bool mReadV0s;
  bool mReadXis;
  bool mReadKinks;
  bool mFinish;

  StMuCut *mTrackFilter;
  StMuCut *mL3TrackFilter;

  TFile *mCurrentFile;
  string mCurrentFileName;

  TChain *mChain;
  TTree *mTTree;

  int mEventCounter;
  int mSplit;
  int mCompression;
  int mBufferSize;

  TObjArray mVtxList; /*!< List of pointers to vertices to keep track of track-vertex assignment when copying from
                         StEvent */

  StuProbabilityPidAlgorithm *mProbabilityPidAlgorithm;

#define saveDelete(t)                                                                                                  \
  {                                                                                                                    \
    delete t;                                                                                                          \
    t = 0;                                                                                                             \
  }

  //! protected:
  virtual void openWrite(string fileName);
  virtual void write();
  virtual void closeWrite();

  void streamerOff();

  virtual int openRead();
  virtual void read();
  void setBranchAddresses();
  virtual void closeRead();

  void setBranchAddresses(TChain *);

  void assignArrays();
  void clearArrays();
  void zeroArrays();
  void createArrays();
  TClonesArray *clonesArray(TClonesArray *&p, const char *type, int size, int &counter);

  void fill();
  void fillTrees(StEvent *ev, StMuCut *cut = 0);
  void fillEvent(StEvent *ev, StMuCut *cut = 0);
  void fillVertices(StEvent *ev);
  void fillpp2pp(StEvent *ev);

  void fillsttrigger(StEvent *ev);
  void fillPmd(StEvent *ev);
  void fillEpd(StEvent *ev); // MALisa
  void fillEmc(StEvent *ev);
  void fillFms(StEvent *ev);
#ifndef __NO_STRANGE_MUDST__
  void fillStrange(StStrangeMuDstMaker *);
#endif
  void fillMC();
  void fillL3Tracks(StEvent *ev, StMuCut *cut = 0);
  void fillTracks(StEvent *ev, StMuCut *cut = 0);
  void fillDetectorStates(StEvent *ev);
  void fillL3AlgorithmInfo(StEvent *ev);
  void fillTof(StEvent *ev);
  /// dongx
  void fillBTof(StEvent *ev);
  void fillMtd(StEvent *ev);
  void fillFgt(StEvent *ev);

  void fillEzt(StEvent *ev);

  void fillHddr();

  template <class T, class U> int addType(TClonesArray *tcaTo, U &u, T *t);
  template <class T> int addType(TClonesArray *tcaFrom, TClonesArray *&tcaTo, T *t);
  template <class T> int addType(TClonesArray *tcaTo, T &t);
  void addTrackNode(const StEvent *ev, const StTrackNode *node, StMuCut *cut, TClonesArray *gTCA = 0,
                    TClonesArray *pTCA = 0, TClonesArray *oTCA = 0, TClonesArray *covgTCA = 0,
                    TClonesArray *covpTCA = 0, bool l3 = false);
  int addTrack(TClonesArray *tca, const StEvent *event, const StTrack *track, const StVertex *vtx, StMuCut *cut,
               int index2Global, bool l3 = false, TClonesArray *covgTCA = 0, TClonesArray *covpTCA = 0);
  /*   int addType(TClonesArray* tcaTo , StMuEmcCollection t); */

  StRichSpectra *richSpectra(const StTrack *track);

  void setStEvent(StEvent *);
  StEvent *stEvent();
#ifndef __NO_STRANGE_MUDST__
  void setStStrangeMuDstMaker(StStrangeMuDstMaker *);
  StStrangeMuDstMaker *stStrangeMuDstMaker();
#endif
  unsigned int trackType();
  bool readTracks();
#ifndef __NO_STRANGE_MUDST__
  bool readV0s();
  bool readXis();
  bool readKinks();
#endif
  void setTrackType(unsigned int);
  void setReadTracks(bool);
#ifndef __NO_STRANGE_MUDST__
  void setReadV0s(bool);
  void setReadXis(bool);
  void setReadKinks(bool);
#endif
  string basename(string);
  string dirname(string);
  string buildFileName(string dir, string fileName, string extention);

  friend class StMuDst;
  friend class StMuDstFilterMaker;

  // Beware that this was added to counteract contiguous memory.
  // See implementation for caveat
  TClonesArray *mAArrays[__NALLARRAYS__];
  TClonesArray **mArrays; //[__NARRAYS__       ];
#ifndef __NO_STRANGE_MUDST__
  TClonesArray **mStrangeArrays; //[__NSTRANGEARRAYS__];
#endif
  TClonesArray **mMCArrays;  //[__NMCARRAYS__];
  TClonesArray **mEmcArrays; //[__NEMCARRAYS__    ];
  TClonesArray **mFmsArrays; //[__NFMSARRAYS__    ];
  TClonesArray **mPmdArrays; //[__NPMDARRAYS__    ];
  TClonesArray **mTofArrays; //[__NTOFARRAYS__    ];
  /// dongx
  TClonesArray **mBTofArrays; //[__NBTOFARRAYS__   ];
  TClonesArray **mEpdArrays;  //[__NEPDARRAYS__    ];  /// MALisa
  TClonesArray **mMtdArrays;  //[__NMTDARRAYS__    ];
  TClonesArray **mFgtArrays;  //[__NFGTARRAYS__    ];
  TClonesArray **mEztArrays;  //[__NEZTARRAYS__    ];

  char mStatusArrays[__NALLARRAYS__];
  TClonesArray *mEmcCollectionArray; // Needed to hold old format
  StMuEmcCollection *mEmcCollection;
  StMuFmsCollection *mFmsCollection;
  TClonesArray *mPmdCollectionArray; // Needed to hold old format
  StMuPmdCollection *mPmdCollection;
  //  StMuEpdHitCollection *mMuEpdHitCollection;   // MALisa

  // Increment this by 1 every time the class structure is changed
  ClassDef(StMuDstMaker, 4)
};

inline StMuDst *StMuDstMaker::muDst() { return mStMuDst; }
inline TChain *StMuDstMaker::chain() { return mChain; }
inline TTree *StMuDstMaker::tree() { return mTTree; }
inline void StMuDstMaker::setTrackFilter(StMuCut *c) { mTrackFilter = c; }
inline void StMuDstMaker::setL3TrackFilter(StMuCut *c) { mL3TrackFilter = c; }
inline StMuFilter *StMuDstMaker::trackFilter() { return (StMuFilter *)mTrackFilter; }
inline StMuL3Filter *StMuDstMaker::l3TrackFilter() { return (StMuL3Filter *)mL3TrackFilter; }
#ifndef __NO_STRANGE_MUDST__
inline void StMuDstMaker::setStStrangeMuDstMaker(StStrangeMuDstMaker *s) { mStStrangeMuDstMaker = s; }
inline StStrangeMuDstMaker *StMuDstMaker::stStrangeMuDstMaker() { return mStStrangeMuDstMaker; }
#endif
inline void StMuDstMaker::setTrackType(unsigned int t) { mTrackType = t; }
inline unsigned int StMuDstMaker::trackType() { return mTrackType; }

inline bool StMuDstMaker::readTracks() { return mReadTracks; }
#ifndef __NO_STRANGE_MUDST__
inline bool StMuDstMaker::readV0s() { return mReadV0s; }
inline bool StMuDstMaker::readXis() { return mReadXis; }
inline bool StMuDstMaker::readKinks() { return mReadKinks; }
#endif
inline void StMuDstMaker::setReadTracks(bool b) { mReadTracks = b; }
#ifndef __NO_STRANGE_MUDST__
inline void StMuDstMaker::setReadV0s(bool b) { mReadV0s = b; }
inline void StMuDstMaker::setReadXis(bool b) { mReadXis = b; }
inline void StMuDstMaker::setReadKinks(bool b) { mReadKinks = b; }
#endif

inline void StMuDstMaker::setSplit(int split) { mSplit = split; }
inline void StMuDstMaker::setCompression(int comp) { mCompression = comp; }
inline void StMuDstMaker::setBufferSize(int buf) { mBufferSize = buf; }

#endif
