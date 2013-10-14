/*
** Copyright (C) 2009-2013 Mischa Sandberg <mischasan@gmail.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License Version 2 as
** published by the Free Software Foundation.  You may not use, modify or
** distribute this program under any other version of the GNU General
** Public License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#ifndef _PSEARCH_H
#define _PSEARCH_H

#ifndef MSUTIL_H
#include <stdint.h>
#include <stdlib.h> // malloc
#include <string.h> // memcpy
typedef int (*qsort_cmp)(const void *, const void *);
#endif

typedef uint32_t TRAN, STATE, STRNO;
typedef uint16_t SYMBOL;
typedef unsigned _SYMBOL; // An efficient stacklocal SYMBOL

enum { IS_MATCH = 0x80000000UL,
       IS_SUFFIX = 0x40000000UL,
       T_FLAGS = IS_MATCH | IS_SUFFIX
};

typedef struct { STATE state; STRNO strno; } STRASH;

typedef enum { BASE=2, USED=1 } USES;

struct psearch {
    TRAN *tranv;
    STRASH *hashv;
    unsigned flags;
#   define IS_MMAP 1
    TRAN sym_mask;
    unsigned sym_bits;
    unsigned hash_mod; // search hashv starting at (state + sym) % hash_mod.
    unsigned hash_size; // #(hashv): hash_mod plus the overflows past [hash_mod-1]
    unsigned tran_size; // #(tranv)
    unsigned nsyms, nchars, nstrs, maxlen;
    SYMBOL symv[256];
};

#include "psearch.h"

// p_size: size of tranv + hashv
static inline unsigned p_size(PSEARCH const *psp)
{ return psp->hash_size * sizeof*psp->hashv
       + psp->tran_size * sizeof*psp->tranv; }

// With gcc -O3, t_valid takes 1 op, p_tran and t_next take 3 ops.

static inline unsigned p_hash(PSEARCH const *psp, STATE s)
    { return s * 107 % psp->hash_mod; }

static inline TRAN p_tran(PSEARCH const *psp, STATE s, _SYMBOL sym)
    { return psp->tranv[s + sym] ^ sym; }

static inline void set_tranv(PSEARCH *psp, void *mem)
    { psp->hashv = (STRASH*)&(psp->tranv = mem)[psp->tran_size]; }

// TRAN bitfield accessors
static inline _SYMBOL t_sym(PSEARCH const *psp, TRAN t)
    { return t & psp->sym_mask; }

static inline STATE t_next(PSEARCH const *psp, TRAN t)
    { return (t & ~T_FLAGS) >> psp->sym_bits; }

static inline int t_isleaf(PSEARCH const *psp, TRAN t)
    { return t_next(psp, t) >= psp->tran_size; }

static inline int t_strno(PSEARCH const *psp, TRAN t)
    { return t_next(psp, t) - psp->tran_size; }

static inline _SYMBOL t_valid(PSEARCH const *psp, TRAN t)
    { return !t_sym(psp, t); }

#endif//_PSEARCH_H
