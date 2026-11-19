// pathing/BMotion.cpp - the BMotion family + the ignore-list helpers.
// Subsystem: pathing (ratified 2026-11-19).
//
// BMotion::updateMovement @0x49CB60 [0x49CB60,0x49E3FC) = 6300 bytes
// (0x189C; the earlier "2204" figure was an extent arithmetic slip - 0x89C
// vs 0x189C - corrected 2026-11-19), int __thiscall(BMotion*,
// int doAvailablePathCheck, BUnitGroup* uG), retn 8.
//
// THIS BODY IS THE STRICT-CONTROL-FLOW FORM (owner-directed 2026-11-19,
// re-derived fresh from the full listing after the structured form kept
// desyncing): every L_<addr> label is the asm label at that address, every
// goto is the asm jump, the fall-throughs are preserved. No restructuring,
// no stage flags. The strict re-walk findings applied:
// - B1/B2/B3: the three repath-region minus-result buffers are +4 carves;
//   the zero lands on .z (not .y) at 0x49D2BA / 0x49D444-0x49D44E /
//   0x49D96B, and the norms are planar x^2+y^2.
// - C1/C2: the find_path (0x49D3BF) and findTilePath (0x49D8CA) returns are
//   masked AND eax, 0FFh before use.
// - D: the arm-2/findTile creation-time stamps read the literal chain
//   currentPlayer->world->world_time (RGE_Player.world @+0x8C,
//   idb_types.h:3700 -> RGE_Game_World.world_time @+0x10; the .c's unnamed
//   *(owner+140)+16 render).
// - The vtbl question at 0x49DF51 (slot +0x30C) is settled: header member
//   195 = lastMoveTime (verified by the owner; the displaced-header finding
//   was a probe off-by-one, retracted).
// - The constants: dword_65BBF0 = 4, dword_65BBEC = 2, BBitVectorAND_4 = 1
//   (the .rdata dword verifies), flt_6545BC = 0.1f, flt_6565F4 = 1e-7f,
//   flt_6565F0 = 10.0f, flt_654978 = 0.01f, dbl_6545B0 =
//   (double)float32(1e-7), SINGLE_MAXIMUM_VALUE = FLT_MAX.
#include "stdafx.h"

#pragma float_control(precise, on, push)

static const double dbl_6545B0 = (double)1e-07f;       // .rdata:006545B0 - the arrival/facing
                                                       // deadband = (double)float32(1e-7)

// Replacement for resetIgnoreList. Original:
// @ 0x004A0340 [0x4A0340,0x4A034A), void __fastcall(). SEMANTIC: zeroes
// the ignore-count mirror; the ignoreUnits contents stay (the count
// governs every read).
void __fastcall resetIgnoreList_new()
{
    ignoreListSize = 0;                                // asm 0x4A0340-0x4A0345 (mov ignoreListSize, 0)
}

// Replacement for addToIgnoreList. Original:
// @ 0x4A0350 [0x4A0350,0x4A03B6), bool __fastcall(obj in ECX). SEMANTIC:
// appends the unit's id to ignoreUnits (capacity-bounded), then - for a
// type-80 building - appends its linked_child ids; returns 0 when the
// list is full or obj is null, 1 when appended.
bool __fastcall addToIgnoreList_new(RGE_Static_Object* obj)
{
    int count = ignoreListSize;                        // asm 0x4A0350-0x4A0356 (mov edx, ignoreListSize)
    if (count >= _countof(ignoreUnits))                // asm 0x4A0357-0x4A035D (cmp 80h; jge -> return 0)
        return 0;                                      // asm loc_4A03B3-0x4A03B6 (xor al,al)
    if (obj == 0)                                      // asm 0x4A035F-0x4A0361 (test ecx, ecx)
        return 0;                                      // asm (jz loc_4A03B3)
    ignoreUnits[count] = obj->id;                      // asm 0x4A0363-0x4A0366 ([+0x10])
    ++count;                                           // asm 0x4A036D
    ignoreListSize = count;                            // asm 0x4A036E
    if (obj->type == 80)                               // asm 0x4A0374-0x4A0379 (byte @+0x5E; the runtime-type evidence for the cast)
    {
        TRIBE_Building_Object* buildingObj = (TRIBE_Building_Object*)obj;
        for (size_t li = 0; li < _countof(buildingObj->linked_child); ++li)   // asm 0x4A037F-0x4A03AD (the linked walk: 4 entries)
        {
            RGE_Static_Object* child = (RGE_Static_Object*)buildingObj->linked_child[li];   // asm 0x4A0387-0x4A0389 ([ecx])
            if (child == 0)                            // asm 0x4A0389-0x4A038D (test eax, eax)
                continue;                              // asm (jz loc_4A03A6 - the increment)
            ignoreUnits[count] = child->id;            // asm 0x4A038D-0x4A0390 ([+0x10])
            ++count;                                   // asm 0x4A0397
            ignoreListSize = count;                    // asm 0x4A039E
            if (count >= _countof(ignoreUnits))        // asm 0x4A0398-0x4A03A4 (cmp 80h; jge -> return 0 - the cap-exit FAILS the add)
                return 0;                              // asm (jge loc_4A03B3)
        }
    }
    return 1;                                          // asm 0x4A03AF-0x4A03B2 (mov al, 1)
}

