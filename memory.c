 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Memory management
  *
  * (c) 1995 Bernd Schmidt
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "config.h"
#include "options.h"
#include "uae.h"
#include "memory.h"
#include "ersatz.h"
#include "zfile.h"
#include "custom.h"
#include "events.h"
#include "newcpu.h"
#include "autoconf.h"
#include "savestate.h"
#include "ar.h"

#ifdef USE_MAPPED_MEMORY
#include <sys/mman.h>
#endif

#ifdef JIT
int canbang;

/* Set by each memory handler that does not simply access real memory.  */
int special_mem;
#endif

int ersatzkickfile;

#ifdef CD32
extern int cd32_enabled;
#endif
#ifdef CDTV
extern int cdtv_enabled;
#endif

uae_u32 allocated_chipmem;
uae_u32 allocated_fastmem;
uae_u32 allocated_bogomem;
uae_u32 allocated_gfxmem;
uae_u32 allocated_z3fastmem;
uae_u32 allocated_a3000mem;

static long chip_filepos;
static long bogo_filepos;
static long rom_filepos;

addrbank *mem_banks[MEMORY_BANKS];

/* This has two functions. It either holds a host address that, when added
   to the 68k address, gives the host address corresponding to that 68k
   address (in which case the value in this array is even), OR it holds the
   same value as mem_banks, for those banks that have baseaddr==0. In that
   case, bit 0 is set (the memory access routines will take care of it).  */

uae_u8 *baseaddr[MEMORY_BANKS];

#ifdef NO_INLINE_MEMORY_ACCESS
__inline__ uae_u32 longget (uaecptr addr)
{
    return call_mem_get_func (get_mem_bank (addr).lget, addr);
}
__inline__ uae_u32 wordget (uaecptr addr)
{
    return call_mem_get_func (get_mem_bank (addr).wget, addr);
}
__inline__ uae_u32 byteget (uaecptr addr)
{
    return call_mem_get_func (get_mem_bank (addr).bget, addr);
}
__inline__ void longput (uaecptr addr, uae_u32 l)
{
    call_mem_put_func (get_mem_bank (addr).lput, addr, l);
}
__inline__ void wordput (uaecptr addr, uae_u32 w)
{
    call_mem_put_func (get_mem_bank (addr).wput, addr, w);
}
__inline__ void byteput (uaecptr addr, uae_u32 b)
{
    call_mem_put_func (get_mem_bank (addr).bput, addr, b);
}
#endif

uae_u32 chipmem_mask, kickmem_mask, extendedkickmem_mask, bogomem_mask, a3000mem_mask;

static int illegal_count;
/* A dummy bank that only contains zeros */

static uae_u32 dummy_lget (uaecptr) REGPARAM;
static uae_u32 dummy_wget (uaecptr) REGPARAM;
static uae_u32 dummy_bget (uaecptr) REGPARAM;
static void dummy_lput (uaecptr, uae_u32) REGPARAM;
static void dummy_wput (uaecptr, uae_u32) REGPARAM;
static void dummy_bput (uaecptr, uae_u32) REGPARAM;
static int dummy_check (uaecptr addr, uae_u32 size) REGPARAM;

#define MAX_ILG 20

uae_u32 REGPARAM2 dummy_lget (uaecptr addr)
{
#ifdef JIT
    special_mem |= S_READ;
#endif
    if (currprefs.illegal_mem) {
	if (illegal_count < MAX_ILG) {
	    illegal_count++;
	    write_log ("Illegal lget at %08lx\n", addr);
	}
    }
    if (currprefs.cpu_level >= 2)
	return 0;
    return (regs.irc << 16) | regs.irc;
}

uae_u32 REGPARAM2 dummy_wget (uaecptr addr)
{
#ifdef JIT
    special_mem |= S_READ;
#endif
    if (currprefs.illegal_mem) {
	if (illegal_count < MAX_ILG) {
	    illegal_count++;
	    write_log ("Illegal wget at %08lx\n", addr);
	}
    }
    if (currprefs.cpu_level >= 2)
	return 0;
    return regs.irc;
}

uae_u32 REGPARAM2 dummy_bget (uaecptr addr)
{
#ifdef JIT
    special_mem |= S_READ;
#endif
    if (currprefs.illegal_mem) {
	if (illegal_count < MAX_ILG) {
	    illegal_count++;
	    write_log ("Illegal bget at %08lx\n", addr);
	}
    }
    if (currprefs.cpu_level >= 2)
	return 0;
    return (addr & 1) ? regs.irc : regs.irc >> 8;
}

void REGPARAM2 dummy_lput (uaecptr addr, uae_u32 l)
{
#ifdef JIT
    special_mem |= S_WRITE;
#endif 
   if (currprefs.illegal_mem) {
	if (illegal_count < MAX_ILG) {
	    illegal_count++;
	    write_log ("Illegal lput at %08lx\n", addr);
	}
    }
}
void REGPARAM2 dummy_wput (uaecptr addr, uae_u32 w)
{
#ifdef JIT
    special_mem |= S_WRITE;
#endif
    if (currprefs.illegal_mem) {
	if (illegal_count < MAX_ILG) {
	    illegal_count++;
	    write_log ("Illegal wput at %08lx\n", addr);
	}
    }
}
void REGPARAM2 dummy_bput (uaecptr addr, uae_u32 b)
{
#ifdef JIT
    special_mem |= S_WRITE;
#endif
    if (currprefs.illegal_mem) {
	if (illegal_count < MAX_ILG) {
	    illegal_count++;
	    write_log ("Illegal bput at %08lx\n", addr);
	}
    }
}

int REGPARAM2 dummy_check (uaecptr addr, uae_u32 size)
{
#ifdef JIT
    special_mem |= S_READ;
#endif
    if (currprefs.illegal_mem) {
	if (illegal_count < MAX_ILG) {
	    illegal_count++;
	    write_log ("Illegal check at %08lx\n", addr);
	}
    }

    return 0;
}

#ifdef AUTOCONFIG

/* A3000 "motherboard resources" bank.  */
static uae_u32 mbres_lget (uaecptr) REGPARAM;
static uae_u32 mbres_wget (uaecptr) REGPARAM;
static uae_u32 mbres_bget (uaecptr) REGPARAM;
static void mbres_lput (uaecptr, uae_u32) REGPARAM;
static void mbres_wput (uaecptr, uae_u32) REGPARAM;
static void mbres_bput (uaecptr, uae_u32) REGPARAM;
static int mbres_check (uaecptr addr, uae_u32 size) REGPARAM;

static int mbres_val = 0;

uae_u32 REGPARAM2 mbres_lget (uaecptr addr)
{
    special_mem |= S_READ;
    if (currprefs.illegal_mem)
	write_log ("Illegal lget at %08lx\n", addr);

    return 0;
}

uae_u32 REGPARAM2 mbres_wget (uaecptr addr)
{
    special_mem |= S_READ;
    if (currprefs.illegal_mem)
	write_log ("Illegal wget at %08lx\n", addr);

    return 0;
}

