package haparanda.utils;

import java.util.concurrent.Callable;

/**
 * Callable task, intended to be used for thread parallelization
 * 
 * @author Malin Kallen 2018 
 */
public abstract class Task implements Callable<Void> {
	protected int id;
	protected int numTasks;	// Total number of tasks that shares this work

	/**
	 * @param id ID of the task
	 * @param numTasks Number of tasks sharing the work
	 */
	public Task(int id, int numTasks) {
		this.id = id;
		this.numTasks = numTasks;
	}
	
	/**
	 * @return ID of the current task
	 */
	public int getId() {
		return id;
	}
	
	/**
	 * @return Number of tasks sharing the work
	 */
	public int getNumTasks() {
		return numTasks;
	}
}
