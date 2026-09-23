
/**
 * @file util.c
 * @brief Uncategorised utilities.
 */

#include "common.h"
#include "k.h"

V *k_index(K x, SZ i) {
    switch (xt) {
        case 0:  return xK[i];
        case KB: return &(xG[i]);
        case UU: return &xU[i];
        case KG: return &xG[i];
        case KH: return &xH[i];
        case KI: return &xI[i];
        case KJ: return &xJ[i];
        case KE: return &xE[i];
        case KF: return &xF[i];
        case KC: return &xC[i];
        case KS: return &xS[i];
        case KP: return &xJ[i];
        case KM: return &xI[i];
        case KD: return &xI[i];
        case KZ: return &xF[i];
        case KN: return &xJ[i];
        case KU: return &xI[i];
        case KV: return &xI[i];
        case KT: return &xI[i];
        case XT: return &xG[i];
        case XD: return &xG[i];
        default: return x;
    }
}

