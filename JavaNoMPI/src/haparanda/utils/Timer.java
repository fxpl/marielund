package haparanda.utils;

public class Timer {
	
	private double startTime_;
	private double totalTime_;
	private boolean isRunning_;
	
	// Default constructor with no name, not started upon creation.
	public Timer() {
		startTime_ = 0;
		totalTime_ = 0;
		isRunning_ = false;
	}

	// Returns current wall-clock time in seconds.*/
	public final double getWallTime() {
		return 1.0e-3 * System.currentTimeMillis();
	}

	// Start the timer
	public final void start(boolean reset) {	// false default in C++ implementation
		isRunning_ = true;
		if (reset) totalTime_ = 0;
		startTime_ = getWallTime();
	}

	// Stop the timer
	public final double stop() {
		if (isRunning_) {
			totalTime_ += ( getWallTime() - startTime_ );
			isRunning_ = false;
			startTime_ = 0;
		}
		return totalTime_;
	}

	// Return the total time (in seconds) accumulated by this timer
	public final double totalElapsedTime(boolean readCurrentTime) {	// false default in C++ implementation
		if(readCurrentTime)
			return getWallTime() - startTime_ + totalTime_;
		return totalTime_;
	}

	// Reset the cumulative time
	public final void reset() { totalTime_ = 0; }
}