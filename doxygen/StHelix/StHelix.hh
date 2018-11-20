/**
 * \class StHelix
 * \author Thomas Ullrich, Sep 26 1997
 *
 * Parametrization of a helix. Can also cope with straight tracks, i.e.
 * with zero curvature. This represents only the mathematical model of
 * a helix. See the SCL user guide for more.
 */
#ifndef ST_HELIX_HH
#define ST_HELIX_HH

#include <math.h>
#include <utility>
#include <algorithm>
#include "StThreeVector.hh"
#include "SystemOfUnits.h"

#if !defined(ST_NO_NAMESPACES)
using std::pair;
using std::swap;
using std::max;
#endif

/*!
 * \brief StHelix类，该类主要定义一个螺旋前进的粒子。
 */
class StHelix {
 public:
  /*!
   * \brief StHelix 构造函数
   * \param c 粒子的曲率curvature
   * \param dip 粒子的倾斜角，为什么会有这样的一个角度，不是应该为0吗？
   * \param phase 粒子的方位角(XY平面)
   * \param o 粒子的起始点
   * \param h what is this variable? the default value: positive particle, positive
   * field(-sign(q*B)).
   *          the value of mH is -1.
   */
  StHelix(double c, double dip, double phase, const StThreeVector<double>& o, int h = -1);

  virtual ~StHelix();

  /*!
   * \brief get the dip angle of the particle.
   * \return dipAngle.
   */
  double dipAngle() const;

  /*!
   * \brief 曲率
   * \return 返回粒子在XY平面的曲率1/R。
   */
  double curvature() const;

  /*!
   * \brief 粒子在ring环中的方位角
   * \return 粒子方位角
   */
  double phase() const;

  /*!
   * \brief 在XY平面中，环的中心点的x坐标
   * \return x坐标
   */
  double xcenter() const;

  /*!
   * \brief 在XY平面中，环的中心点的y坐标
   * \return y坐标
   */
  double ycenter() const;

  /*!
   * \brief h是粒子的螺度
   * \return 螺度(-sing(q*B))
   */
  int h() const;

  /*!
   * \brief 粒子的起始点
   * \return 粒子的三坐标的矢量StThreeVector
   */
  const StThreeVector<double>& origin() const;

  /*!
   * \brief
   * 该函数充当设定上述的作用，该类没有默认的构造函数，为什么又要来这样的一个函数？
   * \param c
   * \param dip
   * \param phase
   * \param o
   * \param h
   */
  void setParameters(double c, double dip, double phase, const StThreeVector<double>& o,
                     int h);

  /*!
   * \brief 螺旋线粒子的x坐标
   * \param s the number of helix advancing.
   * \return 粒子的x坐标
   */
  double x(double s) const;

  /*!
   * \brief 螺旋线粒子的y坐标
   * \param s the number of helix advancing.
   * \return 粒子的y坐标
   */
  double y(double s) const;

  /*!
   * \brief 螺旋线粒子的z坐标
   * \param s the number of helix advancing.
   * \return 粒子的z坐标
   */
  double z(double s) const;

  /*!
   * \brief 螺旋线粒子的三个坐标值。
   * \param s the number of helix advancing.
   * \return 粒子的三矢量值。
   */
  StThreeVector<double> at(double s) const;

  /*!
   * \brief cx pointing vector of helix at point s???
   * \param s
   * \return
   */
  double cx(double s) const;
  double cy(double s) const;
  double cz(double s = 0) const;

  /*!
   * \brief cat pointing vector of helix at point s???
   * \param s
   * \return
   */
  StThreeVector<double> cat(double s) const;

  /*!
   * \brief returns period length of helix
   * \return
   */
  double period() const;

  /*!
   * \brief path length at given r (cylindrical r)
   * \param r
   * \return
   */
  pair<double, double> pathLength(double r) const;

  /*!
   * \brief path length at given r (rylindrical r, cylinder axis at x,y)
   * \param r
   * \param x
   * \param y
   * \return
   */
  pair<double, double> pathLength(double r, double x, double y);

  /*!
   * \brief path length at distance of closest approach to a given point
   * \param p
   * \param scanPeriods
   * \return
   */
  double pathLength(const StThreeVector<double>& p, bool scanPeriods = true) const;

  /*!
   * \brief path length at intersection with plane
   * \param r
   * \param n
   * \return
   */
  double pathLength(const StThreeVector<double>& r, const StThreeVector<double>& n) const;

  /*!
   * \brief path length at distance of closest approach in the xy-plane to a
   * given point
   * \param x
   * \param y
   * \return
   */
  double pathLength(double x, double y) const;

  /*!
   * \brief path lengths at dca between two helices
   * \param minStepSize
   * \param minRange
   * \return
   */
  pair<double, double> pathLengths(const StHelix&, double minStepSize = 10 * micrometer,
                                   double minRange = 10 * centimeter) const;

  /*!
   * \brief minimal distance between point and helix
   * \param p
   * \param scanPeriods
   * \return
   */
  double distance(const StThreeVector<double>& p, bool scanPeriods = true) const;

  /*!
   * \brief checks for valid parametrization
   * \param world
   * \return
   */
  bool valid(double world = 1.e+5) const { return !bad(world); }

