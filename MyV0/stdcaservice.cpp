#include <cmath>

#include "TMath.h"

#include "SystemOfUnits.h"

#include "stdcaservice.h"

bool kStHelixDca = false;
bool kMinimize = true;
float kShiftConnect = 2;
float kShiftContain = 2;

bool kPeriod = false;

double closestDistance(const StPhysicalHelixD &helix1, const StPhysicalHelixD &helix2, double magnet,
                       const StThreeVectorF &pv, StThreeVectorF &vx0, StThreeVectorF &op1, StThreeVectorF &op2)

{
  double PI = TMath::Pi();
  float x[3], p1[3], p2[3], d;
  StTrackHelix protonobject;
  StTrackHelix pionobject;
  StTrackHelix *proton = &protonobject;
  StTrackHelix *pion = &pionobject;

  StPhysicalHelixD helix = helix1;

}
void dcaPToPi(float *fi_p, float *fi_pi, StTrackHelix *proton, StTrackHelix *pion, float *d_root, float *v0,
              float alfa);
double getDcaToPV(const StPhysicalHelixD &helix, const StThreeVectorF &pv);
double closestDistance(const StThreeVectorF &XV0, const StThreeVectorF &pv0, const StPhysicalHelixD *helix,
                       double magnet, StThreeVectorF &x1, StThreeVectorF &op);
