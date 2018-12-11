#ifndef STDCASERVICE_H
#define STDCASERVICE_H

#include <QtGlobal>

#include "StPhysicalHelixD.hh"
#include "StThreeVectorF.hh"

struct StTrackHelix {
  float pid = 0.f;
  short id = 0;
  double Xc = 0.;
  double Yc = 0.;
  double Zc = 0.;
  double X = 0.;
  double Y = 0.;
  double Z = 0.;
  double Px = 0.;
  double Py = 0.;
  double Pz = 0.;
  double r = 0.;
  float theta = 0.f;
  float Vz = 0.;
  int h = 0;
  int h2 = 0;
  int Flag = 0;
  int q = 0;
  int nhits = 0;
  float dca = 0.f;
  float dedx = 0.f;
};

extern bool kStHelixDca;
extern bool kMinimize;
extern float kShiftConnect;
extern float kShiftContain;
extern bool kPeriod;

double closestDistance(const StPhysicalHelixD &helix1, const StPhysicalHelixD &helix2, double magnet,
                       const StThreeVectorF &pv, StThreeVectorF &vx0, StThreeVectorF &op1, StThreeVectorF &op2);
void dcaPToPi(float *fi_p, float *fi_pi, StTrackHelix *proton, StTrackHelix *pion, float *d_root, float *v0,
              float alfa);
double getDcaToPV(const StPhysicalHelixD &helix, const StThreeVectorF &pv);
double closestDistance(const StThreeVectorF &XV0, const StThreeVectorF &pv0, const StPhysicalHelixD *helix,
                       double magnet, StThreeVectorF &x1, StThreeVectorF &op);

#endif  // STDCASERVICE_H
