#if defined _omp_gta_pedgrp_included
    #endinput
#endif
#define _omp_gta_pedgrp_included

#include <omp_gta_peds_ide>

#define GTA_PEDGRP_FILE "pedgrp.dat"

#define GTA_PEDGRP_MAX_PEDS (32)

#define GTA_PEDGRP_NAME_LENGTH (16)
#define GTA_PEDGRP_MAX_LINE_LENGTH (512)

enum E_GTA_PEDGRP {
    GTA_PEDGRP_WORKERS = 0,
    GTA_PEDGRP_WORKERS_SF,
    GTA_PEDGRP_WORKERS_VEGAS,
    GTA_PEDGRP_BUSINESS,
    GTA_PEDGRP_BUSINESS_SF,
    GTA_PEDGRP_BUSINESS_VEGAS,
    GTA_PEDGRP_CLUBBERS,
    GTA_PEDGRP_CLUBBERS_SF,
    GTA_PEDGRP_CLUBBERS_VEGAS,
    GTA_PEDGRP_FARMERS,
    GTA_PEDGRP_BEACHFOLK,
    GTA_PEDGRP_PARKFOLK,
    GTA_PEDGRP_PARKFOLK_SF,
    GTA_PEDGRP_PARKFOLK_VEGAS,
    GTA_PEDGRP_CASUAL_RICH,
    GTA_PEDGRP_CASUAL_RICH_SF,
    GTA_PEDGRP_CASUAL_RICH_VEGAS,
    GTA_PEDGRP_CASUAL_AVERAGE,
    GTA_PEDGRP_CASUAL_AVERAGE_SF,
    GTA_PEDGRP_CASUAL_AVERAGE_VEGAS,
    GTA_PEDGRP_CASUAL_POOR,
    GTA_PEDGRP_CASUAL_POOR_SF,
    GTA_PEDGRP_CASUAL_POOR_VEGAS,
    GTA_PEDGRP_PROSTITUTES,
    GTA_PEDGRP_PROSTITUTES_SF,
    GTA_PEDGRP_PROSTITUTES_VEGAS,
    GTA_PEDGRP_CRIMINALS,
    GTA_PEDGRP_CRIMINALS_SF,
    GTA_PEDGRP_CRIMINALS_VEGAS,
    GTA_PEDGRP_GOLFERS,
    GTA_PEDGRP_SERVANTS,
    GTA_PEDGRP_SERVANTS_SF,
    GTA_PEDGRP_SERVANTS_VEGAS,
    GTA_PEDGRP_AIRCREW,
    GTA_PEDGRP_AIRCREW_SF,
    GTA_PEDGRP_AIRCREW_VEGAS,
    GTA_PEDGRP_ENTERTAINERS,
    GTA_PEDGRP_ENTERTAINERS_SF,
    GTA_PEDGRP_ENTERTAINERS_VEGAS,
    GTA_PEDGRP_OUT_OF_TOWN_FACTORY_WORKERS,
    GTA_PEDGRP_DESERT_FOLK,
    GTA_PEDGRP_AIRCREW_RUNWAY,
    GTA_PEDGRP_BALLAS,
    GTA_PEDGRP_GROVE,
    GTA_PEDGRP_VAGOS,
    GTA_PEDGRP_SF_RIFA,
    GTA_PEDGRP_DA_NANG_BOYS,
    GTA_PEDGRP_ITALIAN_MAFIA,
    GTA_PEDGRP_TRIADS,
    GTA_PEDGRP_VARRIO_LOS_AZTECAS,
    GTA_PEDGRP_UNUSED_GANG_1,
    GTA_PEDGRP_UNUSED_GANG_2,
    GTA_PEDGRP_DEALERS,
    GTA_PEDGRP_SHOPKEEPERS,
    GTA_PEDGRP_OFFICE_WORKERS,
    GTA_PEDGRP_HUSBANDS,
    GTA_PEDGRP_WIVES,
    GTA_PEDGRP_COUNT
};

#define GTA_PEDGRP_MAX_COUNT (_:GTA_PEDGRP_COUNT)

/* Includes GTA's two unused gang group slots. */
#define GTA_PEDGRP_GANG_FIRST (_:GTA_PEDGRP_BALLAS)
#define GTA_PEDGRP_GANG_COUNT (_:GTA_PEDGRP_UNUSED_GANG_2 - _:GTA_PEDGRP_BALLAS + 1)

static gPedGroupNames[GTA_PEDGRP_MAX_COUNT][GTA_PEDGRP_MAX_PEDS][GTA_PEDGRP_NAME_LENGTH];
static gPedGroupPedCount[GTA_PEDGRP_MAX_COUNT];
static gPedGroupPeds[GTA_PEDGRP_MAX_COUNT][GTA_PEDGRP_MAX_PEDS];
static bool:gPedGroupsLoaded;
static bool:gPedGroupsResolved;

