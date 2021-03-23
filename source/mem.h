/*
 * mem.h
 *
 * Copyright (c) 2019, shchmue.
 * Copyright (c) 2020-2021, DarkMatterCore <pabloacurielz@gmail.com>.
 *
 * This file is part of nxdumptool (https://github.com/DarkMatterCore/nxdumptool).
 *
 * nxdumptool is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * nxdumptool is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MEM_H__
#define __MEM_H__

typedef enum {
    MemoryProgramSegmentType_Text   = BIT(0),
    MemoryProgramSegmentType_Rodata = BIT(1),
    MemoryProgramSegmentType_Data   = BIT(2)
} MemoryProgramSegmentType;

typedef struct {
    u64 program_id;
    u8 mask;        ///< MemoryProgramSegmentType. Used with memRetrieveProgramMemorySegment(). Ignored in memRetrieveFullProgramMemory().
    u8 *data;
    u64 data_size;
} MemoryLocation;

/// Retrieves memory segment (.text, .rodata, .data) data from a running program.
/// These are memory pages with read permission (Perm_R) enabled and type MemType_CodeStatic or MemType_CodeMutable.
bool memRetrieveProgramMemorySegment(MemoryLocation *location);

/// Retrieves full memory data from a running program.
/// These are any type of memory pages with read permission (Perm_R) enabled and no MemoryAttribute flag set.
/// MemType_Unmapped, MemType_Io, MemType_ThreadLocal and MemType_Reserved memory pages are excluded if FS program memory is being retrieved, in order to avoid hangs.
bool memRetrieveFullProgramMemory(MemoryLocation *location);

/// Frees a populated MemoryLocation element.
NX_INLINE void memFreeMemoryLocation(MemoryLocation *location)
{
    if (!location) return;
    if (location->data) free(location->data);
    location->data = NULL;
    location->data_size = 0;
}

#endif /* __MEM_H__ */

#ifdef __cplusplus
}
#endif