uae_u32 REGPARAM2 mbres_bget (uaecptr addr)
{
    special_mem |= S_READ;
    if (currprefs.illegal_mem)
	write_log ("Illegal bget at %08lx\n", addr);

    return (addr & 0xFFFF) == 3 ? mbres_val : 0;
}

void REGPARAM2 mbres_lput (uaecptr addr, uae_u32 l)
{
    special_mem |= S_WRITE;
    if (currprefs.illegal_mem)
	write_log ("Illegal lput at %08lx\n", addr);
}
void REGPARAM2 mbres_wput (uaecptr addr, uae_u32 w)
{
    special_mem |= S_WRITE;
    if (currprefs.illegal_mem)
	write_log ("Illegal wput at %08lx\n", addr);
}
void REGPARAM2 mbres_bput (uaecptr addr, uae_u32 b)
{
    special_mem |= S_WRITE;
    if (currprefs.illegal_mem)
	write_log ("Illegal bput at %08lx\n", addr);

    if ((addr & 0xFFFF) == 3)
	mbres_val = b;
}

int REGPARAM2 mbres_check (uaecptr addr, uae_u32 size)
{
    if (currprefs.illegal_mem)
	write_log ("Illegal check at %08lx\n", addr);

    return 0;
}

#endif

/* Chip memory */

uae_u8 *chipmemory;

static int chipmem_check (uaecptr addr, uae_u32 size) REGPARAM;
static uae_u8 *chipmem_xlate (uaecptr addr) REGPARAM;

#ifdef AGA

/* AGA ce-chipram access */

static void ce2_timeout (void)
{
    wait_cpu_cycle_read (0, -1);
}

uae_u32 REGPARAM2 chipmem_lget_ce2 (uaecptr addr)
{
    uae_u32 *m;

    special_mem |= S_READ;
    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
    m = (uae_u32 *)(chipmemory + addr);
    ce2_timeout ();
    return do_get_mem_long (m);
}

uae_u32 REGPARAM2 chipmem_wget_ce2 (uaecptr addr)
{
    uae_u16 *m;

    special_mem |= S_READ;
    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
    m = (uae_u16 *)(chipmemory + addr);
    ce2_timeout ();
    return do_get_mem_word (m);
}

uae_u32 REGPARAM2 chipmem_bget_ce2 (uaecptr addr)
{
    special_mem |= S_READ;
    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
    ce2_timeout ();
    return chipmemory[addr];
}

void REGPARAM2 chipmem_lput_ce2 (uaecptr addr, uae_u32 l)
{
    uae_u32 *m;

    special_mem |= S_WRITE;
    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
    m = (uae_u32 *)(chipmemory + addr);
    ce2_timeout ();
    do_put_mem_long (m, l);
}

void REGPARAM2 chipmem_wput_ce2 (uaecptr addr, uae_u32 w)
{
    uae_u16 *m;

    special_mem |= S_WRITE;
    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
    m = (uae_u16 *)(chipmemory + addr);
    ce2_timeout ();
    do_put_mem_word (m, w);
}

void REGPARAM2 chipmem_bput_ce2 (uaecptr addr, uae_u32 b)
{
    special_mem |= S_WRITE;
    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
    ce2_timeout ();
    chipmemory[addr] = b;
}

#endif

uae_u32 REGPARAM2 chipmem_lget (uaecptr addr)
{
    uae_u32 *m;

    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
    m = (uae_u32 *)(chipmemory + addr);
    return do_get_mem_long (m);
}

uae_u32 REGPARAM2 chipmem_wget (uaecptr addr)
{
    uae_u16 *m;

    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
    m = (uae_u16 *)(chipmemory + addr);
    return do_get_mem_word (m);
}

uae_u32 REGPARAM2 chipmem_bget (uaecptr addr)
{
    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
    return chipmemory[addr];
}

void REGPARAM2 chipmem_lput (uaecptr addr, uae_u32 l)
{
    uae_u32 *m;

    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
    m = (uae_u32 *)(chipmemory + addr);
    do_put_mem_long (m, l);
}


void REGPARAM2 chipmem_wput (uaecptr addr, uae_u32 w)
{
    uae_u16 *m;

    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
    m = (uae_u16 *)(chipmemory + addr);
    do_put_mem_word (m, w);
}

void REGPARAM2 chipmem_bput (uaecptr addr, uae_u32 b)
{
    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
    chipmemory[addr] = b;
}

int REGPARAM2 chipmem_check (uaecptr addr, uae_u32 size)
{
    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
    return (addr + size) <= allocated_chipmem;
}

uae_u8 REGPARAM2 *chipmem_xlate (uaecptr addr)
{
    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
    return chipmemory + addr;
}

/* Slow memory */

static uae_u8 *bogomemory;

static uae_u32 bogomem_lget (uaecptr) REGPARAM;
static uae_u32 bogomem_wget (uaecptr) REGPARAM;
static uae_u32 bogomem_bget (uaecptr) REGPARAM;
static void bogomem_lput (uaecptr, uae_u32) REGPARAM;
static void bogomem_wput (uaecptr, uae_u32) REGPARAM;
static void bogomem_bput (uaecptr, uae_u32) REGPARAM;
static int bogomem_check (uaecptr addr, uae_u32 size) REGPARAM;
static uae_u8 *bogomem_xlate (uaecptr addr) REGPARAM;

uae_u32 REGPARAM2 bogomem_lget (uaecptr addr)
{
    uae_u32 *m;
    addr -= bogomem_start & bogomem_mask;
    addr &= bogomem_mask;
    m = (uae_u32 *)(bogomemory + addr);
    return do_get_mem_long (m);
}

uae_u32 REGPARAM2 bogomem_wget (uaecptr addr)
{
    uae_u16 *m;
    addr -= bogomem_start & bogomem_mask;
    addr &= bogomem_mask;
    m = (uae_u16 *)(bogomemory + addr);
    return do_get_mem_word (m);
}

uae_u32 REGPARAM2 bogomem_bget (uaecptr addr)
{
    addr -= bogomem_start & bogomem_mask;
    addr &= bogomem_mask;
    return bogomemory[addr];
}

void REGPARAM2 bogomem_lput (uaecptr addr, uae_u32 l)
{
    uae_u32 *m;
    addr -= bogomem_start & bogomem_mask;
    addr &= bogomem_mask;
    m = (uae_u32 *)(bogomemory + addr);
    do_put_mem_long (m, l);
}

void REGPARAM2 bogomem_wput (uaecptr addr, uae_u32 w)
{
    uae_u16 *m;
    addr -= bogomem_start & bogomem_mask;
    addr &= bogomem_mask;
    m = (uae_u16 *)(bogomemory + addr);
    do_put_mem_word (m, w);
}

void REGPARAM2 bogomem_bput (uaecptr addr, uae_u32 b)
{
    addr -= bogomem_start & bogomem_mask;
    addr &= bogomem_mask;
    bogomemory[addr] = b;
}

