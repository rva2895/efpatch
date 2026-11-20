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
// retn 8. SHIPPED FORM (owner-directed 2026-11-20: the strict-control-flow
// form retired). SEMANTIC: the BMotion-family movement driver - the pause
// countdown,
int __fastcall BMotion__updateMovement_new(BMotion* this_, DWORD dummy, int doAvailablePathCheck, BUnitGroup* uG)
{
    (void)dummy;
    (void)uG;
    // RESTRUCTURE (owner-directed 2026-11-20): the suite-verified body
    // restated with named locals and staged control flow (no gotos, no IDA
    // v-names). EQUIVALENCE PROOF: one dispatch loop = the asm's
    // loc_49CBEB; the sequential stages = the asm's dispatch order; every
    // `continue` = an asm back-edge to loc_49CBEB; the stage falls = the
    // asm's sequential dispatch; the recompiled /Fa is diffed against the
    // pre-restructure baseline listing, and the live arm traces re-verify
    // the body on the replay. STAGE MAP: head (loc_49CBEB-0x49CCA0) -> the
    // increment arm (loc_49CCA0-0x49CE8A, flags bit0) -> the state-4/5
    // successor stage (loc_49D01C-0x49D131) -> the state-2 repath arm
    // (loc_49D131-0x49DB6A) -> the walk arm (loc_49DB6A-0x49DDF6) -> the
    // OBSystem pass (loc_49DDF8-0x49DFFF) -> the target gate (loc_49DFFF-
    // 0x49E151) -> the range gate (loc_49E151-0x49E20B) -> the facing arm
    // (loc_49E20B-0x49E3DF).
    // the shared BPath__waypoint out-buffer (single-fetch sites only: each
    // fetched waypoint is consumed before the next fetch). MULTI-FETCH sites
    // use the dedicated waypointScratch2 - BPath__waypoint COPIES the internal
    // waypoint into the out-buffer and RETURNS THE OUT-BUFFER'S ADDRESS
    // (0x4BC440: mov ecx, [esp+outVector] / mov [ecx], esi / retn 8), so two
    // live fetches on one buffer alias. THE TAIL CHECKS (arm1's smooth-tail
    // and the findTile quality check) hold TWO waypoints live together, and
    // the aliasing silently disabled their discards - the restructured-body
    // desync at wt=2185372.
    BVector waypointScratch;
    BVector waypointScratch2;

    if (this_->mPathData == 0)                         // asm 0x49CB83-0x49CB8B
        return 0;
    // asm loc_49CB94: the pause countdown
    if (this_->mPathData->pauseTime > 0.0)
    {
        this_->mPathData->pauseTime = this_->mPathData->pauseTime
            - RGE_Game_World__getWorldTimeDeltaSeconds((RGE_Game_World*)(*base_game)->world);
        if (this_->mPathData->pauseTime > 0.0)
        {
            this_->vfptr->stopMoving(this_);
            return 0;
        }
        this_->mPathData->pauseTime = 0.0;
    }
    int passCount = 0;
    while (true)
    {
        if (++passCount > 20)                          // asm loc_49CBEB-0x49CBF7
            return 0;                                  // loc_49E14A
        BPath* activePath = this_->mPathData->path;    // asm 0x49CBFD-0x49CC08
        if (!activePath)
        {
            activePath = RGE_Game_World__path((RGE_Game_World*)(*base_game)->world,
                this_->mPathData->pathID);             // asm 0x49CC0A-0x49CC1F
            if (!activePath)
                return 0;                              // loc_49E14A
            this_->mPathData->path = activePath;       // asm 0x49CC29 (the cache)
        }
        // asm loc_49CC32: the target object + its distance
        RGE_Static_Object* targetObj = 0;
        float targetDistance = 3.4028235e38;           // SINGLE_MAXIMUM_VALUE
        if (this_->mPathData->targetID > -1)
        {
            targetObj = RGE_Game_World__object((RGE_Game_World*)(*base_game)->world,
                this_->mPathData->targetID);
            if (targetObj != 0)
                targetDistance = (float)RGE_Static_Object__distance_to_position(targetObj,
                    this_->mPosition.mPosition.x,
                    this_->mPosition.mPosition.y,
                    this_->mPosition.mPosition.z);     // asm 0x49CC6A-0x49CC7D (the store-round)
        }
        bool patrolFlag = (this_->mPathData->mFlags.valueValue & 4) != 0;
                                                       // asm 0x49CC87-0x49CC9C

        // ---- the increment arm (asm loc_49CCA0-0x49CE8A; flags bit0) ----
        if ((this_->mPathData->mFlags.valueValue & 1) != 0)
        {
            if ((this_->mPathData->mFlags.valueValue & 2) != 0)
            {
                if (this_->mPathData->waypoint < activePath->waypointsCount)
                {
                    BVector* snapWaypoint = BPath__waypoint(activePath, &waypointScratch,
                        this_->mPathData->waypoint);
                    BVector snapPosition = { snapWaypoint->x, snapWaypoint->z, snapWaypoint->y };
                    this_->vfptr->setPosition(this_, snapPosition);   // the {x,z,y} copies
                }
                BPathData__setFlag(this_->mPathData, 1, 0);
            }
            BPathData__setFlag(this_->mPathData, 0, 0);
            ++this_->mPathData->waypoint;
            if ((activePath->flags.valueValue & 1) != 0)
            {
                // ---- the patrol arm (asm loc_49CE8A-0x49D01C) ----
                this_->mPathData->waypoint = BPath__nextPatrolWaypoint(activePath,
                    this_->mPathData->waypoint - 1);
                this_->mPathData->state = 2;
                if (this_->mPathData->waypoint == -1)
                {
                    // the patrol wrapped past the end: the wrap gates
                    BVector patrolSeed;
                    patrolSeed.x = this_->mPathUnit->world_x;
                    patrolSeed.y = this_->mPathUnit->world_y;
                    patrolSeed.z = 0.0f;
                    BVector* patrolTail = BPath__waypoint(activePath, &waypointScratch,
                        activePath->waypointsCount - 1);
                    BVector patrolDelta;
                    BVector__operator_minus(&patrolSeed, &patrolDelta, patrolTail);
                    float wrapDistance = (float)sqrt((double)patrolDelta.x * patrolDelta.x
                        + (double)patrolDelta.z * patrolDelta.z
                        + (double)patrolDelta.y * patrolDelta.y);
                    double wrapSeed;
                    bool wrapArrived = false;
                    if (targetObj != 0)
                    {
                        wrapSeed = (double)RGE_Static_Object__distance_to_position(targetObj,
                                this_->mPathUnit->world_x,
                                0.0,
                                this_->mPathUnit->world_y)
                            - (double)this_->mPathData->range
                            - (double)0.1f;            // the (x, 0, y) seed form; the x87
                                                       // chain at 53-bit, UNSTORED
                        if ((this_->mPathData->mFlags.valueValue & 4) != 0)
                        {
                            BPathData__setFlag(this_->mPathData, 2, 0);
                            this_->mPathData->state = 4;
                            wrapArrived = true;        // asm 0x49CF88 -> loc_49D01C
                        }
                    }
                    else
                    {
                        wrapSeed = (double)wrapDistance
                            - (double)this_->mPathData->range
                            - (double)0.1f;            // the STORED m32; the chain UNSTORED
                    }
                    if (!wrapArrived)
                    {
                        // asm loc_49CFAD: the shared gate - the UNROUNDED seed in st0
                        if (wrapSeed < 1e-7f)
                        {
                            this_->mPathData->state = 4;
                        }
                        else if (targetObj == 0
                            && wrapSeed < 1.0f
                            && this_->mPathData->waypointLevel != 0)
                        {
                            this_->mPathData->state = 4;
                        }
                        else if (wrapDistance < 1e-7f)
                        {
                            this_->mPathData->state = 4;
                        }
                        else
                        {
                            this_->mPathData->waypoint = 0;   // the wrap restarts at
                            this_->mPathData->state = 2;      // the FIRST waypoint
                        }
                    }
                }
            }
            else
            {
                // ---- the linear arm (asm loc_49CD28-0x49CE85) ----
                BVector* previousWaypoint = BPath__waypoint(activePath, &waypointScratch,
                    this_->mPathData->waypoint - 1);
                BVector arriveDelta;
                BVector__operator_minus(&this_->mPosition.mPosition, &arriveDelta, previousWaypoint);
                float distanceToWaypoint3D = (float)sqrt((double)arriveDelta.x * arriveDelta.x
                    + (double)arriveDelta.z * arriveDelta.z
                    + (double)arriveDelta.y * arriveDelta.y);   // the store-round
                float gateDistance;
                if (targetObj != 0)
                {
                    gateDistance = (float)(RGE_Static_Object__distance_to_position(targetObj,
                            this_->mPosition.mPosition.x,
                            this_->mPosition.mPosition.y,
                            this_->mPosition.mPosition.z)
                        - (double)this_->mPathData->range
                        - (double)0.1f);               // the call-tail fsub chain at
                                                       // 53-bit; ONE fstp store
                    if ((this_->mPathData->mFlags.valueValue & 4) != 0)
                    {
                        gateDistance = 0.0f;           // the chain discarded
                        BPathData__setFlag(this_->mPathData, 2, 0);
                    }
                }
                else
                {
                    gateDistance = (float)((double)distanceToWaypoint3D
                        - (double)this_->mPathData->range
                        - (double)0.1f);               // the STORED m32; ONE fstp store
                }
                if (this_->mPathData->waypoint >= activePath->waypointsCount)
                {
                    // the final waypoint: the arrival gates
                    if (gateDistance < 1e-7f)
                    {
                        this_->mPathData->state = 4;
                    }
                    else if (targetObj == 0
                        && gateDistance < 1.0f
                        && this_->mPathData->waypointLevel != 0)
                    {
                        this_->mPathData->state = 4;
                    }
                    else if (distanceToWaypoint3D < 1e-7f)
                    {
                        this_->mPathData->state = 4;
                    }
                    else
                    {
                        // not arrived: walk back to the passed waypoint and repath
                        --this_->mPathData->waypoint;
                        this_->mPathData->state = 2;
                    }
                }
                else if (this_->mPathData->waypointLevel > 0)
                {
                    this_->mPathData->state = 2;
                }
            }
        }

        // ---- the state-4/5 successor stage (asm loc_49D01C-0x49D131) ----
        if (this_->mPathData->state == 4 || this_->mPathData->state == 5)
        {
            BVector* notifyWaypoint = BPath__waypoint(activePath, &waypointScratch,
                activePath->waypointsCount - 1);
            this_->vfptr->checkPositionNotifies_2(this_, notifyWaypoint);   // slot +0x8C = 35
            BPathData* relink = this_->mPathData->linkedPath;
            if (relink == 0)
            {
                // asm loc_49E1F4: the shared reset+stop+1 exit
                BPathData__reset(this_->mPathData);
                this_->vfptr->stopMoving(this_);
                return 1;
            }
            int linkedType = this_->mPathData->linkedPathType;
            if (linkedType == 1 || patrolFlag)
            {
                // asm loc_49E061: the plain successor swap (the state carried)
                this_->mPathData->linkedPath = 0;
                if (this_->mPathData != 0)             // the guarded teardown
                {
                    BPathData__destructor(this_->mPathData);
                    operator_delete_internal(this_->mPathData);
                }
                BPathData__setFlag(relink, 0, 1);
                this_->mPathData = relink;
                continue;                              // asm 0x49E093
            }
            if (linkedType == 2)
            {
                // asm loc_49D085-0x49D0F3: the patrol-aware swap
                this_->mPathData->linkedPath = 0;
                if (this_->mPathData != 0)
                {
                    BPathData__destructor(this_->mPathData);
                    operator_delete_internal(this_->mPathData);
                }
                this_->mPathData = relink;
                relink->state = 2;
                if (relink->path != 0
                    && (relink->path->flags.valueValue & 1) != 0)
                {
                    relink->waypoint = BPath__nextPatrolWaypoint(relink->path, relink->waypoint);
                    if (relink->waypoint == -1)
                    {
                        // asm loc_49E098: the wrap-fail exit
                        BPathData__reset(this_->mPathData);
                        this_->vfptr->stopMoving(this_);
                        return 1;
                    }
                }
                continue;                              // asm 0x49D0F3
            }
            if (linkedType == 3)
            {
                // asm loc_49D0FD-0x49D127
                this_->mPathData->linkedPath = 0;
                if (this_->mPathData != 0)
                {
                    BPathData__destructor(this_->mPathData);
                    operator_delete_internal(this_->mPathData);
                }
                this_->mPathData = relink;
                relink->state = 2;
                continue;                              // asm 0x49D127
            }
            // any other linked type: fall through to the state-2 test
            // (asm loc_49D0F8 -> loc_49D131)
        }
        // ---- the state-2 repath stage (asm loc_49D131-0x49DB6A) ----
        if (this_->mPathData->state == 2)
        {
            RGE_Player* currentPlayer = (RGE_Player*)this_->mPathUnit->owner;   // asm 0x49D13A
            if (doAvailablePathCheck == 1
                && !RGE_Player__availablePathingAttempts(currentPlayer,
                    this_->mNumberWaitDelays))
            {
                this_->vfptr->stopMoving(this_);       // loc_49E0AF
                ++this_->mNumberWaitDelays;
                return 0;
            }
            RGE_Player__incrementPathingAttempts(currentPlayer);   // asm 0x49D169
            this_->mNumberWaitDelays = 0;
            // the patrol gate: only a LIVE PATROL linked path skips the give-up gate
            BPath* linked = BPathData__highestLinkedPathPointer(this_->mPathData);
            if ((linked == 0
                    && (linked = RGE_Game_World__path((RGE_Game_World*)(*base_game)->world,
                        BPathData__highestLinkedPath(this_->mPathData))) == 0)
                || (linked->flags.valueValue & 1) == 0)
            {
                if (BPathData__continuesLeft(this_->mPathData, 1) <= 0)
                {
                    // asm loc_49E102: the shared failure exit
                    BPathData__setFailure(this_->mPathData);
                    BPathData__reset(this_->mPathData);
                    this_->vfptr->stopMoving(this_);
                    return 0;
                }
            }
            // the successor allocation (the SEH chunk 0x642650 = the alloc unwind)
            BPathData* successor = (BPathData*)operator_new_internal(sizeof(BPathData));
            if (successor != 0)
                BPathData__BPathData(successor);
            if (successor == 0)
            {
                BPathData__setFailure(this_->mPathData);   // loc_49E0CA
                BPathData__reset(this_->mPathData);
                this_->vfptr->stopMoving(this_);
                return 0;
            }
            BPath* newPath = RGE_Game_World__createPath((RGE_Game_World*)(*base_game)->world);
            if (newPath == 0)
            {
                if (successor != 0)                        // loc_49E0EE
                {
                    BPathData__destructor(successor);
                    operator_delete_internal(successor);
                }
                BPathData__setFailure(this_->mPathData);   // loc_49E102
                BPathData__reset(this_->mPathData);
                this_->vfptr->stopMoving(this_);
                return 0;
            }
            bool repathArmed = false;                      // asm 0x49D22A (var_268)
            int smoothUsed = 0;                            // asm 0x49D22E (var_258.x)
            bool validityReached = false;                  // asm loc_49D822
            bool findTileReached = false;                  // asm loc_49D83C
            bool installReached = false;                   // asm loc_49D9AD
            if (RGE_Static_Object__isFlagSet((RGE_Static_Object*)this_->mPathUnit, 1) == 0)
            {
                // the stub arm: the 2-waypoint path (no validity/findTile stage)
                BPath__addWaypointAtStart(newPath, &this_->mPosition.mPosition);
                BVector* stubWaypoint = BPath__waypoint(activePath, &waypointScratch,
                    this_->mPathData->waypoint);
                BPath__addWaypointAtEnd(newPath, stubWaypoint);
                BPath__calculatePathLength(newPath);
                repathArmed = 1;
                installReached = true;                     // asm 0x49D276 (loc_49D9AD)
            }
            else
            {
                // ---- the flagged arm (asm loc_49D27B-0x49D9AD) ----
                BVector* repathWaypoint = BPath__waypoint(activePath, &waypointScratch,
                    this_->mPathData->waypoint);
                BVector repathDelta;
                BVector__operator_minus(repathWaypoint, &repathDelta,
                    &this_->mPosition.mPosition);
                repathDelta.z = 0.0f;                      // FINDING B1
                float distanceToWaypoint = (float)sqrt((double)repathDelta.x * repathDelta.x
                    + (double)repathDelta.y * repathDelta.y);   // the store-round
                // ---- arm 1: the map-analysis smooth path ----
                if (this_->mPathData->waypointLevel == 2
                    && *dword_689568 == 1
                    && distanceToWaypoint > (double)*flt_68F494)
                {
                    RGE_Map_Analysis* mapAnalysis = RGE_Map_Analysis_System__get_map_analysis(
                        (*base_game)->world->map_analysis_system,
                        (RGE_Master_Static_Object*)this_->mPathUnit->master_obj);
                    if (mapAnalysis != 0)
                    {
                        float targetRadius = 0.0f;         // asm 0x49D325
                        if (this_->mPathData->targetID != -1)
                        {
                            RGE_Static_Object* mapTarget = RGE_Game_World__object(
                                (RGE_Game_World*)(*base_game)->world,
                                this_->mPathData->targetID);
                            if (mapTarget != 0)
                                targetRadius = mapTarget->master_obj->radius_x;
                        }
                        float radiusSum = (float)((double)targetRadius
                            + (double)this_->mPathUnit->master_obj->radius_x);
                        BVector* mapWaypoint = BPath__waypoint(activePath, &waypointScratch,
                            this_->mPathData->waypoint);
                        int findPathStatus = (RGE_Map_Analysis__find_path(mapAnalysis,
                            newPath,
                            this_->mPosition.mPosition.x,
                            this_->mPosition.mPosition.y,
                            mapWaypoint->x,
                            mapWaypoint->y,
                            radiusSum,
                            this_->mPathData->range,
                            currentPlayer,
                            0, 0) & 0xFF) != 0;   // asm 0x49D3BF (and eax, 0FFh) - FINDING
                                                  // C1: the masked status feeds cmp 3 / cmp
                                                  // 4 / test eax,eax. The mask is
                                                  // value-identical vs the u8-return read
                                                  // (al, zero-extended), so it compiles as
                                                  // the AL narrowing itself
                        if (findPathStatus != 3 && findPathStatus != 4)
                        {
                            if (findPathStatus != 0)
                            {
                                smoothUsed = 1;
                                repathArmed = 1;
                                smoothMidLevelPath(this_->mPathUnit, newPath);
                                // asm 0x49D3F8-0x49D407: the tail check - smoothedTail
                                // (newPath's tail) AND originalWaypoint live together:
                                // waypointScratch2 prevents the aliasing
                                BVector* smoothedTail = BPath__waypoint(newPath, &waypointScratch2,
                                    newPath->waypointsCount - 1);
                                BVector* originalWaypoint = BPath__waypoint(activePath,
                                    &waypointScratch, this_->mPathData->waypoint);
                                BVector tailDelta;
                                BVector__operator_minus(originalWaypoint, &tailDelta, smoothedTail);
                                tailDelta.z = 0.0f;        // FINDING B2
                                if (sqrt((double)tailDelta.y * tailDelta.y
                                    + (double)tailDelta.x * tailDelta.x)
                                    > (double)distanceToWaypoint)
                                {
                                    // the smooth missed the waypoint: discard
                                    newPath->waypointsCount = 0;
                                    repathArmed = 0;
                                }
                            }
                        }
                    }
                    else
                    {
                        // no map analysis: the 2-waypoint stub
                        BPath__addWaypointAtStart(newPath, &this_->mPosition.mPosition);
                        BVector* stubWaypoint2 = BPath__waypoint(activePath, &waypointScratch,
                            this_->mPathData->waypoint);
                        BPath__addWaypointAtEnd(newPath, stubWaypoint2);
                        smoothUsed = 1;
                        repathArmed = 1;
                    }
                    BPath__calculatePathLength(newPath);
                    newPath->creationTimeValue = (*base_game)->world->world_time;
                    if (repathArmed && newPath->waypointsCount > 2)
                        validityReached = true;            // asm 0x49D4BD (loc_49D6AA)
                }
                if (!validityReached)
                {
                    // ---- arm 2: findLowLevelPath ----
                    if (*dword_689564 == 1 && distanceToWaypoint <= 10.0f)
                    {
                        smoothUsed = 0;
                        newPath->waypointsCount = 0;
                        int soloProbeSeed = this_->mPathUnit->id;
                        BUnitGroup* unitGroup = RGE_Game_World__getUnitGroup(
                            (RGE_Game_World*)(*base_game)->world, this_->mPathUnit->mGroupID);
                        int findLowResult;
                        if (unitGroup != 0)
                        {
                            RGE_Master_Moving_Object* currentMaster = this_->mPathUnit->master_obj;
                            RGE_Obstruction_Manager* obstructionManagerToUse =
                                (currentMaster->movement_type == 1)
                                ? (RGE_Obstruction_Manager*)*OBSystem_Air : *OBSystem_Land;
                            int ownerId = this_->mPathUnit->owner->id;
                            int terrainIndex = currentMaster->terrain;
                            float* terrainTable = (*base_game)->world->terrains[terrainIndex];
                            int ignoreCount = unitGroup->mNumberUnits;
                            int* ignorePtr = unitGroup->mUnits;
                            BVector* lowGoal = BPath__waypoint(activePath, &waypointScratch,
                                this_->mPathData->waypoint);
                            findLowResult = findLowLevelPath(obstructionManagerToUse,
                                &this_->mPosition.mPosition,
                                lowGoal,
                                currentMaster->radius_x,
                                this_->mPathData->range,
                                this_->mPathData->targetID,
                                ignorePtr,
                                ignoreCount,
                                0,
                                0,
                                newPath,
                                terrainTable,
                                0,
                                ownerId);
                        }
                        else
                        {
                            RGE_Master_Moving_Object* currentMaster = this_->mPathUnit->master_obj;
                            RGE_Obstruction_Manager* obstructionManagerToUse =
                                (currentMaster->movement_type == 1)
                                ? (RGE_Obstruction_Manager*)*OBSystem_Air : *OBSystem_Land;
                            int ownerId = this_->mPathUnit->owner->id;
                            float* terrainTable =
                                (*base_game)->world->terrains[currentMaster->terrain];
                            BVector* lowGoal = BPath__waypoint(activePath, &waypointScratch,
                                this_->mPathData->waypoint);
                            findLowResult = findLowLevelPath(obstructionManagerToUse,
                                &this_->mPosition.mPosition,
                                lowGoal,
                                currentMaster->radius_x,
                                this_->mPathData->range,
                                this_->mPathData->targetID,
                                &soloProbeSeed,
                                1,
                                0,
                                0,
                                newPath,
                                terrainTable,
                                0,
                                ownerId);
                        }
                        BPath__calculatePathLength(newPath);
                        newPath->creationTimeValue = currentPlayer->world->world_time;   // FINDING D
                        if (findLowResult != 3 && findLowResult != 4
                            && newPath->pathLengthValue > 1e-7f)
                        {
                            repathArmed = 1;
                            validityReached = true;        // asm loc_49D81A -> loc_49D822
                        }
                    }
                }
                if (!validityReached)
                {
                    // asm loc_49D6AA: the repathArmed re-test
                    if (repathArmed)
                        validityReached = true;
                    else if (X87_LT_NAN(distanceToWaypoint, 1.0f)
                        || X87_LT_NAN(targetDistance, 1.0f))
                    {
                        // ---- the arm3 OBSystem probe ----
                        int probeSeed = this_->mPathUnit->id;
                        BUnitGroup* probeUnitGroup = RGE_Game_World__getUnitGroup(
                            (RGE_Game_World*)(*base_game)->world, this_->mPathUnit->mGroupID);
                        if (probeUnitGroup != 0)
                        {
                            RGE_Obstruction_Manager* obstructionManagerToUse =
                                (this_->mPathUnit->master_obj->movement_type == 1)
                                ? (RGE_Obstruction_Manager*)*OBSystem_Air : *OBSystem_Land;
                            BVector* probeWaypoint = BPath__waypoint(activePath, &waypointScratch,
                                this_->mPathData->waypoint);
                            findObSystemCollisions(obstructionManagerToUse,
                                this_->mPathUnit,
                                &this_->mPosition.mPosition,
                                probeWaypoint,
                                probeUnitGroup->mUnits,
                                probeUnitGroup->mNumberUnits,
                                1, 1.0);                   // checkTerrain = 1, radius 1.0
                        }
                        else
                        {
                            RGE_Obstruction_Manager* obstructionManagerToUse =
                                (this_->mPathUnit->master_obj->movement_type == 1)
                                ? (RGE_Obstruction_Manager*)*OBSystem_Air : *OBSystem_Land;
                            BVector* probeWaypoint = BPath__waypoint(activePath, &waypointScratch,
                                this_->mPathData->waypoint);
                            findObSystemCollisions(obstructionManagerToUse,
                                this_->mPathUnit,
                                &this_->mPosition.mPosition,
                                probeWaypoint,
                                &probeSeed,                // the LOCAL's address
                                1, 1, 1.0);
                        }
                        if (*numberObSystemCollisions == 0)   // asm loc_49D7C8 (jnz = findTile)
                        {
                            // clear: the 2-waypoint stub, then the validity check
                            newPath->waypointsCount = 0;
                            BPath__addWaypointAtStart(newPath, &this_->mPosition.mPosition);
                            BVector* fallbackWaypoint = BPath__waypoint(activePath, &waypointScratch,
                                this_->mPathData->waypoint);
                            BPath__addWaypointAtEnd(newPath, fallbackWaypoint);
                            newPath->creationTimeValue = (*base_game)->world->world_time;
                            BPath__calculatePathLength(newPath);
                            repathArmed = 1;               // asm loc_49D81A
                            validityReached = true;        // asm loc_49D822
                        }
                        else
                            findTileReached = true;        // asm loc_49D83C
                    }
                    else
                        findTileReached = true;            // asm loc_49D83C (the probe skip)
                }
                if (validityReached)
                {
                    // asm loc_49D822: the validity check
                    if (newPath->waypointsCount >= 2 && newPath->pathLengthValue >= 0.01f)
                        installReached = true;
                    else
                        findTileReached = true;
                }
                if (findTileReached)
                {
                    // ---- the findTile stage (asm loc_49D83C-0x49D9AD) ----
                    if (*dword_68956C == 1)
                    {
                        newPath->waypointsCount = 0;
                        BVector* tileWaypoint = BPath__waypoint(activePath, &waypointScratch,
                            this_->mPathData->waypoint);
                        int tileGoalY = (int)tileWaypoint->y;      // asm 0x49D881 (__ftol)
                        int tileGoalX = (int)tileWaypoint->x;      // asm 0x49D8A3 (__ftol)
                        int tileStartY = (int)this_->mPosition.mPosition.y;
                        int tileStartX = (int)this_->mPosition.mPosition.x;
                        repathArmed = (PathingSystem__findTilePath(pathSystem,
                            tileStartX, tileStartY, tileGoalX, tileGoalY,
                            this_->mPathUnit,
                            this_->mPathData->range,           // asm 0x49D86F (+0x20)
                            this_->mPathData->targetID,        // asm 0x49D868 (+0x24)
                            0,                                 // rLength
                            -1, -1,                            // the unobstructible ids
                            newPath,                           // rPath
                            0));                               // useRangeWithoutTarget -
                                                               // asm 0x49D8CA (and eax,
                                                               // 0FFh) / 0x49D8CF (mov
                                                               // var_268, eax) - FINDING
                                                               // C2: the binary's AND
                                                               // keeps exactly the AL
                                                               // byte, which IS the bool
                                                               // return's own value -
                                                               // this direct bool read
                                                               // is the same narrowing
                                                               // (the literal & 0xFF was
                                                               // rejected: a compiled
                                                               // no-op, C4806)
                        newPath->creationTimeValue = currentPlayer->world->world_time;   // FINDING D
                        BPath__calculatePathLength(newPath);
                        if (newPath->pathLengthValue >= 1e-7f)
                        {
                            // asm loc_49D903: the tail quality check - both norms
                            // UNROUNDED into the fcompp; the {<, NaN} set discards.
                            // TWO waypoints live together here: the tile path's tail
                            // AND the original path's waypoint - waypointScratch2
                            // prevents the aliasing (the second fetch overwrites the
                            // first's out-buffer when they share one buffer).
                            BVector* newTail = BPath__waypoint(newPath, &waypointScratch2,
                                newPath->waypointsCount - 1);
                            BVector* originalWaypoint2 = BPath__waypoint(activePath,
                                &waypointScratch, this_->mPathData->waypoint);
                            BVector tailDelta2;
                            BVector__operator_minus(originalWaypoint2, &tailDelta2, newTail);
                            tailDelta2.z = 0.0f;               // FINDING B3
                            if (X87_LT_NAN(sqrt((double)repathDelta.x * repathDelta.x
                                + (double)repathDelta.y * repathDelta.y
                                + (double)repathDelta.z * repathDelta.z),
                                sqrt((double)tailDelta2.x * tailDelta2.x
                                    + (double)tailDelta2.y * tailDelta2.y)))
                            {
                                newPath->waypointsCount = 0;   // asm 0x49D9A5
                                newPath->pathLengthValue = 0.0f;
                            }
                        }
                        else
                        {
                            repathArmed = 0;                   // asm loc_49D8F6
                        }
                    }
                    installReached = true;                     // asm loc_49D9AD
                }
                // ---- the install gate (asm loc_49D9AD-0x49DA75) ----
                if (newPath->waypointsCount > 1)
                {
                    // ---- the install success (asm loc_49DA7A-0x49DB6A) ----
                    this_->mPathData->state = 0;
                    this_->mPathData->continueCounter = 10;    // +0x2C = 0Ah
                    if (this_->mPathData->waypointLevel == 2 && smoothUsed == 1)
                    {
                        BPathData__setReadyToPath(successor, newPath,
                            this_->mPathData->range, this_->mPathData->targetID);
                        BPathData__setLinkedPath(successor, this_->mPathData,
                            (repathArmed == 1) ? 1 : 2);
                        successor->waypointLevel = 1;
                    }
                    else
                    {
                        BPathData__setReadyToFollowPath(successor, newPath);
                        BPathData__setLinkedPath(successor, this_->mPathData,
                            (repathArmed == 1) ? 1 : 2);
                        successor->waypointLevel = 0;
                    }
                    successor->range = this_->mPathData->range;
                    successor->targetID = this_->mPathData->targetID;
                    this_->mPathData = successor;
                    activePath = newPath;              // the active path = the successor
                    if (*dword_689564 == 1 || *dword_68956C == 1)
                        continue;                      // asm 0x49DAFB-0x49DB12
                    // the config off: fall into the walk arm
                }
                else
                {
                    // ---- the install failure arms (asm loc_49D9BD-0x49DA75) ----
                    if (successor != 0)                // the guarded teardown
                    {
                        BPathData__destructor(successor);
                        operator_delete_internal(successor);
                    }
                    RGE_Game_World__destroyPath2((RGE_Game_World*)(*base_game)->world, newPath);
                    if (repathArmed == 1)
                    {
                        if ((activePath->flags.valueValue & 1) == 0)
                        {
                            // asm loc_49E126
                            this_->vfptr->stopMoving(this_);
                            BPathData__reset(this_->mPathData);
                            return 1;
                        }
                        BPathData__setFlag(this_->mPathData, 0, 1);
                        continue;                      // asm 0x49DA09
                    }
                    if (this_->mPathData->waypoint >= activePath->waypointsCount - 1)
                    {
                        if (BPathData__continuesLeft(this_->mPathData, 0) > 0)
                        {
                            this_->vfptr->stopMoving(this_);       // loc_49E118
                            return 0;
                        }
                        if ((activePath->flags.valueValue & 1) != 0)
                        {
                            // asm loc_49E13D: the DIRECT write - no pauseMovement vtbl member
                            this_->mPathData->pauseTime = 0.5f;
                            return 0;
                        }
                        if (this_->mPathData->waypointLevel >= 2
                            || BPathData__highestLinkedPathHasMoreWaypoints(this_->mPathData) != 1)
                        {
                            // loc_49E102: the shared failure exit
                            BPathData__setFailure(this_->mPathData);
                            BPathData__reset(this_->mPathData);
                            this_->vfptr->stopMoving(this_);
                            return 0;
                        }
                        this_->mPathData->state = 5;
                        continue;                      // asm 0x49DA75
                    }
                    BPathData__setFlag(this_->mPathData, 0, 1);
                    continue;                          // asm 0x49DA26
                }
            }
        }
        // ---- the walk arm (asm loc_49DB6A-0x49DDF6) ----
        // reached with state != 2, or directly after a non-relooping install
        BVector* moveWaypoint = BPath__waypoint(activePath, &waypointScratch,
            this_->mPathData->waypoint);
        BVector arriveDelta;
        BVector__operator_minus(&this_->mPosition.mPosition, &arriveDelta, moveWaypoint);
        arriveDelta.z = 0.0f;
        double arriveNorm = sqrt((double)arriveDelta.x * arriveDelta.x
            + (double)arriveDelta.y * arriveDelta.y);
        if (X87_LT_NAN(arriveNorm, 1e-7f))
        {
            // asm loc_49DBCD: already at the waypoint - arm the increment
            BPathData__setFlag(this_->mPathData, 1, 1);
            BPathData__setFlag(this_->mPathData, 0, 1);
            continue;
        }
        // asm 0x49DBC0-0x49DBCB (jz short loc_49DBF0): the not-at-waypoint edge
        // falls into the integration. (The stray unconditional goto that sat here
        // armed the flags on every call - the tick-1 desync, fixed 2026-11-20.)
        // ---- the movement integration (asm loc_49DBF0-0x49DDF6) ----
        BVector* integrationWaypoint = BPath__waypoint(activePath, &waypointScratch,
            this_->mPathData->waypoint);
        BVector moveDelta;
        BVector__operator_minus(integrationWaypoint, &moveDelta,
            &this_->mPosition.mPosition);
        double moveNorm = sqrt((double)moveDelta.x * moveDelta.x
            + (double)moveDelta.y * moveDelta.y);  // the planar x^2+y^2; consumed
                                               // UNROUNDED by the three fdivs
        BVector dirNormalized;
        dirNormalized.x = (float)((double)moveDelta.x / moveNorm);
        dirNormalized.z = (float)(0.0 / moveNorm);
        dirNormalized.y = (float)((double)moveDelta.y / moveNorm);
        // the st0-boundary discipline: the returned delta's raw copy consumed
        // UNROUNDED in BOTH multiplies - no m32 store; ONE fstp on the product
        double timeDeltaRaw = RGE_Game_World__getWorldTimeDeltaSeconds(
            (RGE_Game_World*)(*base_game)->world);
        float displacement = (float)(((double)timeDeltaRaw * this_->mAcceleration.y
            + this_->mVelocity.y) * timeDeltaRaw);
        BVector dirScaled;
        dirScaled.z = dirNormalized.z * displacement;
        dirScaled.y = dirNormalized.y * displacement;
        dirScaled.x = dirNormalized.x * displacement;
        BVector integratedPos;
        BVector__operator_plus(&this_->mPosition.mPosition, &integratedPos, &dirScaled);
        {
            BVector* freshWaypoint = BPath__waypoint(activePath, &waypointScratch,
                this_->mPathData->waypoint);
            BVector waypointMinusPosition;
            BVector waypointMinusIntegrated;
            BVector integratedMinusPosition;
            BVector__operator_minus(freshWaypoint, &waypointMinusPosition,
                &this_->mPosition.mPosition);
            BVector__operator_minus(freshWaypoint, &waypointMinusIntegrated, &integratedPos);
            BVector__operator_minus(&integratedPos, &integratedMinusPosition,
                &this_->mPosition.mPosition);
            waypointMinusPosition.z = 0.0f;
            waypointMinusIntegrated.z = 0.0f;
            double waypointDistance = sqrt((double)waypointMinusPosition.x * waypointMinusPosition.x
                + (double)waypointMinusPosition.y * waypointMinusPosition.y);   // planar; UNROUNDED
            double integratedDistance = sqrt(
                (double)integratedMinusPosition.x * integratedMinusPosition.x
                + (double)integratedMinusPosition.z * integratedMinusPosition.z
                + (double)integratedMinusPosition.y * integratedMinusPosition.y);   // the FULL 3-comp
            float integratedDistanceStored = (float)integratedDistance;
                                                   // the store the 0.1 gate re-reads
            if (!X87_LE_NAN(integratedDistance, waypointDistance))
            {
                // crossed the waypoint segment this pass: the duplicated flag tail
                BPathData__setFlag(this_->mPathData, 1, 1);
                BPathData__setFlag(this_->mPathData, 0, 1);
                bool advance = (waypointDistance >= sqrt(
                    (double)waypointMinusIntegrated.y * waypointMinusIntegrated.y
                    + (double)waypointMinusIntegrated.x * waypointMinusIntegrated.x))
                    && integratedDistanceStored >= 0.1f;
                if (!advance)
                    continue;                      // asm loc_49DBCD re-loop
            }
            // not crossed: straight to the OB pass (no flags)
        }
        // ---- the OBSystem pass (asm loc_49DDF8-0x49DFFF) ----
        resetIgnoreList_new();
        addToIgnoreList_new((RGE_Static_Object*)this_->mPathUnit);
        BUnitGroup* unitGroup = RGE_Game_World__getUnitGroup(
            (RGE_Game_World*)(*base_game)->world, this_->mPathUnit->mGroupID);
        if (unitGroup != 0 && unitGroup->mNumberUnits > 0)
        {
            int memberIndex = 0;
            do
            {
                RGE_Static_Object* groupMember = RGE_Game_World__object(
                    (RGE_Game_World*)(*base_game)->world, unitGroup->mUnits[memberIndex]);
                addToIgnoreList_new(groupMember);
                ++memberIndex;
            } while (memberIndex < unitGroup->mNumberUnits);
        }
        RGE_Obstruction_Manager* obstructionManagerToUse =
            (this_->mPathUnit->master_obj->movement_type == 1)
            ? (RGE_Obstruction_Manager*)*OBSystem_Air : *OBSystem_Land;
        findObSystemCollisions(obstructionManagerToUse,
            this_->mPathUnit,
            &this_->mPosition.mPosition,           // p1 = the current position
            &integratedPos,                        // p2 = the integrated next position
            ignoreUnits,
            ignoreListSize,
            0, 1.0);                               // checkTerrain = 0, radiusModifier = 1.0
        if (*numberObSystemCollisions > 0)         // asm 0x49DEB8-0x49DEBF (jle = the skip)
        {
            int collisionTargetFound = 0;
            int freshBlockers = 0;
            for (int recordIndex = 0; recordIndex < *numberObSystemCollisions; ++recordIndex)
            {
                ObsRecord* record = obSystemCollisions[recordIndex];
                RGE_Static_Object* hitObject = record->Object;
                if (hitObject != 0)
                {
                    RGE_Static_Object* blocker = RGE_Game_World__object(
                        (RGE_Game_World*)(*base_game)->world, hitObject->id);
                    if (blocker != 0
                        && BPath__ignoreUnitsContain(activePath, blocker->id) != 1)
                    {
                        BPath* highest = BPathData__highestLinkedPathPointer(this_->mPathData);
                        if (highest == 0
                            || BPath__ignoreUnitsContain(highest, blocker->id) != 1)
                        {
                            if (this_->mPathData->targetID >= 0
                                && blocker->id == this_->mPathData->targetID)
                                collisionTargetFound = 1;
                            if (activePath->creationTimeValue
                                < blocker->vfptr->lastMoveTime(blocker))   // slot +0x30C = 195
                                ++freshBlockers;
                        }
                    }
                }
            }
            if (*numberObSystemCollisions == 1 && collisionTargetFound == 1)
            {
                BPathData__setFlag(this_->mPathData, 0, 1);
                BPathData__setFlag(this_->mPathData, 2, 1);
                continue;                          // asm 0x49DFAA
            }
            if (freshBlockers > 0)
            {
                BPathData* relink = this_->mPathData->linkedPath;
                if (relink == 0)
                {
                    // asm loc_49E1F4: the shared reset+stop+1 exit
                    BPathData__reset(this_->mPathData);
                    this_->vfptr->stopMoving(this_);
                    return 1;
                }
                relink->state = 2;
                this_->mPathData->linkedPath = 0;
                if (this_->mPathData != 0)         // the guarded teardown
                {
                    BPathData__destructor(this_->mPathData);
                    operator_delete_internal(this_->mPathData);
                }
                this_->mPathData = relink;
                continue;                          // asm 0x49DFFA
            }
        }
        // ---- the target gate (asm loc_49DFFF-0x49E151) ----
        bool faceNow;
        if (targetObj != 0)
        {
            RGE_Static_Object* freshTarget = RGE_Game_World__object(
                (RGE_Game_World*)(*base_game)->world, this_->mPathData->targetID);
            // the st0 -> fsub range -> fabs -> fcomp dbl_6545B0; NO m32 store
            double freshDistance = fabs(RGE_Static_Object__distance_to_position(freshTarget,
                this_->mPosition.mPosition.x,
                this_->mPosition.mPosition.y,
                this_->mPosition.mPosition.z)
                - (double)this_->mPathData->range);
            if (!(freshDistance >= (double)dbl_6545B0))
            {
                BPathData__setFlag(this_->mPathData, 0, 1);
                continue;                          // asm 0x49E05C
            }
            faceNow = true;
        }
        else
        {
            // loc_49E151: the no-target arrival gate
            if ((activePath->flags.valueValue & 1) != 0)
                faceNow = true;
            else if (X87_LE_NAN(this_->mPathData->range, 0.1f))
                faceNow = true;                    // the {<=, NaN} taken set
            else
            {
                BPath* highest = BPathData__highestLinkedPathPointer(this_->mPathData);
                if (highest == 0)
                    faceNow = true;
                else
                {
                    BVector* lastLinkedWaypoint = BPath__waypoint(highest, &waypointScratch,
                        highest->waypointsCount - 1);
                    BVector arrivalDelta;
                    BVector__operator_minus(&this_->mPosition.mPosition, &arrivalDelta,
                        lastLinkedWaypoint);
                    // the FULL 3-comp norm in the asm's ((y^2)+(z^2))+x^2 order,
                    // UNROUNDED into the fsub/fcomp
                    double arrivalDistance = sqrt((double)arrivalDelta.y * arrivalDelta.y
                        + (double)arrivalDelta.z * arrivalDelta.z
                        + (double)arrivalDelta.x * arrivalDelta.x);
                    arrivalDistance -= (double)this_->mPathData->range;
                    if (arrivalDistance >= 1e-7f)
                        faceNow = true;
                    else
                    {
                        // asm loc_49E1E3: fully arrived
                        BPathData__reset(this_->mPathData);
                        this_->vfptr->stopMoving(this_);
                        return 1;
                    }
                }
            }
        }
        if (faceNow)
        {
            // ---- the facing arm (asm loc_49E20B-0x49E3DF) ----
            BVector facingDirCur;
            facingDirCur.x = this_->mPosition.mOrientationForward.x;
            facingDirCur.y = this_->mPosition.mOrientationForward.y;
            double facingNorm = sqrt((double)facingDirCur.x * facingDirCur.x
                + (double)facingDirCur.y * facingDirCur.y);   // x^2+y^2 planar; fsqrt,
                                                          // consumed UNROUNDED by the fdivs
            facingDirCur.x = (float)((double)facingDirCur.x / facingNorm);
            facingDirCur.z = (float)(0.0 / facingNorm);   // the +4 slot = .z; the division
                                                          // kept (the dead store, as in the asm)
            facingDirCur.y = (float)((double)facingDirCur.y / facingNorm);
            float facingTimeDelta = (float)RGE_Game_World__getWorldTimeDeltaSeconds(
                (RGE_Game_World*)(*base_game)->world);        // the STORE-ROUND is the original's own
            float facingYaw = (float)BVector__angleBetweenVector(&dirNormalized, &facingDirCur);
            double facingYawCap = (double)facingTimeDelta * (double)this_->mMaximumYawPerSecondMoving;
            if (facingYaw > facingYawCap)
                facingYaw = (float)facingYawCap;
            BVector yawCross;
            BVector__operator_mul(&dirNormalized, &yawCross, &facingDirCur);
            if (yawCross.z > 0.0)
                facingYaw = -facingYaw;
            if (fabs((double)facingYaw) > dbl_6545B0)
            {
                BMatrix facingRotate;
                BMatrix__makeRotateArbitrary(&facingRotate, facingYaw,
                    &this_->mNewPosition.mOrientationUp);
                this_->mNewPosition.mOrientationForward = *BMatrix__operator_mul(&facingRotate,
                    &waypointScratch, &this_->mNewPosition.mOrientationForward);
                this_->mNewPosition.mOrientationRight = *BMatrix__operator_mul(&facingRotate,
                    &waypointScratch, &this_->mNewPosition.mOrientationRight);
                double forwardNorm = sqrt(
                    (double)this_->mNewPosition.mOrientationForward.z * this_->mNewPosition.mOrientationForward.z
                    + (double)this_->mNewPosition.mOrientationForward.y * this_->mNewPosition.mOrientationForward.y
                    + (double)this_->mNewPosition.mOrientationForward.x * this_->mNewPosition.mOrientationForward.x);
                this_->mNewPosition.mOrientationForward.x = this_->mNewPosition.mOrientationForward.x / forwardNorm;
                this_->mNewPosition.mOrientationForward.z = this_->mNewPosition.mOrientationForward.z / forwardNorm;
                this_->mNewPosition.mOrientationForward.y = this_->mNewPosition.mOrientationForward.y / forwardNorm;
                double rightNorm = sqrt(
                    (double)this_->mNewPosition.mOrientationRight.x * this_->mNewPosition.mOrientationRight.x
                    + (double)this_->mNewPosition.mOrientationRight.z * this_->mNewPosition.mOrientationRight.z
                    + (double)this_->mNewPosition.mOrientationRight.y * this_->mNewPosition.mOrientationRight.y);
                this_->mNewPosition.mOrientationRight.x = this_->mNewPosition.mOrientationRight.x / rightNorm;
                this_->mNewPosition.mOrientationRight.z = this_->mNewPosition.mOrientationRight.z / rightNorm;
                this_->mNewPosition.mOrientationRight.y = this_->mNewPosition.mOrientationRight.y / rightNorm;
                this_->vfptr->setFlag(this_, 0, 1);
            }
            this_->vfptr->setVelocityForward(this_, this_->mMaximumVelocityForward);
        }
        else
        {
            // asm loc_49E1E3: fully arrived
            BPathData__reset(this_->mPathData);
            this_->vfptr->stopMoving(this_);
        }
        return 1;
        }
    }
#pragma float_control(pop)


void install_BMotion()
{
    setHook((void*)0x0049CB60, BMotion__updateMovement_new);
    //setHook((void*)0x004A0350, addToIgnoreList_new);
    //setHook((void*)0x004A0340, resetIgnoreList_new);
}
