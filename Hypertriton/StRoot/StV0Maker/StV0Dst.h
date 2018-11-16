#ifndef StV0Dst_def
#define StV0Dst_def

#define MAX_NUM_V0 3000
struct StV0Dst {
    int nrefmult;
    float primvertexX;
    float primvertexY;
    float primvertexZ;
    float vpdVz;
    float magn;
    int nv0;
    long eventid;
    long runid;

    // StV0Data v0[1000];
    float v0mass[MAX_NUM_V0];
    float v0pt[MAX_NUM_V0];
    float v0rapidity[MAX_NUM_V0];
    float v0eta[MAX_NUM_V0];
    float v0x[MAX_NUM_V0];
    float v0y[MAX_NUM_V0];
    float v0z[MAX_NUM_V0];
    float v0px[MAX_NUM_V0];
    float v0py[MAX_NUM_V0];
    float v0pz[MAX_NUM_V0];
    float v0xpath[MAX_NUM_V0];
    float v0ypath[MAX_NUM_V0];
    float v0zpath[MAX_NUM_V0];
    float v0pxpath[MAX_NUM_V0];
    float v0pypath[MAX_NUM_V0];
    float v0pzpath[MAX_NUM_V0];
    float v0declen[MAX_NUM_V0];
    float v0declenH[MAX_NUM_V0];
    float v0dca[MAX_NUM_V0];
    float v0rdotp[MAX_NUM_V0];
    float v0sinth[MAX_NUM_V0];
    float v0theta[MAX_NUM_V0];
    float dca1to2[MAX_NUM_V0];
    // dau1******************************************
    int dau1id[MAX_NUM_V0];
    float dau1dca[MAX_NUM_V0];
    float dau1mass[MAX_NUM_V0];
    float dau1eta[MAX_NUM_V0];
    int dau1nhitsfit[MAX_NUM_V0];
    int dau1nhitsdedx[MAX_NUM_V0];
    int dau1nhitsposs[MAX_NUM_V0];
    int dau1hftbits[MAX_NUM_V0];
    float dau1nsigma[MAX_NUM_V0];
    float dau1pt[MAX_NUM_V0];
    float dau1px[MAX_NUM_V0];
    float dau1py[MAX_NUM_V0];
    float dau1pz[MAX_NUM_V0];
    float dau1dEdx[MAX_NUM_V0];
    float dau1Z[MAX_NUM_V0];
    float dau1beta[MAX_NUM_V0];
    float dau1diffinvbeta[MAX_NUM_V0];
    float dau1toflocaly[MAX_NUM_V0];
    float dau1toflocalz[MAX_NUM_V0];
    //    int dau1PXL1[MAX_NUM_V0];
    //    int dau1PXL2[MAX_NUM_V0];
    //    int dau1PXL3[MAX_NUM_V0];
    //    int dau1IST1[MAX_NUM_V0];
    //    int dau1IST2[MAX_NUM_V0];
    //    int dau1SSD1[MAX_NUM_V0];
    //    int dau1SSD2[MAX_NUM_V0];
    // dau2********************************************
    int dau2id[MAX_NUM_V0];
    float dau2dca[MAX_NUM_V0];
    float dau2mass[MAX_NUM_V0];
    float dau2eta[MAX_NUM_V0];
    int dau2nhitsfit[MAX_NUM_V0];
    int dau2nhitsdedx[MAX_NUM_V0];
    int dau2nhitsposs[MAX_NUM_V0];
    int dau2hftbits[MAX_NUM_V0];
    float dau2nsigma[MAX_NUM_V0];
    float dau2pt[MAX_NUM_V0];
    float dau2px[MAX_NUM_V0];
    float dau2py[MAX_NUM_V0];
    float dau2pz[MAX_NUM_V0];
    float dau2dEdx[MAX_NUM_V0];
    float dau2Z[MAX_NUM_V0];
    float dau2beta[MAX_NUM_V0];
    float dau2diffinvbeta[MAX_NUM_V0];
    float dau2toflocaly[MAX_NUM_V0];
    float dau2toflocalz[MAX_NUM_V0];
    //    int dau2PXL1[MAX_NUM_V0];
    //    int dau2PXL2[MAX_NUM_V0];
    //    int dau2PXL3[MAX_NUM_V0];
    //    int dau2IST1[MAX_NUM_V0];
    //    int dau2IST2[MAX_NUM_V0];
    //    int dau2SSD1[MAX_NUM_V0];
    //    int dau2SSD2[MAX_NUM_V0];
};

#endif