int REGPARAM2 bogomem_check (uaecptr addr, uae_u32 size)
{
    addr -= bogomem_start & bogomem_mask;
    addr &= bogomem_mask;
    return (addr + size) <= allocated_bogomem;
}

uae_u8 REGPARAM2 *bogomem_xlate (uaecptr addr)
{
    addr -= bogomem_start & bogomem_mask;
    addr &= bogomem_mask;
    return bogomemory + addr;
}

#ifdef AUTOCONFIG

/* A3000 motherboard fast memory */

static uae_u8 *a3000memory;

static uae_u32 a3000mem_lget (uaecptr) REGPARAM;
static uae_u32 a3000mem_wget (uaecptr) REGPARAM;
static uae_u32 a3000mem_bget (uaecptr) REGPARAM;
static void a3000mem_lput (uaecptr, uae_u32) REGPARAM;
static void a3000mem_wput (uaecptr, uae_u32) REGPARAM;
static void a3000mem_bput (uaecptr, uae_u32) REGPARAM;
static int a3000mem_check (uaecptr addr, uae_u32 size) REGPARAM;
static uae_u8 *a3000mem_xlate (uaecptr addr) REGPARAM;

uae_u32 REGPARAM2 a3000mem_lget (uaecptr addr)
{
    uae_u32 *m;
    addr -= a3000mem_start & a3000mem_mask;
    addr &= a3000mem_mask;
    m = (uae_u32 *)(a3000memory + addr);
    return do_get_mem_long (m);
}

uae_u32 REGPARAM2 a3000mem_wget (uaecptr addr)
{
    uae_u16 *m;
    addr -= a3000mem_start & a3000mem_mask;
    addr &= a3000mem_mask;
    m = (uae_u16 *)(a3000memory + addr);
    return do_get_mem_word (m);
}

uae_u32 REGPARAM2 a3000mem_bget (uaecptr addr)
{
    addr -= a3000mem_start & a3000mem_mask;
    addr &= a3000mem_mask;
    return a3000memory[addr];
}

void REGPARAM2 a3000mem_lput (uaecptr addr, uae_u32 l)
{
    uae_u32 *m;
    addr -= a3000mem_start & a3000mem_mask;
    addr &= a3000mem_mask;
    m = (uae_u32 *)(a3000memory + addr);
    do_put_mem_long (m, l);
}

void REGPARAM2 a3000mem_wput (uaecptr addr, uae_u32 w)
{
    uae_u16 *m;
    addr -= a3000mem_start & a3000mem_mask;
    addr &= a3000mem_mask;
    m = (uae_u16 *)(a3000memory + addr);
    do_put_mem_word (m, w);
}

void REGPARAM2 a3000mem_bput (uaecptr addr, uae_u32 b)
{
    addr -= a3000mem_start & a3000mem_mask;
    addr &= a3000mem_mask;
    a3000memory[addr] = b;
}

int REGPARAM2 a3000mem_check (uaecptr addr, uae_u32 size)
{
    addr -= a3000mem_start & a3000mem_mask;
    addr &= a3000mem_mask;
    return (addr + size) <= allocated_a3000mem;
}

uae_u8 REGPARAM2 *a3000mem_xlate (uaecptr addr)
{
    addr -= a3000mem_start & a3000mem_mask;
    addr &= a3000mem_mask;
    return a3000memory + addr;
}

#endif

/* Kick memory */

uae_u8 *kickmemory;
uae_u16 kickstart_version;
int kickmem_size = 0x80000;

/*
 * A1000 kickstart RAM handling
 *
 * RESET instruction unhides boot ROM and disables write protection
 * write access to boot ROM hides boot ROM and enables write protection
 *
 */
static int a1000_kickstart_mode;
static uae_u8 *a1000_bootrom;
static void a1000_handle_kickstart (int mode)
{
    if (mode == 0) {
	a1000_kickstart_mode = 0;
	memcpy (kickmemory, kickmemory + 262144, 262144);
    } else {
	a1000_kickstart_mode = 1;
	memset (kickmemory, 0, 262144);
	memcpy (kickmemory, a1000_bootrom, 65536);
	memcpy (kickmemory + 131072, a1000_bootrom, 65536);
        kickstart_version = 0;
    }
    kickstart_version = (kickmemory[262144 + 12] << 8) | kickmemory[262144 + 13];
}

void a1000_reset (void)
{
    if (a1000_bootrom)
	a1000_handle_kickstart (1);
}

static uae_u32 kickmem_lget (uaecptr) REGPARAM;
static uae_u32 kickmem_wget (uaecptr) REGPARAM;
static uae_u32 kickmem_bget (uaecptr) REGPARAM;
static void kickmem_lput (uaecptr, uae_u32) REGPARAM;
static void kickmem_wput (uaecptr, uae_u32) REGPARAM;
static void kickmem_bput (uaecptr, uae_u32) REGPARAM;
static int kickmem_check (uaecptr addr, uae_u32 size) REGPARAM;
static uae_u8 *kickmem_xlate (uaecptr addr) REGPARAM;

uae_u32 REGPARAM2 kickmem_lget (uaecptr addr)
{
    uae_u32 *m;
    addr -= kickmem_start & kickmem_mask;
    addr &= kickmem_mask;
    m = (uae_u32 *)(kickmemory + addr);
    return do_get_mem_long (m);
}

uae_u32 REGPARAM2 kickmem_wget (uaecptr addr)
{
    uae_u16 *m;
    addr -= kickmem_start & kickmem_mask;
    addr &= kickmem_mask;
    m = (uae_u16 *)(kickmemory + addr);
    return do_get_mem_word (m);
}

uae_u32 REGPARAM2 kickmem_bget (uaecptr addr)
{
    addr -= kickmem_start & kickmem_mask;
    addr &= kickmem_mask;
    return kickmemory[addr];
}

void REGPARAM2 kickmem_lput (uaecptr addr, uae_u32 b)
{
    uae_u32 *m;
#ifdef JIT
    special_mem |= S_WRITE;
#endif
    if (a1000_kickstart_mode) {
	if (addr >= 0xfc0000) {
	    addr -= kickmem_start & kickmem_mask;
	    addr &= kickmem_mask;
	    m = (uae_u32 *)(kickmemory + addr);
	    do_put_mem_long (m, b);
	    return;
	} else
	    a1000_handle_kickstart (0);
    } else if (currprefs.illegal_mem)
	write_log ("Illegal kickmem lput at %08lx\n", addr);
}

void REGPARAM2 kickmem_wput (uaecptr addr, uae_u32 b)
{
    uae_u16 *m;
#ifdef JIT
    special_mem |= S_WRITE;
#endif
    if (a1000_kickstart_mode) {
	if (addr >= 0xfc0000) {
	    addr -= kickmem_start & kickmem_mask;
	    addr &= kickmem_mask;
	    m = (uae_u16 *)(kickmemory + addr);
	    do_put_mem_word (m, b);
	    return;
	} else
	    a1000_handle_kickstart (0);
    } else if (currprefs.illegal_mem)
	write_log ("Illegal kickmem wput at %08lx\n", addr);
}