// Replacement for BMotion::updateMovement. Original:
// @ 0x0049CB60 [0x49CB60,0x49E3FC), int __thiscall(this_, doAvailablePathCheck, uG),
// retn 8. THE STRICT-CONTROL-FLOW FORM: the asm's labels/jumps transcribed
// 1:1 - every L_<addr> label is the asm label at that address, every goto is
// the asm jump, the fall-throughs are preserved; no restructuring, no stage
// flags. SEMANTIC: the BMotion-family movement driver - the pause countdown,
// the 20-pass loop over the BPathData state machine (the waypoint
// increment/patrol arms, the state 4/5 relink arms, the state-2 repath with
// its four path-building arms, the install gate), then the movement arm (the
// waypoint-arrival check, the position integration, the OBSystem
// ignore-build + collision pass, the two arrival checks) and the tail facing
// rotation. Returns 0 = blocked or out of tries, 1 = moved or arrived.
int __fastcall BMotion__updateMovement_new(
    BMotion* this_, DWORD dummy, int doAvailablePathCheck, BUnitGroup* uG)
{
    (void)dummy;
    (void)uG;                                          // asm: the uG param unreferenced
    int result;
    int tries;                                         // var_214
    BPath* path;                                       // ebp - the active path
    RGE_Static_Object* targetObj;                      // var_218/ebx
    float distanceToTarget;                            // var_210.x
    int patrolFlag;                                    // var_260 (the loop-top latch)
    float gateDistance;                                // var_268 (the increment arm)
    float distanceToWaypoint3D;                        // var_264 (the increment arm)
    BVector deltaBuf;                                  // var_184 (the patrol seed base)
    BVector opResult;                                  // the shared minus-result scratch
    BPathData* obj;                                    // var_25C (the successor BPathData)
    BPath* newPath;                                    // edi
    int repathArmed;                                   // var_268 (state 2)
    int smoothUsed;                                    // var_258.x (state 2)
    float distanceToWaypoint;                          // var_258.y (state 2)
    BVector repathDelta;                               // the -0x230 carve
    BUnitGroup* unitGroup;                             // the group handle
    RGE_Map_Analysis* mapAnalysis;                     // var_260 (state 2, arm 1)
    RGE_Obstruction_Manager* obstructionManagerToUse;  // var_260/var_264 (state 2)
    int findLowResult;                                 // var_264 (state 2)
    int collisionTargetFound;                          // var_258.x (the OB pass)
    int freshBlockers;                                 // var_25C (the OB pass)
    BVector dirNormalized;                             // the -0x23C carve
    BVector facingDirCur;                              // the -0x248 carve
    BVector dirScaled;                                 // the -0x84 carve (operator_mul sret)
    BVector plusResult;                                // the -0x1B4 carve (op_plus sret)
    BVector integratedPos;                             // the -0x1C0 carve
    BVector wpMinusPos;                                // p2 (-0x1D0)
    BVector wpMinusIntegrated;                         // var_224
    BVector integratedMinusPos;                        // var_1DC
    float integratedDistStored;                        // var_258.z (the OB pass)
    float facingTimeDelta;                             // var_258.z (the facing arm)
    float facingYaw;                                   // var_268 (the facing arm)
    double facingYawCap;
    BVector facingCross;                               // the -0x1B4 carve (the facing arm)
    BMatrix facingRotate;                              // the -0x3C scratch
    BVector rotScratchF;                               // var_170
    BVector rotScratchR;                               // var_54
    BVector waypointScratch;                           // the dead BPath__waypoint out-buffer (every
                                                       // data flow uses the returned pointer)
    // the arm-2 convergence args (the per-arm pushes materialized before the
    // shared findLowLevelPath call at loc_49D668)
    BVector* lowGoal;
    float unitRadius;
    int* ignorePtr;
    int ignoreCount;
    float* terrainTable;
    int ownerId;

    if (this_->mPathData == 0)                         // asm 0x49CB83-0x49CB8B (edi = mPathData @+0xA8)
    {
        result = 0;                                    // asm 0x49CB8D (xor eax, eax)
        goto L_49E3E4;                                 // asm 0x49CB8F (jmp loc_49E3E4)
    }
    // asm loc_49CB94:
    if (this_->mPathData->pauseTime > 0.0f)            // asm 0x49CB94-0x49CBA2 (fld [edi+28h]; fcomp
                                                       // const_f_zero; test ah,41h; jnz loc_49CBE7 - the
                                                       // literal > MATCH row: the jnz targets the skip)
    {
        // the pause countdown: the returned delta consumed UNROUNDED in the
        // subtraction (fsubr), ONE fstp m32 store (the st0-boundary discipline)
        this_->mPathData->pauseTime =
            (float)((double)this_->mPathData->pauseTime
            - RGE_Game_World__getWorldTimeDeltaSeconds(
                (RGE_Game_World*)(*base_game)->world)); // asm 0x49CBA4-0x49CBBD
        if (!(this_->mPathData->pauseTime > 0.0f))     // asm 0x49CBC6-0x49CBD4 (fcomp; test ah,41h; jnz
                                                       // loc_49CBE4 - <= 0 or NaN clamps)
        {
            // asm loc_49CBE4:
            this_->mPathData->pauseTime = 0.0f;        // asm 0x49CBE4 (mov [ecx+28h], ebx(0))
        }
    }
    // asm loc_49CBE7:
    tries = 0;                                         // asm 0x49CBE7 (mov var_214, ebx)
L_49CBEB:                                              // asm loc_49CBEB (the movement loop head; the 13
                                                       // asm back-edges)
    ++tries;                                           // asm 0x49CBEB-0x49CBF3 (inc; mov)
    if (tries > 20)                                    // asm 0x49CBF7 (cmp 14h; jg loc_49E14A)
    {
        result = 0;                                    // asm loc_49E14A (xor eax, eax)
        goto L_49E3E4;                                 // asm 0x49E14C
    }
    path = this_->mPathData->path;                     // asm 0x49CBFD-0x49CC03 (+0x10; ebp)
    if (path == 0)                                     // asm 0x49CC06-0x49CC08
    {
        path = RGE_Game_World__path((RGE_Game_World*)(*base_game)->world,
            this_->mPathData->pathID);                 // asm 0x49CC0A-0x49CC1F (+0x14)
        if (path == 0)                                 // asm 0x49CC21-0x49CC23 (jz loc_49E14A)
        {
            result = 0;
            goto L_49E3E4;
        }
        this_->mPathData->path = path;                 // asm 0x49CC29-0x49CC2F (the cache)
    }
    // asm loc_49CC32:
    targetObj = 0;                                     // asm 0x49CC38 (xor ebx, ebx)
    distanceToTarget = FLT_MAX;                        // asm 0x49CC3A (SINGLE_MAXIMUM_VALUE)
    if (this_->mPathData->targetID > -1)               // asm 0x49CC46-0x49CC4C (+0x24; jle loc_49CC81)
    {
        targetObj = RGE_Game_World__object((RGE_Game_World*)(*base_game)->world,
            this_->mPathData->targetID);               // asm 0x49CC4E-0x49CC5B
        if (targetObj != 0)                            // asm 0x49CC62-0x49CC68 (jz loc_49CC81)
        {
            distanceToTarget =
                (float)RGE_Static_Object__distance_to_position(targetObj,
                    this_->mPosition.mPosition.x,
                    this_->mPosition.mPosition.y,
                    this_->mPosition.mPosition.z);     // asm 0x49CC6A-0x49CC7D (fstp var_210.x - the
                                                       // store-round)
        }
    }
    // asm loc_49CC81:
    patrolFlag = ((this_->mPathData->mFlags.valueValue & 4) != 0) ? 1 : 0;
                                                       // asm 0x49CC87-0x49CC9C (dword_65BBF0 = 4; the
                                                       // cmp/sbb/neg idiom)
    if ((this_->mPathData->mFlags.valueValue & 1) == 0)// asm 0x49CCA0-0x49CCA6 (test BBitVectorAND_4;
                                                       // jbe loc_49D01C)
        goto L_49D01C;
    if ((this_->mPathData->mFlags.valueValue & 2) != 0)// asm 0x49CCAE-0x49CCB4 (test dword_65BBEC = 2;
                                                       // jbe loc_49CCFD)
    {
        if (this_->mPathData->waypoint < path->waypointsCount) // asm 0x49CCB6-0x49CCBE (+0x18 vs
                                                               // path+0xC; jge loc_49CCEE)
        {
            BVector* setWp = BPath__waypoint(path, &waypointScratch,
                this_->mPathData->waypoint);           // asm 0x49CCC0-0x49CCCF
            this_->vfptr->setPosition(this_, *setWp);  // asm 0x49CCD4-0x49CCEB (slot +8 = 2; the
                                                       // by-value BVector: sub esp,0Ch + the
                                                       // {x,z,y} copies)
        }
        // asm loc_49CCEE:
        BPathData__setFlag(this_->mPathData, 1, 0);    // asm 0x49CCEE-0x49CCF8
    }
    // asm loc_49CCFD:
    BPathData__setFlag(this_->mPathData, 0, 0);        // asm 0x49CCFD-0x49CD07
    ++this_->mPathData->waypoint;                      // asm 0x49CD0C-0x49CD16 (+0x18)
    if ((path->flags.valueValue & 1) != 0)             // asm 0x49CD19-0x49CD22 (path+0x20; ja loc_49CE8A)
        goto L_49CE8A;
    // ---- the linear arm (asm 0x49CD28-0x49CE6E) ----
    {
        BVector* prevWp = BPath__waypoint(path, &waypointScratch,
            this_->mPathData->waypoint - 1);           // asm 0x49CD28-0x49CD40 (wp - 1)
        BVector__operator_minus(&this_->mPosition.mPosition, &opResult, prevWp);
                                                       // asm 0x49CD45-0x49CD50 (this = &mPosition)
        distanceToWaypoint3D = (float)sqrt((double)opResult.x * opResult.x
            + (double)opResult.z * opResult.z
            + (double)opResult.y * opResult.y);        // asm 0x49CD55-0x49CD87 (fstp var_264 - the
                                                       // store-round)
        if (targetObj != 0)                            // asm 0x49CD8B (jz loc_49CDD4)
        {
            gateDistance = (float)(RGE_Static_Object__distance_to_position(targetObj,
                    this_->mPosition.mPosition.x,
                    this_->mPosition.mPosition.y,
                    this_->mPosition.mPosition.z)
                - (double)this_->mPathData->range
                - (double)0.1f);                       // asm 0x49CD8D-0x49CDB9 (the call-tail fsub chain at
                                                       // 53-bit; ONE fstp var_268 store)
            if ((this_->mPathData->mFlags.valueValue & 4) != 0) // asm 0x49CDA9-0x49CDBD (test
                                                                // dword_65BBF0; jbe loc_49CDEB)
            {
                gateDistance = 0.0f;                   // asm 0x49CDC5 (the chain discarded)
                BPathData__setFlag(this_->mPathData, 2, 0); // asm 0x49CDBF-0x49CDD3
            }
            goto L_49CDEB;
        }
        // asm loc_49CDD4:
        gateDistance = (float)((double)distanceToWaypoint3D
            - (double)this_->mPathData->range
            - (double)0.1f);                           // asm 0x49CDDA-0x49CDE7 (fld var_264 - the STORED
                                                       // m32; ONE fstp store)
L_49CDEB:                                              // asm loc_49CDEB
        if (this_->mPathData->waypoint >= path->waypointsCount) // asm 0x49CDEB-0x49CDF9 (jl loc_49CE73)
        {
            if (gateDistance < 1e-7f)                  // asm 0x49CDFB-0x49CE0A (fcomp flt_6565F4; test ah,1;
                                                       // jz loc_49CE18 - the literal < MATCH row)
            {
                this_->mPathData->state = 4;           // asm 0x49CE0C (+0x1C)
                goto L_49D01C;                         // asm 0x49CE13
            }
            // asm loc_49CE18:
            if (targetObj == 0                         // asm 0x49CE18-0x49CE1A (jnz loc_49CE40)
                && gateDistance < 1.0f                 // asm 0x49CE1C-0x49CE2B (fcomp const_f_one; test ah,1;
                                                       // jz loc_49CE40)
                && this_->mPathData->waypointLevel != 0) // asm 0x49CE2D-0x49CE32 (+0xC; jz loc_49CE40)
            {
                this_->mPathData->state = 4;           // asm 0x49CE34
                goto L_49D01C;                         // asm 0x49CE3B
            }
            goto L_49CE40;                             // asm (any conjunct false -> loc_49CE40)
L_49CE40:                                              // asm loc_49CE40
            if (distanceToWaypoint3D < 1e-7f)          // asm 0x49CE40-0x49CE4F (fcomp flt_6565F4; test ah,1;
                                                       // jz loc_49CE5D - the literal < MATCH row)
            {
                this_->mPathData->state = 4;           // asm 0x49CE51
                goto L_49D01C;                         // asm 0x49CE58
            }
            // asm loc_49CE5D:
            --this_->mPathData->waypoint;              // asm 0x49CE5D-0x49CE5E (dec edx; mov [ecx+18h])
            this_->mPathData->state = 2;               // asm 0x49CE61-0x49CE67
            goto L_49D01C;                             // asm 0x49CE6E
        }
        // asm loc_49CE73:
        if (this_->mPathData->waypointLevel > 0)       // asm 0x49CE73-0x49CE78 (+0xC; jle loc_49D01C)
            this_->mPathData->state = 2;               // asm 0x49CE7E
        goto L_49D01C;                                 // asm 0x49CE85
    }
L_49CE8A:                                              // asm loc_49CE8A (the patrol arm)
    {
        this_->mPathData->waypoint = BPath__nextPatrolWaypoint(path,
            this_->mPathData->waypoint - 1);           // asm 0x49CE8A-0x49CEA2 (wp - 1)
        this_->mPathData->state = 2;                   // asm 0x49CEA5-0x49CEAB
        if (this_->mPathData->waypoint != -1)          // asm 0x49CEB2-0x49CEBC (jnz loc_49D01C)
            goto L_49D01C;
        // the patrol wrapped:
        BVector* patrolTail = BPath__waypoint(path, &waypointScratch,
            path->waypointsCount - 1);                 // asm 0x49CEC8-0x49CEF6
        deltaBuf.x = this_->mPathUnit->world_x;        // asm 0x49CEDB (+0x48)
        deltaBuf.y = this_->mPathUnit->world_y;        // asm 0x49CECD-0x49CED3 (fstp, +0x4C)
        deltaBuf.z = 0.0f;                             // asm 0x49CEEB
        BVector__operator_minus(&deltaBuf, &opResult, patrolTail); // asm 0x49CEFB-0x49CF0B
        distanceToWaypoint3D = (float)sqrt((double)opResult.x * opResult.x
            + (double)opResult.z * opResult.z
            + (double)opResult.y * opResult.y);        // asm 0x49CF10-0x49CF42 (fstp var_264 - the
                                                       // store-round)
        double wrapSeed;
        if (targetObj != 0)                            // asm 0x49CF46 (jz loc_49CF9A)
        {
            wrapSeed = (double)RGE_Static_Object__distance_to_position(targetObj,
                    this_->mPathUnit->world_x,
                    0.0,
                    this_->mPathUnit->world_y)
                - (double)this_->mPathData->range
                - (double)0.1f;                        // asm 0x49CF48-0x49CF73 (the (x, 0, y) seed form; the
                                                       // x87 chain at 53-bit, UNSTORED into the gate)
            if ((this_->mPathData->mFlags.valueValue & 4) != 0) // asm 0x49CF68-0x49CF79 (test
                                                                // dword_65BBF0; jbe loc_49CFAD)
            {
                BPathData__setFlag(this_->mPathData, 2, 0); // asm 0x49CF7B (the seed DISCARDED - fstp st)
                this_->mPathData->state = 4;           // asm 0x49CF88
                goto L_49D01C;                         // asm 0x49CF95
            }
        }
        else
        {
            wrapSeed = (double)distanceToWaypoint3D
                - (double)this_->mPathData->range
                - (double)0.1f;                        // asm 0x49CF9A-0x49CFA7 (fld var_264 - the STORED m32;
                                                       // the chain UNSTORED)
        }
        // asm loc_49CFAD (the shared gate - the UNROUNDED seed in st0):
        if (wrapSeed < 1e-7f)                          // asm 0x49CFAD-0x49CFB8 (fcom flt_6565F4; test ah,1;
                                                       // jz loc_49CFCB - the literal < MATCH row)
        {
            this_->mPathData->state = 4;               // asm 0x49CFBA-0x49CFC2
            goto L_49D01C;                             // asm 0x49CFC9
        }
        // asm loc_49CFCB:
        if (targetObj == 0                             // asm 0x49CFCB-0x49CFCD (jnz loc_49CFEC)
            && wrapSeed < 1.0f                         // asm 0x49CFCF-0x49CFDA (fcomp const_f_one ON THE
                                                       // UNROUNDED SEED; test ah,1; jz loc_49CFEE - the
                                                       // literal < MATCH row)
            && this_->mPathData->waypointLevel != 0)   // asm 0x49CFDC-0x49CFE1 (jz loc_49CFEE)
        {
            this_->mPathData->state = 4;               // asm 0x49CFE3
            goto L_49D01C;                             // asm 0x49CFEA
        }
        // asm loc_49CFEC (fstp st - the seed popped); loc_49CFEE:
        if (distanceToWaypoint3D < 1e-7f)              // asm 0x49CFEE-0x49CFFD (fld var_264 - the STORED
                                                       // m32; test ah,1; jz loc_49D008)
        {
            this_->mPathData->state = 4;               // asm 0x49CFFF
            goto L_49D01C;                             // asm 0x49D006
        }
        // asm loc_49D008:
        this_->mPathData->waypoint = 0;                // asm 0x49D008 (the wrap restarts at the FIRST
                                                       // waypoint)
        this_->mPathData->state = 2;                   // asm 0x49D00F
        // fall to the dispatch
    }
L_49D01C:                                              // asm loc_49D01C (the state dispatch)
    if (this_->mPathData->state == 4)                  // asm 0x49D01C-0x49D028 (+0x1C; jz loc_49D033)
        goto L_49D033;
    if (this_->mPathData->state != 5)                  // asm 0x49D02A-0x49D02D (jnz loc_49D12C)
        goto L_49D12C;
L_49D033:                                              // asm loc_49D033 (states 4/5)
    {
        BVector* notifyWp = BPath__waypoint(path, &waypointScratch,
            path->waypointsCount - 1);                 // asm 0x49D033-0x49D044
        this_->vfptr->checkPositionNotifies_2(this_, notifyWp); // asm 0x49D049-0x49D04C (slot +0x8C = 35)
        BPathData* relink = this_->mPathData->linkedPath; // asm 0x49D052-0x49D05B (+4)
        if (relink == 0)                               // asm 0x49D05D (jz loc_49E1F4 - the shared
                                                       // reset+stop+1 exit, defined at function scope)
            goto L_49E1F4;
        int linkedType = this_->mPathData->linkedPathType; // asm 0x49D063-0x49D068 (+8)
        if (linkedType == 1 || patrolFlag != 0)        // asm 0x49D06B-0x49D076 (jz loc_49E061 x2)
            goto L_49E061;
        if (linkedType != 2)                           // asm 0x49D081-0x49D083 (jnz loc_49D0F8)
            goto L_49D0F8;
        this_->mPathData->linkedPath = 0;              // asm 0x49D085
        if (this_->mPathData != 0)                     // asm 0x49D08C-0x49D094 (the guarded teardown)
        {
            BPathData__destructor(this_->mPathData);   // asm 0x49D096
            operator_delete_internal(this_->mPathData); // asm 0x49D09D-0x49D09E
        }
        // asm loc_49D0A6:
        this_->mPathData = relink;                     // asm 0x49D0A6
        relink->state = 2;                             // asm 0x49D0AC
        if (relink->path != 0                          // asm 0x49D0AF-0x49D0BA (jz loc_49CBEB)
            && (relink->path->flags.valueValue & 1) != 0) // asm 0x49D0C0-0x49D0CB (jbe loc_49CBEB)
        {
            relink->waypoint = BPath__nextPatrolWaypoint(relink->path, relink->waypoint);
                                                           // asm 0x49D0D1-0x49D0E0 (the CURRENT waypoint)
            if (relink->waypoint == -1)                    // asm 0x49D0E3-0x49D0ED (jz loc_49E098)
            {
                // asm loc_49E098 (the wrap-fail exit):
                BPathData__reset(this_->mPathData);        // asm 0x49E098-0x49E09E
                this_->vfptr->stopMoving(this_);           // asm 0x49E0A3-0x49E0A7 (slot +0x7C)
                result = 1;                                // asm loc_49E3DF
                goto L_49E3E4;                             // asm 0x49E0AA
            }
        }
        goto L_49CBEB;                                 // asm 0x49D0F3
L_49E061:                                              // asm loc_49E061 (the plain successor swap)
        this_->mPathData->linkedPath = 0;              // asm 0x49E061-0x49E068 ([eax+4] = 0)
        if (this_->mPathData != 0)                     // asm 0x49E068-0x49E070 (the guarded teardown)
        {
            BPathData__destructor(this_->mPathData);   // asm 0x49E072-0x49E074
            operator_delete_internal(this_->mPathData); // asm 0x49E079-0x49E07A
        }
        // asm loc_49E082:
        BPathData__setFlag(relink, 0, 1);              // asm 0x49E082-0x49E08E (ecx = edi; n = 0, v = 1)
        this_->mPathData = relink;                     // asm 0x49E088 (mov [esi+0A8h], edi)
        goto L_49CBEB;                                 // asm 0x49E093
    }
L_49D0F8:                                              // asm loc_49D0F8 (the linkedType-3 arm)
    {
        BPathData* relink = this_->mPathData->linkedPath; // (edi - unchanged since 0x49D05B; the fresh
                                                          // read is value-identical)
        if (this_->mPathData->linkedPathType != 3)     // asm 0x49D0F8-0x49D0FB (jnz loc_49D131)
            goto L_49D131;
        this_->mPathData->linkedPath = 0;              // asm 0x49D0FD
        if (this_->mPathData != 0)                     // asm 0x49D104-0x49D10C
        {
            BPathData__destructor(this_->mPathData);   // asm 0x49D110
            operator_delete_internal(this_->mPathData); // asm 0x49D115-0x49D116
        }
        // asm loc_49D11E:
        this_->mPathData = relink;                     // asm 0x49D11E
        relink->state = 2;                             // asm 0x49D124
        goto L_49CBEB;                                 // asm 0x49D127
    }
L_49D12C:                                              // asm loc_49D12C (state not 4/5; mov ebx, 2)
    // fall
L_49D131:                                              // asm loc_49D131
    if (this_->mPathData->state != 2)                  // asm 0x49D131-0x49D134 (cmp ebx(2); jnz
                                                       // loc_49DB6A)
        goto L_49DB6A;
    // ---- the state == 2 repath block (asm 0x49D13A-0x49DA75) ----
    {
        RGE_Player* currentPlayer = (RGE_Player*)this_->mPathUnit->owner; // asm 0x49D13A-0x49D140 (+0x18)
        if (doAvailablePathCheck == 1                  // asm 0x49D143-0x49D14D (jnz loc_49D169)
            && RGE_Player__availablePathingAttempts(currentPlayer,
                this_->mNumberWaitDelays) == 0)        // asm 0x49D153-0x49D163 (+0xB4; jz loc_49E0AF)
        {
            // asm loc_49E0AF:
            this_->vfptr->stopMoving(this_);           // asm 0x49E0AF-0x49E0B3 (slot +0x7C)
            ++this_->mNumberWaitDelays;                // asm 0x49E0B6-0x49E0BD (+0xB4)
            result = 0;                                // asm 0x49E0C3
            goto L_49E3E4;
        }
        // asm loc_49D169:
        RGE_Player__incrementPathingAttempts(currentPlayer); // asm 0x49D169-0x49D16B
        this_->mNumberWaitDelays = 0;                  // asm 0x49D170-0x49D178 (+0xB4)
        BPath* linked = BPathData__highestLinkedPathPointer(this_->mPathData); // asm 0x49D17E-0x49D183
        if (linked == 0)                               // asm 0x49D183-0x49D185 (jnz loc_49D1AA)
        {
            linked = RGE_Game_World__path((RGE_Game_World*)(*base_game)->world,
                BPathData__highestLinkedPath(this_->mPathData)); // asm 0x49D187-0x49D1A1
            if (linked == 0)                           // asm 0x49D1A6-0x49D1A8 (jz loc_49D1B7)
                goto L_49D1B7;
        }
        // asm loc_49D1AA:
        if ((linked->flags.valueValue & 1) != 0)       // asm 0x49D1AA-0x49D1B5 (test BBitVectorAND_4; ja
                                                       // loc_49D1CC - only a LIVE PATROL linked path skips
                                                       // the gate)
            goto L_49D1CC;
L_49D1B7:                                              // asm loc_49D1B7 (the give-up gate)
        if (BPathData__continuesLeft(this_->mPathData, 1) <= 0) // asm 0x49D1B7-0x49D1C6 (reduce = 1; jle
                                                                // loc_49E102)
        {
L_49E102:                                              // asm loc_49E102 (the shared failure tail)
            BPathData__setFailure(this_->mPathData);   // asm 0x49E102-0x49E108
            BPathData__reset(this_->mPathData);        // asm 0x49E10D-0x49E113
L_49E118:                                              // asm loc_49E118 (the shared stop exit)
            this_->vfptr->stopMoving(this_);           // asm 0x49E118-0x49E11C (slot +0x7C)
            result = 0;                                // asm 0x49E11F
            goto L_49E3E4;
        }
        // asm loc_49D1CC (the successor allocation; the SEH chunk 0x642650 = the allocation-unwind,
        // regenerated by the guarded-alloc shape):
L_49D1CC:                                              // asm loc_49D1CC
        obj = (BPathData*)operator_new_internal(sizeof(BPathData)); // asm 0x49D1CC-0x49D1D3 (0x34 = sizeof)
        if (obj != 0)                                  // asm 0x49D1DA-0x49D1EC
            BPathData__BPathData(obj);
        if (obj == 0)                                  // asm 0x49D1EE-0x49D1FF (jz loc_49E0CA)
        {
            // asm loc_49E0CA:
            BPathData__setFailure(this_->mPathData);   // asm 0x49E0CA-0x49E0D0
            BPathData__reset(this_->mPathData);        // asm 0x49E0D5-0x49E0DB
            this_->vfptr->stopMoving(this_);           // asm 0x49E0E0-0x49E0E4 (slot +0x7C)
            result = 0;
            goto L_49E3E4;
        }
        newPath = RGE_Game_World__createPath((RGE_Game_World*)(*base_game)->world); // asm
                                                                                    // 0x49D205-0x49D211
        if (newPath == 0)                              // asm 0x49D218-0x49D21C (jz loc_49E0EE)
        {
            // asm loc_49E0EE:
            if (obj != 0)                              // asm 0x49E0EE-0x49E0F0 (jz loc_49E102)
            {
                BPathData__destructor(obj);
                operator_delete_internal(obj);
            }
            goto L_49E102;
        }
        repathArmed = 0;                               // asm 0x49D22A-0x49D22E (var_268)
        smoothUsed = 0;                                // asm 0x49D22E (var_258.x)
        if (RGE_Static_Object__isFlagSet((RGE_Static_Object*)this_->mPathUnit, 1) != 0) // asm 0x49D222-0x49D239 (jnz
                                                                                        // loc_49D27B; the cross-class cast)
            goto L_49D27B;
        // the stub arm: the 2-waypoint path
        BPath__addWaypointAtStart(newPath, &this_->mPosition.mPosition); // asm 0x49D23B-0x49D241
        BVector* stubWp = BPath__waypoint(path, &waypointScratch,
            this_->mPathData->waypoint);               // asm 0x49D246-0x49D25A
        BPath__addWaypointAtEnd(newPath, stubWp);      // asm 0x49D25F-0x49D262
        BPath__calculatePathLength(newPath);           // asm 0x49D267-0x49D269
        repathArmed = 1;                               // asm 0x49D26E
        goto L_49D9AD;                                 // asm 0x49D276 (jmp loc_49D9AD - no
                                                       // validity/findTile stage)
L_49D27B:                                              // asm loc_49D27B (the flagged arm)
        {
            BVector* repathWp = BPath__waypoint(path, &waypointScratch,
                this_->mPathData->waypoint);           // asm 0x49D27B-0x49D298
            BVector__operator_minus(repathWp, &repathDelta,
                &this_->mPosition.mPosition);          // asm 0x49D29D-0x49D29F (this = the wp; the buffer =
                                                       // the -0x230 carve)
            repathDelta.z = 0.0f;                      // asm 0x49D2BA (the +4 slot = .z - the vertical
                                                       // zeroed; FINDING B1)
            distanceToWaypoint = (float)sqrt((double)repathDelta.x * repathDelta.x
                + (double)repathDelta.y * repathDelta.y); // asm 0x49D2A4-0x49D2C8 (fstp var_258.y - the
                                                          // store-round)
            if (this_->mPathData->waypointLevel != 2   // asm 0x49D2C4-0x49D2CE (+0xC; jnz loc_49D4CF)
                || *dword_689568 != 1                  // asm 0x49D2D4-0x49D2DB (jnz loc_49D4CF)
                || !(distanceToWaypoint > *flt_68F494)) // asm 0x49D2E1-0x49D2F0 (fcomp; test ah,41h; jnz
                                                        // loc_49D4CF - the literal > MATCH row: the jnz
                                                        // targets the skip)
                goto L_49D4CF;
            // ---- arm 1: the map-analysis smooth path ----
            mapAnalysis = RGE_Map_Analysis_System__get_map_analysis(
                (RGE_Map_Analysis_System*)(*base_game)->world->map_analysis_system, // asm
                                                                                    // 0x49D2F6-0x49D30E (world+0x74)
                (RGE_Master_Static_Object*)this_->mPathUnit->master_obj); // asm 0x49D2FB-0x49D30A (+0x14;
                                                                          // the cross-class cast)
            if (mapAnalysis == 0)                      // asm 0x49D313-0x49D319 (jz loc_49D46F)
                goto L_49D46F;
            {
                float targetRadius = 0.0f;             // asm 0x49D325 (var_210.z = 0)
                if (this_->mPathData->targetID != -1)  // asm 0x49D32D-0x49D333 (jz loc_49D355)
                {
                    RGE_Static_Object* mapTarget = RGE_Game_World__object(
                        (RGE_Game_World*)(*base_game)->world,
                        this_->mPathData->targetID);   // asm 0x49D335-0x49D342
                    if (mapTarget != 0)                // asm 0x49D347-0x49D349
                        targetRadius = mapTarget->master_obj->radius_x; // asm 0x49D34B-0x49D351 (master
                                                                        // @+0x14; radius_x @+0x3C)
                }
                // asm loc_49D355:
                float radiusSum = (float)((double)targetRadius
                    + (double)this_->mPathUnit->master_obj->radius_x); // asm 0x49D361-0x49D382 (fld
                                                                               // var_210.z; fadd master+0x3C;
                                                                               // fstp into the arg slot -
                                                                               // store)
                BVector* mapWp = BPath__waypoint(path, &waypointScratch,
                    this_->mPathData->waypoint);       // asm 0x49D378-0x49D389
                float destY = mapWp->y;                // asm 0x49D394 ([eax+8])
                BVector* startWp = BPath__waypoint(path, &waypointScratch,
                    this_->mPathData->waypoint);       // asm 0x49D39F-0x49D3A6 (the same waypoint - the
                                                       // redundant fetch)
                int findPathStatus = RGE_Map_Analysis__find_path(mapAnalysis,
                    newPath,
                    this_->mPosition.mPosition.x,
                    this_->mPosition.mPosition.y,
                    startWp->x,
                    destY,
                    radiusSum,
                    this_->mPathData->range,
                    currentPlayer,
                    0, 0) & 0xFF;                      // asm 0x49D3AB-0x49D3BF (the 10 stack args; AND 0FFh -
                                                       // FINDING C1)
                if (findPathStatus == 3                // asm 0x49D3C4-0x49D3C7 (jz loc_49D4A5)
                    || findPathStatus == 4             // asm 0x49D3CD-0x49D3D0 (jz loc_49D4A5)
                    || findPathStatus == 0)            // asm 0x49D3D6-0x49D3D8 (jz loc_49D4A5)
                    goto L_49D4A5;
                smoothUsed = 1;                        // asm 0x49D3E4-0x49D3EB (var_258.x)
                repathArmed = 1;                       // asm 0x49D3EF (var_268)
                smoothMidLevelPath(this_->mPathUnit, newPath); // asm 0x49D3F3
                BVector* smoothedTail = BPath__waypoint(newPath, &waypointScratch,
                    newPath->waypointsCount - 1);      // asm 0x49D3F8-0x49D407
                BVector* originalWp = BPath__waypoint(path, &waypointScratch,
                    this_->mPathData->waypoint);       // asm 0x49D40D-0x49D429
                BVector tailDelta;                     // the -0x19C carve
                BVector__operator_minus(originalWp, &tailDelta, smoothedTail); // asm 0x49D42E-0x49D430 (this
                                                                               // = the original wp)
                tailDelta.z = 0.0f;                    // asm 0x49D444-0x49D44E (the +4 slot = .z; FINDING B2)
                // the norm consumed UNROUNDED by the fcomp (the literal > MATCH row)
                if (sqrt((double)tailDelta.x * tailDelta.x
                    + (double)tailDelta.y * tailDelta.y)
                    > (double)distanceToWaypoint)      // asm 0x49D435-0x49D466 (test ah,41h; jnz loc_49D4A5 -
                                                       // keep)
                    goto L_49D4A5;
                newPath->waypointsCount = 0;           // asm 0x49D468-0x49D46A
                repathArmed = 0;                       // asm 0x49D46D-0x49D4A4 (jmp with eax = 0 - the
                                                       // discard latch)
                goto L_49D4A1;
            }
L_49D46F:                                              // asm loc_49D46F (no map analysis)
            BPath__addWaypointAtStart(newPath, &this_->mPosition.mPosition); // asm 0x49D46F-0x49D472
            BVector* stubWp2 = BPath__waypoint(path, &waypointScratch,
                this_->mPathData->waypoint);           // asm 0x49D477-0x49D48B
            BPath__addWaypointAtEnd(newPath, stubWp2); // asm 0x49D490-0x49D493
            repathArmed = 1;                           // asm 0x49D498-0x49D49D (eax = 1; the fall into
                                                       // loc_49D4A1)
L_49D4A1:                                              // asm loc_49D4A1 (mov var_268, eax - the two arrivals
                                                       // pre-assigned above)
L_49D4A5:                                              // asm loc_49D4A5
            BPath__calculatePathLength(newPath);       // asm 0x49D4A5-0x49D4A7
            newPath->creationTimeValue = (*base_game)->world->world_time; // asm 0x49D4AC-0x49D4C0 (+0x1C)
            if (repathArmed != 0                       // asm 0x49D4B7-0x49D4BD (jz loc_49D4CF)
                && newPath->waypointsCount > 2)        // asm 0x49D4C5-0x49D4C9 (jg loc_49D6AA)
                goto L_49D6AA;                         // (the arm-1 success leftover -> the validity check -
                                                       // the LABEL_110 semantics)
            goto L_49D4CF;
        }
L_49D4CF:                                              // asm loc_49D4CF
        if (*dword_689564 != 1                         // asm 0x49D4CF-0x49D4D6 (jnz loc_49D6AA)
            || distanceToWaypoint > 10.0f)             // asm 0x49D4DC-0x49D4EB (fld var_258.y; fcomp
                                                       // flt_6565F0; test ah,41h; jz loc_49D6AA - the literal
                                                       // > MATCH row: {<=, NaN} falls through to arm 2)
            goto L_49D6AA;
        // ---- arm 2: findLowLevelPath ----
        smoothUsed = 0;                                // asm 0x49D4F1-0x49D4F3 (var_258.x)
        newPath->waypointsCount = 0;                   // asm 0x49D4F7
        int soloProbeSeed = this_->mPathUnit->id;      // asm 0x49D4FA-0x49D509 (unitsToIgnore = the unit's
                                                       // id)
        unitGroup = RGE_Game_World__getUnitGroup((RGE_Game_World*)(*base_game)->world,
            this_->mPathUnit->mGroupID);               // asm 0x49D510-0x49D51D (+0x88)
        if (unitGroup != 0)                            // asm 0x49D522-0x49D524 (jz loc_49D5D9)
        {
            RGE_Master_Moving_Object* currentMaster = this_->mPathUnit->master_obj; // asm
                                                                                    // 0x49D52A-0x49D533
            obstructionManagerToUse = (currentMaster->movement_type == 1)
                ? (RGE_Obstruction_Manager*)*OBSystem_Air : *OBSystem_Land; // asm 0x49D537-0x49D54F (byte
                                                                            // @+0x78; the slot reads)
            ownerId = this_->mPathUnit->owner->id;     // asm 0x49D56C-0x49D579 (+0xA0)
            int terrainIndex = currentMaster->terrain; // asm 0x49D590 (movsx word @+0x76)
            terrainTable = (*base_game)->world->terrains[terrainIndex]; // asm 0x49D594 (world->terrains
                                                                        // @+0x60)
            ignorePtr = unitGroup->mUnits;             // asm 0x49D573 (+0xD8)
            ignoreCount = unitGroup->mNumberUnits;     // asm 0x49D55F (+0xD0)
            unitRadius = currentMaster->radius_x;      // asm 0x49D5B9 (+0x3C)
            lowGoal = BPath__waypoint(path, &waypointScratch,
                this_->mPathData->waypoint);           // asm 0x49D5B4-0x49D5C8
            goto L_49D668;                             // asm 0x49D5D4 (the convergence)
        }
        // asm loc_49D5D9 (the solo variant):
        {
            RGE_Master_Moving_Object* currentMaster = this_->mPathUnit->master_obj; // asm
                                                                                    // 0x49D5D9-0x49D5E2
            obstructionManagerToUse = (currentMaster->movement_type == 1)
                ? (RGE_Obstruction_Manager*)*OBSystem_Air : *OBSystem_Land; // asm 0x49D5E6-0x49D5FD
            ownerId = this_->mPathUnit->owner->id;     // asm 0x49D620 (+0xA0)
            int terrainIndex = currentMaster->terrain; // asm 0x49D627
            terrainTable = (*base_game)->world->terrains[terrainIndex]; // asm 0x49D62B
            unitRadius = currentMaster->radius_x;      // asm 0x49D64A-0x49D650 (+0x3C)
            ignorePtr = &soloProbeSeed;                // asm 0x49D62E (the LOCAL's address)
            ignoreCount = 1;                           // asm 0x49D63E
            lowGoal = BPath__waypoint(path, &waypointScratch,
                this_->mPathData->waypoint);           // asm 0x49D64A-0x49D65C
            // fall into loc_49D668
        }
L_49D668:                                              // asm loc_49D668 (the shared call)
        findLowResult = findLowLevelPath(obstructionManagerToUse,
            &this_->mPosition.mPosition,
            lowGoal,
            unitRadius,
            this_->mPathData->range,
            this_->mPathData->targetID,
            ignorePtr,
            ignoreCount,
            0, 0,                                      // asm 0x49D5A4-0x49D5A6 / 0x49D63A-0x49D63C
                                                       // (collisionUnits = 0, numberCollisionUnits = 0)
            newPath,
            terrainTable,
            false,
            ownerId);                                  // asm 0x49D668 (ecx = the OB system, edx = &mPosition)
        BPath__calculatePathLength(newPath);           // asm 0x49D66D-0x49D673
        newPath->creationTimeValue =
            currentPlayer->world->world_time;          // asm 0x49D678-0x49D685 (owner+0x8C = RGE_Player.world;
                                                       // +0x10 = world_time; FINDING D - the literal chain)
        if (findLowResult == 3                         // asm 0x49D68C-0x49D68F (jz loc_49D6AA)
            || findLowResult == 4                      // asm 0x49D691-0x49D694 (jz loc_49D6AA)
            || X87_LE_NAN(newPath->pathLengthValue, 1e-7f)) // asm 0x49D696-0x49D6A4 (fcomp flt_6565F4; test
                                                            // ah,41h; jz loc_49D81A - the {<=, NaN} fall = the
                                                            // failure arm)
            goto L_49D6AA;
        goto L_49D81A;
L_49D6AA:                                              // asm loc_49D6AA (the LABEL_110 re-test)
        if (repathArmed != 0)                          // asm 0x49D6AA-0x49D6B0 (jnz loc_49D822)
            goto L_49D822;
        if (distanceToWaypoint >= 1.0f                 // asm 0x49D6B6-0x49D6C5 (fcomp const_f_one; test ah,1;
                                                       // jnz loc_49D6DC - the literal < MATCH row)
            && distanceToTarget >= 1.0f)               // asm 0x49D6C7-0x49D6D6 (fcomp const_f_one; test ah,1;
                                                       // jz loc_49D83C - the literal < MATCH row: the jz
                                                       // targets the skip)
            goto L_49D83C;
        // ---- the OBSystem probe arm (loc_49D6DC) ----
        {
            int probeSeed = this_->mPathUnit->id;      // asm 0x49D6DC-0x49D6EB (anonymous_0)
            unitGroup = RGE_Game_World__getUnitGroup((RGE_Game_World*)(*base_game)->world,
                this_->mPathUnit->mGroupID);           // asm 0x49D6F2-0x49D6FF
            if (unitGroup != 0)                        // asm 0x49D704-0x49D706 (jz loc_49D769)
            {
                obstructionManagerToUse =
                    (this_->mPathUnit->master_obj->movement_type == 1)
                    ? (RGE_Obstruction_Manager*)*OBSystem_Air : *OBSystem_Land; // asm
                                                                                // 0x49D708-0x49D729
                BVector* probeWpGroup = BPath__waypoint(path, &waypointScratch,
                    this_->mPathData->waypoint);       // asm 0x49D742-0x49D756
                findObSystemCollisions(obstructionManagerToUse,
                    this_->mPathUnit,
                    &this_->mPosition.mPosition,
                    probeWpGroup,
                    unitGroup->mUnits,
                    unitGroup->mNumberUnits,
                    1, 1.0);                           // asm 0x49D739-0x49D767 (checkTerrain = 1,
                                                       // radiusModifier = 1.0)
                goto L_49D7C3;                         // asm 0x49D767
            }
            // asm loc_49D769 (the solo probe):
            obstructionManagerToUse =
                (this_->mPathUnit->master_obj->movement_type == 1)
                ? (RGE_Obstruction_Manager*)*OBSystem_Air : *OBSystem_Land; // asm 0x49D769-0x49D789
            BVector* probeWp = BPath__waypoint(path, &waypointScratch,
                this_->mPathData->waypoint);           // asm 0x49D78D-0x49D7B2
            findObSystemCollisions(obstructionManagerToUse,
                this_->mPathUnit,
                &this_->mPosition.mPosition,
                probeWp,
                &probeSeed,                            // asm 0x49D79A-0x49D79D (the LOCAL's address)
                1, 1, 1.0);                            // asm 0x49D798-0x49D7BD
            // fall into loc_49D7C3
        }
L_49D7C3:                                              // asm loc_49D7C3
        if (*numberObSystemCollisions != 0)            // asm 0x49D7C8-0x49D7CF (jnz loc_49D83C)
            goto L_49D83C;
        newPath->waypointsCount = 0;                   // asm 0x49D7D1-0x49D7D4
        BPath__addWaypointAtStart(newPath, &this_->mPosition.mPosition); // asm 0x49D7D6-0x49D7DB
        BVector* fallbackWp = BPath__waypoint(path, &waypointScratch,
            this_->mPathData->waypoint);               // asm 0x49D7E0-0x49D7F4
        BPath__addWaypointAtEnd(newPath, fallbackWp);  // asm 0x49D7F9-0x49D7FC
        newPath->creationTimeValue = (*base_game)->world->world_time; // asm 0x49D801-0x49D810
        BPath__calculatePathLength(newPath);           // asm 0x49D813-0x49D815
        // fall into loc_49D81A
L_49D81A:                                              // asm loc_49D81A
        repathArmed = 1;                               // asm 0x49D81A (var_268 = 1)
        // fall into the validity check
L_49D822:                                              // asm loc_49D822 (the validity check)
        if (newPath->waypointsCount >= 2               // asm 0x49D822-0x49D826 (+0xC; jl loc_49D83C)
            && newPath->pathLengthValue >= 0.01f)      // asm 0x49D828-0x49D836 (fcomp flt_654978; test ah,1;
                                                       // jz loc_49D9AD - the literal >= MATCH row)
            goto L_49D9AD;                             // -> the install gate
        goto L_49D83C;
L_49D83C:                                              // asm loc_49D83C (the findTilePath arm)
        if (*dword_68956C != 1)                        // asm 0x49D83C-0x49D843 (jnz loc_49D9AD)
            goto L_49D9AD;
        newPath->waypointsCount = 0;                   // asm 0x49D849
        {
            BVector* tileWp = BPath__waypoint(path, &waypointScratch,
                this_->mPathData->waypoint);           // asm 0x49D850-0x49D87C
            int tileGoalY = (int)tileWp->y;            // asm 0x49D881-0x49D884 (fld [eax+8]; __ftol)
            int tileGoalX = (int)tileWp->x;            // asm 0x49D8A3-0x49D8A5 (fld [eax]; __ftol)
            int tileStartY = (int)this_->mPosition.mPosition.y; // asm 0x49D8AA-0x49D8AE (__ftol)
            int tileStartX = (int)this_->mPosition.mPosition.x; // asm 0x49D8B3-0x49D8BB (__ftol)
            repathArmed = (unsigned)PathingSystem__findTilePath(pathSystem,
                tileStartX, tileStartY, tileGoalX, tileGoalY,
                this_->mPathUnit,
                this_->mPathData->range,               // asm 0x49D86F (+0x20)
                this_->mPathData->targetID,            // asm 0x49D868 (+0x24)
                0,                                     // asm 0x49D866 (rLength)
                -1, -1,                                // asm 0x49D862-0x49D864 (unobstructiblePlayerID,
                                                       // unobstructibleGroupID)
                newPath,                               // asm 0x49D858 (rPath)
                false) & 0xFF;                         // asm 0x49D856 (useRangeWithoutTarget; 0x49D8CA AND
                                                       // 0FFh - FINDING C2; the result store 0x49D8CF; the
                                                       // explicit cast: the prototype's bool return + the byte
                                                       // mask = the asm's raw-eax low byte)
        }
        newPath->creationTimeValue =
            currentPlayer->world->world_time;          // asm 0x49D8D3-0x49D8DC (FINDING D)
        BPath__calculatePathLength(newPath);           // asm 0x49D8DF-0x49D8E1
        if (X87_LT_NAN(newPath->pathLengthValue, 1e-7f)) // asm 0x49D8E6-0x49D8F4 (fcomp flt_6565F4; test ah,1;
                                                         // jz loc_49D903 - the {<, NaN} fall = the re-arm)
        {
            repathArmed = 0;                           // asm 0x49D8F6-0x49D8FE
            goto L_49D9AD;                             // asm 0x49D8FE
        }
        // asm loc_49D903 (the tail quality check):
        {
            BVector* newTail = BPath__waypoint(newPath, &waypointScratch,
                newPath->waypointsCount - 1);          // asm 0x49D903-0x49D912
            BVector* originalWp = BPath__waypoint(path, &waypointScratch,
                this_->mPathData->waypoint);           // asm 0x49D917-0x49D934
            BVector tailDelta2;                        // var_190 (the aligned buffer)
            BVector__operator_minus(originalWp, &tailDelta2, newTail); // asm 0x49D939-0x49D93B
            tailDelta2.z = 0.0f;                       // asm 0x49D96B (the aligned buffer's +4 = .z; FINDING
                                                       // B3)
            // both norms UNROUNDED into the fcompp; the {<, NaN} set discards
            if (X87_LT_NAN(sqrt((double)repathDelta.x * repathDelta.x
                + (double)repathDelta.y * repathDelta.y
                + (double)repathDelta.z * repathDelta.z),
                sqrt((double)tailDelta2.x * tailDelta2.x
                    + (double)tailDelta2.y * tailDelta2.y))) // asm 0x49D940-0x49D9A3 (repathDelta.z = 0 from
                                                             // B1; test ah,1; jz loc_49D9AD - keep)
            {
                newPath->waypointsCount = 0;           // asm 0x49D9A5-0x49D9AA
                newPath->pathLengthValue = 0.0f;       // asm 0x49D9AA (+0x18)
            }
            // fall to the install gate
        }
L_49D9AD:                                              // asm loc_49D9AD (the install gate)
        if (newPath->waypointsCount > 1)                // asm 0x49D9AD-0x49D9B7 (jg loc_49DA7A)
            goto L_49DA7A;
        {
            // ---- the failure arms (asm 0x49D9BD-0x49DA75) ----
            if (obj != 0)                              // asm 0x49D9BD-0x49D9C9 (the guarded teardown)
            {
                BPathData__destructor(obj);
                operator_delete_internal(obj);
            }
            RGE_Game_World__destroyPath2((RGE_Game_World*)(*base_game)->world, newPath); // asm
                                                                                         // 0x49D9CE-0x49D9DD
            if (repathArmed == 1)                      // asm 0x49D9E2-0x49D9E7 (jnz loc_49DA0E)
            {
                if ((path->flags.valueValue & 1) == 0)  // asm 0x49D9E9-0x49D9F4 (jbe loc_49E126)
                {
                    // asm loc_49E126:
                    this_->vfptr->stopMoving(this_);        // asm 0x49E126-0x49E12A (slot +0x7C)
                    BPathData__reset(this_->mPathData);     // asm 0x49E12D-0x49E133
                    result = 1;                             // asm loc_49E3DF
                    goto L_49E3E4;                          // asm 0x49E138
                }
                BPathData__setFlag(this_->mPathData, 0, 1); // asm 0x49D9FA-0x49DA04
                goto L_49CBEB;                              // asm 0x49DA09
            }
            // asm loc_49DA0E:
            if (this_->mPathData->waypoint >= path->waypointsCount - 1) // asm 0x49DA0E-0x49DA1B (jge
                                                                                // loc_49DA2B)
            {
                // asm loc_49DA2B:
                if (BPathData__continuesLeft(this_->mPathData, 0) > 0) // asm 0x49DA2B-0x49DA34 (jg
                                                                       // loc_49E118)
                    goto L_49E118;
                if ((path->flags.valueValue & 1) != 0)  // asm 0x49DA3A-0x49DA44 (ja loc_49E13D)
                {
                    // asm loc_49E13D:
                    this_->mPathData->pauseTime = 0.5f;     // asm 0x49E13D-0x49E143 (the DIRECT write - no
                                                            // pauseMovement vtbl member)
                    result = 0;                             // asm (falls into loc_49E14A)
                    goto L_49E3E4;
                }
                if (this_->mPathData->waypointLevel >= 2    // asm 0x49DA4A-0x49DA54 (+0xC; jge loc_49E102)
                    || BPathData__highestLinkedPathHasMoreWaypoints(this_->mPathData) != 1) // asm
                                                                                            // 0x49DA5A-0x49DA62
                    goto L_49E102;
                this_->mPathData->state = 5;                // asm 0x49DA68-0x49DA6E (+0x1C)
                goto L_49CBEB;                              // asm 0x49DA75
            }
            BPathData__setFlag(this_->mPathData, 0, 1);     // asm 0x49DA1D-0x49DA26 (the waypoint < count-1
                                                            // arm: setFlag(0, 1))
            goto L_49CBEB;
        }
L_49DA7A:                                              // asm loc_49DA7A (the install success)
        this_->mPathData->state = 0;                   // asm 0x49DA7A-0x49DA80 (+0x1C)
        this_->mPathData->continueCounter = 10;        // asm 0x49DA87-0x49DA8D (+0x2C = 0Ah)
        if (this_->mPathData->waypointLevel == 2       // asm 0x49DA94-0x49DAA4 (+0xC; var_258.x = smoothUsed)
            && smoothUsed == 1)
        {
            BPathData__setReadyToPath(obj, newPath,
                this_->mPathData->range,               // asm 0x49DAA9-0x49DAB1 (+0x20)
                this_->mPathData->targetID);           // asm (+0x24)
            BPathData__setLinkedPath(obj, this_->mPathData,
                (repathArmed == 1) ? 1 : 2);           // asm 0x49DABA-0x49DAD5 (the two pushes)
            obj->waypointLevel = 1;                    // asm 0x49DADA (+0xC)
        }
        else
        {
            BPathData__setReadyToFollowPath(obj, newPath); // asm 0x49DB17-0x49DB1E
            BPathData__setLinkedPath(obj, this_->mPathData,
                (repathArmed == 1) ? 1 : 2);           // asm 0x49DB23-0x49DB3C
            obj->waypointLevel = 0;                    // asm 0x49DB43 (+0xC)
        }
        obj->range = this_->mPathData->range;          // asm 0x49DADD-0x49DAE6 / 0x49DB4A-0x49DB53 (+0x20)
        obj->targetID = this_->mPathData->targetID;    // asm 0x49DAE9-0x49DAF2 / 0x49DB56-0x49DB5F (+0x24)
        this_->mPathData = obj;                        // asm 0x49DAF5 / 0x49DB62 (+0xA8)
        path = newPath;                                // asm 0x49DB00 / 0x49DB68 (ebp = edi - the active
                                                       // path is the successor)
        if (*dword_689564 == 1                         // asm 0x49DAFB-0x49DB04 (jz loc_49CBEB)
            || *dword_68956C == 1)                     // asm 0x49DB0A-0x49DB12 (jnz loc_49DB6A)
            goto L_49CBEB;
        // fall into the movement arm (asm jnz loc_49DB6A)
    }
L_49DB6A:                                              // asm loc_49DB6A (the movement arm)
    {
        BVector* moveWp = BPath__waypoint(path, &waypointScratch,
            this_->mPathData->waypoint);               // asm 0x49DB6A-0x49DB81
        BVector newPosDelta;                           // the -0x1A8 carve
        BVector__operator_minus(&this_->mPosition.mPosition, &newPosDelta, moveWp); // asm
                                                                                    // 0x49DB86-0x49DB91
        newPosDelta.z = 0.0f;                          // asm 0x49DBAF (the carve's +4 = .z - the vertical
                                                       // zeroed)
        if (X87_LT_NAN(sqrt((double)newPosDelta.x * newPosDelta.x
            + (double)newPosDelta.y * newPosDelta.y), 1e-7f)) // asm 0x49DB96-0x49DBCB (the norm UNROUNDED
                                                              // into the fcomp; test ah,1; jz loc_49DBF0)
        {
            BPathData__setFlag(this_->mPathData, 1, 1); // asm 0x49DBCD-0x49DBD7 (loc_49DBCD)
            BPathData__setFlag(this_->mPathData, 0, 1); // asm 0x49DBDC-0x49DBE6
            goto L_49CBEB;                              // asm 0x49DBEB
        }
        // ---- the movement integration (asm 0x49DBF0-0x49DDF6) ----
        {
            BVector* integrationWp = BPath__waypoint(path, &waypointScratch,
                this_->mPathData->waypoint);            // asm 0x49DBF0-0x49DC04
            BVector moveDelta;
            BVector__operator_minus(integrationWp, &moveDelta,
                &this_->mPosition.mPosition);           // asm 0x49DC09-0x49DC11 (this = the wp)
            double moveNorm = sqrt((double)moveDelta.x * moveDelta.x
                + (double)moveDelta.y * moveDelta.y);   // asm 0x49DC16-0x49DC4F (the planar x^2+y^2;
                                                        // consumed UNROUNDED by the three fdivs)
            dirNormalized.x = (float)((double)moveDelta.x / moveNorm);  // asm 0x49DC51-0x49DC57 (the
                                                                        // -0x23C carve slot+0)
            dirNormalized.z = (float)(0.0 / moveNorm);                  // asm 0x49DC5B-0x49DC63 (slot+4 -
                                                                        // const_f_zero / norm)
            dirNormalized.y = (float)((double)moveDelta.y / moveNorm);  // asm 0x49DC67-0x49DC6D (slot+8)
            // THE ST0-BOUNDARY DISCIPLINE: the asm duplicates the returned delta with fld st
            // (0x49DC7E) and consumes the RAW copy UNROUNDED in BOTH multiplies - no m32 store
            double timeDeltaRaw = RGE_Game_World__getWorldTimeDeltaSeconds(
                (RGE_Game_World*)(*base_game)->world);  // asm 0x49DC71-0x49DC79
            float displacement = (float)(((double)timeDeltaRaw * this_->mAcceleration.y
                + this_->mVelocity.y) * timeDeltaRaw);  // asm 0x49DC7E-0x49DC94 (mAcceleration.y @+0x78;
                                                        // mVelocity.y @+0x6C; ONE fstp var_27C store)
            // the scalar operator_mul INLINED (the moveForward precedent - the free operator_mul is
            // un-exported; the element-wise a * v)
            dirScaled.z = dirNormalized.z * displacement;   // asm 0x49DC99-region (the result = the
                                                            // -0x84 carve)
            dirScaled.y = dirNormalized.y * displacement;
            dirScaled.x = dirNormalized.x * displacement;
            BVector__operator_plus(&this_->mPosition.mPosition, &plusResult, &dirScaled); // asm
                                                                                          // 0x49DC9E-0x49DCA9
            integratedPos = plusResult;                 // asm 0x49DCBC-0x49DCD7 (the -0x1B4 -> -0x1C0 copy)
            BVector* freshWp = BPath__waypoint(path, &waypointScratch,
                this_->mPathData->waypoint);            // asm 0x49DCDE-0x49DCEC
            BVector__operator_minus(freshWp, &wpMinusPos,
                &this_->mPosition.mPosition);           // asm 0x49DCFA-0x49DCFE (p2 = wp - pos)
            BVector__operator_minus(freshWp, &wpMinusIntegrated,
                &integratedPos);                        // asm 0x49DD0A-0x49DD12
            BVector__operator_minus(&integratedPos, &integratedMinusPos,
                &this_->mPosition.mPosition);           // asm 0x49DD17-0x49DD27
            wpMinusPos.z = 0.0f;                        // asm 0x49DD48
            wpMinusIntegrated.z = 0.0f;                 // asm 0x49DD53
            double wpDist = sqrt((double)wpMinusPos.x * wpMinusPos.x
                + (double)wpMinusPos.y * wpMinusPos.y); // asm 0x49DD2C-0x49DD5D (planar; UNROUNDED into
                                                        // both compares)
            double integratedDist = sqrt((double)integratedMinusPos.x * integratedMinusPos.x
                + (double)integratedMinusPos.z * integratedMinusPos.z
                + (double)integratedMinusPos.y * integratedMinusPos.y); // asm 0x49DD5F-0x49DD8D (the FULL
                                                                        // 3-comp; st0 UNROUNDED)
            integratedDistStored = (float)integratedDist; // asm 0x49DD8F (fst var_258.z - the store the 0.1
                                                          // gate re-reads)
            if (X87_LE_NAN(integratedDist, wpDist))     // asm 0x49DD93-0x49DD9A (fcomp st(1) on the UNROUNDED
                                                        // st0; test ah,41h; jnz loc_49DDF6 -> the OBSystem
                                                        // fall; loc_49DDF6 = fstp st, NO flags)
            {
                goto L_49DDF8;
            }
            // the advance checks: BOTH gates' failure joins the shared loc_49DBCD setFlags+loop block
            bool advance = (wpDist >= sqrt((double)wpMinusIntegrated.y * wpMinusIntegrated.y
                + (double)wpMinusIntegrated.x * wpMinusIntegrated.x))   // asm 0x49DD9C-0x49DDBB (fcompp on
                                                                        // the UNROUNDED values; test ah,1; jnz
                                                                        // loc_49DBCD - the literal >= MATCH row)
                && integratedDistStored >= 0.1f;         // asm 0x49DDC1-0x49DDD0 (fld var_258.z = the ROUNDED
                                                         // re-read; fcomp flt_6545BC; test ah,1; jnz
                                                         // loc_49DBCD)
            BPathData__setFlag(this_->mPathData, 1, 1); // asm 0x49DDDC-0x49DDE4 / loc_49DBCD-region (the
                                                        // duplicated tail)
            BPathData__setFlag(this_->mPathData, 0, 1); // asm 0x49DDE5-0x49DDEF / loc_49DBCD-region
            if (advance)
                goto L_49DDF8;                          // asm 0x49DDF4 (jmp loc_49DDF8 - the OBSystem arm)
            goto L_49CBEB;                              // asm (the loc_49DBCD re-loop)
        }
    }
L_49DDF8:                                              // asm loc_49DDF8 (the OBSystem pass)
    resetIgnoreList_new();                             // asm 0x49DDF8 (the _new direct call per the test
                                                       // config)
    addToIgnoreList_new((RGE_Static_Object*)this_->mPathUnit); // asm 0x49DDFD-0x49DE03 (obj = mPathUnit)
    unitGroup = RGE_Game_World__getUnitGroup((RGE_Game_World*)(*base_game)->world,
        this_->mPathUnit->mGroupID);                   // asm 0x49DE08-0x49DE21
    if (unitGroup != 0 && unitGroup->mNumberUnits > 0) // asm 0x49DE28-0x49DE3C (jz/jle loc_49DE77)
    {
        int gi = 0;                                    // ebx - the member loop counter
        do                                             // asm loc_49DE42 (the +4 walk = the indexed form)
        {
            RGE_Static_Object* groupMember = RGE_Game_World__object(
                (RGE_Game_World*)(*base_game)->world,
                unitGroup->mUnits[gi]);                // asm 0x49DE42-0x49DE55 ([eax])
            addToIgnoreList_new(groupMember);          // asm 0x49DE5C
            ++gi;                                      // asm 0x49DE6B
        } while (gi < unitGroup->mNumberUnits);        // asm 0x49DE6F-0x49DE75 (jl loc_49DE42)
    }
    // asm loc_49DE77:
    obstructionManagerToUse = (this_->mPathUnit->master_obj->movement_type == 1)
        ? (RGE_Obstruction_Manager*)*OBSystem_Air : *OBSystem_Land; // asm 0x49DE7D-0x49DE8C (byte @+0x78)
    findObSystemCollisions(obstructionManagerToUse,
        this_->mPathUnit,
        &this_->mPosition.mPosition,                   // asm 0x49DEAF-0x49DEB2 (p1 = esi+4)
        &integratedPos,                                // asm 0x49DEA2 (p2 = the -0x1C0 carve)
        ignoreUnits,                                   // asm 0x49DEA9 (offset ignoreUnits - the owner-fixed
                                                       // argument)
        ignoreListSize,                                // asm 0x49DE94-0x49DEA1 (the owned global)
        0, 1.0);                                       // asm 0x49DEA0/0x49DE9B (checkTerrain = 0,
                                                       // radiusModifier = 1.0)
    if (*numberObSystemCollisions > 0)                 // asm 0x49DEB8-0x49DEBF (jle loc_49DFFF)
    {
        collisionTargetFound = 0;                      // asm 0x49DEC5-0x49DECD (var_258.x)
        freshBlockers = 0;                             // asm 0x49DEC9-0x49DED1 (var_25C)
        int recordIndex = 0;                           // asm 0x49DEC5-0x49DED8 (var_260 = the index; the
                                                       // var_264 +4 walk = the indexed form)
        do                                             // asm loc_49DED9 (the entry guaranteed by the > 0 gate)
        {
            ObsRecord* rec = obSystemCollisions[recordIndex]; // asm 0x49DED1-0x49DEDD (the entries =
                                                              // ObsRecord*)
            RGE_Static_Object* hitObject = rec->Object; // asm 0x49DEDF-0x49DEE2 (ObsRecord.Object @+8)
            if (hitObject != 0)                         // asm 0x49DEE2-0x49DEE4 (jz loc_49DF5F)
            {
                RGE_Static_Object* blocker = RGE_Game_World__object(
                    (RGE_Game_World*)(*base_game)->world,
                    hitObject->id);                     // asm 0x49DEE6-0x49DEF6 ([+0x10] = the blocker's
                                                        // own id)
                if (blocker != 0                        // asm 0x49DEFD-0x49DEFF (jz loc_49DF5F)
                    && BPath__ignoreUnitsContain(path, blocker->id) != 1) // asm 0x49DF01-0x49DF0F (jz = skip)
                {
                    BPath* highest = BPathData__highestLinkedPathPointer(this_->mPathData); // asm
                                                                                            // 0x49DF11-0x49DF1E
                    if (highest == 0
                        || BPath__ignoreUnitsContain(highest, blocker->id) != 1) // asm 0x49DF20-0x49DF2E (jz =
                                                                                                 // skip)
                    {
                        if (this_->mPathData->targetID >= 0
                            && blocker->id == this_->mPathData->targetID) // asm 0x49DF30-0x49DF40
                            collisionTargetFound = 1;   // asm 0x49DF42 (var_258.x)
                        if (path->creationTimeValue
                            < blocker->vfptr->lastMoveTime(blocker)) // asm 0x49DF4A-0x49DF57 (slot +0x30C = 195
                                                                     // lastMoveTime - the header VERIFIED by the
                                                                     // owner; the eax consumption; creationTimeValue
                                                                     // @path+0x1C; jnb skip)
                            ++freshBlockers;            // asm 0x49DF5B (var_25C)
                    }
                }
            }
            ++recordIndex;                              // asm 0x49DF6C (inc ecx)
        } while (recordIndex < *numberObSystemCollisions); // asm 0x49DF6F-0x49DF7A (jl loc_49DED9)
        if (*numberObSystemCollisions == 1
            && collisionTargetFound == 1)               // asm 0x49DF80-0x49DF8A (jnz loc_49DFAF x2)
        {
            BPathData__setFlag(this_->mPathData, 0, 1); // asm 0x49DF8C-0x49DF96
            BPathData__setFlag(this_->mPathData, 2, 1); // asm 0x49DF9B-0x49DFA5
            goto L_49CBEB;                              // asm 0x49DFAA
        }
        // asm loc_49DFAF:
        if (freshBlockers > 0)                          // asm 0x49DFAF-0x49DFB7 (jle loc_49DFFF)
        {
            BPathData* relink = this_->mPathData->linkedPath; // asm 0x49DFB9-0x49DFC4 (+4)
            if (relink == 0)                            // asm 0x49DFC4-0x49DFC6 (jz loc_49E1F4)
                goto L_49E1F4;
            relink->state = 2;                          // asm 0x49DFCA (+0x1C)
            this_->mPathData->linkedPath = 0;           // asm 0x49DFD1-0x49DFD7 (+4)
            if (this_->mPathData != 0)                  // asm 0x49DFDA-0x49DFE2 (the guarded teardown)
            {
                BPathData__destructor(this_->mPathData);
                operator_delete_internal(this_->mPathData);
            }
            this_->mPathData = relink;                  // asm 0x49DFF4
            goto L_49CBEB;                              // asm 0x49DFFA
        }
    }
    // asm loc_49DFFF (the targetObj gate):
    if (targetObj == 0)                                 // asm 0x49DFFF-0x49E003 (jz loc_49E151)
        goto L_49E151;
    {
        RGE_Static_Object* freshTarget =
            RGE_Game_World__object((RGE_Game_World*)(*base_game)->world,
                this_->mPathData->targetID);            // asm 0x49E009-0x49E01E (the fresh fetch)
        // THE ST0-BOUNDARY DISCIPLINE: the call's st0 -> fsub range -> fabs -> fcomp dbl_6545B0;
        // NO m32 store - the compare consumes the UNROUNDED value
        double freshDist = fabs(RGE_Static_Object__distance_to_position(freshTarget,
            this_->mPosition.mPosition.x,
            this_->mPosition.mPosition.y,
            this_->mPosition.mPosition.z)
            - (double)this_->mPathData->range);         // asm 0x49E023-0x49E041
        if (!(freshDist >= (double)dbl_6545B0))         // asm 0x49E041-0x49E04C (fcomp dbl_6545B0; test ah,1;
                                                        // jz loc_49E20B - the literal >= MATCH row; NaN -> the
                                                        // flag arm in both)
        {
            BPathData__setFlag(this_->mPathData, 0, 1); // asm 0x49E052-0x49E057 (n = ebx = 0, v = 1)
            goto L_49CBEB;                              // asm 0x49E05C
        }
        goto L_49E20B;                                  // asm (the facing arm)
    }
L_49E151:                                              // asm loc_49E151 (the no-target arrival)
    if ((path->flags.valueValue & 1) != 0)              // asm 0x49E151-0x49E15C (ja loc_49E20B)
        goto L_49E20B;
    if (X87_LE_NAN(this_->mPathData->range, 0.1f))      // asm 0x49E162-0x49E176 (fld [ecx+20h]; fcomp
                                                        // flt_6545BC; test ah,41h; jnz loc_49E20B - the {<=,
                                                        // NaN} taken set)
        goto L_49E20B;
    {
        BPath* highest = BPathData__highestLinkedPathPointer(this_->mPathData); // asm 0x49E17C-0x49E183
        if (highest == 0)                               // asm 0x49E181-0x49E183 (jz loc_49E20B)
            goto L_49E20B;
        BVector* lastLinkedWp = BPath__waypoint(highest, &waypointScratch,
            highest->waypointsCount - 1);               // asm 0x49E189-0x49E198 (count @+0xC)
        BVector arrivalDelta;                           // var_224
        BVector__operator_minus(&this_->mPosition.mPosition, &arrivalDelta,
            lastLinkedWp);                              // asm 0x49E19D-0x49E1A6 (this = &mPosition)
        // the FULL 3-comp norm in the asm's ((y^2)+(z^2))+x^2 order, UNROUNDED into the fsub/fcomp
        double arrivalDist = sqrt((double)arrivalDelta.y * arrivalDelta.y
            + (double)arrivalDelta.z * arrivalDelta.z
            + (double)arrivalDelta.x * arrivalDelta.x); // asm 0x49E1AB-0x49E1CD
        arrivalDist -= (double)this_->mPathData->range; // asm 0x49E1D3 (fsub range - still unrounded)
        if (arrivalDist >= 1e-7f)                       // asm 0x49E1D6-0x49E1E1 (fcomp flt_6565F4; test ah,1;
                                                        // jz loc_49E20B - the literal >= MATCH row)
            goto L_49E20B;
        BPathData__reset(this_->mPathData);             // asm 0x49E1E3-0x49E1E9
        this_->vfptr->stopMoving(this_);                // asm 0x49E1E8-0x49E1EC (slot +0x7C)
        result = 1;                                     // asm loc_49E3DF
        goto L_49E3E4;                                  // asm 0x49E1EF
    }
// asm loc_49E1F4 (the shared reset+stop+1 exit - hoisted to function scope; the two gotos enter it
// without crossing any initialization):
L_49E1F4:                                              // asm loc_49E1F4
    BPathData__reset(this_->mPathData);                 // asm 0x49E1F4-0x49E1FA
    this_->vfptr->stopMoving(this_);                    // asm 0x49E1FF-0x49E203 (slot +0x7C)
    result = 1;                                         // asm loc_49E3DF
    goto L_49E3E4;                                      // asm 0x49E206
L_49E20B:                                              // asm loc_49E20B (the facing arm)
    {
        // ---- the current-forward normalize (asm 0x49E20B-0x49E263) ----
        // the source = mPosition.mOrientationForward (+0x10/.x, +0x18/.y); the middle component (.z
        // @+0x14) is fetched at 0x49E21B but its slot is overwritten by the 0.0 - a dead store,
        // deliberately not written
        double facingNorm = sqrt((double)this_->mPosition.mOrientationForward.x
            * this_->mPosition.mOrientationForward.x
            + (double)this_->mPosition.mOrientationForward.y
            * this_->mPosition.mOrientationForward.y);  // asm 0x49E21E-0x49E23B (x^2+y^2 planar; fsqrt,
                                                        // consumed UNROUNDED by the fdivs)
        facingDirCur.x = (float)((double)this_->mPosition.mOrientationForward.x / facingNorm);
                                                        // asm 0x49E23D-0x49E243 (fdiv; fstp - store)
        facingDirCur.z = (float)(0.0 / facingNorm);     // asm 0x49E247-0x49E24F (const_f_zero / norm; fstp -
                                                        // the +4 slot = .z; the division kept - the existing
                                                        // movement-arm form)
        facingDirCur.y = (float)((double)this_->mPosition.mOrientationForward.y / facingNorm);
                                                        // asm 0x49E253-0x49E259
        // ---- the frame delta + the yaw (asm 0x49E265-0x49E27C) ----
        facingTimeDelta = (float)RGE_Game_World__getWorldTimeDeltaSeconds(
            (RGE_Game_World*)(*base_game)->world);      // asm 0x49E265-0x49E26A (fstp var_258.z - the
                                                        // STORE-ROUND is the original's own here)
        facingYaw = (float)BVector__angleBetweenVector(&dirNormalized, &facingDirCur);
                                                        // asm 0x49E26E-0x49E27C (this = dirNormalized, v =
                                                        // facingDirCur; the st0 return; ONE fstp var_268 store)
        // ---- the clamp (asm 0x49E280-0x49E29F) ----
        facingYawCap = (double)facingTimeDelta * this_->mMaximumYawPerSecondMoving;
                                                        // asm 0x49E280-0x49E284 (fld var_258.z - the ROUNDED
                                                        // delta; fmul @+0x9C - the 53-bit product)
        if (facingYaw > facingYawCap)                   // asm 0x49E28A-0x49E295 (fld var_268; fcomp st(1); test
                                                        // ah,41h; jnz loc_49E29D - keep - the literal > MATCH
                                                        // row)
            facingYaw = (float)facingYawCap;            // asm 0x49E297 (fstp var_268 - the clamp store)
        // else asm loc_49E29D: fstp st (the cap discarded; the angle kept)
        // ---- the cross product + the sign (asm 0x49E29F-0x49E2D3) ----
        BVector__operator_mul(&dirNormalized, &facingCross, &facingDirCur);
                                                        // asm 0x49E29F-0x49E2B0 (this = dirNormalized; result =
                                                        // the -0x1B4 carve; v = facingDirCur; the cross)
        if (facingCross.z > 0.0f)                       // asm 0x49E2B5-0x49E2C7 (fld the cross .z - the +4 slot;
                                                        // fcomp const_f_zero; test ah,41h; jnz loc_49E2D3 - the
                                                        // literal > MATCH row)
            facingYaw = -facingYaw;                     // asm 0x49E2C9-0x49E2CF (fld; fchs; fstp)
        // ---- the deadband (asm 0x49E2D3-0x49E2E4) ----
        if (X87_LE_NAN(fabs((double)facingYaw), dbl_6545B0)) // asm 0x49E2D3-0x49E2E4 (fld var_268; fabs; fcomp
                                                             // dbl_6545B0; test ah,41h; jnz loc_49E3D1 - the {<=,
                                                             // NaN} skip)
            goto L_49E3D1;
        // ---- the rotation (asm 0x49E2EA-0x49E3C9) ----
        BMatrix__makeRotateArbitrary(&facingRotate, facingYaw,
            &this_->mNewPosition.mOrientationUp);       // asm 0x49E2EA-0x49E2FA (rads = var_268; axis = esi+4Ch =
                                                        // mOrientationUp; the matrix = the -0x3C scratch)
        BMatrix__operator_mul(&facingRotate, &rotScratchF,
            &this_->mNewPosition.mOrientationForward);  // asm 0x49E2FF-0x49E312 (vec = esi+40h; the result =
                                                        // var_170; eax = the result ptr)
        this_->mNewPosition.mOrientationForward = rotScratchF; // asm 0x49E317-0x49E32A (the 12-byte copy [eax]
                                                               // -> [edi])
        BMatrix__operator_mul(&facingRotate, &rotScratchR,
            &this_->mNewPosition.mOrientationRight);    // asm 0x49E32D-0x49E33C (vec = esi+58h; the result =
                                                        // var_54)
        this_->mNewPosition.mOrientationRight = rotScratchR;   // asm 0x49E341-0x49E350
        {
            // the forward renorm - the asm's ((y^2+z^2)+x^2) association (0x49E353-0x49E389); the norm
            // consumed UNROUNDED by the fdivs
            double n = sqrt((double)this_->mNewPosition.mOrientationForward.y
                * this_->mNewPosition.mOrientationForward.y
                + (double)this_->mNewPosition.mOrientationForward.z
                * this_->mNewPosition.mOrientationForward.z
                + (double)this_->mNewPosition.mOrientationForward.x
                * this_->mNewPosition.mOrientationForward.x);
            this_->mNewPosition.mOrientationForward.x =
                (float)((double)this_->mNewPosition.mOrientationForward.x / n); // asm 0x49E373-0x49E378
            this_->mNewPosition.mOrientationForward.z =
                (float)((double)this_->mNewPosition.mOrientationForward.z / n); // asm 0x49E379-0x49E37E
            this_->mNewPosition.mOrientationForward.y =
                (float)((double)this_->mNewPosition.mOrientationForward.y / n); // asm 0x49E381-0x49E386
        }
        {
            // the right renorm - the asm's ((x^2+z^2)+y^2) association - DIFFERENT from the forward's
            // (0x49E38B-0x49E3C1)
            double n = sqrt((double)this_->mNewPosition.mOrientationRight.x
                * this_->mNewPosition.mOrientationRight.x
                + (double)this_->mNewPosition.mOrientationRight.z
                * this_->mNewPosition.mOrientationRight.z
                + (double)this_->mNewPosition.mOrientationRight.y
                * this_->mNewPosition.mOrientationRight.y);
            this_->mNewPosition.mOrientationRight.x =
                (float)((double)this_->mNewPosition.mOrientationRight.x / n); // asm 0x49E3AB-0x49E3B0
            this_->mNewPosition.mOrientationRight.z =
                (float)((double)this_->mNewPosition.mOrientationRight.z / n); // asm 0x49E3B1-0x49E3B6
            this_->mNewPosition.mOrientationRight.y =
                (float)((double)this_->mNewPosition.mOrientationRight.y / n); // asm 0x49E3B9-0x49E3BE
        }
        this_->vfptr->setFlag(this_, 0, 1);             // asm 0x49E3C1-0x49E3CB (slot +0x98 = 38; INSIDE the
                                                        // gate)
        // fall into loc_49E3D1
    }
L_49E3D1:                                              // asm loc_49E3D1 (the gate join)
    this_->vfptr->setVelocityForward(this_, this_->mMaximumVelocityForward);
                                                        // asm 0x49E3D1-0x49E3DC (slot +0x28 = 10; @+0xB0)
    result = 1;                                         // asm loc_49E3DF (mov eax, 1)
    // fall into the epilogue
L_49E3E4:                                              // asm loc_49E3E4 (the shared epilogue)
    return result;                                     // asm 0x49E3E4-0x49E3FC (the SEH chain restore; add
                                                       // esp,268h; retn 8)
}