static stock bool:PedGrp_CopyTrimmedRange(const source[], start, end, destination[], destinationSize) {
    while(start < end && (source[start] == ' ' || source[start] == '\t'))
        start++;

    while(end > start && (source[end - 1] == ' ' || source[end - 1] == '\t' || source[end - 1] == '\r' || source[end - 1] == '\n'))
        end--;

    new length = end - start;

    if(length <= 0 || length >= destinationSize)
        return false;

    for(new i = 0; i < length; i++)
        destination[i] = source[start + i];

    destination[length] = '\0';
    return true;
}

static stock bool:PedGrp_IsLineEmpty(const line[]) {
    for(new i = 0; line[i] != '\0'; i++) {
        if(line[i] != ' ' && line[i] != '\t' && line[i] != '\r' && line[i] != '\n')
            return false;
    }

    return true;
}

static stock bool:PedGrp_IsCommentLine(const line[]) {
    new i = 0;

    while(line[i] == ' ' || line[i] == '\t')
        i++;

    return line[i] == '#';
}

static stock bool:ResolvePedGrp() {
    if(!IsPedIdeLoaded()) {
        print("[omp-gta-pedgrp] ERROR: peds.ide has not been loaded.");
        return false;
    }

    if(!gPedGroupsLoaded) {
        print("[omp-gta-pedgrp] ERROR: pedgrp.dat has not been loaded.");
        return false;
    }

    gPedGroupsResolved = false;

    for(new group = 0; group < GTA_PEDGRP_MAX_COUNT; group++) {
        for(new i = 0; i < gPedGroupPedCount[group]; i++) {
            new modelId = GetPedsIdeModelIdByName(gPedGroupNames[group][i]);

            if(modelId == -1) {
                printf("[omp-gta-pedgrp] ERROR: Unable to resolve ped: group=%d index=%d name='%s'", group, i, gPedGroupNames[group][i]);
                return false;
            }

            gPedGroupPeds[group][i] = modelId;
        }
    }

    gPedGroupsResolved = true;
    print("[omp-gta-pedgrp] Resolved all ped references.");
    return true;
}

static stock bool:PedGrp_ParseLine(const line[], E_GTA_PEDGRP:group) {
    if(group < GTA_PEDGRP_WORKERS || group >= E_GTA_PEDGRP:GTA_PEDGRP_MAX_COUNT)
        return false;

    new length = strlen(line);
    new commentPos = length;

    /*
    * Ignore optional comments.
    */
    for(new i = 0; i < length; i++) {
        if(line[i] == '#') {
            commentPos = i;
            break;
        }
    }

    new first = 0;

    while(first < commentPos && (line[first] == ' ' || line[first] == '\t'))
        first++;

    if(first >= commentPos)
        return false;

    new pedCount = 0;
    new tokenStart = first;

    for(new i = first; i <= commentPos; i++) {
        if(i == commentPos || line[i] == ',') {
            if(pedCount >= GTA_PEDGRP_MAX_PEDS) {
                printf("[omp-gta-pedgrp] ERROR: Group %d contains more than %d peds.", _:group, GTA_PEDGRP_MAX_PEDS);
                return false;
            }

            new pedName[GTA_PEDGRP_NAME_LENGTH];

            if(!PedGrp_CopyTrimmedRange(line, tokenStart, i, pedName, sizeof(pedName))) {
                printf("[omp-gta-pedgrp] ERROR: Empty/invalid ped name: group=%d index=%d", _:group, pedCount);
                return false;
            }

            format(gPedGroupNames[_:group][pedCount], GTA_PEDGRP_NAME_LENGTH, "%s", pedName);

            pedCount++;
            tokenStart = i + 1;
        }
    }

    gPedGroupPedCount[_:group] = pedCount;
    return true;
}

