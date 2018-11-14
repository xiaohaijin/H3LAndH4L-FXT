#ifndef StDcaService_def
#define StDcaService_def

#include "StThreeVectorF.hh"
#include "StPhysicalHelixD.hh"

// this struct is for Hui LONG's dcaPToPi function ONLY.
// it is just the 'Helix' in his code.
// some useless items are commented out.
struct StTrackHelix {
    // float  pid;
    // short  id;
    double Xc;
    double Yc;
    double Zc;
    double pt;
    // double X;
    // double Y;
    // double Z;
    // double Px;
    // double Py;
    double Pz;
    double r;
    float theta;
    float Vz;
    int h;
    // int    h2;
    int Flag;
    // int    q;
    // int    nhits;
    // float  dca;
    // float  dedx;
};

extern bool kStHelixDca;
extern bool kMinimize;
extern float kShiftConnect;
extern float kShiftContain;

// wrapper to Hui LONG's two helix dca code
double closestDistance(const StPhysicalHelixD& helix1, const StPhysicalHelixD& helix2, double magnet,
                       const StThreeVectorF& pv, StThreeVectorF& xv0, StThreeVectorF& op1, StThreeVectorF& op2);
void dcaPToPi(float* fi_p, float* fi_pi, StTrackHelix* proton, StTrackHelix* pion, float* d_root, float* v0,
              float alfa);

// wrapper to Hui LONG's dca to primary vertex code (OBSOLETE)
double getDcaToPV(const StPhysicalHelixD& helix, const StThreeVectorF& pv);
// wrapper to Hui LONG's helix to straight line dca code (for Xi or Omega reconstruction)(OBSOLETE)
double closestDistance(const StThreeVectorF& xv0, const StThreeVectorF& pv0, const StPhysicalHelixD& helix,
                       double magnet, StThreeVectorF& xxi, StThreeVectorF& op);

#endif
