/* Copyright 2007-2009 Theodore S. Norvell. All rights reserved.
   Updated 2009 Mar 13

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
   3. Neither the source nor binary form shall be included in any product used by
      or intended for use by any military organization. 

THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THEODORE
NORVELL BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

package monitor;

import java.util.ArrayList ;

/** A class for Monitors.
 * Monitors provide coordination of concurrent threads. Each Monitor protects
 *  some resource, usually data.  At each point in time a monitor
 *  object is occupied by at most one thread.
 * <P> Use this class as a base class if you can.
 * <P> Use class <code>{@link Monitor}</code> if you are already extending another class.
 *  <p> See <br>C.A.R. Hoare, 'Monitors: an operating system structuring
 *      concept', <EM> Communications of the ACM 17</EM>, Oct. 1974,
 *      pp. 549-557. Reprinted in C.A.R.Hoare and C.B. Jones,
 *      <EM> Essay's in Computing Science</EM>, Prentice-Hall, 1989.
 *      <br>and<br> P. Brinch Hansen, The programming language
 *      Concurrent Pascal, <EM>IEEE Transactions on Software
 *      Engineering 1</EM>, June 1975, 199-207.
 * <p> The details of pre- and post-conditions for signal and await
 *      and the idea to use a count function come from Ole-Johan Dahl,
 *      'Monitors revisited', in <i>A Classical Mind</i>, A. W. Roscoe (ed.), 1994.
 * @author Theodore S. Norvell
 * @version 2.0
 * @see Condition
 * @see Monitor
*/
public abstract class AbstractMonitor
{
    final Semaphore entrance = new Semaphore( 1 ) ;
    volatile Thread occupant = null ;
	final private ArrayList<MonitorListener> listOfListeners = new ArrayList<MonitorListener>();
	final private String name  ;
	
	public String getName() {
		return name ; }
	
    protected AbstractMonitor(){ this(null); } 
	
    protected AbstractMonitor(String name){ this.name = name ; } 
    
    /** The invariant.
     * The default implementation always returns true.
     * This method should be overridden if at all possible
     * with the strongest economically evaluable invariant. */
    protected boolean invariant( ) {return true ; }

    /** Enter the monitor. Any thread calling this method is
        delayed until the monitor is unoccupied. Upon returning
        from this method, the monitor is considered occupied.
        <p> A thread must not attempt to enter a Monitor it is
        already in.
    */
    protected void enter() {
    	notifyCallEnter();
        entrance.acquire() ;
        // The following assertion should never trip!
        Assertion.check(occupant==null, "2 threads in one monitor") ;
        occupant = Thread.currentThread() ;
    	notifyReturnFromEnter();
        Assertion.check(invariant(), "Invariant of monitor "+getName() ) ; }

    /** Leave the monitor. After returning from this method, the
    * thread no longer occupies the monitor.
    * <p> Only a thread that is in the monitor may leave it.
    * @throws AssertionError if the thread that leaves is not the occupant.
    */
    protected void leave() {
    	notifyLeaveMonitor() ;
    	leaveWithoutATrace() ; }
    
    /** Leave the monitor. After returning from this method, the
     * thread no longer occupies the monitor.
     * <p> Only a thread that is in the monitor may leave it.
     * @throws AssertionError if the thread that leaves is not the occupant.
     */
     protected<T> T leave( T result ) {
     	leave() ;
     	return result ; }
     
     /*package*/ void leaveWithoutATrace() {
        Assertion.check(invariant(), "Invariant of monitor "+getName()) ;
        Assertion.check( occupant == Thread.currentThread(),
        		         "Thread is not occupant") ;
        occupant = null ;
        entrance.release(); }

     /** Run the runnable inside the monitor.
      * <p>Any thread calling this method will be delayed
      * until the monitor is empty. The "run"
      * method of its argument is then executed
      * within the protection of the monitor.
      * <p>When the run method returns, if the thread still occupies
      * the monitor, it leaves the monitor. 
      * @param runnable A Runnable object.
      */
    protected void doWithin( Runnable runnable ) {
    	enter() ;
    	try {
    		runnable.run() ; }
    	finally {
    		if( occupant == Thread.currentThread() ) leave() ; }
    }

    /** Run the runnable inside the monitor.
     * <p>Any thread calling this method will be delayed
     * until the monitor is empty. The "run"
     * method of its argument is then executed
     * within the protection of the monitor.
     * <p>When the run method returns, if the thread still occupies
     * the monitor, it leaves the monitor. 
     * <p>Thus the signalAndLeave method may be called within the
     * run method.
     * @param runnable A RunnableWithResult object.
     * @return The value computed by the run method of the runnable.
     */
   protected<T> T doWithin( RunnableWithResult<T> runnable ) {
    	enter() ;
    	try {
    		return runnable.run() ; }
    	finally {
    		if( occupant == Thread.currentThread() ) leave() ; }
    }
    
    /** Create a condition queue associated with a checked Assertion.
        The Assertion will be checked prior to an signal of the
        condition.
    */
    protected Condition makeCondition( Assertion prop ) {
        return makeCondition( null, prop ) ; }

    /** Create a condition queue with no associated checked Assertion.
    */
    protected Condition makeCondition() {
        return makeCondition( null, True_Assertion.singleton ) ; }
    
    /** Create a condition queue associated with a checked Assertion.
        The Assertion will be checked prior to an signal of the
        condition.
    */
    protected Condition makeCondition( String name, Assertion prop ) {
        return new Condition( name, this, prop ) ; }

    /** Create a condition queue with no associated checked Assertion.
    */
    protected Condition makeCondition(String name ) {
        return makeCondition( name, True_Assertion.singleton ) ; }
    
    /** Register a listener  */
    public void addListener( MonitorListener newListener ) {
    	listOfListeners .add( newListener  ) ;
    }

	private void notifyCallEnter() {
		for( MonitorListener listener : listOfListeners ) {
			listener.callEnterMonitor( this ) ; } }
    
	private void notifyReturnFromEnter() {
		for( MonitorListener listener : listOfListeners ) {
			listener.returnFromEnterMonitor( this ) ; } }
    
	private void notifyLeaveMonitor() {
		for( MonitorListener listener : listOfListeners ) {
			listener.leaveMonitor( this ) ; } }
    
	/*package*/ void notifyCallAwait(Condition condition) {
		for( MonitorListener listener : listOfListeners ) {
			listener.callAwait( condition, this ) ; } }
    
	/*package*/ void notifyReturnFromAwait(Condition condition) {
		for( MonitorListener listener : listOfListeners ) {
			listener.returnFromAwait( condition, this ) ; } }
    
	/*package*/ void notifySignallerAwakesAwaitingThread(Condition condition) {
		for( MonitorListener listener : listOfListeners ) {
			listener.signallerAwakesAwaitingThread( condition, this ) ; } }
    
	/*package*/ void notifySignallerLeavesTemporarily(Condition condition) {
		for( MonitorListener listener : listOfListeners ) {
			listener.signallerLeavesTemporarily( condition, this ) ; } }
    
	/*package*/ void notifySignallerReenters(Condition condition) {
		for( MonitorListener listener : listOfListeners ) {
			listener.signallerReenters( condition, this ) ; } }
    
	/*package*/ void notifySignallerLeavesMonitor(Condition condition) {
		for( MonitorListener listener : listOfListeners ) {
			listener.signallerLeavesMonitor( condition, this ) ; } }
}