stock bool:LoadPedGrp(const filename[] = GTA_PEDGRP_FILE) {
    gPedGroupsLoaded = false;
    gPedGroupsResolved = false;

    new File:file = fopen(filename, io_read);

    if(!file) {
        printf("[omp-gta-pedgrp] Failed to open %s", filename);
        return false;
    }

    for(new group = 0; group < GTA_PEDGRP_MAX_COUNT; group++) {
        gPedGroupPedCount[group] = 0;

        for(new ped = 0; ped < GTA_PEDGRP_MAX_PEDS; ped++) {
            gPedGroupNames[group][ped][0] = '\0';
            gPedGroupPeds[group][ped] = -1;
        }
    }

    new line[GTA_PEDGRP_MAX_LINE_LENGTH];
    new groupIndex = 0;
    new length;

    while((length = fread(file, line)) > 0) {
        if(length >= GTA_PEDGRP_MAX_LINE_LENGTH - 1 && line[length - 1] != '\n') {
            printf("[omp-gta-pedgrp] ERROR: Line exceeds maximum length of %d characters.", GTA_PEDGRP_MAX_LINE_LENGTH - 1);

            fclose(file);
            return false;
        }

        while(length > 0 && (line[length - 1] == '\r' || line[length - 1] == '\n'))
            line[--length] = '\0';

        if(PedGrp_IsLineEmpty(line) || PedGrp_IsCommentLine(line))
            continue;

        if(groupIndex >= GTA_PEDGRP_MAX_COUNT) {
            printf("[omp-gta-pedgrp] ERROR: Unexpected extra ped group: %s", line);
            fclose(file);
            return false;
        }

        if(!PedGrp_ParseLine(line, E_GTA_PEDGRP:groupIndex)) {
            printf("[omp-gta-pedgrp] Failed to parse group %d: %s", groupIndex, line);
            fclose(file);
            return false;
        }

        groupIndex++;
    }

    fclose(file);

    if(groupIndex != GTA_PEDGRP_MAX_COUNT) {
        printf("[omp-gta-pedgrp] Expected %d ped groups, loaded %d.", GTA_PEDGRP_MAX_COUNT, groupIndex);
        return false;
    }

    for(new group = 0; group < GTA_PEDGRP_MAX_COUNT; group++) {
        if(gPedGroupPedCount[group] <= 0) {
            printf("[omp-gta-pedgrp] Missing/empty group: %d", group);
            return false;
        }
    }

    gPedGroupsLoaded = true;

    printf("[omp-gta-pedgrp] Loaded %d ped groups.", GTA_PEDGRP_MAX_COUNT);

    if(!ResolvePedGrp()) {
        gPedGroupsLoaded = false;
        return false;
    }

    return true;
}

stock bool:IsPedGrpLoaded() {
    return gPedGroupsLoaded && gPedGroupsResolved;
}

/*
* Returns how many peds are in a certain ped group.
*/
stock GetPedGrpPedCount(E_GTA_PEDGRP:group) {
    if(!IsPedGrpLoaded())
        return 0;

    if(group < GTA_PEDGRP_WORKERS || group >= E_GTA_PEDGRP:GTA_PEDGRP_MAX_COUNT)
        return 0;

    return gPedGroupPedCount[_:group];
}

/*
* Gets the ped model name from a ped group by index.
*/
stock bool:GetPedGrpPedName(E_GTA_PEDGRP:group, index, output[], size = sizeof(output)) {
    if(size <= 0)
        return false;

    output[0] = '\0';

    if(!IsPedGrpLoaded() || group < GTA_PEDGRP_WORKERS || group >= E_GTA_PEDGRP:GTA_PEDGRP_MAX_COUNT || index < 0 || index >= gPedGroupPedCount[_:group])
        return false;

    format(output, size, "%s", gPedGroupNames[_:group][index]);
    return true;
}

/*
* Gets the ped model ID from a ped group by index.
*/
stock GetPedGrpPedModel(E_GTA_PEDGRP:group, index) {
    if(!IsPedGrpLoaded() || group < GTA_PEDGRP_WORKERS || group >= E_GTA_PEDGRP:GTA_PEDGRP_MAX_COUNT || index < 0 || index >= gPedGroupPedCount[_:group])
        return -1;

    return gPedGroupPeds[_:group][index];
}

/*
* Gets a random ped model from a pedgrp.dat group.
*/
stock GetRandomPedGrpPed(E_GTA_PEDGRP:group) {
    if(!IsPedGrpLoaded() || group < GTA_PEDGRP_WORKERS || group >= E_GTA_PEDGRP:GTA_PEDGRP_MAX_COUNT)
        return -1;

    new count = gPedGroupPedCount[_:group];

    if(count <= 0)
        return -1;

    return gPedGroupPeds[_:group][random(count)];
}

/*
* Checks if a model ID belongs to a ped group.
*/
stock bool:IsPedInPedGrp(E_GTA_PEDGRP:group, modelId) {
    if(!IsPedGrpLoaded() || group < GTA_PEDGRP_WORKERS || group >= E_GTA_PEDGRP:GTA_PEDGRP_MAX_COUNT)
        return false;

    new count = gPedGroupPedCount[_:group];

    for(new i = 0; i < count; i++) {
        if(gPedGroupPeds[_:group][i] == modelId)
            return true;
    }

    return false;
}

