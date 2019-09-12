//=======================================================================
//
//  HAParaNDA: High-dimensional Adaptive Parallel Numerical Dynamics
//  Copyright (2011) Magnus Gustafsson, Uppsala University
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//  Contact: Magnus Gustafsson <magnus.gustafsson@it.uu.se>
//
//=======================================================================

#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <ctime>
#include <sys/time.h>
#include <iomanip>

namespace Haparanda
{
  namespace Utils
  {
	class Timer
	{
	public:
		// Default constructor with no name, not started upon creation.
		Timer();

		// Returns current wall-clock time in seconds.*/
		double getWallTime() const;

		// Start the timer
		void start(bool reset = false);

		// Stop the timer
		double stop();

		// Return the total time (in seconds) accumulated by this timer
		double totalElapsedTime(bool readCurrentTime = false) const;

		// Reset the cumulative time
		void reset() { totalTime_ = 0; }

	private:
		double startTime_;
		double totalTime_;
		bool isRunning_;
	};

	//==============================================================================
	Timer::Timer()
	: startTime_(0), totalTime_(0), isRunning_(false)
	{
	}

	//==============================================================================
	double Timer::getWallTime() const
	{
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return tv.tv_sec + 1.0e-6*tv.tv_usec;
	}

	// Inline definitions of start and stop functions
	inline void Timer::start(bool reset_in)
	//==============================================================================
	{
		isRunning_ = true;
		if (reset_in) totalTime_ = 0;
		startTime_ = getWallTime();
	}

	//==============================================================================
	inline double Timer::stop()
	{
		if (isRunning_) {
			totalTime_ += ( getWallTime() - startTime_ );
			isRunning_ = false;
			startTime_ = 0;
		}
		return totalTime_;
	}

	//==============================================================================
	double Timer::totalElapsedTime(bool readCurrentTime) const
	{
		if(readCurrentTime)
			return getWallTime() - startTime_ + totalTime_;
		return totalTime_;
	}


  } // end namespace Utils
} // end namespace Haparanda


#endif /* TIMER_HPP_ */
