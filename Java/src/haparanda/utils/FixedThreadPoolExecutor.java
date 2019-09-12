package haparanda.utils;

import java.util.ArrayList;
import java.util.Collection;
import java.util.concurrent.Callable;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadPoolExecutor;

/**
 * A fixed-sized Java ThreadPoolExecutor wrapped in a singleton class. The
 * number of threads in the thread pool is the value system variable THR. If
 * THR is not set, only 1 thread will be used.
 * 
 * @author Malin Kallen 2018
 */
public class FixedThreadPoolExecutor {
	private static ThreadPoolExecutor executor;
	
	/**
	 * Get the single instance of the ThreadPoolExecutor.
	 */
	public static ThreadPoolExecutor getThreadPoolExecutor() {
		if (null==executor) {
			final int numThreads = Integer.getInteger("THR", 1);
			executor = (ThreadPoolExecutor) Executors.newFixedThreadPool(numThreads);
		}
		return executor;
	}
	
	/**
	 * Shut down and delete the ThreadPoolExecutor, but first await termination
	 * of all currently running threads.
	 * (If the executor is needed again, a new instance will be created.)  
	 */
	public static void destroy() {
		if (null!=executor) {
			executor.shutdown();
		}
		executor = null;
	}
	
	public interface TaskCreator {
		/**
		 * Create an instance of Task that should be executed in parallel.
		 */
		Task create(int taskId, int numTasks);
	}
	
	/**
	 * Execute a number of tasks in the thread pool executor. The number of
	 * tasks is the same as the number of threads in the thread pool. The method
	 * will not return until all tasks are finished!
	 * 
	 * @param creator Specifies how the tasks that will be run are created
	 */
	public static void execute(TaskCreator creator) {
		ThreadPoolExecutor executor = getThreadPoolExecutor();
		int numThreads = executor.getMaximumPoolSize();	// Should be the same as poolSize, but some thread may be restarted in this moment
		Collection<Callable<Void>> tasks = new ArrayList<Callable<Void>>(numThreads);
		for (int t=0; t<numThreads; t++) {
			tasks.add(creator.create(t, numThreads));
		}
		try {
			executor.invokeAll(tasks);
		} catch (InterruptedException e) {
			System.out.println("Thread pool executor interrupted!");
			e.printStackTrace();
			System.exit(1);
		}
	}
}