/*
* Tests the loaded pedgrp.dat data.
* TestPedGrp();
* TestPedGrp(GTA_PEDGRP_WORKERS);
*/
stock bool:TestPedGrp(E_GTA_PEDGRP:filterGroup = E_GTA_PEDGRP:-1) {
    if(!IsPedGrpLoaded()) {
        printf("[omp-gta-pedgrp] ERROR: pedgrp.dat is not loaded/resolved.");
        return false;
    }

    if(filterGroup != E_GTA_PEDGRP:-1) {
        if(_:filterGroup < 0 || _:filterGroup >= GTA_PEDGRP_MAX_COUNT) {
            printf("[omp-gta-pedgrp] ERROR: Invalid test group: %d", _:filterGroup);
            return false;
        }
    }

    printf("[omp-gta-pedgrp] ============================================================");
    printf("[omp-gta-pedgrp] PEDGRP.DAT DATA TEST");

    if(filterGroup == E_GTA_PEDGRP:-1) {
        printf("[omp-gta-pedgrp] Filter: ALL");
    } else {
        printf("[omp-gta-pedgrp] Filter: group=%d", _:filterGroup);
    }

    printf("[omp-gta-pedgrp] ============================================================");

    new groupsChecked = 0;
    new pedsChecked = 0;
    new invalidGroups = 0;
    new invalidPeds = 0;
    new unresolvedPeds = 0;

    new minimumGroupSize = GTA_PEDGRP_MAX_PEDS + 1;
    new maximumGroupSize = 0;

    for(new group = 0; group < GTA_PEDGRP_MAX_COUNT; group++) {
        if(filterGroup != E_GTA_PEDGRP:-1 && group != _:filterGroup)
            continue;

        new E_GTA_PEDGRP:pedGroup = E_GTA_PEDGRP:group;
        new count = GetPedGrpPedCount(pedGroup);
        new bool:groupValid = true;

        printf("[omp-gta-pedgrp] GROUP %d", group);
        printf("[omp-gta-pedgrp] pedCount          = %d", count);

        if(count <= 0 || count > GTA_PEDGRP_MAX_PEDS) {
            groupValid = false;
            invalidGroups++;
        }

        if(count < minimumGroupSize)
            minimumGroupSize = count;

        if(count > maximumGroupSize)
            maximumGroupSize = count;

        for(new i = 0; i < count; i++) {
            new name[GTA_PEDGRP_NAME_LENGTH];
            new modelId = GetPedGrpPedModel(pedGroup, i);

            if(!GetPedGrpPedName(pedGroup, i, name, sizeof(name))) {
                printf("[omp-gta-pedgrp]   [%d] ERROR: Could not read ped name.", i);
                invalidPeds++;
                groupValid = false;
                continue;
            }

            if(modelId == -1) {
                printf("[omp-gta-pedgrp]   [%d] name=%s modelId=UNRESOLVED", i, name);
                unresolvedPeds++;
                groupValid = false;
                continue;
            }

            new resolvedName[GTA_PEDGRP_NAME_LENGTH];
            resolvedName[0] = '\0';

            if(!GetPedsIdeNameByModel(modelId, resolvedName, sizeof(resolvedName)) || strcmp(name, resolvedName, true)) {
                printf("[omp-gta-pedgrp]   [%d] ERROR: name=%s modelId=%d peds.ide='%s'", i, name, modelId, resolvedName);
                invalidPeds++;
                groupValid = false;
                continue;
            }

            printf("[omp-gta-pedgrp]   [%d] name=%s modelId=%d", i, name, modelId);
            pedsChecked++;
        }

        printf("[omp-gta-pedgrp] status            = %s", groupValid ? ("OK") : ("FAILED"));
        printf("[omp-gta-pedgrp] ------------------------------------------------------------");

        groupsChecked++;
    }

    if(groupsChecked == 0)
        minimumGroupSize = 0;

    printf("[omp-gta-pedgrp] ============================================================");
    printf("[omp-gta-pedgrp] SUMMARY");
    printf("[omp-gta-pedgrp] groupsChecked      = %d", groupsChecked);
    printf("[omp-gta-pedgrp] pedsChecked        = %d", pedsChecked);
    printf("[omp-gta-pedgrp] invalidGroups      = %d", invalidGroups);
    printf("[omp-gta-pedgrp] invalidPeds        = %d", invalidPeds);
    printf("[omp-gta-pedgrp] unresolvedPeds     = %d", unresolvedPeds);
    printf("[omp-gta-pedgrp] minimumGroupSize   = %d", minimumGroupSize);
    printf("[omp-gta-pedgrp] maximumGroupSize   = %d", maximumGroupSize);
    printf("[omp-gta-pedgrp] ============================================================");

    return invalidGroups == 0 && invalidPeds == 0 && unresolvedPeds == 0;
}
