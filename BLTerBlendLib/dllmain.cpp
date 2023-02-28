#include "pch.h"
#include "blv20intrn.h"
#include <stdlib.h>

#include "detours.h"

// (Using fixed addresses because this version of the game allows me to...)

struct LightInfo
{
    enum Type {
        Point = 0,
        Spot = 1,
        Vector = 2,
        Ambient = 3
    };
    Type        mType;

    float    mPos[3];
    float    mDirection[3]; ///< For spot and vector lights, indicates the direction of the light.
    float    mColor[4];
    float    mAmbient[4];
    float       mRadius;    ///< For point and spot lights, indicates the effective radius of the light.

    S32         mScore;     ///< Used internally to track importance of light.
};

typedef void (__fastcall* relightFn) (void* ecx, void* edx, LightInfo* stk1);
extern MologieDetours::Detour<relightFn> relight;

void __fastcall SceneLighting_TerrainProxy_lightVector(void* _this, void* ignoreEDX, LightInfo* light)
{
    LightInfo proxy = *light;
    float temp = 0;

    temp = proxy.mAmbient[0];
    proxy.mAmbient[0] = proxy.mAmbient[2];
    proxy.mAmbient[2] = temp;

    temp = proxy.mColor[0];
    proxy.mColor[0] = proxy.mColor[2];
    proxy.mColor[2] = temp;

    relight.GetOriginalFunction()(_this, ignoreEDX, &proxy);
}
MologieDetours::Detour<relightFn> relight((relightFn)0x60dcf0, (relightFn)SceneLighting_TerrainProxy_lightVector);


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        // No need
        break;
    }
    return TRUE;
}

