/***************************************************************************
 *
 * $Id: StMuTrack.h,v 1.52 2017/04/17 19:19:51 smirnovd Exp $
 * Author: Frank Laue, BNL, laue@bnl.gov
 *
 ***************************************************************************/

/** @class StMuTrack
 * Class holding the track properties
 * All unitis are in standard STAR units: GeV,cm
 * Please keep in mind, for primary tracks the first measured point is the primary vertex.
 * This affects dca, length calculations, inner helix.
*/

#ifndef StMuTrack_h
#define StMuTrack_h

#include "COMStMuDst.h"
#include "StMuEvent.h"
#include "StMuHelix.h"
#include "StMuUtilities.h"
#include "StMuProbPidTraits.h"
#include "StMuBTofPidTraits.h" /// dongx
#include "StMuMtdPidTraits.h" /// Bingchu
#include "StMuPrimaryTrackCovariance.h"

#include "StEvent/StTrackTopologyMap.h"
#include "StEvent/StRunInfo.h"
#include "StEvent/StDcaGeometry.h"
#include "StarClassLibrary/StPhysicalHelixD.hh"
#include "StarClassLibrary/StThreeVectorD.hh"
#include "StarClassLibrary/StThreeVectorF.hh"
#include "StarClassLibrary/SystemOfUnits.h"

////////////////////////////////////////////////////
/// was added by xiaohai(begin)
////////////////////////////////////////////////////
#include "COMMON/StMuDst.h"
#include "COMMON/StMuEvent.h"
#include "COMMON/StMuHelix.h"
#include "COMMON/StMuUtilities.h"
#include "COMMON/StMuProbPidTraits.h"
#include "COMMON/StMuBTofPidTraits.h" /// dongx
#include "COMMON/StMuMtdPidTraits.h" /// Bingchu
#include "COMMON/StMuPrimaryTrackCovariance.h"
#include "StTrackTopologyMap.h"
#include "StRunInfo.h"
#include "StDcaGeometry.h"
#include "StPhysicalHelixD.hh"
#include "StThreeVectorD.hh"
#include "StThreeVectorF.hh"
#include "SystemOfUnits.h"
////////////////////////////////////////////////////
/// was added by xiaohai(end)
////////////////////////////////////////////////////

#include "TObject.h"
#include "TVector.h"

#define __PROB_SCALE__ 1000.
#define __SIGMA_SCALE__ 1000.
#define __NOVALUE__ -999

class StRichSpectra;
class StEvent;
class StTrack;
class StVertex;
class StEmcGeom;
class StEmcPosition;
class StuProbabilityPidAlgorithm;

class TObjArray;