void REGPARAM2 kickmem_bput (uaecptr addr, uae_u32 b)
{
#ifdef JIT
    special_mem |= S_WRITE;
#endif
    if (a1000_kickstart_mode) {
	if (addr >= 0xfc0000) {
	    addr -= kickmem_start & kickmem_mask;
	    addr &= kickmem_mask;
	    kickmemory[addr] = b;
	    return;
	} else
	    a1000_handle_kickstart (0);
    } else if (currprefs.illegal_mem)
	write_log ("Illegal kickmem lput at %08lx\n", addr);
}

void REGPARAM2 kickmem2_lput (uaecptr addr, uae_u32 l)
{
    uae_u32 *m;
#ifdef JIT
    special_mem |= S_WRITE;
#endif
    addr -= kickmem_start & kickmem_mask;
    addr &= kickmem_mask;
    m = (uae_u32 *)(kickmemory + addr);
    do_put_mem_long (m, l);
}

void REGPARAM2 kickmem2_wput (uaecptr addr, uae_u32 w)
{
    uae_u16 *m;
#ifdef JIT
    special_mem |= S_WRITE;
#endif
    addr -= kickmem_start & kickmem_mask;
    addr &= kickmem_mask;
    m = (uae_u16 *)(kickmemory + addr);
    do_put_mem_word (m, w);
}

void REGPARAM2 kickmem2_bput (uaecptr addr, uae_u32 b)
{
#ifdef JIT
    special_mem |= S_WRITE;
#endif
    addr -= kickmem_start & kickmem_mask;
    addr &= kickmem_mask;
    kickmemory[addr] = b;
}

int REGPARAM2 kickmem_check (uaecptr addr, uae_u32 size)
{
    addr -= kickmem_start & kickmem_mask;
    addr &= kickmem_mask;
    return (addr + size) <= kickmem_size;
}

uae_u8 REGPARAM2 *kickmem_xlate (uaecptr addr)
{
    addr -= kickmem_start & kickmem_mask;
    addr &= kickmem_mask;
    return kickmemory + addr;
}

/* CD32/CDTV extended kick memory */

uae_u8 *extendedkickmemory;
static int extendedkickmem_size;
static uae_u32 extendedkickmem_start;

#define EXTENDED_ROM_CD32 1
#define EXTENDED_ROM_CDTV 2

static int extromtype (void)
{
    switch (extendedkickmem_size) {
    case 524288:
	return EXTENDED_ROM_CD32;
    case 262144:
	return EXTENDED_ROM_CDTV;
    }
    return 0;
}

static uae_u32 extendedkickmem_lget (uaecptr) REGPARAM;
static uae_u32 extendedkickmem_wget (uaecptr) REGPARAM;
static uae_u32 extendedkickmem_bget (uaecptr) REGPARAM;
static void extendedkickmem_lput (uaecptr, uae_u32) REGPARAM;
static void extendedkickmem_wput (uaecptr, uae_u32) REGPARAM;
static void extendedkickmem_bput (uaecptr, uae_u32) REGPARAM;
static int extendedkickmem_check (uaecptr addr, uae_u32 size) REGPARAM;
static uae_u8 *extendedkickmem_xlate (uaecptr addr) REGPARAM;

uae_u32 REGPARAM2 extendedkickmem_lget (uaecptr addr)
{
    uae_u32 *m;
    addr -= extendedkickmem_start & extendedkickmem_mask;
    addr &= extendedkickmem_mask;
    m = (uae_u32 *)(extendedkickmemory + addr);
    return do_get_mem_long (m);
}

uae_u32 REGPARAM2 extendedkickmem_wget (uaecptr addr)
{
    uae_u16 *m;
    addr -= extendedkickmem_start & extendedkickmem_mask;
    addr &= extendedkickmem_mask;
    m = (uae_u16 *)(extendedkickmemory + addr);
    return do_get_mem_word (m);
}

uae_u32 REGPARAM2 extendedkickmem_bget (uaecptr addr)
{
    addr -= extendedkickmem_start & extendedkickmem_mask;
    addr &= extendedkickmem_mask;
    return extendedkickmemory[addr];
}

void REGPARAM2 extendedkickmem_lput (uaecptr addr, uae_u32 b)
{
#ifdef JIT
    special_mem |= S_WRITE;
#endif
    if (currprefs.illegal_mem)
	write_log ("Illegal extendedkickmem lput at %08lx\n", addr);
}

void REGPARAM2 extendedkickmem_wput (uaecptr addr, uae_u32 b)
{
#ifdef JIT
    special_mem |= S_WRITE;
#endif
    if (currprefs.illegal_mem)
	write_log ("Illegal extendedkickmem wput at %08lx\n", addr);
}

void REGPARAM2 extendedkickmem_bput (uaecptr addr, uae_u32 b)
{
#ifdef JIT
    special_mem |= S_WRITE;
#endif
    if (currprefs.illegal_mem)
	write_log ("Illegal extendedkickmem lput at %08lx\n", addr);
}

int REGPARAM2 extendedkickmem_check (uaecptr addr, uae_u32 size)
{
    addr -= extendedkickmem_start & extendedkickmem_mask;
    addr &= extendedkickmem_mask;
    return (addr + size) <= extendedkickmem_size;
}

uae_u8 REGPARAM2 *extendedkickmem_xlate (uaecptr addr)
{
    addr -= extendedkickmem_start & extendedkickmem_mask;
    addr &= extendedkickmem_mask;
    return extendedkickmemory + addr;
}

/* Default memory access functions */

int REGPARAM2 default_check (uaecptr a, uae_u32 b)
{
    return 0;
}

static int be_cnt;

uae_u8 REGPARAM2 *default_xlate (uaecptr a)
{
    if (quit_program == 0) {
	/* do this only in 68010+ mode, there are some tricky A500 programs.. */
	if (currprefs.cpu_level > 0 || !currprefs.cpu_compatible) {
	    if (be_cnt < 3) {
		int i, j;
		uaecptr a2 = a - 32;
		uaecptr a3 = m68k_getpc() - 32;
		write_log ("Your Amiga program just did something terribly stupid %p PC=%p\n", a, m68k_getpc());
		m68k_dumpstate (0, 0);
		for (i = 0; i < 10; i++) {
		    write_log ("%08.8X ", i >= 5 ? a3 : a2);
		    for (j = 0; j < 16; j += 2) {
			write_log (" %04.4X", get_word (i >= 5 ? a3 : a2));
			if (i >= 5) a3 +=2; else a2 += 2;
		    }
		    write_log ("\n");
		}
	    }
	    be_cnt++;
	    if (be_cnt > 1000) {
		uae_reset (0);
		be_cnt = 0;
	    } else {
		regs.panic = 1;
		regs.panic_pc = m68k_getpc ();
		regs.panic_addr = a;
		set_special (SPCFLAG_BRK);
	    }
	}
    }
    return kickmem_xlate (0);	/* So we don't crash. */
}

