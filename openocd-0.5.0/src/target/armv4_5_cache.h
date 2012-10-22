/***************************************************************************
 *   Copyright (C) 2005 by Dominic Rath                                    *
 *   Dominic.Rath@gmx.de                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef ARMV4_5_CACHE_H
#define ARMV4_5_CACHE_H

#include <helper/types.h>

struct command_context;

struct armv4_5_cachesize
{
	int linelen;
	int associativity;
	int nsets;
	int cachesize;
};

struct armv4_5_cache_common
{
	int ctype;	/* specify supported cache operations */
	int separate;	/* separate caches or unified cache */
	struct armv4_5_cachesize d_u_size;	/* data cache */
	struct armv4_5_cachesize i_size; /* instruction cache */
	int i_cache_enabled;
	int d_u_cache_enabled;
};

int armv4_5_identify_cache(uint32_t cache_type_reg,
		struct armv4_5_cache_common *cache);
int armv4_5_cache_state(uint32_t cp15_control_reg,
		struct armv4_5_cache_common *cache);

int armv4_5_handle_cache_info_command(struct command_context *cmd_ctx,
		struct armv4_5_cache_common *armv4_5_cache);

enum
{
	ARMV4_5_D_U_CACHE_ENABLED = 0x4,
	ARMV4_5_I_CACHE_ENABLED = 0x1000,
	ARMV4_5_WRITE_BUFFER_ENABLED = 0x8,
	ARMV4_5_CACHE_RR_BIT = 0x5000,
};

#endif /* ARMV4_5_CACHE_H */