class StMuTrack : public TObject {
public:
    StMuTrack()
        : mVertexIndex(0)
        , mNHitsPossInner(0)
        , mNHitsFitInner(0)
        , mNHitsPossTpc(255)
        , mNHitsFitTpc(255)
        , mIndex2Cov(-1)
        , mIdTruth(0)
        , mQuality(0)
        , mIdParentVx(0) { /* no-op*/ }; ///< default constructor
    StMuTrack(const StEvent*, const StTrack*, const StVertex*, Int_t index2Global = -2, Int_t index2RichSpectra = -2,
              Bool_t l3 = false, TObjArray* vtx_list = 0); ///< constructor from StEvent and StTrack
    short id() const; ///< Returns the track id(or key), is unique for a track node, i.e. global and primary tracks have
    /// the same id.
    short type() const; ///< Returns the track type: 0=global, 1=primary, etc (see StEvent manual for type information)
    short flag() const; ///< Returns flag, (see StEvent manual for type information)
    UInt_t flagExtension() const
    {
        return mFlagExtension;
    }
    Int_t bad() const; // track is bad
    /// Returns index of associated global track. If not in order can be set with StMuDst::fixTrackIndeces() (but is
    /// taken
    /// care of in StMuDstReader.)
    Int_t index2Global() const;
    Int_t index2Cov() const;
    Int_t index2RichSpectra() const; ///< Returns index of associated rich spectra.
    Int_t index2BTofHit() const; /// dongx
    Int_t index2MtdHit() const; ///
    Int_t vertexIndex() const; ///< Returns index of associated primary vertex.
    void setVertexIndex(Int_t i)
    {
        mVertexIndex = i;
    } ///< Set index of primary vertex for which dca is stored
    const StMuTrack* globalTrack() const; ///< Returns pointer to associated global track. Null pointer if no global
    /// track available.
    const StMuTrack* primaryTrack() const; ///< Returns pointer to associated primary track. Null pointer if no global
    /// track available.
    const StRichSpectra* richSpectra() const; ///< Returns pointer to associated rich spectra. Null pointer if no global
    /// track available.
    const StMuBTofHit* tofHit() const; /// dongx
    const StMuMtdHit* mtdHit() const; /// Bingchu
    UShort_t nHits() const; ///< Return total number of hits on track.
    UShort_t nHitsPoss() const; ///< Return number of possible hits on track.
    UShort_t nHitsPoss(StDetectorId) const; ///< Return number of possible hits on track.
    UShort_t nHitsDedx() const; ///< Return number of hits used for dEdx.
    UShort_t nHitsFit() const; ///< Return total number of hits used in fit.
    UShort_t nHitsFit(StDetectorId) const; ///< Return number of hits used in fit.
    Double_t pidProbElectron() const; ///< Returns Aihong's probability of being an electron.
    Double_t pidProbPion() const; ///< Returns Aihong's probability of being a pion.
    Double_t pidProbKaon() const; ///< Returns Aihong's probability of being a kaon.
    Double_t pidProbProton() const; ///< Returns Aihong's probability of being a proton.
    Double_t nSigmaElectron() const; ///< Returns Craig's distance to the calculated dE/dx band for electrons in units
    /// of
    /// sigma.
    Double_t nSigmaPion() const; ///< Returns Craig's distance to the calculated dE/dx band for pions in units of sigma.
    Double_t nSigmaKaon() const; ///< Returns Craig's distance to the calculated dE/dx band for kaons in units of sigma.
    Double_t nSigmaProton() const; ///< Returns Craig's distance to the calculated dE/dx band for protons in units of
    /// sigma.
    Double_t dEdxPull(Double_t mass, Bool_t fit = kFALSE, Int_t charge = 1) const;
    Double_t dEdxPullElectronI70() const
    {
        return dEdxPull(0.51099907e-3);
    }
    Double_t dEdxPullPionI70() const
    {
        return dEdxPull(0.13956995);
    }
    Double_t dEdxPullKaonI70() const
    {
        return dEdxPull(0.493677);
    }
    Double_t dEdxPullProtonI70() const
    {
        return dEdxPull(0.93827231);
    }
    Double_t dEdxPullElectronFit() const
    {
        return dEdxPull(0.51099907e-3, kTRUE);
    }
    Double_t dEdxPullPionFit() const
    {
        return dEdxPull(0.13956995, kTRUE);
    }
    Double_t dEdxPullKaonFit() const
    {
        return dEdxPull(0.493677, kTRUE);
    }
    Double_t dEdxPullProtonFit() const
    {
        return dEdxPull(0.93827231, kTRUE);
    }
    Double_t nSigmaElectron70() const
    {
        return dEdxPullElectronI70();
    }
    Double_t nSigmaPion70() const
    {
        return dEdxPullPionI70();
    }
    Double_t nSigmaKaon70() const
    {
        return dEdxPullKaonI70();
    }
    Double_t nSigmaProton70() const
    {
        return dEdxPullProtonI70();
    }
    Double_t nSigmaElectronFit() const
    {
        return dEdxPullElectronFit();
    }
    Double_t nSigmaPionFit() const
    {
        return dEdxPullPionFit();
    }
    Double_t nSigmaKaonFit() const
    {
        return dEdxPullKaonFit();
    }
    Double_t nSigmaProtonFit() const
    {
        return dEdxPullProtonFit();
    }
    Double_t dEdx() const; ///< Returns measured dE/dx value.
    Double_t chi2() const; ///< Returns chi2 of fit.
    Double_t chi2prob() const; ///< Returns probability of fit.
    Double_t chi2xy() const; ///< Returns chi2 of fit.         ATTENTIONS: does not return xy chi2 (historic)
    Double_t chi2z() const; ///< Returns probability of fit.  ATTENTIONS: does not return z chi2 (historic)
    Double_t pt() const; ///< Returns pT at point of dca to primary vertex.
    Double_t phi() const; ///< Returns phi at point of dca to primary vertex.
    Double_t eta() const; ///< Returns pseudo rapidity at point of dca to primary vertex.
    Double_t length() const; ///< Returns length of track (cm) from primary vertex to last measured point.
    Double_t lengthMeasured() const; ///< Returns length of track (cm) from first to last measured point.
    StTrackTopologyMap topologyMap() const; ///< Returns topology map.
    Short_t charge() const; ///< Returns charge.
    const StThreeVectorF& p() const; ///< Returns 3-momentum at dca to primary vertex.
    const StThreeVectorF& momentum() const; ///< Returns 3-momentum at dca to primary vertex.
    StThreeVectorF dca(Int_t vtx_id = -1) const; ///< Returns 3D distance of closest approach to primary vertex.
    StThreeVectorF dcaGlobal(Int_t vtx_id = -1) const; ///< Returns 3D distance of closest approach to primary vertex of
    /// associated global track.
    Float_t dcaD(Int_t vtx_id = -1) const; ///< Signed radial component of global DCA (projected)
    Float_t dcaZ(Int_t vtx_id = -1) const; ///< Z component of global DCA
    const StThreeVectorF& firstPoint() const; ///< Returns positions of first measured point.
    const StThreeVectorF& lastPoint() const; ///< Returns positions of last measured point.
    StPhysicalHelixD helix() const; ///< Returns inner helix (first measured point)
    StPhysicalHelixD outerHelix() const; ///< Returns outer helix (last measured point)
    const StMuHelix& muHelix() const
    {
        return mHelix;
    } ///< Returns inner helix (first measured point)
    const StMuHelix& muOuterHelix() const
    {
        return mOuterHelix;
    } ///< Returns outer helix (last measured point)
    const StMuProbPidTraits& probPidTraits() const; ///< Returns Yuri Fisyak new pid probabilities.
    const StMuBTofPidTraits& btofPidTraits() const; /// dongx
    const StMuMtdPidTraits& mtdPidTraits() const; ///
    void setBTofPidTraits(const StMuBTofPidTraits& pid); /// dongx
    void setMtdPidTraits(const StMuMtdPidTraits& pid); ///
    static void setProbabilityPidAlgorithm(StuProbabilityPidAlgorithm*); ///< Sets the StuProbabilityPidAlgorithm.
    /// Important in order to calculate Aihong's
    /// pids.
    static void setProbabilityPidCentrality(Double_t cent); ///< Sets the centrality for calculating Aihong's pid.
    virtual void Print(Option_t* option = "") const; ///< Print track info
    void setIndex2BTofHit(Int_t i)
    {
        mIndex2BTofHit = i;
    } /// dongx
    void setIndex2MtdHit(Int_t i)
    {
        mIndex2MtdHit = i;
    } /// Bingchu
    void setIndex2Cov(Int_t i)
    {
        mIndex2Cov = i;
    } ///< Set index of associated DCA geoemtry for the global track.