/* Address banks */

addrbank dummy_bank = {
    dummy_lget, dummy_wget, dummy_bget,
    dummy_lput, dummy_wput, dummy_bput,
    default_xlate, dummy_check, NULL
};

#ifdef AUTOCONFIG
addrbank mbres_bank = {
    mbres_lget, mbres_wget, mbres_bget,
    mbres_lput, mbres_wput, mbres_bput,
    default_xlate, mbres_check, NULL
};
#endif

addrbank chipmem_bank = {
    chipmem_lget, chipmem_wget, chipmem_bget,
    chipmem_lput, chipmem_wput, chipmem_bput,
    chipmem_xlate, chipmem_check, NULL
};

#ifdef AGA
addrbank chipmem_bank_ce2 = {
    chipmem_lget_ce2, chipmem_wget_ce2, chipmem_bget_ce2,
    chipmem_lput_ce2, chipmem_wput_ce2, chipmem_bput_ce2,
    chipmem_xlate, chipmem_check, NULL
};
#endif

addrbank bogomem_bank = {
    bogomem_lget, bogomem_wget, bogomem_bget,
    bogomem_lput, bogomem_wput, bogomem_bput,
    bogomem_xlate, bogomem_check, NULL
};

#ifdef AUTOCONFIG
addrbank a3000mem_bank = {
    a3000mem_lget, a3000mem_wget, a3000mem_bget,
    a3000mem_lput, a3000mem_wput, a3000mem_bput,
    a3000mem_xlate, a3000mem_check, NULL
};
#endif

addrbank kickmem_bank = {
    kickmem_lget, kickmem_wget, kickmem_bget,
    kickmem_lput, kickmem_wput, kickmem_bput,
    kickmem_xlate, kickmem_check, NULL
};

addrbank kickram_bank = {
    kickmem_lget, kickmem_wget, kickmem_bget,
    kickmem2_lput, kickmem2_wput, kickmem2_bput,
    kickmem_xlate, kickmem_check, NULL
};

addrbank extendedkickmem_bank = {
    extendedkickmem_lget, extendedkickmem_wget, extendedkickmem_bget,
    extendedkickmem_lput, extendedkickmem_wput, extendedkickmem_bput,
    extendedkickmem_xlate, extendedkickmem_check, NULL
};

static int decode_cloanto_rom (uae_u8 *mem, int size, int real_size)
{
    struct zfile *keyf;
    uae_u8 *p;
    long cnt, t;
    int keysize;

    if (strlen (currprefs.keyfile) == 0) {
#ifndef SINGLEFILE
	gui_message ("No filename given for ROM key file and ROM image is an encrypted \"Amiga Forever\" ROM file.\n");
#endif
	return 0;
    } else {
	keyf = zfile_fopen (currprefs.keyfile, "rb");
        if (keyf == 0)  {
            keyf = zfile_fopen( "..\\shared\\rom\\rom.key", "rb" );
            if( keyf == 0 )
            {
#ifndef SINGLEFILE
                gui_message ("Could not find specified ROM key-file.\n");
#endif
	        return 0;
            }
	}
	p = (uae_u8 *)xmalloc (524288);
	keysize = zfile_fread (p, 1, 524288, keyf);
	for (t = cnt = 0; cnt < size; cnt++, t = (t + 1) % keysize)  {
	    mem[cnt] ^= p[t];
	    if (real_size == cnt + 1)
		t = keysize - 1;
	}
	zfile_fclose (keyf);
	free (p);
    }
    return 1;
}

static int kickstart_checksum (uae_u8 *mem, int size)
{
    uae_u32 cksum = 0, prevck = 0;
    int i;
    for (i = 0; i < size; i+=4) {
	uae_u32 data = mem[i]*65536*256 + mem[i+1]*65536 + mem[i+2]*256 + mem[i+3];
	cksum += data;
	if (cksum < prevck)
	    cksum++;
	prevck = cksum;
    }
#ifndef SINGLEFILE
    if (cksum != 0xFFFFFFFFul)
	gui_message("Kickstart checksum incorrect. You probably have a corrupted ROM image.\n");
#endif
    return 0;
}

static int read_kickstart (struct zfile *f, uae_u8 *mem, int size, int dochecksum, int *cloanto_rom)
{
    unsigned char buffer[20];
    int i, cr = 0;

    if (cloanto_rom)
	*cloanto_rom = 0;
    i = zfile_fread (buffer, 1, 11, f);
    if (strncmp ((char *)buffer, "AMIROMTYPE1", 11) != 0) {
	zfile_fseek (f, 0, SEEK_SET);
    } else {
	cr = 1;
    }

    i = zfile_fread (mem, 1, size, f);
    zfile_fclose (f);
    if ((i != 8192 && i != 65536) && i != 131072 && i != 262144 && i != 524288) {
	gui_message ("Error while reading Kickstart.\n");
	return 0;
    }
    if (i == size / 2)
	memcpy (mem + size / 2, mem, size / 2);

    if (cr)
	decode_cloanto_rom (mem, size, i);
    if (i == 8192 || i == 65536) {
        a1000_bootrom = malloc (65536);
        memcpy (a1000_bootrom, kickmemory, 65536);
        a1000_handle_kickstart (1);
    }
    if (dochecksum && i >= 262144)
	kickstart_checksum (mem, size);
    if (cloanto_rom)
	*cloanto_rom = cr;
    return i;
}

static int load_extendedkickstart (void)
{
    struct zfile *f;
    int size;

    if (strlen(currprefs.romextfile) == 0)
	return 0;
    f = zfile_fopen (currprefs.romextfile, "rb");
    if (!f) {
	gui_message("No extended Kickstart ROM found");
	return 0;
    }
    zfile_fseek (f, 0, SEEK_END);
    size = zfile_ftell (f);
    if (size > 300000)
	extendedkickmem_size = 524288;
    else
	extendedkickmem_size = 262144;
    zfile_fseek (f, 0, SEEK_SET);
    switch (extromtype ()) {

    case EXTENDED_ROM_CDTV:
	extendedkickmemory = (uae_u8 *) mapped_malloc (extendedkickmem_size, "rom_f0");
	extendedkickmem_bank.baseaddr = (uae_u8 *) extendedkickmemory;
	break;
    case EXTENDED_ROM_CD32:
	extendedkickmemory = (uae_u8 *) mapped_malloc (extendedkickmem_size, "rom_e0");
	extendedkickmem_bank.baseaddr = (uae_u8 *) extendedkickmemory;
	break;
    }
    read_kickstart (f, extendedkickmemory, extendedkickmem_size,  0, 0);
    extendedkickmem_mask = extendedkickmem_size - 1;
    return 1;
}

