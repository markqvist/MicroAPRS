/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Notifier obj (interface).
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Francesco Sacchi <batt@develer.com>
 */

#ifndef DT_DNOTIFIER_H
#define DT_DNOTIFIER_H

#include <cfg/debug.h>

#include <dt/dtag.h>
#include <struct/list.h>

//Fwd declaretion.
struct DNotifier;
struct DFilter;

typedef void (* update_func_ptr)(struct DNotifier *, dtag_t, dval_t);
typedef void (* update_filter_ptr)(struct DFilter *, dtag_t, dval_t);

/**
 * Base object for receive and forward messages.
 * It contains an update function used to update itslef and a list to
 * notify other DNotifer eventually connected.
 */
typedef struct DNotifier
{
	/// Receive new attributes from other notifiers.
	update_func_ptr update;

	/// List of target notifiers to set new attributes to.
	List targets;
} DNotifier;

/**
 * Map for messages.
 * Used to translate src message to dst message.
 */
typedef struct DFilterMap
{
	DTagItem src;
	DTagItem dst;
} DFilterMap;


/**
 * A filter is an interface between two notifier.
 * It can translate messages between them through a map (if it is not null).
 */
typedef struct DFilter
{
	/// Allow creating a list of dfilter objects.
	Node link;

	/// Target of the filter
	DNotifier *target;

	/// Update function called by the source dnotifier
	update_filter_ptr update;

	///Map for translating messages for target
	const DFilterMap *map;

	///Used in debug to prevent inserting this filter in more than one list
	DB(uint8_t magic;)
} DFilter;

/// Type for filter-mask checking
typedef unsigned int dfilter_mask_t;

/// Filter init
void filter_init(DFilter *f, const DFilterMap *map, bool masked, DNotifier *source, DNotifier *target);

/// Filter update function without masking capabilities.
void filter_update(DFilter *f, dtag_t tag, dval_t val);

/// Filter update function with masking capabilities.
void filter_mask_update(DFilter *f, dtag_t tag, dval_t val);

/// Notifier init
void notifier_init(DNotifier *n);


/**
 * Macro to notify the target object.
 */
INLINE void dnotify(DNotifier *target, dtag_t tag, dval_t val)
{
	if (target)
		target->update(target, tag, val);
}

/**
 * Macro to notify all the targets of \a target object.
 */
INLINE void dnotify_targets(DNotifier *target, dtag_t tag, dval_t val)
{
	DFilter *f;
	if (!LIST_EMPTY(&target->targets))
		FOREACH_NODE(f, &target->targets)
			f->update(f, tag, val);
}


/**
 * Macro that connect \a src notifier to \a tgt using \a map and passing \a opt for filtering option.
 * It declares a static filter to achieve connection and messages translation.
 * \note Due its static filter declaration, DCONNECT MUST NOT be used inside loops or in functions called multiple times.
 * Failing to do so will lead to unpredictable connections between notifiers.
 */
#define DCONNECT(src, tgt, map, opt) \
	do { \
		static DFilter _filter_; /* Declare a filter */ \
		filter_init(&(_filter_), map, opt, src, tgt); /* Init it. */ \
	} while (0)


#endif /* DT_DNOTIFIER_H */
