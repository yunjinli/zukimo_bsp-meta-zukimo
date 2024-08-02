/* SPDX-License-Identifier: GPL-2.0+
 *
 * Copyright (C) 2023 Dream Chip Technologies
 */

#ifndef __ZUKIMO_HAPS_H
#define __ZUKIMO_HAPS_H

#include <configs/zukimo.h>

#undef PHYS_SRAM
#undef PHYS_SRAM_SIZE
#undef PHYS_SDRAM_1
#undef PHYS_SDRAM_1_SIZE

/* Physical Memory Map */
#define PHYS_SRAM				0x10000000ULL	/* lower part of internal SRAM */
#define PHYS_SRAM_SIZE			0x00080000ULL

#define PHYS_SDRAM_1			0x80000000ULL	/* SDRAM Bank #1, 2GB */
#define PHYS_SDRAM_1_SIZE		0x80000000ULL

#endif /* __ZUKIMO_HAPS_H */