static void kickstart_fix_checksum (uae_u8 *mem, int size)
{
    uae_u32 cksum = 0, prevck = 0;
    int i, ch = size == 524288 ? 0x7ffe8 : 0x3e;
    
    mem[ch] = 0;
    mem[ch + 1] = 0;
    mem[ch + 2] = 0;
    mem[ch + 3] = 0;
    for (i = 0; i < size; i+=4) {
	uae_u32 data = (mem[i] << 24) | (mem[i + 1] << 16) | (mem[i + 2] << 8) | mem[i + 3];
	cksum += data;
	if (cksum < prevck)
	    cksum++;
	prevck = cksum;
    }
    cksum ^= 0xffffffff;
    mem[ch++] = cksum >> 24;
    mem[ch++] = cksum >> 16;
    mem[ch++] = cksum >> 8;
    mem[ch++] = cksum >> 0;
}

static int load_kickstart (void)
{
    struct zfile *f = zfile_fopen (currprefs.romfile, "rb");
    char tmprom[512], tmpkey[512];

    strcpy (tmprom, currprefs.romfile);
    strcpy (tmpkey, currprefs.keyfile);
    if (f == NULL) {
	strcpy (currprefs.romfile, "roms/kick.rom");
	f = zfile_fopen (currprefs.romfile, "rb");
	if (f == NULL) {
	    strcpy( currprefs.romfile, "kick.rom" );
	    f = zfile_fopen( currprefs.romfile, "rb" );
	    if (f == NULL) {
		strcpy( currprefs.romfile, "..\\shared\\rom\\kick.rom" );
		f = zfile_fopen( currprefs.romfile, "rb" );
	    }
        }
    }
    if( f == NULL ) { /* still no luck */
#if defined(AMIGA)||defined(__POS__)
#define USE_UAE_ERSATZ "USE_UAE_ERSATZ"
	if( !getenv(USE_UAE_ERSATZ)) 
        {
	    write_log ("Using current ROM. (create ENV:%s to "
		"use uae's ROM replacement)\n",USE_UAE_ERSATZ);
	    memcpy(kickmemory,(char*)0x1000000-kickmem_size,kickmem_size);
	    kickstart_checksum (kickmemory, kickmem_size);
	    goto chk_sum;
	}
#else
	goto err;
#endif
    }

    if (f != NULL) {
	int size = read_kickstart (f, kickmemory, 0x80000, 1, &cloanto_rom);
	if (size == 0)
	    goto err;
	kickmem_mask = size - 1;
    }

#if defined(AMIGA)
    chk_sum:
#endif

    if (currprefs.kickshifter && kickmem_size >= 524288) {
	/* Patch Kickstart ROM for ShapeShifter - from Christian Bauer.
	 * Changes 'lea $400,a0' and 'lea $1000,a0' to 'lea $3000,a0' for
	 * ShapeShifter compatability.
	*/
	int i;
	uae_u8 kickshift1[] = { 0x41, 0xf8, 0x04, 0x00 };
	uae_u8 kickshift2[] = { 0x41, 0xf8, 0x10, 0x00 };
	uae_u8 kickshift3[] = { 0x43, 0xf8, 0x04, 0x00 };
	
	for (i = 0x200; i < 0x300; i++) {
	    if (!memcmp (kickmemory + i, kickshift1, sizeof (kickshift1)) ||
	    !memcmp (kickmemory + i, kickshift2, sizeof (kickshift2)) ||
	    !memcmp (kickmemory + i, kickshift3, sizeof (kickshift3))) {
		kickmemory[i + 2] = 0x30;
	        write_log ("Kickstart KickShifted @%04.4X\n", i);
	    }
	}
	kickstart_fix_checksum (kickmemory, kickmem_size);
    }
    kickstart_version = (kickmemory[12] << 8) | kickmemory[13];
    return 1;
err:
    strcpy (currprefs.romfile, tmprom);
    strcpy (currprefs.keyfile, tmpkey);
    return 0;
}

#ifndef NATMEM_OFFSET

uae_u8 *mapped_malloc (size_t s, char *file)
{
    return malloc (s);
}

void mapped_free (uae_u8 *p)
{
    free (p);
}

#else

#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/mman.h>

shmpiece *shm_start;

static void dumplist(void)
{
    shmpiece *x = shm_start;
    write_log ("Start Dump:\n");
    while (x) {
	write_log ("this=%p,Native %p,id %d,prev=%p,next=%p,size=0x%08x\n",
		x, x->native_address, x->id, x->prev, x->next, x->size);
	x = x->next;
    }
    write_log ("End Dump:\n");
}

static shmpiece *find_shmpiece (uae_u8 *base)
{
    shmpiece *x = shm_start;

    while (x && x->native_address != base)
	x = x->next;
    if (!x) {
	write_log ("NATMEM: Failure to find mapping at %p\n",base);
	dumplist ();
	canbang = 0;
	return 0;
    }
    return x;
}

static void delete_shmmaps (uae_u32 start, uae_u32 size)
{
    if (!canbang)
	return;

    while (size) {
	uae_u8 *base = mem_banks[bankindex (start)]->baseaddr;
	if (base) {
	    shmpiece *x;
	    //base = ((uae_u8*)NATMEM_OFFSET)+start;

	    x = find_shmpiece (base);
	    if (!x)
		return;

	    if (x->size > size) {
		write_log ("NATMEM: Failure to delete mapping at %08x(size %08x, delsize %08x)\n",start,x->size,size);
		dumplist ();
		canbang = 0;
		return;
	    }
	    shmdt (x->native_address);
	    size -= x->size;
	    start += x->size;
	    if (x->next)
		x->next->prev = x->prev;	/* remove this one from the list */
	    if (x->prev)
		x->prev->next = x->next;
	    else
		shm_start = x->next;
	    free (x);
	} else {
	    size -= 0x10000;
	    start += 0x10000;
	}
    }
}
 
static void add_shmmaps (uae_u32 start, addrbank *what)
{
    shmpiece *x = shm_start;
    shmpiece *y;
    uae_u8 *base = what->baseaddr;

    if (!canbang)
	return;
    if (!base)
	return;

    x = find_shmpiece (base);
    if (!x)
	return;
    y = malloc (sizeof (shmpiece));
    *y = *x;
    base = ((uae_u8 *) NATMEM_OFFSET) + start;
    y->native_address = shmat (y->id, base, 0);
    if (y->native_address == (void *) -1) {
	write_log ("NATMEM: Failure to map existing at %08x(%p)\n",start,base);
	dumplist ();
	canbang = 0;
	return;
    }
    y->next = shm_start;
    y->prev = NULL;
    if (y->next)
	y->next->prev = y;
    shm_start = y;
}

