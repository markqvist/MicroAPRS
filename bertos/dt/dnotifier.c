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
 *
 * \brief Notifier obj (implementation).
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Francesco Sacchi <batt@develer.com>
 */

#include <cfg/debug.h>

#include <dt/dtag.h>
#include <dt/dnotifier.h>
#include <struct/list.h>

/**
 * Default update used to notify target: notify all trasparently all
 * targets in the list.
 */
static void notifier_update(DNotifier *n, dtag_t tag, dval_t val)
{
	dnotify_targets(n, tag, val);
}

/**
 * Init.
 */
void notifier_init(DNotifier *n)
{
	// Init instance
	n->update = notifier_update;
	LIST_INIT(&n->targets);
}

/**
 * Search in the map a tag and a val corresponding to the ones supplied.
 * If a match is found change them to the corresponding ones in the map.
 * If map is NULL the filter is trasparent and all messages sent to filter
 * will be forwarded to its target.
 */
void filter_update(DFilter *f, dtag_t tag, dval_t val)
{

	const DFilterMap *map = f->map;

	if (map)
	{
		while (map->src.tag != TAG_END)
		{
			if ((map->src.tag == tag) && (map->src.val == val))
			{
				tag = map->dst.tag;
				val = map->dst.val;
				break;
			}
			/* TAG_ANY matches anything */
			if (map->src.tag == TAG_ANY)
				break;
			map++;
		}

		if (map->src.tag != TAG_END)
			dnotify(f->target, tag, val);
	}
	else
		dnotify(f->target, tag, val);
}


/**
 * Search in the table a tag corresponding to the one supplied and a val
 * that has at least the mask map supplied bits to one.
 * If a match is found change them to the corresponding ones in the map.
 * If map is NULL the filter is trasparent and all messages sent to filter
 * will be forwarded to its target.
 */
void filter_mask_update(DFilter *f, dtag_t tag, dval_t val)
{

	const DFilterMap *map = f->map;
	dfilter_mask_t mask;

	if (map)
	{
		while (map->src.tag != TAG_END)
		{
			mask = (dfilter_mask_t) map->src.val;
			if ((map->src.tag == tag) && ((mask & (dfilter_mask_t)val) == mask))
			{
				tag = map->dst.tag;
				val = map->dst.val;
				break;
			}
			/* TAG_ANY matches anything */
			if (map->src.tag == TAG_ANY)
				break;
			map++;
		}


		if (map->src.tag != TAG_END)
			dnotify(f->target, tag, val);
	}
	else
		dnotify(f->target, tag, val);
}


#define FILTER_MAGIC_ACTIVE 0xAA
/**
 * Init filter.
 * If \a masked is true, all the fields value in \a map must be interpreted as a mask of bits.
 */
void filter_init(DFilter *f, const DFilterMap *map, bool masked, DNotifier *source, DNotifier *target)
{
	// Init instance
	if (masked)
		f->update = (update_filter_ptr)filter_mask_update;
	else
		f->update = (update_filter_ptr)filter_update;

	/* set filter map and target */
	f->map = map;
	f->target = target;

	/* these ensure that the filter is not inserted in more than one list */
	ASSERT(f->magic != FILTER_MAGIC_ACTIVE);
	DB(f->magic = FILTER_MAGIC_ACTIVE;)

	/* Add the filter to source filter list */
	ADDTAIL(&source->targets, &f->link);
}