  /*!
   * \brief bad
   * \param world
   * \return
   */
  int bad(double world = 1.e+5) const;

  /*!
   * \brief move the origin along the helix to s which becomes then s = 0
   * \param s
   */
  virtual void moveOrigin(double s);

  static const double NoSolution;

 protected:
  StHelix();

  /*!
   * \brief performs also various checks
   */
  void setCurvature(double);
  void setPhase(double);
  void setDipAngle(double);

  /*!
   * \brief value of S where distance in x-y plane is minimal
   * \return
   */
  double fudgePathLength(const StThreeVector<double>&) const;

 protected:
  /*! 对于磁场为0的特殊情况记为mSingualrity */
  bool mSingularity;
  StThreeVector<double> mOrigin;
  double mDipAngle;
  double mCurvature;
  double mPhase;
  int mH;  // -sign(q*B);

  double mCosDipAngle;
  double mSinDipAngle;
  double mCosPhase;
  double mSinPhase;

#ifdef __ROOT__
  ClassDef(StHelix, 1)
#endif
};

//
//     Non-member functions
//
int operator==(const StHelix&, const StHelix&);
int operator!=(const StHelix&, const StHelix&);
ostream& operator<<(ostream&, const StHelix&);

/*!
 * \brief StHelix::h
 * \return
 */
inline int StHelix::h() const { return mH; }

/*!
 * \brief StHelix::dipAngle
 * \return
 */
inline double StHelix::dipAngle() const { return mDipAngle; }

/*!
 * \brief StHelix::curvature
 * \return
 */
inline double StHelix::curvature() const { return mCurvature; }

/*!
 * \brief StHelix::phase
 * \return
 */
inline double StHelix::phase() const { return mPhase; }

/*!
 * \brief StHelix::x
 * \param s
 * \return
 */
inline double StHelix::x(double s) const {
  if (mSingularity) {
    return mOrigin.x() - s * mCosDipAngle * mSinPhase;
  } else {
    return mOrigin.x() +
           (cos(mPhase + s * mH * mCurvature * mCosDipAngle) - mCosPhase) / mCurvature;
  }
}

/*!
 * \brief StHelix::y
 * \param s
 * \return
 */
inline double StHelix::y(double s) const {
  if (mSingularity) {
    return mOrigin.y() + s * mCosDipAngle * mCosPhase;
  } else {
    return mOrigin.y() +
           (sin(mPhase + s * mH * mCurvature * mCosDipAngle) - mSinPhase) / mCurvature;
  }
}

/*!
 * \brief StHelix::z
 * \param s
 * \return
 */
inline double StHelix::z(double s) const { return mOrigin.z() + s * mSinDipAngle; }

/*!
 * \brief StHelix::cx
 * \param s
 * \return
 */
inline double StHelix::cx(double s) const {
  if (mSingularity) {
    return -mCosDipAngle * mSinPhase;
  } else {
    return -sin(mPhase + s * mH * mCurvature * mCosDipAngle) * mH * mCosDipAngle;
  }
}

/*!
 * \brief StHelix::cy
 * \param s
 * \return
 */
inline double StHelix::cy(double s) const {
  if (mSingularity) {
    return mCosDipAngle * mCosPhase;
  } else {
    return cos(mPhase + s * mH * mCurvature * mCosDipAngle) * mH * mCosDipAngle;
  }
}

/*!
 * \brief StHelix::cz
 * \return
 */
inline double StHelix::cz(double /* s */) const { return mSinDipAngle; }

/*!
 * \brief StHelix::origin
 * \return
 */
inline const StThreeVector<double>& StHelix::origin() const { return mOrigin; }

/*!
 * \brief StHelix::at
 * \param s
 * \return
 */
inline StThreeVector<double> StHelix::at(double s) const {
  return StThreeVector<double>(x(s), y(s), z(s));
}

/*!
 * \brief StHelix::cat
 * \param s
 * \return
 */
inline StThreeVector<double> StHelix::cat(double s) const {
  return StThreeVector<double>(cx(s), cy(s), cz(s));
}

/*!
 * \brief StHelix::pathLength
 * \param X
 * \param Y
 * \return
 */
inline double StHelix::pathLength(double X, double Y) const {
  return fudgePathLength(StThreeVector<double>(X, Y, 0));
}

/*!
 * \brief StHelix::bad
 * \param WorldSize
 * \return
 */
inline int StHelix::bad(double WorldSize) const {

  int ierr;
  if (!::finite(mDipAngle)) {
    return 11;
  }
  if (!::finite(mCurvature)) {
    return 12;
  }

  ierr = mOrigin.bad(WorldSize);
  if (ierr) {
    return 3 + ierr * 100;
  }
  if (::fabs(mDipAngle) > 1.58) {
    return 21;
  }

  double qwe = ::fabs(::fabs(mDipAngle) - M_PI / 2);
  if (qwe < 1. / WorldSize) {
    return 31;
  }
  if (::fabs(mCurvature) > WorldSize) {
    return 22;
  }
  if (mCurvature < 0) {
    return 32;
  }
  if (abs(mH) != 1) {
    return 24;
  }

  return 0;
}

#endif