uae_u8 *mapped_malloc (size_t s, char *file)
{
    int id;
    void *answer;
    shmpiece *x;

    if (!canbang)
	return xmalloc (s);

    id = shmget (IPC_PRIVATE, s, 0x1ff, file);
    if (id == -1) {
	canbang = 0;
	return mapped_malloc (s, file);
    }
    answer = shmat (id, 0, 0);
    shmctl (id, IPC_RMID, NULL);
    if (answer != (void *) -1) {
	x = malloc (sizeof (shmpiece));
	x->native_address = answer;
	x->id = id;
	x->size = s;
	x->next = shm_start;
	x->prev = NULL;
	if (x->next)
	    x->next->prev = x;
	shm_start = x;

	return answer;
    }
    canbang = 0;
    return mapped_malloc (s, file);
}

#endif

static void init_mem_banks (void)
{
    int i;
    for (i = 0; i < MEMORY_BANKS; i++)
	put_mem_bank (i << 16, &dummy_bank, 0);
#ifdef NATMEM_OFFSET
    delete_shmmaps (0, 0xFFFF0000);
#endif
}

void clearexec (void)
{
    if (chipmemory)
	memset (chipmemory + 4, 0,  4);
}

static void allocate_memory (void)
{
    if (allocated_chipmem != currprefs.chipmem_size) {
	if (chipmemory)
	    mapped_free (chipmemory);
	chipmemory = 0;

	allocated_chipmem = currprefs.chipmem_size;
	chipmem_mask = allocated_chipmem - 1;

	chipmemory = mapped_malloc (allocated_chipmem, "chip");
	if (chipmemory == 0) {
	    write_log ("Fatal error: out of memory for chipmem.\n");
	    allocated_chipmem = 0;
	} else
	clearexec ();
    }

    if (allocated_bogomem != currprefs.bogomem_size) {
	if (bogomemory)
	    mapped_free (bogomemory);
	bogomemory = 0;

	allocated_bogomem = currprefs.bogomem_size;
	bogomem_mask = allocated_bogomem - 1;

	if (allocated_bogomem) {
	    bogomemory = mapped_malloc (allocated_bogomem, "bogo");
	    if (bogomemory == 0) {
		write_log ("Out of memory for bogomem.\n");
		allocated_bogomem = 0;
	    }
	}
	clearexec ();
    }
#ifdef AUTOCONFIG
    if (allocated_a3000mem != currprefs.a3000mem_size) {
	if (a3000memory)
	    mapped_free (a3000memory);
	a3000memory = 0;

	allocated_a3000mem = currprefs.a3000mem_size;
	a3000mem_mask = allocated_a3000mem - 1;

	if (allocated_a3000mem) {
	    a3000memory = mapped_malloc (allocated_a3000mem, "a3000");
	    if (a3000memory == 0) {
		write_log ("Out of memory for a3000mem.\n");
		allocated_a3000mem = 0;
	    }
	}
	clearexec ();
    }
#endif
    if (savestate_state == STATE_RESTORE) {
	restore_ram (chip_filepos, chipmemory);
	if (allocated_bogomem > 0)
    	    restore_ram (bogo_filepos, bogomemory);
    }
    chipmem_bank.baseaddr = chipmemory;
#ifdef AGA
    chipmem_bank_ce2.baseaddr = chipmemory;
#endif
    bogomem_bank.baseaddr = bogomemory;
}

void map_overlay (int chip)
{
    int i = allocated_chipmem > 0x200000 ? (allocated_chipmem >> 16) : 32;
    addrbank *cb;

    cb = &chipmem_bank;
#ifdef AGA
    if (currprefs.cpu_cycle_exact && currprefs.cpu_level >= 2)
	cb = &chipmem_bank_ce2;
#endif
    if (chip)
	map_banks (cb, 0, i, allocated_chipmem);
    else
	map_banks (&kickmem_bank, 0, i, 0x80000);
    if (savestate_state != STATE_RESTORE && savestate_state != STATE_REWIND)
        m68k_setpc(m68k_getpc());
}

void memory_reset (void)
{
    int custom_start;

    be_cnt = 0;
    currprefs.chipmem_size = changed_prefs.chipmem_size;
    currprefs.bogomem_size = changed_prefs.bogomem_size;
    currprefs.a3000mem_size = changed_prefs.a3000mem_size;

    init_mem_banks ();
    allocate_memory ();

    if (strcmp (currprefs.romfile, changed_prefs.romfile) != 0
	|| strcmp (currprefs.keyfile, changed_prefs.keyfile) != 0)
    {
	ersatzkickfile = 0;
	memcpy (currprefs.romfile, changed_prefs.romfile, sizeof currprefs.romfile);
	memcpy (currprefs.keyfile, changed_prefs.keyfile, sizeof currprefs.keyfile);
        if (savestate_state != STATE_RESTORE)
	    clearexec ();
        load_extendedkickstart ();
	if (!load_kickstart ()) {
#ifdef AUTOCONFIG
            init_ersatz_rom (kickmemory);
	    ersatzkickfile = 1;
#endif
	}
    }

    custom_start = 0xC0;

    map_banks (&custom_bank, custom_start, 0xE0 - custom_start, 0);
    map_banks (&cia_bank, 0xA0, 32, 0);
    map_banks (&clock_bank, 0xDC, 1, 0);

    /* @@@ Does anyone have a clue what should be in the 0x200000 - 0xA00000
     * range on an Amiga without expansion memory?  */
    custom_start = allocated_chipmem >> 16;
    if (custom_start < 0x20 + (currprefs.fastmem_size >> 16))
	custom_start = 0x20 + (currprefs.fastmem_size >> 16);
    map_banks (&dummy_bank, custom_start, 0xA0 - custom_start, 0);
    /*map_banks (&mbres_bank, 0xDE, 1); */

    if (bogomemory != 0) {
	int t = allocated_bogomem >> 16;
	if (t > 0x1C)
	    t = 0x1C;
	map_banks (&bogomem_bank, 0xC0, t, allocated_bogomem);
    }
#ifdef AUTOCONFIG
    if (a3000memory != 0)
	map_banks (&a3000mem_bank, a3000mem_start >> 16, allocated_a3000mem >> 16,
		   allocated_a3000mem);

    map_banks (&rtarea_bank, RTAREA_BASE >> 16, 1, 0);
#endif

    map_banks (&kickmem_bank, 0xF8, 8, 0);
    if (currprefs.maprom)
	map_banks (&kickram_bank, currprefs.maprom >> 16, 8, 0);

    if (a1000_bootrom)
        a1000_handle_kickstart (1);
#ifdef AUTOCONFIG
    map_banks (&expamem_bank, 0xE8, 1, 0);
#endif

    /* Map the chipmem into all of the lower 8MB */
    map_overlay (1);

#ifdef CDTV
    cdtv_enabled = 0;
#endif
#ifdef CD32
    cd32_enabled = 0;
#endif

    switch (extromtype ()) {

#ifdef CDTV
    case EXTENDED_ROM_CDTV:
	map_banks (&extendedkickmem_bank, 0xF0, 4, 0);
	cdtv_enabled = 1;
	break;
#endif
#ifdef CD32
    case EXTENDED_ROM_CD32:
	map_banks (&extendedkickmem_bank, 0xE0, 8, 0);
	cd32_enabled = 1;
	break;
#endif
    default:
	if (cloanto_rom && !currprefs.maprom)
	    map_banks (&kickmem_bank, 0xE0, 8, 0);
    }

#ifdef ACTION_REPLAY
    action_replay_memory_reset(); 
    #ifdef ACTION_REPLAY_HRTMON
    hrtmon_map_banks();
    #endif

    #ifndef ACTION_REPLAY_HIDE_CARTRIDGES
    #ifdef ACTION_REPLAY
    action_replay_map_banks(); 
    #endif
    #endif
#endif
}

