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
 *
 * -->
 *
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * \brief Low-level timer module for Qt emulator (implementation).
 */
#include <cfg/compiler.h> /* hptime.t */

// Qt headers
#include <QtCore/QDateTime>
#include <QtCore/QTimer>

#if CONFIG_KERN_IRQ
#include <kern/irq.h>
#endif


// The user interrupt server routine
void timer_isr(void);


/**
 * Singleton class for Qt-based hardware timer emulation.
 */
class EmulTimer : public QObject
{
private:
	Q_OBJECT;

	/// System timer (counts ms since application startup)
	QTime system_time;

	/// The 1ms "hardware" tick counter.
	QTimer timer;

	/**
	 * We deliberately don't use RAII because the real hardware
	 * we're simulating needs to be initialized manually.
	 */
	bool initialized;

	/// Private ctor (singleton)
	EmulTimer() : initialized(false) { }

public:
	/// Return singleton instance
	static EmulTimer &instance()
	{
		static EmulTimer et;
		return et;
	}

	/// Start timer emulator.
	void init()
	{
		// Timer initialized twice?
		ASSERT(!initialized);

		// Record initial time
		system_time.start();

		#if CONFIG_KERN_IRQ
			irq_register(SIGALRM, timer_isr);
		#endif

		// Activate timer interrupt
		connect(&timer, SIGNAL(timeout()), SLOT(timerInterrupt()));
		timer.start(1000 / TIMER_TICKS_PER_SEC);

		initialized = true;
	}

	void cleanup()
	{
		// Timer cleaned twice?
		ASSERT(initialized);

		timer.stop();
		timer.disconnect();

		initialized = false;
	}

	/// Return current time in high-precision format.
	hptime_t hpread()
	{
		ASSERT(initialized);
		return system_time.elapsed();
	}

public slots:
	void timerInterrupt(void)
	{
		// Just call user interrupt server, timer restarts automatically.
		#if CONFIG_KERN_IRQ
			irq_entry(SIGALRM);
		#else
			timer_isr();
		#endif
	}

};

#include "timer_qt_moc.cpp"


/// HW dependent timer initialization.
static void timer_hw_init(void)
{
	EmulTimer::instance().init();
}

static void timer_hw_cleanup(void)
{
	EmulTimer::instance().cleanup();
}

INLINE hptime_t timer_hw_hpread(void)
{
	return EmulTimer::instance().hpread();
}

/** Not needed, timer IRQ handler called only for timer source */
#define timer_hw_triggered() (true)