    // Matching to BEMC related functions
    TArrayI getTower(Bool_t useExitRadius = false, Int_t det = 1) const; // Get Tower track is pointing too -  1=BTOW,
    // 3=BSMDE, 4=BSMDP //1=BTOW, 3=BSMDE, 4=BSMDP
    // Returns TVector tower. tower[0] is module,
    // tower[1] is eta, tower[2] is sub, and
    // tower[3] is id
    Double_t energyBEMC() const;
    Bool_t matchBEMC() const;
    Int_t idTruth() const
    {
        return mIdTruth;
    }
    Int_t qaTruth() const
    {
        return mQuality;
    }
    Int_t idParentVx() const
    {
        return mIdParentVx;
    }
    void setType(short type)
    {
        mType = type;
    }
    void setIdTruth(Int_t idtru, Int_t qatru = 0)
    {
        mIdTruth = (UShort_t)idtru;
        mQuality = (UShort_t)qatru;
    }
    void setIdParentVx(Int_t Id)
    {
        mIdParentVx = Id;
    }

protected:
    Int_t mId;
    Short_t mType;
    Short_t mFlag;
    UInt_t mFlagExtension; // bit wise fast detector matching status
    Int_t mIndex2Global;
    Int_t mIndex2RichSpectra;
    Int_t mIndex2BTofHit; /*!< dongx */
    Int_t mIndex2MtdHit;
    Int_t mVertexIndex; /*!<  Primary vertex id for this track's dca */
    UChar_t mNHits; /*!< Total number of points (was (F)tpc only) */
    UChar_t mNHitsPoss; /*!< Total possible points (was (F)tpc only) */
    UChar_t mNHitsDedx;
    UChar_t mNHitsFit; /*! Total fitted points (was (F)tpc only) */
    UChar_t mNHitsPossInner; // Svt/Ist (3 bit) and Ssd (2 bit) possible hits, Pixel (3 bits)
    UChar_t mNHitsFitInner; // Svt/Ist (3 bit) and Ssd (2 bit) fitted hits, Pixel (3 bits)
    UChar_t mNHitsPossTpc; // Possible (F)tpc hits (Ftpc flagged with first 2 bits)
    UChar_t mNHitsFitTpc; // Fitted (F)tpc hits (Ftpc flagged with first 2 bits)