void memory_init (void)
{
    allocated_chipmem = 0;
    allocated_bogomem = 0;
    kickmemory = 0;
    extendedkickmemory = 0;
    extendedkickmem_size = 0;
    chipmemory = 0;
#ifdef AUTOCONFIG
    allocated_a3000mem = 0;
    a3000memory = 0;
#endif
    bogomemory = 0;

    kickmemory = mapped_malloc (kickmem_size, "kick");
    memset (kickmemory, 0, kickmem_size);
    kickmem_bank.baseaddr = kickmemory;
    currprefs.romfile[0] = 0;
    currprefs.keyfile[0] = 0;
#ifdef AUTOCONFIG
    init_ersatz_rom (kickmemory);
    ersatzkickfile = 1;
#endif

#ifdef ACTION_REPLAY   
    action_replay_load();
    action_replay_init(1);
    
    #ifdef ACTION_REPLAY_HRTM
    hrtmon_load(1);
    #endif
#endif

    init_mem_banks ();
}

void memory_cleanup (void)
{
#ifdef AUTOCONFIG
    if (a3000memory)
	mapped_free (a3000memory);
    a3000memory = 0;
#endif
    if (bogomemory)
	mapped_free (bogomemory);
    if (kickmemory)
	mapped_free (kickmemory);
    if (a1000_bootrom)
	free (a1000_bootrom);
    if (chipmemory)
	mapped_free (chipmemory);
  
    bogomemory = 0;
    kickmemory = 0;
    a1000_bootrom = 0;
    chipmemory = 0;

    #ifdef ACTION_REPLAY
    action_replay_cleanup();
    #endif
}

void map_banks (addrbank *bank, int start, int size, int realsize)
{
    int bnr;
    unsigned long int hioffs = 0, endhioffs = 0x100;
    addrbank *orgbank = bank;
    uae_u32 realstart = start;

    flush_icache (1);		/* Sure don't want to keep any old mappings around! */
#ifdef NATMEM_OFFSET
    delete_shmmaps (start << 16, size << 16);
#endif

    if (!realsize)
	realsize = size << 16;

    if ((size << 16) < realsize) {
	//write_log ("Please report to bmeyer@cs.monash.edu.au, and mention:\n");
	write_log ("Broken mapping, size=%x, realsize=%x\n", size, realsize);
	write_log ("Start is %x\n", start);
	write_log ("Reducing memory sizes, especially chipmem, may fix this problem\n");
	abort ();
    }

#ifndef ADDRESS_SPACE_24BIT
    if (start >= 0x100) {
	int real_left = 0;
	for (bnr = start; bnr < start + size; bnr++) {
	    if (!real_left) {
		realstart = bnr;
		real_left = realsize >> 16;
#ifdef NATMEM_OFFSET
		add_shmmaps (realstart << 16, bank);
#endif
	    }
	    put_mem_bank (bnr << 16, bank, realstart << 16);
	    real_left--;
	}
	return;
    }
#endif
    if (currprefs.address_space_24)
	endhioffs = 0x10000;
#ifdef ADDRESS_SPACE_24BIT
    endhioffs = 0x100;
#endif
    for (hioffs = 0; hioffs < endhioffs; hioffs += 0x100) {
	int real_left = 0;
	for (bnr = start; bnr < start + size; bnr++) {
	    if (!real_left) {
		realstart = bnr + hioffs;
		real_left = realsize >> 16;
#ifdef NATMEM_OFFSET
		add_shmmaps (realstart << 16, bank);
#endif
	    }
	    put_mem_bank ((bnr + hioffs) << 16, bank, realstart << 16);
	    real_left--;
	}
    }
}


/* memory save/restore code */

uae_u8 *save_cram (int *len)
{
    *len = allocated_chipmem;
    return chipmemory;
}

uae_u8 *save_bram (int *len)
{
    *len = allocated_bogomem;
    return bogomemory;
}

void restore_cram (int len, long filepos)
{
    chip_filepos = filepos;
    changed_prefs.chipmem_size = len;
}

void restore_bram (int len, long filepos)
{
    bogo_filepos = filepos;
    changed_prefs.bogomem_size = len;
}

uae_u8 *restore_rom (uae_u8 *src)
{
    restore_u32 ();
    restore_u32 ();
    restore_u32 ();
    restore_u32 ();
    restore_u32 ();
    src += strlen (src) + 1;
    if (src[0]) {
	if (zfile_exists (src))
	    strncpy (changed_prefs.romfile , src, 255);
        src+=strlen(src)+1;
    }
    return src;
}

uae_u8 *save_rom (int first, int *len, uae_u8 *dstptr)
{
    static int count;
    uae_u8 *dst, *dstbak;
    uae_u8 *mem_real_start;
    int mem_start, mem_size, mem_type, i, saverom;

    saverom = 0;
    if (first)
	count = 0;
    for (;;) {
	mem_type = count;
	switch (count) {
	case 0:		/* Kickstart ROM */
	    mem_start = 0xf80000;
	    mem_real_start = kickmemory;
	    mem_size = kickmem_size;
	    /* 256KB or 512KB ROM? */
	    for (i = 0; i < mem_size / 2 - 4; i++) {
		if (longget (i + mem_start) != longget (i + mem_start + mem_size / 2))
		    break;
	    }
	    if (i == mem_size / 2 - 4) {
		mem_size /= 2;
		mem_start += 262144;
	    }
	    mem_type = 0;
	    break;
	default:
	    return 0;
	}
	count++;
	if (mem_size)
	    break;
    }
    if (dstptr)
	dstbak = dst = dstptr;
    else
        dstbak = dst = malloc (4 + 4 + 4 + 4 + 4 + mem_size);
    save_u32 (mem_start);
    save_u32 (mem_size);
    save_u32 (mem_type);
    save_u32 (longget (mem_start + 12));	/* version+revision */
    save_u32 (CRC32 (0, kickmemory, mem_size));
    sprintf (dst, "Kickstart %d.%d", wordget (mem_start + 12), wordget (mem_start + 14));
    dst += strlen (dst) + 1;
    strcpy (dst, currprefs.romfile);/* rom image name */
    dst += strlen(dst) + 1;
    if (saverom) {
	for (i = 0; i < mem_size; i++)
	    *dst++ = byteget (mem_start + i);
    }
    *len = dst - dstbak;
    return dstbak;
}
