/*
 * (C) Copyright 2012
 * Emcraft Systems, <www.emcraft.com>
 * Alexander Potashev <aspotashev@emcraft.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* (C) Copyright 2021
 * S.E.R.I.A.N.E., <www.seriane.fr>
 * Patrick Foubet <pfoubet@seriane.fr>
 *
 * MAJ du 23/06/2021 - Licence GPL-2 (cf ci-dessus)
 */

#include <linux/module.h>

#include <mach/exti.h>

struct stm32_exti_regs {
	u32 imr;	/* Interrupt mask register */
	u32 emr;	/* Event mask register */
	u32 rtsr;	/* Rising trigger selection register */
	u32 ftsr;	/* Falling trigger selection register */
	u32 swier;	/* Software interrupt event register */
	u32 pr;		/* Pending register */
};

/*
 * EXTI register map base
 */
#define STM32F4_EXTI_BASE	0x40013c00
#define STM32F4_EXTI		((volatile struct stm32_exti_regs *) \
				STM32F4_EXTI_BASE)

/*
 * Enable or disable interrupt on the rising edge of a event line
 */
int stm32_exti_enable_int(unsigned int line, int enable)
{
	if (line >= STM32F4_EXTI_NUM_LINES) return -EINVAL;
	if (enable) {
		stm32_exti_clear_pending(line);

		/* Enable trigger on rising edge */
		STM32F4_EXTI->rtsr |= (1 << line);
		/* Disable trigger on falling edge */
		STM32F4_EXTI->ftsr &= ~(1 << line);
		/* Enable interrupt for the event */
		STM32F4_EXTI->imr |= (1 << line);
	} else {
		/* Disable interrupt for the event */
		STM32F4_EXTI->imr &= ~(1 << line);
		/* Disable trigger on rising edge */
		STM32F4_EXTI->rtsr &= ~(1 << line);
		/* Disable trigger on falling edge */
		STM32F4_EXTI->ftsr &= ~(1 << line);

		stm32_exti_clear_pending(line);
	}
	return 0;
}
EXPORT_SYMBOL(stm32_exti_enable_int);

/*
 * Clear the pending state of a given event
 */
int stm32_exti_clear_pending(unsigned int line)
{
	if (line >= STM32F4_EXTI_NUM_LINES) return -EINVAL;

	STM32F4_EXTI->pr = (1 << line);
	return 0;
}
EXPORT_SYMBOL(stm32_exti_clear_pending);