    UShort_t mPidProbElectron;
    UShort_t mPidProbPion;
    UShort_t mPidProbKaon;
    UShort_t mPidProbProton;

    Int_t mNSigmaElectron;
    Int_t mNSigmaPion;
    Int_t mNSigmaKaon;
    Int_t mNSigmaProton;

    Float_t mdEdx;
    Float_t mChiSqXY;
    Float_t mChiSqZ;
    Float_t mPt;
    Float_t mEta;
    Float_t mPhi;

    StTrackTopologyMap mTopologyMap;
    StThreeVectorF mP;
    StThreeVectorF mDCA;
    StThreeVectorF mDCAGlobal;
    StThreeVectorF mFirstPoint;
    StThreeVectorF mLastPoint;

    StMuHelix mHelix;
    StMuHelix mOuterHelix;
    StMuProbPidTraits mProbPidTraits; ///< Class holding the new Yuri Fisyak pid probabilities.
    StMuBTofPidTraits mBTofPidTraits; /// dongx
    StMuMtdPidTraits mMtdPidTraits; /// Bingchu
    Int_t mIndex2Cov;
    // IdTruth
    UShort_t mIdTruth; // MC track id if any
    UShort_t mQuality; // quality of this information (percentage of hits coming the above MC track)
    Int_t mIdParentVx;

    /*!
     * \brief Set index of associated global track.
     * \param i
     */
    void setIndex2Global(Int_t i)
    {
        mIndex2Global = i;
    }

    /*!
     * \brief Set index of associated rich spectra.
     * \param i
     */
    void setIndex2RichSpectra(Int_t i)
    {
        mIndex2RichSpectra = i;
    }

    /*!
     * \brief Calculate dca to a given point
     * \param pos
     * \return
     */
    StThreeVectorF dca(const StThreeVectorF& pos) const;

    /*!
     * \brief Helper function: Calculates dca from a given
     * \param vertex
     * \return
     */
    StThreeVectorD dca(const StTrack*, const StVertex* vertex) const;

    /*! StTrack and the primary vertex taken from StEvent */

    /*!
     * \brief Helper function: Calculates the momentum at dca a given StTrack and the primary
     *        vertex taken from StEvent.
     * \param event
     * \param track
     * \param vertex
     * \return
     */
    StThreeVectorD momentumAtPrimaryVertex(const StEvent* event, const StTrack* track, const StVertex* vertex) const;

    /*!
     * \brief Helper function to fill all the different pid values
     */
    void fillMuProbPidTraits(const StEvent*, const StTrack*);

    /*!
     * \brief fillMuBTofPidTraits
     */
    void fillMuBTofPidTraits(const StTrack*);

    /*!
     * \brief fillMuMtdPidTraits
     */
    void fillMuMtdPidTraits(const StTrack*);

    /*!
     * \brief mProbabilityPidAlgorithm, we will use the same algorithm for all tracks
     */
    static StuProbabilityPidAlgorithm* mProbabilityPidAlgorithm;

    /*!
     * \brief Centrality for Aihong's pid prob calculations. Will set when new
     *        StMuEvent is made from StEvent
     */
    static Double_t mProbabilityPidCentrality;

    friend class StMuDst;
    friend class StMuDstFilterMaker;
    friend class StMuMomentumShiftMaker;
    ClassDef(StMuTrack, 15)
};

inline short StMuTrack::id() const
{
    return mId;
}

inline short StMuTrack::type() const
{
    return mType;
}

inline short StMuTrack::flag() const
{
    return mFlag;
}

inline Int_t StMuTrack::index2Global() const
{
    return mIndex2Global;
}

inline Int_t StMuTrack::index2Cov() const
{
    return mIndex2Cov;
}

inline Int_t StMuTrack::index2RichSpectra() const
{
    return mIndex2RichSpectra;
}

inline Int_t StMuTrack::index2BTofHit() const
{
    return mIndex2BTofHit;
}

inline Int_t StMuTrack::index2MtdHit() const
{
    return mIndex2MtdHit;
}

inline UShort_t StMuTrack::nHits() const
{
    return mNHits;
}

inline UShort_t StMuTrack::nHitsDedx() const
{
    return mNHitsDedx;
}

inline UShort_t StMuTrack::nHitsFit() const
{
    return mNHitsFit;
}

inline Double_t StMuTrack::pidProbElectron() const
{
    return unPack(mPidProbElectron, __PROB_SCALE__);
}