// ---- THE ARM TRACE (diagnostics-only, owner-directed 2026-11-19 - the
// desync localization: the captured window starts after the root, so the
// _ida body's per-arm decisions are dumped to ida_arm_log.txt). The ORIG
// side's decisions are inferable from val_log_orig.txt's post fields (the
// state/wp changes + the path shapes). THE CRT EXCEPTION (removed before
// the final ship).
static void BMotion_ida_armlog(const char* tag, BMotion* this_, float a1, float a2)
{
    static FILE* f = 0;
    if (!f)
        f = fopen("ida_arm_log.txt", "at");
    if (f)
        fprintf(f, "%s uid=%d wt=%u a1=%.9g a2=%.9g\n", tag,
            this_->mPathUnit ? this_->mPathUnit->id : -1,
            (*base_game)->world->world_time, a1, a2);
}

int __fastcall BMotion__updateMovement_new_ida(BMotion* this_, DWORD dummy, int doAvailablePathCheck, BUnitGroup* uG)
{
    BPathData* v4; // edi
    BPathData* v6; // ecx
    BPathData* v7; // eax
    BPath* v8; // ebp
    BPathData* v9; // ecx
    RGE_Static_Object* v10; // ebx
    BPathData* v11; // ecx
    int v12; // eax
    BMotionVtbl* v13; // edi
    BVector* v14; // eax
    BVector* v15; // eax
    BPathData* v16; // edi
    BPathData* v17; // ecx
    int v18; // edx
    RGE_Moving_Object* v19; // eax
    double v20; // st7
    float v21; // edx
    int v22; // eax
    BVector* v23; // eax
    BPathData* v24; // edi
    double v25; // st7
    BPathData* v26; // eax
    int v27; // ecx
    BMotionVtbl* v28; // edi
    BVector* v29; // eax
    BPathData* v30; // edx
    BPathData* v31; // edi
    int v32; // ecx
    BPathData* v33; // ebp
    BPathData* v34; // eax
    BPath* v35; // ecx
    BPathData* v36; // ebp
    RGE_Player* v37; // edi
    BPathData* v38; // ecx
    BPathData* v39; // ebx
    BPath* v40; // eax
    RGE_Game_World* v41; // edi
    int v42; // eax
    BPathData* v43; // eax
    BPath* v44; // edi
    BVector* v45; // eax
    BPosition* v46; // ebx
    BVector* v47; // eax
    BPathData* v48; // edx
    BPathData* v49; // eax
    int v50; // eax
    RGE_Static_Object* v51; // eax
    RGE_Moving_Object* v52; // eax
    BPathData* v53; // ecx
    BVector* v54; // eax
    int v55; // eax
    BVector* v56; // eax
    int v57; // eax
    BVector* v58; // eax
    RGE_Moving_Object* v59; // eax
    BUnitGroup* v60; // eax
    TRIBE_World* v61; // ecx
    BPathData* v62; // eax
    BVector* v63; // eax
    int v64; // eax
    BPathData* v65; // eax
    BVector* v66; // eax
    RGE_Moving_Object* v67; // eax
    BUnitGroup* v68; // eax
    BVector* v69; // eax
    BVector* v70; // eax
    BVector* v71; // eax
    BPathData* v72; // eax
    BVector* v73; // eax
    BPathData* v74; // ecx
    BPathData* v75; // ecx
    BPathData* v76; // eax
    BVector* v77; // eax
    BVector* v78; // eax
    long double v79; // st7
    double v80; // st7
    BVector* v81; // eax
    BPathData* v82; // ecx
    BVector* v83; // ebx
    long double v84; // st7
    long double v85; // st6
    BUnitGroup* v86; // eax
    BUnitGroup* v87; // edi
    int v88; // ebx
    RGE_Static_Object* v89; // eax
    RGE_Moving_Object* v90; // edx
    RGE_Obstruction_Manager* v91; // ecx
    RGE_Static_Object* v92; // eax
    RGE_Static_Object* v93; // edi
    BPath* v94; // eax
    int v95; // eax
    unsigned int v96; // ebx
    bool v97; // cc
    BPathData* v98; // edi
    BPathData* v99; // ebx
    RGE_Static_Object* v100; // eax
    BPathData* v101; // edi
    BPathData* v102; // ebp
    BPathData* v103; // ecx
    BPath* v104; // eax
    BVector* v105; // eax
    BPathData* v106; // ecx
    long double v107; // st7
    double v108; // st7
    BVector* v109; // edi
    long double v110; // st4
    long double v111; // st7
    float v112; // [esp-8h] [ebp-2A4h]
    float v113; // [esp-8h] [ebp-2A4h]
    float v114; // [esp-4h] [ebp-2A0h]
    float v115; // [esp-4h] [ebp-2A0h]
    int v116; // [esp-4h] [ebp-2A0h]
    int dest_target_radius; // [esp+0h] [ebp-29Ch]
    int dest_target_radiusa; // [esp+0h] [ebp-29Ch]
    int dest_target_radiusb; // [esp+0h] [ebp-29Ch]
    RGE_Moving_Object* v120; // [esp+4h] [ebp-298h]
    int v121; // [esp+8h] [ebp-294h]
    float v122; // [esp+8h] [ebp-294h]
    float v123; // [esp+Ch] [ebp-290h]
    int v124; // [esp+Ch] [ebp-290h]
    float a; // [esp+10h] [ebp-28Ch]
    float v126; // [esp+14h] [ebp-288h]
    int* v127; // [esp+14h] [ebp-288h]
    RGE_Player* v128; // [esp+18h] [ebp-284h]
    float* v129; // [esp+18h] [ebp-284h]
    float* v130; // [esp+18h] [ebp-284h]
    int v131; // [esp+18h] [ebp-284h]
    BVector* v132; // [esp+20h] [ebp-27Ch]
    int v133; // [esp+20h] [ebp-27Ch]
    int v134; // [esp+20h] [ebp-27Ch]
    BVector* v135; // [esp+20h] [ebp-27Ch]
    int v136; // [esp+20h] [ebp-27Ch]
    int v137; // [esp+20h] [ebp-27Ch]
    float v138; // [esp+20h] [ebp-27Ch]
    float v139; // [esp+34h] [ebp-268h]
    bool v140; // [esp+34h] [ebp-268h]
    float v141; // [esp+34h] [ebp-268h]
    float v142; // [esp+38h] [ebp-264h]
    float v143; // [esp+38h] [ebp-264h]
    RGE_Master_Moving_Object* v144; // [esp+38h] [ebp-264h]
    RGE_Obstruction_Manager* v145; // [esp+38h] [ebp-264h]
    int v146; // [esp+38h] [ebp-264h]
    int* v147; // [esp+38h] [ebp-264h]
    ObsRecord** v148; // [esp+38h] [ebp-264h]
    bool v149; // [esp+3Ch] [ebp-260h]
    RGE_Map_Analysis* v150; // [esp+3Ch] [ebp-260h]
    RGE_Obstruction_Manager* v151; // [esp+3Ch] [ebp-260h]
    RGE_Master_Moving_Object* v152; // [esp+3Ch] [ebp-260h]
    RGE_Obstruction_Manager* v153; // [esp+3Ch] [ebp-260h]
    RGE_Obstruction_Manager* v154; // [esp+3Ch] [ebp-260h]
    int v155; // [esp+3Ch] [ebp-260h]
    BPathData* v156; // [esp+40h] [ebp-25Ch]
    int v157; // [esp+40h] [ebp-25Ch]
    int v158; // [esp+44h] [ebp-258h]
    int v159; // [esp+44h] [ebp-258h]
    int* v160; // [esp+48h] [ebp-254h]
    float v161; // [esp+48h] [ebp-254h]
    float v162; // [esp+48h] [ebp-254h]
    float v163; // [esp+4Ch] [ebp-250h]
    RGE_Player* v164; // [esp+50h] [ebp-24Ch]
    BVector v; // [esp+54h] [ebp-248h] BYREF
    BVector v166; // [esp+60h] [ebp-23Ch] BYREF
    BVector v167; // [esp+6Ch] [ebp-230h] BYREF
    BVector v168; // [esp+78h] [ebp-224h] BYREF
    RGE_Static_Object* v169; // [esp+84h] [ebp-218h]
    int v170; // [esp+88h] [ebp-214h]
    BVector v171; // [esp+8Ch] [ebp-210h]
    float v172; // [esp+98h] [ebp-204h]
    BVector v173; // [esp+9Ch] [ebp-200h] BYREF
    BVector v174; // [esp+A8h] [ebp-1F4h] BYREF
    BVector v175; // [esp+B4h] [ebp-1E8h] BYREF
    BVector v176; // [esp+C0h] [ebp-1DCh] BYREF
    BVector p2; // [esp+CCh] [ebp-1D0h] BYREF
    int v178; // [esp+D8h] [ebp-1C4h]
    BVector v179; // [esp+DCh] [ebp-1C0h] BYREF
    BVector v180; // [esp+E8h] [ebp-1B4h] BYREF
    BVector v181; // [esp+F4h] [ebp-1A8h] BYREF
    BVector v182; // [esp+100h] [ebp-19Ch] BYREF
    BVector v183; // [esp+10Ch] [ebp-190h] BYREF
    BVector v184; // [esp+118h] [ebp-184h] BYREF
    int unitsToIgnore; // [esp+124h] [ebp-178h] BYREF
    BVector v186; // [esp+12Ch] [ebp-170h] BYREF
    int v187; // [esp+138h] [ebp-164h] BYREF
    BVector v188; // [esp+140h] [ebp-15Ch] BYREF
    BVector v189; // [esp+14Ch] [ebp-150h] BYREF
    BVector v190; // [esp+158h] [ebp-144h] BYREF
    BVector v191; // [esp+164h] [ebp-138h] BYREF
    BVector v192; // [esp+170h] [ebp-12Ch] BYREF
    BVector v193; // [esp+17Ch] [ebp-120h] BYREF
    BVector v194; // [esp+188h] [ebp-114h] BYREF
    BVector v195; // [esp+194h] [ebp-108h] BYREF
    BVector v196; // [esp+1A0h] [ebp-FCh] BYREF
    BVector v197; // [esp+1ACh] [ebp-F0h] BYREF
    BVector v198; // [esp+1B8h] [ebp-E4h] BYREF
    BVector v199; // [esp+1C4h] [ebp-D8h] BYREF
    BVector v200; // [esp+1D0h] [ebp-CCh] BYREF
    BVector v201; // [esp+1DCh] [ebp-C0h] BYREF
    BVector v202; // [esp+1E8h] [ebp-B4h] BYREF
    BVector v203; // [esp+1F4h] [ebp-A8h] BYREF
    BVector outVector; // [esp+200h] [ebp-9Ch] BYREF
    BVector v205; // [esp+20Ch] [ebp-90h] BYREF
    BVector v206; // [esp+218h] [ebp-84h] BYREF
    BVector v207; // [esp+224h] [ebp-78h] BYREF
    BVector v208; // [esp+230h] [ebp-6Ch] BYREF
    BVector v209; // [esp+23Ch] [ebp-60h] BYREF
    BVector v210; // [esp+248h] [ebp-54h] BYREF
    BVector v211; // [esp+254h] [ebp-48h] BYREF
    BMatrix v212; // [esp+260h] [ebp-3Ch] BYREF
    int v213; // [esp+298h] [ebp-4h]

    v4 = this_->mPathData;
    if (!v4)
        return 0;
    if (v4->pauseTime > 0.0)
    {
        this_->mPathData->pauseTime = v4->pauseTime
            - RGE_Game_World__getWorldTimeDeltaSeconds((RGE_Game_World*)(*base_game)->world);
        v6 = this_->mPathData;
        if (v6->pauseTime > 0.0)
        {
            this_->vfptr->stopMoving(this_);
            return 0;
        }
        v6->pauseTime = 0.0;
    }
    v170 = 0;
    while (1)
    {
        while (1)
        {
            while (1)
            {
                while (1)
                {
                    while (1)
                    {
                        while (1)
                        {
                            if (++v170 > 20)
                                return 0;
                            v7 = this_->mPathData;
                            v8 = v7->path;
                            if (!v8)
                            {
                                v8 = RGE_Game_World__path((RGE_Game_World*)(*base_game)->world, v7->pathID);
                                if (!v8)
                                    return 0;
                                this_->mPathData->path = v8;
                            }
                            v9 = this_->mPathData;
                            v10 = 0;
                            v171.x = 3.4028235e38;
                            v169 = 0;
                            if (v9->targetID > -1)
                            {
                                v10 = RGE_Game_World__object((RGE_Game_World*)(*base_game)->world, v9->targetID);
                                v169 = v10;
                                if (v10)
                                    v171.x = RGE_Static_Object__distance_to_position(
                                        v10,
                                        this_->mPosition.mPosition.x,
                                        this_->mPosition.mPosition.y,
                                        this_->mPosition.mPosition.z);
                            }
                            v11 = this_->mPathData;
                            v12 = v11->mFlags.valueValue;
                            v149 = (v12 & 4) != 0;
                            if ((v12 & 1) != 0)
                            {
                                if ((v12 & 2) != 0)
                                {
                                    if (v11->waypoint < v8->waypointsCount)
                                    {
                                        v13 = this_->vfptr;
                                        v14 = BPath__waypoint(v8, &v205, v11->waypoint);
                                        BVector tmp = { v14->x, v14->z, v14->y };
                                        v13->setPosition(this_, tmp);
                                    }
                                    BPathData__setFlag(this_->mPathData, 1, 0);
                                }
                                BPathData__setFlag(this_->mPathData, 0, 0);
                                ++this_->mPathData->waypoint;
                                if ((1 & v8->flags.valueValue) != 0)
                                {
                                    this_->mPathData->waypoint = BPath__nextPatrolWaypoint(v8, this_->mPathData->waypoint - 1);
                                    this_->mPathData->state = 2;
                                    if (this_->mPathData->waypoint == -1)
                                    {
                                        v19 = this_->mPathUnit;
                                        v20 = v19->world_y;
                                        v21 = v19->world_x;
                                        v22 = v8->waypointsCount;
                                        v184.y = v20;
                                        v184.x = v21;
                                        v184.z = 0.0;
                                        v23 = BPath__waypoint(v8, &v201, v22 - 1);
                                        BVector__operator_minus(&v184, &v175, v23);
                                        v143 = sqrt((double)v175.x * v175.x + (double)v175.z * v175.z + (double)v175.y * v175.y);
                                        if (v10)
                                        {
                                            v24 = this_->mPathData;
                                            v25 = RGE_Static_Object__distance_to_position(
                                                v10,
                                                this_->mPathUnit->world_x,
                                                0.0,
                                                this_->mPathUnit->world_y)
                                                - v24->range
                                                - 0.1f;
                                            if ((v24->mFlags.valueValue & 4) != 0)
                                            {
                                                BPathData__setFlag(v24, 2, 0);
                                                this_->mPathData->state = 4;
                                                goto LABEL_51;
                                            }
                                        }
                                        else
                                        {
                                            v24 = this_->mPathData;
                                            v25 = (double)v143 - (double)v24->range - (double)0.1f;
                                        }
                                        if (v25 >= 1e-7f)
                                        {
                                            if (v10 || v25 >= 1.0f || !v24->waypointLevel)
                                            {
                                                if (v143 >= 1e-7f)
                                                {
                                                    v24->waypoint = 0;
                                                    this_->mPathData->state = 2;
                                                    BMotion_ida_armlog("patrol.wrap", this_, (float)v143, (float)v25);
                                                }
                                                else
                                                {
                                                    v24->state = 4;
                                                }
                                            }
                                            else
                                            {
                                                v24->state = 4;
                                            }
                                        }
                                        else
                                        {
                                            this_->mPathData->state = 4;
                                        }
                                    }
                                }
                                else
                                {
                                    v15 = BPath__waypoint(v8, &v208, this_->mPathData->waypoint - 1);
                                    BVector__operator_minus(&this_->mPosition.mPosition, &v174, v15);
                                    v142 = sqrt((double)v174.x * v174.x + (double)v174.z * v174.z + (double)v174.y * v174.y);
                                    if (v10)
                                    {
                                        v16 = this_->mPathData;
                                        v139 = RGE_Static_Object__distance_to_position(
                                            v10,
                                            this_->mPosition.mPosition.x,
                                            this_->mPosition.mPosition.y,
                                            this_->mPosition.mPosition.z)
                                            - v16->range
                                            - 0.1f;
                                        if ((v16->mFlags.valueValue & 4) != 0)
                                        {
                                            v139 = 0.0;
                                            BPathData__setFlag(v16, 2, 0);
                                        }
                                    }
                                    else
                                    {
                                        v139 = (float)((double)v142 - (double)this_->mPathData->range - (double)0.1f);
                                    }
                                    v17 = this_->mPathData;
                                    v18 = v17->waypoint;
                                    if (v18 < v8->waypointsCount)
                                    {
                                        if (v17->waypointLevel > 0)
                                        {
                                            v17->state = 2;
                                            BMotion_ida_armlog("inc.pass", this_, v142, v139);
                                        }
                                    }
                                    else if (v139 >= 1e-7f)
                                    {
                                        if (v10 || v139 >= 1.0f || !v17->waypointLevel)
                                        {
                                            if (v142 >= 1e-7f)
                                            {
                                                v17->waypoint = v18 - 1;
                                                this_->mPathData->state = 2;
                                                BMotion_ida_armlog("inc.repath", this_, v142, v139);
                                            }
                                            else
                                            {
                                                v17->state = 4;
                                            }
                                        }
                                        else
                                        {
                                            v17->state = 4;
                                        }
                                    }
                                    else
                                    {
                                        v17->state = 4;
                                    }
                                }
                            }
                        LABEL_51:
                            v26 = this_->mPathData;
                            v27 = v26->state;
                            if (v27 != 4 && v27 != 5)
                                break;
                            v28 = this_->vfptr;
                            v29 = BPath__waypoint(v8, &v191, v8->waypointsCount - 1);
                            v28->checkPositionNotifies_2(this_, v29);
                            v30 = this_->mPathData;
                            v31 = v30->linkedPath;
                            if (!v31)
                                goto LABEL_194;
                            v26 = this_->mPathData;
                            v32 = v30->linkedPathType;
                            if (v32 == 1 || v149)
                            {
                                v30->linkedPath = 0;
                                v102 = this_->mPathData;
                                if (v102)
                                {
                                    BPathData__destructor(this_->mPathData);
                                    operator_delete_internal(v102);
                                }
                                this_->mPathData = v31;
                                BPathData__setFlag(v31, 0, 1);
                            }
                            else if (v32 == 2)
                            {
                                v30->linkedPath = 0;
                                v33 = this_->mPathData;
                                if (v33)
                                {
                                    BPathData__destructor(this_->mPathData);
                                    operator_delete_internal(v33);
                                }
                                this_->mPathData = v31;
                                v31->state = 2;
                                v34 = this_->mPathData;
                                v35 = v34->path;
                                if (v35)
                                {
                                    if ((v35->flags.valueValue & 1) != 0)
                                    {
                                        this_->mPathData->waypoint = BPath__nextPatrolWaypoint(v35, v34->waypoint);
                                        if (this_->mPathData->waypoint == -1)
                                        {
                                        LABEL_194:
                                            BPathData__reset(this_->mPathData);
                                            this_->vfptr->stopMoving(this_);
                                            return 1;
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if (v32 != 3)
                                    break;
                                v30->linkedPath = 0;
                                v36 = this_->mPathData;
                                if (v36)
                                {
                                    BPathData__destructor(this_->mPathData);
                                    operator_delete_internal(v36);
                                }
                                this_->mPathData = v31;
                                v31->state = 2;
                            }
                        }
                        if (v26->state != 2)
                            goto LABEL_155;
                        v37 = (RGE_Player*)this_->mPathUnit->owner;
                        v164 = v37;
                        if (doAvailablePathCheck == 1 && !RGE_Player__availablePathingAttempts(v37, this_->mNumberWaitDelays))
                        {
                            this_->vfptr->stopMoving(this_);
                            ++this_->mNumberWaitDelays;
                            return 0;
                        }
                        RGE_Player__incrementPathingAttempts(v37);
                        v38 = this_->mPathData;
                        v39 = 0;
                        this_->mNumberWaitDelays = 0;
                        v40 = BPathData__highestLinkedPathPointer(v38);
                        if (!v40
                            && (v41 = (RGE_Game_World*)(*base_game)->world,
                                v42 = BPathData__highestLinkedPath(this_->mPathData),
                                (v40 = RGE_Game_World__path(v41, v42)) == 0)
                            || (v40->flags.valueValue & 1) == 0)
                        {
                            if (BPathData__continuesLeft(this_->mPathData, 1) <= 0)
                                goto LABEL_198;
                        }
                        v43 = (BPathData*)operator_new_internal(sizeof(BPathData));
                        v213 = 0;
                        if (v43)
                            v39 = BPathData__BPathData(v43);
                        v156 = v39;
                        v213 = -1;
                        if (!v39)
                        {
                            BPathData__setFailure(this_->mPathData);
                            BPathData__reset(this_->mPathData);
                            this_->vfptr->stopMoving(this_);
                            return 0;
                        }
                        v44 = RGE_Game_World__createPath((RGE_Game_World*)(*base_game)->world);
                        if (!v44)
                        {
                            BPathData__destructor(v39);
                            operator_delete_internal(v39);
                        LABEL_198:
                            BPathData__setFailure(this_->mPathData);
                            BPathData__reset(this_->mPathData);
                        LABEL_199:
                            this_->vfptr->stopMoving(this_);
                            return 0;
                        }
                        v140 = 0;
                        v158 = 0;
                        if (!RGE_Static_Object__isFlagSet((RGE_Static_Object*)this_->mPathUnit, 1))
                        {
                            BPath__addWaypointAtStart(v44, &this_->mPosition.mPosition);
                            v45 = BPath__waypoint(v8, &v207, this_->mPathData->waypoint);
                            BPath__addWaypointAtEnd(v44, v45);
                            BPath__calculatePathLength(v44);
                            v140 = 1;
                            goto LABEL_132;
                        }
                        v46 = &this_->mPosition;
                        v47 = BPath__waypoint(v8, &v193, this_->mPathData->waypoint);
                        BVector__operator_minus(v47, &v167, &this_->mPosition.mPosition);
                        v48 = this_->mPathData;
                        v167.z = 0.0;
                        v163 = sqrt((double)v167.x * v167.x + (double)v167.y * v167.y);
                        BMotion_ida_armlog("st2.d2wp", this_, v163, (float)v48->waypointLevel);
                        if (v48->waypointLevel == 2 && *dword_689568 == 1 && v163 > (double)*flt_68F494)
                        {
                        BMotion_ida_armlog("arm1.enter", this_, v163, (float)this_->mPathData->targetID);
                            v150 = RGE_Map_Analysis_System__get_map_analysis(
                                (*base_game)->world->map_analysis_system,
                                (RGE_Master_Static_Object*)this_->mPathUnit->master_obj);
                            if (v150)
                            {
                                v49 = this_->mPathData;
                                v171.z = 0.0;
                                v50 = v49->targetID;
                                if (v50 != -1)
                                {
                                    v51 = RGE_Game_World__object((RGE_Game_World*)(*base_game)->world, v50);
                                    if (v51)
                                        v171.z = v51->master_obj->radius_x;
                                }
                                v52 = this_->mPathUnit;
                                v53 = this_->mPathData;
                                v128 = (RGE_Player*)v52->owner;
                                v126 = v53->range;
                                a = (float)((double)v171.z + (double)v52->master_obj->radius_x);
                                v123 = BPath__waypoint(v8, &v203, v53->waypoint)->y;
                                v54 = BPath__waypoint(v8, &v195, this_->mPathData->waypoint);
                                v55 = RGE_Map_Analysis__find_path(
                                    v150,
                                    v44,
                                    v46->mPosition.x,
                                    this_->mPosition.mPosition.y,
                                    v54->x,
                                    v123,
                                    a,
                                    v126,
                                    v128,
                                    0,
                                    0);
                                if (v55 != 3 && v55 != 4)
                                {
                                    if (v55)
                                    {
                                        v158 = 1;
                                        v140 = 1;
                                        smoothMidLevelPath(this_->mPathUnit, v44);
                                        v132 = BPath__waypoint(v44, &v211, v44->waypointsCount - 1);
                                        v56 = BPath__waypoint(v8, &v197, this_->mPathData->waypoint);
                                        BVector__operator_minus(v56, &v182, v132);
                                        v182.z = 0.0;
                                        if (sqrt((double)v182.y * v182.y + (double)v182.x * v182.x) > v163)
                                        {
                                            v57 = 0;
                                            v44->waypointsCount = 0;
                                            goto LABEL_93;
                                        }
                                    }
                                }
                            }
                            else
                            {
                                BPath__addWaypointAtStart(v44, &this_->mPosition.mPosition);
                                v58 = BPath__waypoint(v8, &v188, this_->mPathData->waypoint);
                                BPath__addWaypointAtEnd(v44, v58);
                                v57 = 1;
                                v158 = 1;
                            LABEL_93:
                                v140 = v57;
                            }
                            BPath__calculatePathLength(v44);
                            v44->creationTimeValue = (*base_game)->world->world_time;
                            if (v140 && v44->waypointsCount > 2)
                                goto LABEL_110;
                        }
                        if (*dword_689564 == 1 && v163 <= 10.0f)
                        {
                        BMotion_ida_armlog("arm2.enter", this_, v163, (float)this_->mPathData->waypointLevel);
                            v158 = 0;
                            v44->waypointsCount = 0;
                            v59 = this_->mPathUnit;
                            unitsToIgnore = v59->id;
                            v60 = RGE_Game_World__getUnitGroup((RGE_Game_World*)(*base_game)->world, v59->mGroupID);
                            if (v60)
                            {
                                v144 = this_->mPathUnit->master_obj;
                                v151 = (RGE_Obstruction_Manager*)(v144->movement_type == 1 ? (RGE_Obstruction_Manager*)*OBSystem_Air : *OBSystem_Land);
                                v61 = (*base_game)->world;
                                v178 = v60->mNumberUnits;
                                v160 = v60->mUnits;
                                v133 = v164->id;
                                v62 = this_->mPathData;
                                v129 = v61->terrains[v144->terrain];
                                v121 = v178;
                                dest_target_radius = v62->targetID;
                                v114 = v62->range;
                                v112 = v144->radius_x;
                                v63 = BPath__waypoint(v8, &v199, v62->waypoint);
                                v64 = findLowLevelPath(
                                    v151,
                                    &this_->mPosition.mPosition,
                                    v63,
                                    v112,
                                    v114,
                                    dest_target_radius,
                                    v160,
                                    v121,
                                    0,
                                    0,
                                    v44,
                                    v129,
                                    0,
                                    v133);
                            }
                            else
                            {
                                v152 = this_->mPathUnit->master_obj;
                                v145 = v152->movement_type == 1 ? (RGE_Obstruction_Manager*)*OBSystem_Air : *OBSystem_Land;
                                v65 = this_->mPathData;
                                v134 = v164->id;
                                v130 = (*base_game)->world->terrains[v152->terrain];
                                dest_target_radiusa = v65->targetID;
                                v115 = v65->range;
                                v113 = v152->radius_x;
                                v66 = BPath__waypoint(v8, &v209, v65->waypoint);
                                v64 = findLowLevelPath(
                                    v145,
                                    &this_->mPosition.mPosition,
                                    v66,
                                    v113,
                                    v115,
                                    dest_target_radiusa,
                                    &unitsToIgnore,
                                    1,
                                    0,
                                    0,
                                    v44,
                                    v130,
                                    0,
                                    v134);
                            }
                            v146 = v64;
                            BPath__calculatePathLength(v44);
                            v44->creationTimeValue = v164->world->world_time;
                            if (v146 != 3 && v146 != 4 && v44->pathLengthValue > 1e-7f)
                                goto LABEL_124;
                        }
                    LABEL_110:
                        if (v140)
                            goto LABEL_125;
                        if (X87_LT_NAN(v163, 1.0f) || X87_LT_NAN(v171.x, 1.0f))
                        {
                        BMotion_ida_armlog("arm3.enter", this_, v163, v171.x);
                            v67 = this_->mPathUnit;
                            v187 = v67->id;
                            v68 = RGE_Game_World__getUnitGroup((RGE_Game_World*)(*base_game)->world, v67->mGroupID);
                            if (v68)
                            {
                                v153 = this_->mPathUnit->master_obj->movement_type == 1 ? (RGE_Obstruction_Manager*)*OBSystem_Air : *OBSystem_Land;
                                v131 = v68->mNumberUnits;
                                v127 = v68->mUnits;
                                v69 = BPath__waypoint(v8, &v189, this_->mPathData->waypoint);
                                findObSystemCollisions(v153, this_->mPathUnit, &this_->mPosition.mPosition, v69, v127, v131, 1, 1.0);
                            }
                            else
                            {
                                v154 = this_->mPathUnit->master_obj->movement_type == 1 ? (RGE_Obstruction_Manager*)*OBSystem_Air : *OBSystem_Land;
                                v70 = BPath__waypoint(v8, &v190, this_->mPathData->waypoint);
                                findObSystemCollisions(v154, this_->mPathUnit, &this_->mPosition.mPosition, v70, &v187, 1, 1, 1.0);
                            }
                            if (!*numberObSystemCollisions)    // asm 0x49D7C8-0x49D7CF (jnz loc_49D83C)
                            {
                                v44->waypointsCount = 0;
                                BPath__addWaypointAtStart(v44, &this_->mPosition.mPosition);
                                v71 = BPath__waypoint(v8, &v192, this_->mPathData->waypoint);
                                BPath__addWaypointAtEnd(v44, v71);
                                v44->creationTimeValue = (*base_game)->world->world_time;
                                BPath__calculatePathLength(v44);
                            LABEL_124:
                                v140 = 1;
                            LABEL_125:
                                if (v44->waypointsCount >= 2 && v44->pathLengthValue >= 0.01f)
                                    goto LABEL_132;
                                goto LABEL_127;
                            }
                        }
                    LABEL_127:
                        if (*dword_68956C == 1)
                        {
                        BMotion_ida_armlog("findtile.enter", this_, v163, (float)v140);
                            v44->waypointsCount = 0;
                            v72 = this_->mPathData;
                            v124 = v72->targetID;
                            v122 = v72->range;
                            v120 = this_->mPathUnit;
                            dest_target_radiusb = (int)BPath__waypoint(v8, &v194, v72->waypoint)->y;
                            v116 = (int)BPath__waypoint(v8, &v196, this_->mPathData->waypoint)->x;
                            v140 = PathingSystem__findTilePath(
                                pathSystem,
                                (int)v46->mPosition.x,
                                (int)this_->mPosition.mPosition.y,
                                v116,
                                dest_target_radiusb,
                                v120,
                                v122,
                                v124,
                                0,
                                -1,
                                -1,
                                v44,
                                0);
                            v44->creationTimeValue = v164->world->world_time;
                            BPath__calculatePathLength(v44);
                            if (v44->pathLengthValue >= 1e-7f)
                            {
                                v135 = BPath__waypoint(v44, &v198, v44->waypointsCount - 1);
                                v73 = BPath__waypoint(v8, &v200, this_->mPathData->waypoint);
                                BVector__operator_minus(v73, &v183, v135);
                                v171.y = v167.y;
                                v172 = v167.z;
                                v183.z = 0.0;
                                if (X87_LT_NAN(sqrt((double)v167.x * v167.x + (double)v167.y * v167.y + (double)v167.z * v167.z), sqrt((double)v183.x * v183.x + (double)v183.y * v183.y)))
                                {
                                    v44->waypointsCount = 0;
                                    v44->pathLengthValue = 0.0;
                                }
                            }
                            else
                            {
                                v140 = 0;
                            }
                        }
                    LABEL_132:
                        BMotion_ida_armlog("install.gate", this_, (float)v44->waypointsCount, v44->pathLengthValue);
                        if (v44->waypointsCount > 1)
                            break;
                        BPathData__destructor(v156);
                        operator_delete_internal(v156);
                        RGE_Game_World__destroyPath2((RGE_Game_World*)(*base_game)->world, v44);
                        if (v140)
                        {
                            if ((v8->flags.valueValue & 1) == 0)
                            {
                                this_->vfptr->stopMoving(this_);
                                BPathData__reset(this_->mPathData);
                                return 1;
                            }
                            BPathData__setFlag(this_->mPathData, 0, 1);
                        }
                        else
                        {
                            v74 = this_->mPathData;
                            if (v74->waypoint >= v8->waypointsCount - 1)
                            {
                                if (BPathData__continuesLeft(v74, 0) > 0)
                                    goto LABEL_199;
                                if ((v8->flags.valueValue & 1) != 0)
                                {
                                    this_->mPathData->pauseTime = 0.5;
                                    return 0;
                                }
                                v75 = this_->mPathData;
                                if (v75->waypointLevel >= 2 || BPathData__highestLinkedPathHasMoreWaypoints(v75) != 1)
                                    goto LABEL_198;
                                this_->mPathData->state = 5;
                            }
                            else
                            {
                                BPathData__setFlag(v74, 0, 1);
                            }
                        }
                    }
                    this_->mPathData->state = 0;
                    this_->mPathData->continueCounter = 10;
                    v76 = this_->mPathData;
                    if (v76->waypointLevel != 2 || v158 != 1)
                        break;
                    BPathData__setReadyToPath(v156, v44, v76->range, v76->targetID);
                    if (v140)
                        v136 = 1;
                    else
                        v136 = 2;
                    BPathData__setLinkedPath(v156, this_->mPathData, v136);
                    v156->waypointLevel = 1;
                    v156->range = this_->mPathData->range;
                    v156->targetID = this_->mPathData->targetID;
                    this_->mPathData = v156;
                    v8 = v44;
                    if (*dword_689564 != 1 && *dword_68956C != 1)
                        goto LABEL_155;
                }
                BPathData__setReadyToFollowPath(v156, v44);
                v137 = v140 ? 1 : 2;
                BPathData__setLinkedPath(v156, this_->mPathData, v137);
                v156->waypointLevel = 0;
                v156->range = this_->mPathData->range;
                v156->targetID = this_->mPathData->targetID;
                this_->mPathData = v156;
                v8 = v44;
            LABEL_155:
                v77 = BPath__waypoint(v8, &v202, this_->mPathData->waypoint);
                BVector__operator_minus(&this_->mPosition.mPosition, &v181, v77);
                v181.z = 0.0;
                double arriveNorm = sqrt((double)v181.x * v181.x + (double)v181.y * v181.y);
                if (X87_LT_NAN(arriveNorm, 1e-7f))
                {
                    BMotion_ida_armlog("move.arrive", this_, (float)arriveNorm, 0.0f);
                    goto LABEL_156;
                }
                // asm 0x49DBC0-0x49DBCB (fcomp flt_6565F4; test ah,1; jz short loc_49DBF0): the
                // not-at-waypoint edge FALLS INTO the integration block at loc_49DBF0. A stray
                // unconditional goto LABEL_156 sat here and armed mFlags 1|0 on every walk call;
                // the re-dispatch then snap-advanced the waypoint (the setPosition teleport,
                // 0x49CCC0) and at the last waypoint hit the silent state=4 -> reset+stop - the
                // tick-1 desync of val_log_ida rows 1-5 (uid 13034/13037/11642/12851/13263) and
                // the destroy id=180129 row in path_life_log_ida.txt.
                v78 = BPath__waypoint(v8, &outVector, this_->mPathData->waypoint);
                BVector__operator_minus(v78, &v173, &this_->mPosition.mPosition);
                v79 = sqrt((double)v173.x * v173.x + (double)v173.y * v173.y);
                v166.x = v173.x / v79;
                v166.z = 0.0 / v79;
                v166.y = v173.y / v79;
                v80 = RGE_Game_World__getWorldTimeDeltaSeconds((RGE_Game_World*)(*base_game)->world);
                v138 = (v80 * this_->mAcceleration.y + this_->mVelocity.y) * v80;
                v81 = operator_mul(&v206, v138, &v166);
                BVector__operator_plus(&this_->mPosition.mPosition, &v180, v81);
                v179.x = v180.x;
                v82 = this_->mPathData;
                v179.z = v180.z;
                v179.y = v180.y;
                v83 = BPath__waypoint(v8, &v186, v82->waypoint);
                BVector__operator_minus(v83, &p2, &this_->mPosition.mPosition);
                BVector__operator_minus(v83, &v168, &v179);
                BVector__operator_minus(&v179, &v176, &this_->mPosition.mPosition);
                p2.z = 0.0;
                v168.z = 0.0;
                v84 = sqrt((double)p2.x * p2.x + (double)p2.y * p2.y);
                v85 = sqrt((double)v176.x * v176.x + (double)v176.z * v176.z + (double)v176.y * v176.y);
                if (X87_LE_NAN(v85, v84))
                    break;
                if (v84 >= sqrt((double)v168.x * v168.x + (double)v168.y * v168.y))
                {
                    v161 = v85;
                    if (v161 >= 0.1f)
                    {
                        BPathData__setFlag(this_->mPathData, 1, 1);
                        BPathData__setFlag(this_->mPathData, 0, 1);
                        break;
                    }
                }
            LABEL_156:
                BPathData__setFlag(this_->mPathData, 1, 1);
                BPathData__setFlag(this_->mPathData, 0, 1);
            }
            resetIgnoreList_new();
            addToIgnoreList_new((RGE_Static_Object*)this_->mPathUnit);
            v86 = RGE_Game_World__getUnitGroup((RGE_Game_World*)(*base_game)->world, this_->mPathUnit->mGroupID);
            v87 = v86;
            if (v86)
            {
                v88 = 0;
                if (v86->mNumberUnits > 0)
                {
                    v147 = v86->mUnits;
                    do
                    {
                        v89 = RGE_Game_World__object((RGE_Game_World*)(*base_game)->world, *v147);
                        addToIgnoreList_new(v89);
                        ++v88;
                        ++v147;
                    } while (v88 < v87->mNumberUnits);
                }
            }
            v90 = this_->mPathUnit;
            v91 = (RGE_Obstruction_Manager*)*OBSystem_Air;
            if (v90->master_obj->movement_type != 1)
                v91 = *OBSystem_Land;
            findObSystemCollisions(v91, v90, &this_->mPosition.mPosition, &v179, ignoreUnits, ignoreListSize, 0, 1.0);
            if (*numberObSystemCollisions <= 0)    // asm 0x49DEB8-0x49DEBF (cmp eax, ebx(0); jle
                break;                             // loc_49DFFF) - the VALUE test. The missing
                                                   // dereference ran the collision loop once over
                                                   // the stale obSystemCollisions[0] on every
                                                   // zero-count query - the phantom fresh-blocker
                                                   // successor swap of val_log_ida row 4 (uid=12851,
                                                   // wt=2044684: destroy 179655, repath 180330).
            v159 = 0;
            v157 = 0;
            v155 = 0;
            v148 = obSystemCollisions;
            do
            {
                v92 = (*v148)->Object;
                if (v92)
                {
                    v93 = RGE_Game_World__object((RGE_Game_World*)(*base_game)->world, v92->id);
                    if (v93)
                    {
                        if (BPath__ignoreUnitsContain(v8, v93->id) != 1)
                        {
                            v94 = BPathData__highestLinkedPathPointer(this_->mPathData);
                            if (!v94 || BPath__ignoreUnitsContain(v94, v93->id) != 1)
                            {
                                v95 = this_->mPathData->targetID;
                                if (v95 >= 0 && v93->id == v95)
                                    v159 = 1;
                                v96 = v8->creationTimeValue;
                                if (v96 < v93->vfptr->lastMoveTime(v93))
                                    ++v157;
                            }
                        }
                    }
                }
                v97 = ++v155 < *numberObSystemCollisions;
                ++v148;
            } while (v97);
            if (*numberObSystemCollisions == 1 && v159 == 1)
            {
                BPathData__setFlag(this_->mPathData, 0, 1);
                BPathData__setFlag(this_->mPathData, 2, 1);
            }
            else
            {
                if (v157 <= 0)
                    break;
                v98 = this_->mPathData->linkedPath;
                if (!v98)
                    goto LABEL_194;
                v98->state = 2;
                this_->mPathData->linkedPath = 0;
                v99 = this_->mPathData;
                if (v99)
                {
                    BPathData__destructor(this_->mPathData);
                    operator_delete_internal(v99);
                }
                this_->mPathData = v98;
            }
        }
        if (!v169)
            break;
        v100 = RGE_Game_World__object((RGE_Game_World*)(*base_game)->world, this_->mPathData->targetID);
        v101 = this_->mPathData;
        if (fabs(
            RGE_Static_Object__distance_to_position(
                v100,
                this_->mPosition.mPosition.x,
                this_->mPosition.mPosition.y,
                this_->mPosition.mPosition.z)
            - v101->range) >= dbl_6545B0)
            goto LABEL_209;
        BPathData__setFlag(v101, 0, 1);
    }
    if ((v8->flags.valueValue & 1) != 0
        || (v103 = this_->mPathData, X87_LE_NAN(v103->range, 0.1f))
        || (v104 = BPathData__highestLinkedPathPointer(v103)) == 0
        || (v105 = BPath__waypoint(v104, &v186, v104->waypointsCount - 1),
            BVector__operator_minus(&this_->mPosition.mPosition, &v168, v105),
            v106 = this_->mPathData,
            sqrt((double)v168.y * v168.y + (double)v168.z * v168.z + (double)v168.x * v168.x) - v106->range >= 1e-7f))
    {
    LABEL_209:
        v.x = this_->mPosition.mOrientationForward.x;
        v.y = this_->mPosition.mOrientationForward.y;
        v107 = sqrt((double)v.x * v.x + (double)v.y * v.y);
        v.x = v.x / v107;
        v.z = 0.0 / v107;
        v.y = v.y / v107;
        v162 = RGE_Game_World__getWorldTimeDeltaSeconds((RGE_Game_World*)(*base_game)->world);
        v141 = BVector__angleBetweenVector(&v166, &v);
        v108 = (double)v162 * (double)this_->mMaximumYawPerSecondMoving;
        if (v141 > v108)
            v141 = v108;
        BVector__operator_mul(&v166, &v180, &v);
        if (v180.z > 0.0)
            v141 = -v141;
        if (fabs((double)v141) > dbl_6545B0)
        {
            BMatrix__makeRotateArbitrary(&v212, v141, &this_->mNewPosition.mOrientationUp);
            v109 = &this_->mNewPosition.mOrientationForward;
            this_->mNewPosition.mOrientationForward = *BMatrix__operator_mul(
                &v212,
                &v186,
                &this_->mNewPosition.mOrientationForward);
            this_->mNewPosition.mOrientationRight = *BMatrix__operator_mul(&v212, &v210, &this_->mNewPosition.mOrientationRight);
            v110 = sqrt((double)v109->z * v109->z + (double)v109->y * v109->y + (double)v109->x * v109->x);
            this_->mNewPosition.mOrientationForward.x = this_->mNewPosition.mOrientationForward.x / v110;
            this_->mNewPosition.mOrientationForward.z = this_->mNewPosition.mOrientationForward.z / v110;
            this_->mNewPosition.mOrientationForward.y = this_->mNewPosition.mOrientationForward.y / v110;
            v111 = sqrt(
                (double)this_->mNewPosition.mOrientationRight.x * this_->mNewPosition.mOrientationRight.x
                + (double)this_->mNewPosition.mOrientationRight.z * this_->mNewPosition.mOrientationRight.z
                + (double)this_->mNewPosition.mOrientationRight.y * this_->mNewPosition.mOrientationRight.y);
            this_->mNewPosition.mOrientationRight.x = this_->mNewPosition.mOrientationRight.x / v111;
            this_->mNewPosition.mOrientationRight.z = this_->mNewPosition.mOrientationRight.z / v111;
            this_->mNewPosition.mOrientationRight.y = this_->mNewPosition.mOrientationRight.y / v111;
            this_->vfptr->setFlag(this_, 0, 1);
        }
        this_->vfptr->setVelocityForward(this_, this_->mMaximumVelocityForward);
    }
    else
    {
        BPathData__reset(v106);
        this_->vfptr->stopMoving(this_);
    }
    return 1;
}
// ---- THE DIAGNOSTIC LOGGER (the template's A/B harness, owner-directed
// 2026-11-19 - the desync persists on the _ida form). Three pieces:
// (1) BMotion__updateMovement_orig - the naked trampoline: replays the full
//     6-byte "mov eax, large fs:0" at 0x49CB60 (the setHook E9 covers its
//     first 5 bytes), then enters 0x49CB66 (= the entry + the replayed
//     length 6) via edx - edx is the __fastcall dummy slot, never read by
//     the original; eax (the fs:0 SEH head) is preserved for the replayed
//     push at 0x49CB6D. The trampoline does not touch esp: the original's
//     retn 8 pops the two stack args and returns into the logger (the
//     exemplar's lifecycle-balance form).
// (2) BMotion__updateMovement_logger - the wrapper: ONE val_log line per
//     tick (the "at"-mode lazy fopen, one handle per session): the PRE
//     state (the mPathData machine fields, the position, the orientation
//     forward, mVelocity.y/mAcceleration.y), the return, and the POST
//     state (the mutated pathData + the rotated mNewPosition forward +
//     mVelocity.y). NO POINTERS (owner's correction 2026-11-19: they
//     differ per run; the ids pd/pid/pv were dropped as uninformative).
//     THE IDENTITY + THE STRUCTURE CHARACTERISTICS: the unit's
//     RGE_Moving_Object::id @+0x10 and mGroupID @+0x88, the mPathData
//     machine fields, and the ACTIVE PATH's characteristics (BPath:
//     waypointsCount @+0xC, pathLengthValue @+0x18, flags @+0x20,
//     creationTimeValue @+0x1C = the world_time stamp - deterministic).
//     UM_TRACE_ORIG flips the callee: 1 = the ORIGINAL (the vanilla
//     trace -> val_log_orig.txt), 0 = the _ida replacement (the hooked
//     trace -> val_log_ida.txt); the two logs align row-by-row and the
//     first diverging tick/value names the exact stage.
// (3) THE INSTALLER SWAP below: setHook(0x49CB60, _logger).
// THE CRT NOTE: the bare sprintf/fopen/fwrite are the approved diagnostic
// exception (owner-supervised; removed before the final ship).
static FILE* g_umLogFile = 0;                          // one handle per session
static unsigned g_umLogSeq = 0;                        // the tick sequence
#define UM_TRACE_ORIG 1

__declspec(naked) int __fastcall BMotion__updateMovement_orig(
    BMotion* this_, DWORD dummy, int doAvailablePathCheck, BUnitGroup* uG)
{
    __asm
    {
        mov eax, dword ptr fs:[0]                      // the replay (the full 6-byte instruction; the
                                                       // E9 covers its first 5 bytes)
        mov edx, 0x0049CB66                            // the entry + the replayed length (edx = the
                                                       // dummy slot - unread by the original; eax holds
                                                       // the SEH head for the replayed push at 0x49CB6D)
        jmp edx
    }
}

int __fastcall BMotion__updateMovement_logger(
    BMotion* this_, DWORD dummy, int doAvailablePathCheck, BUnitGroup* uG)
{
    char buf[1024];
    // the identity + the structure characteristics (no pointers - see the block
    // comment; null path = the sentinels wpc=-1/len=-1/pfl=0/ctm=0)
    BPathData* pdPre = this_->mPathData;
    BPath* pathPre = pdPre ? pdPre->path : 0;
    int uidPre = this_->mPathUnit ? this_->mPathUnit->id : -1;
    int gidPre = this_->mPathUnit ? this_->mPathUnit->mGroupID : -1;
    unsigned wtPre = (*base_game)->world->world_time;
    int pidPre = pdPre ? pdPre->pathID : -1;
    int wpcPre = pathPre ? pathPre->waypointsCount : -1;
    float lenPre = pathPre ? pathPre->pathLengthValue : -1.0f;
    unsigned pflgPre = pathPre ? (unsigned)pathPre->flags.valueValue : 0;
    unsigned ctmPre = pathPre ? pathPre->creationTimeValue : 0;
    int tgtPre = pdPre ? pdPre->targetID : -2;
    int stPre = pdPre ? pdPre->state : -1;
    int wpPre = pdPre ? pdPre->waypoint : -2;
    int lvlPre = pdPre ? pdPre->waypointLevel : -1;
    unsigned flgPre = pdPre ? (unsigned)pdPre->mFlags.valueValue : 0;
    float rngPre = pdPre ? pdPre->range : 0.0f;
    float pauPre = pdPre ? pdPre->pauseTime : 0.0f;
    int ccPre = pdPre ? pdPre->continueCounter : -1;
    int lptPre = pdPre ? pdPre->linkedPathType : -1;
    int nwdPre = this_->mNumberWaitDelays;
    float dtNow = (float)RGE_Game_World__getWorldTimeDeltaSeconds(
        (RGE_Game_World*)(*base_game)->world);         // the delta the body consumed (0.1 in the
                                                       // determinism mode - the run-alignment check)
    float px = this_->mPosition.mPosition.x;
    float py = this_->mPosition.mPosition.y;
    float pz = this_->mPosition.mPosition.z;
    float npx = this_->mNewPosition.mPosition.x;
    float npz = this_->mNewPosition.mPosition.z;
    float npy = this_->mNewPosition.mPosition.y;
    float fx = this_->mPosition.mOrientationForward.x;
    float fz = this_->mPosition.mOrientationForward.z;
    float fy = this_->mPosition.mOrientationForward.y;
    float vx = this_->mVelocity.x;
    float vz = this_->mVelocity.z;
    float vy = this_->mVelocity.y;
    float ax = this_->mAcceleration.x;
    float az = this_->mAcceleration.z;
    float ay = this_->mAcceleration.y;
#if UM_TRACE_ORIG
    int ret = BMotion__updateMovement_orig(this_, dummy, doAvailablePathCheck, uG);
#else
    int ret = BMotion__updateMovement_new_ida(this_, dummy, doAvailablePathCheck, uG);
#endif
    BPathData* pdPost = this_->mPathData;
    BPath* pathPost = pdPost ? pdPost->path : 0;
    int pidPost = pdPost ? pdPost->pathID : -1;
    if (!g_umLogFile)
    {
#if UM_TRACE_ORIG
        g_umLogFile = fopen("val_log_orig.txt", "at");
#else
        g_umLogFile = fopen("val_log_ida.txt", "at");
#endif
    }
    if (g_umLogFile)
    {
        int n = sprintf(buf,
            "%u wt=%u dt=%.9g apc=%d uid=%d gid=%d | pre pid=%d tgt=%d st=%d wp=%d lvl=%d flg=%08X rng=%.9g pau=%.9g cc=%d lpt=%d nwd=%d p:wpc=%d p:len=%.9g p:flg=%08X p:ctm=%u pos=%.9g,%.9g,%.9g np=%.9g,%.9g,%.9g fw=%.9g,%.9g,%.9g vx=%.9g vz=%.9g vy=%.9g ax=%.9g az=%.9g ay=%.9g | ret=%d | post pid=%d tgt=%d st=%d wp=%d lvl=%d flg=%08X rng=%.9g pau=%.9g cc=%d lpt=%d nwd=%d p:wpc=%d p:len=%.9g p:flg=%08X p:ctm=%u pos=%.9g,%.9g,%.9g np=%.9g,%.9g,%.9g fw=%.9g,%.9g,%.9g vx=%.9g vz=%.9g vy=%.9g\n",
            ++g_umLogSeq, wtPre, dtNow, doAvailablePathCheck, uidPre, gidPre,
            pidPre,
            tgtPre, stPre, wpPre, lvlPre, flgPre,
            rngPre, pauPre, ccPre, lptPre, nwdPre,
            wpcPre, lenPre, pflgPre, ctmPre,
            px, py, pz, npx, npz, npy, fx, fz, fy,
            vx, vz, vy, ax, az, ay,
            ret,
            pidPost,
            pdPost ? pdPost->targetID : -2,
            pdPost ? pdPost->state : -1,
            pdPost ? pdPost->waypoint : -2,
            pdPost ? pdPost->waypointLevel : -1,
            pdPost ? (unsigned)pdPost->mFlags.valueValue : 0,
            pdPost ? pdPost->range : 0.0f,
            pdPost ? pdPost->pauseTime : 0.0f,
            pdPost ? pdPost->continueCounter : -1,
            pdPost ? pdPost->linkedPathType : -1,
            this_->mNumberWaitDelays,
            pathPost ? pathPost->waypointsCount : -1,
            pathPost ? pathPost->pathLengthValue : -1.0f,
            pathPost ? (unsigned)pathPost->flags.valueValue : 0,
            pathPost ? pathPost->creationTimeValue : 0,
            this_->mPosition.mPosition.x,
            this_->mPosition.mPosition.y,
            this_->mPosition.mPosition.z,
            this_->mNewPosition.mPosition.x,
            this_->mNewPosition.mPosition.z,
            this_->mNewPosition.mPosition.y,
            this_->mNewPosition.mOrientationForward.x,
            this_->mNewPosition.mOrientationForward.z,
            this_->mNewPosition.mOrientationForward.y,
            this_->mVelocity.x,
            this_->mVelocity.z,
            this_->mVelocity.y);
        fwrite(buf, 1, n, g_umLogFile);
    }
    return ret;
}
// ---- THE PATH-LIFECYCLE TRACE (diagnostics-only, owner-directed
// 2026-11-19): the val_log rows localized the first divergence to the WORLD
// PATH OBJECT ITSELF (the peers resolved pathID 180129 to BPath objects with
// different wp[1]/length/creationTime BEFORE the first logged
// updateMovement) - the world's path TABLE diverged in the pre-window era.
// These three hooks trace the table's lifecycle: every createPath /
// destroyPath2 / path-resolve with the resulting BPath characteristics
// (idValue @+8, waypointsCount @+0xC, pathLengthValue @+0x18,
// creationTimeValue @+0x1C). ONE shared sequence + ONE file
// (path_life_log.txt) so the interleaved order is preserved; diffing the
// two builds' files gives the FIRST divergent create/destroy/resolve = the
// origin. THE CRT EXCEPTION (removed before the final ship).
static FILE* g_plLogFile = 0;
static unsigned g_plLogSeq = 0;

__declspec(naked) BPath* __fastcall RGE_Game_World__createPath_orig(RGE_Game_World* this_)
{
    __asm
    {
        push 0FFFFFFFFh                                // the replay (with the next push: 7 bytes >= the
                                                       // E9's 5)
        push 00647CEBh
        mov edx, 0x00624647                            // the entry + the replayed length (edx free; ecx =
                                                       // this preserved)
        jmp edx
    }
}

BPath* __fastcall RGE_Game_World__createPath_logger(RGE_Game_World* this_)
{
    BPath* r = RGE_Game_World__createPath_orig(this_);
    if (!g_plLogFile)
        g_plLogFile = fopen("path_life_log.txt", "at");
    if (g_plLogFile)
    {
        BVector w0;
        BVector w1;
        BPath__waypoint(r, &w0, 0);
        BPath__waypoint(r, &w1, 1);
        fprintf(g_plLogFile, "%u create wt=%u id=%d wpc=%d len=%.9g ctm=%u w0=%.9g,%.9g w1=%.9g,%.9g\n",
            ++g_plLogSeq, (*base_game)->world->world_time,
            r ? r->idValue : -1, r ? r->waypointsCount : -1,
            r ? (double)r->pathLengthValue : -1.0,
            r ? r->creationTimeValue : 0,
            (double)w0.x, (double)w0.y, (double)w1.x, (double)w1.y);
    }
    return r;
}

__declspec(naked) int __fastcall RGE_Game_World__destroyPath2_orig(RGE_Game_World* this_, DWORD dummy, BPath* pathToDestroy)
{
    __asm
    {
        push esi                                       // the replay (push esi + mov esi,[esp+8] = exactly
                                                       // the E9's 5 bytes; the push shifts esp by 4 exactly
                                                       // like the original's own push)
        mov esi, dword ptr [esp+8]                     // esi = pathToDestroy ([esp+4] pre-push + the 4-byte
                                                       // shift)
        mov edx, 0x00624765                            // the entry + the replayed length (edx free)
        jmp edx
    }
}

int __fastcall RGE_Game_World__destroyPath2_logger(RGE_Game_World* this_, DWORD dummy, BPath* pathToDestroy)
{
    int idv = pathToDestroy ? pathToDestroy->idValue : -1;
    int wpc = pathToDestroy ? pathToDestroy->waypointsCount : -1;
    int r = RGE_Game_World__destroyPath2_orig(this_, 0, pathToDestroy);
    if (!g_plLogFile)
        g_plLogFile = fopen("path_life_log.txt", "at");
    if (g_plLogFile)
        fprintf(g_plLogFile, "%u destroy wt=%u id=%d wpc=%d\n",
            ++g_plLogSeq, (*base_game)->world->world_time, idv, wpc);
    return r;
}

__declspec(naked) BPath* __fastcall RGE_Game_World__path_orig(RGE_Game_World* this_, DWORD dummy, int pathID)
{
    __asm
    {
        mov edx, dword ptr [esp+4]                     // the replay: edx = pathID (7 bytes with the cmp -
                                                       // covers the E9's 5; the flags feed the jz at
                                                       // 0x00624617; the entry esp - nothing pushed before
                                                       // this mov)
        cmp edx, 0FFFFFFFFh
        push 0x00624617                                // the entry + the replayed length; push/ret preserves
                                                       // edx (pathID - read by the body's list walk) AND the
                                                       // flags
        ret
    }
}

BPath* __fastcall RGE_Game_World__path_logger(RGE_Game_World* this_, DWORD dummy, int pathID)
{
    BPath* r = RGE_Game_World__path_orig(this_, 0, pathID);
    if (!g_plLogFile)
        g_plLogFile = fopen("path_life_log.txt", "at");
    if (g_plLogFile && r)
    {
        BVector w0;
        BVector w1;
        BPath__waypoint(r, &w0, 0);
        BPath__waypoint(r, &w1, 1);
        fprintf(g_plLogFile, "%u resolve wt=%u req=%d got=%d wpc=%d len=%.9g ctm=%u w0=%.9g,%.9g w1=%.9g,%.9g\n",
            ++g_plLogSeq, (*base_game)->world->world_time, pathID,
            r ? r->idValue : -1, r ? r->waypointsCount : -1,
            r ? (double)r->pathLengthValue : -1.0,
            r ? r->creationTimeValue : 0,
            (double)w0.x, (double)w0.y, (double)w1.x, (double)w1.y);
    }
    return r;
}
#pragma float_control(pop)

void install_BMotion()
{
    setHook((void*)0x0049CB60, BMotion__updateMovement_new_ida);
    //setHook((void*)0x00624640, RGE_Game_World__createPath_logger);
    //setHook((void*)0x00624760, RGE_Game_World__destroyPath2_logger);
    //setHook((void*)0x00624610, RGE_Game_World__path_logger);
    //setHook((void*)0x0049CB60, BMotion__updateMovement_new_ida);   // <- flip back for the ship (or set
                                                                    //    UM_TRACE_ORIG 0 for the
                                                                    //    replacement's trace build)
    //setHook((void*)0x004A0350, addToIgnoreList_new);
    //setHook((void*)0x004A0340, resetIgnoreList_new);
}