inline Double_t StMuTrack::pidProbPion() const
{
    return unPack(mPidProbPion, __PROB_SCALE__);
}

inline Double_t StMuTrack::pidProbKaon() const
{
    return unPack(mPidProbKaon, __PROB_SCALE__);
}

inline Double_t StMuTrack::pidProbProton() const
{
    return unPack(mPidProbProton, __PROB_SCALE__);
}

inline Double_t StMuTrack::nSigmaElectron() const
{
    return unPack(mNSigmaElectron, __SIGMA_SCALE__);
}

inline Double_t StMuTrack::nSigmaPion() const
{
    return unPack(mNSigmaPion, __SIGMA_SCALE__);
}

inline Double_t StMuTrack::nSigmaKaon() const
{
    return unPack(mNSigmaKaon, __SIGMA_SCALE__);
}

inline Double_t StMuTrack::nSigmaProton() const
{
    return unPack(mNSigmaProton, __SIGMA_SCALE__);
}

inline Double_t StMuTrack::dEdx() const
{
    return static_cast<double>(mdEdx);
}

inline Double_t StMuTrack::chi2xy() const
{
    return static_cast<double>(mChiSqXY);
}

inline Double_t StMuTrack::chi2z() const
{
    return static_cast<double>(mChiSqZ);
}

inline Double_t StMuTrack::chi2() const
{
    return static_cast<double>(mChiSqXY);
}

inline Double_t StMuTrack::chi2prob() const
{
    return static_cast<double>(mChiSqZ);
}

inline void StMuTrack::setMtdPidTraits(const StMuMtdPidTraits& pid)
{
    mMtdPidTraits = pid;
}

inline StTrackTopologyMap StMuTrack::topologyMap() const
{
    return mTopologyMap;
}

inline short StMuTrack::charge() const
{
    return mHelix.q();
}

inline Double_t StMuTrack::pt() const
{
    return static_cast<double>(mPt);
}

inline Double_t StMuTrack::eta() const
{
    return static_cast<double>(mEta);
}

inline Double_t StMuTrack::phi() const
{
    return static_cast<double>(mPhi);
}

inline const StThreeVectorF& StMuTrack::p() const
{
    return mP;
}

inline const StThreeVectorF& StMuTrack::momentum() const
{
    return mP;
}

inline const StThreeVectorF& StMuTrack::firstPoint() const
{
    return mFirstPoint;
}

inline const StThreeVectorF& StMuTrack::lastPoint() const
{
    return mLastPoint;
}

/*!
 * \brief Returns Yuri Fisyak new pid probabilities.
 * \return
 */
inline const StMuProbPidTraits& StMuTrack::probPidTraits() const
{
    return mProbPidTraits;
}

inline const StMuBTofPidTraits& StMuTrack::btofPidTraits() const
{
    return mBTofPidTraits;
}

inline const StMuMtdPidTraits& StMuTrack::mtdPidTraits() const
{
    return mMtdPidTraits;
}

inline void StMuTrack::setProbabilityPidAlgorithm(StuProbabilityPidAlgorithm* p)
{
    mProbabilityPidAlgorithm = p;
}

inline void StMuTrack::setProbabilityPidCentrality(Double_t cent)
{
    mProbabilityPidCentrality = cent;
}

inline void StMuTrack::setBTofPidTraits(const StMuBTofPidTraits& pid)
{
    mBTofPidTraits = pid;
}

inline const StMuTrack* StMuTrack::globalTrack() const
{
    return (mIndex2Global >= 0) ? static_cast<StMuTrack*>(StMuDst::array(muGlobal)->UncheckedAt(mIndex2Global)) : 0;
}

inline const StRichSpectra* StMuTrack::richSpectra() const
{
    return (mIndex2RichSpectra >= 0) ? (StRichSpectra*)StMuDst::array(muRich)->UncheckedAt(mIndex2RichSpectra) : 0;
}
inline const StMuBTofHit* StMuTrack::tofHit() const
{
    return (mIndex2BTofHit >= 0) ? (StMuBTofHit*)StMuDst::btofArray(muBTofHit)->UncheckedAt(mIndex2BTofHit) : 0;
} /// dongx
inline const StMuMtdHit* StMuTrack::mtdHit() const
{
    return (mIndex2MtdHit >= 0) ? (StMuMtdHit*)StMuDst::mtdArray(muMTDHit)->UncheckedAt(mIndex2MtdHit) : 0;
} ///
ostream& operator<<(ostream& os, StMuTrack const& v);
#endif
