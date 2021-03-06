/****************************************************************/
/* A Small Real Time System for the Real-Time laboratory        */
/* built by: A.Teitelbaum on an idea of H.G.Mendelbaum          */
/* Jerusalem College of Technology, 5759-64 (1999)              */
/* update  Tishrey   5777                                       */
/* SMARTS77.CPP, SMARTS class body                              */
/****************************************************************/
#include "smarts77.h"

/**********    Function     **********/
unsigned getTimerClocks( )
// Gets the remaining clocks of the timer register
{
	unsigned clocks;
		/* latch counter #0 */
	outportb(0x43,0x00);
		/* read counter #0 low byte */
	clocks=inportb(0x40);
		/* read counter #0 high byte */
	clocks += inportb(0x40)<<8;
	return clocks;
}
////////////////////////////////////////////////////
/**********    class body:  Mutex     **********/
Mutex::Mutex()
{
	s = 1;
	level = 0;
	owner = -1;
	numberSuspended = 0;
	// waitingList = {};
}
void Mutex::	acquire() {
		SMARTS.contextSwitchOff();
		cprintf("acquire %d", SMARTS.getCurrentTask());
	if (s == 1 || owner == SMARTS.getCurrentTask()) {
		s = 0; 
	}
	else {
		// cprintf("aqcuire %d\n", SMARTS.getCurrentTask());
		// cprintf("owner p %d\n", SMARTS.getPriorityOfTask(owner));
		// cprintf("current p %d\n", SMARTS.getPriorityOfTask(SMARTS.getCurrentTask()));

		put(SMARTS.getCurrentTask());
		numberSuspended++;
    SMARTS.setTaskOwnerOfTask(SMARTS.getCurrentTask() ,owner);
		if(SMARTS.getPriorityOfTask(owner)<SMARTS.getPriorityOfTask(SMARTS.getCurrentTask())) {
			int temp=SMARTS.getCurrentTask();
			while(SMARTS.getTaskOwnerOfTask(temp)!=-1){
				SMARTS.setPriorityOfTask(SMARTS.getTaskOwnerOfTask(temp) ,SMARTS.getPriorityOfTask(SMARTS.getCurrentTask()));
				temp=SMARTS.getTaskOwnerOfTask(temp);
			}
		}
			SMARTS.suspended();
		}
    SMARTS.setTaskOwnerOfTask(SMARTS.getCurrentTask() ,-1);
		owner = (SMARTS.getCurrentTask());

	level++;
		SMARTS.contextSwitchOn();
		// cprintf("acurire owner %d",owner);
		// cprintf("priority %d",SMARTS.getPriorityOfTask(owner));
}

void Mutex:: release() {
		SMARTS.contextSwitchOff();
		cprintf("release %d", SMARTS.getCurrentTask());
    if (owner == SMARTS.getCurrentTask())
		{
			SMARTS.setPriorityToOriginal(owner);
			if (--level)
				return;
			else {
				owner = -1;
			if (numberSuspended > 0) {
				numberSuspended--;
				SMARTS.resume(waitingList[numberSuspended]);
			}
			else
				s = 1;
	}
}
	SMARTS.contextSwitchOn();
	// cprintf("release owner %d",owner);
	// cprintf("priority %d",SMARTS.getPriorityOfTask(owner));
}

void Mutex::put(int task)
{
	if(numberSuspended==0){
		waitingList[0]=task;
	}
	else{
	for(int i =numberSuspended -1; i >= 0; i--)
	{
		if(SMARTS.getPriorityOfTask(task) > SMARTS.getPriorityOfTask(waitingList[i])){
			for(int j= numberSuspended; j > i;j--){
				waitingList[j]=waitingList[j-1];
			}
			waitingList[i]=task;
			i=-1;
		}
	}
	}
	// for( int i=0 ;i <= numberSuspended; i++){
	// 			cprintf("------ %d", waitingList[i]);
	// }
}

void Mutex::get(int task)
{
	for(int i = 0; i < numberSuspended; i++)
	{
		if(task == waitingList[i]){
			for(int j= i + 1; j < numberSuspended;j++){
				waitingList[j -1]=waitingList[j];
			}
			i=numberSuspended;
		}
	}
}
////////////////////////////////////////////////////
/**********    class body:  Parallelism     **********/
Parallelism::Parallelism()
{
	currentTask = 0;
	sleepTasks  = 0;
	activeTasks = 0;
	totalTasks  = 0;
	deadlock    = false;
	timeCounter = 0;
	// contextSwitchFlag = true;
	endOfTimeSlice = true;
}
	int Parallelism::getPriorityOfTask(int task){
		return context[task].priority;
	}
void Parallelism::setPriorityOfTask(int task, int priority){
		for(int i=0;i<MaxTask;i++)
		{
			// cprintf(",%d", context[task].originalPriority[i]);
			if(context[task].originalPriority[i]==-1){
				context[task].originalPriority[i]=priority;
				i=MaxTask;
			}
		}
		context[task].priority = priority;
	}

	
	int Parallelism::getTaskOwnerOfTask(int task){
		return context[task].taskOwner;
	}
	void Parallelism::setTaskOwnerOfTask(int task, int taskOwner){
				context[task].taskOwner = taskOwner;
	}
void Parallelism::externalFunctions(void interrupt ( *timerInterruptHandler)(...),
				    void far *scheduler, void far *userTaskEnd,
				    int far (*algorithm)( ))
		// Sets the external functions
{
	this->timerInterruptHandler = timerInterruptHandler;
	this->scheduler = scheduler;
	this->userTaskEnd = userTaskEnd;
	this->algorithm = algorithm;
	contextSched.declare(scheduler, userTaskEnd, 'S',1,55); // prepare the stack of the scheduler task
	for (int i=MaxStack-1; i >= (MaxStack-14); i--)
		schedCopy[i]=contextSched.stack[i];
}

int Parallelism::declareTask(void far *code, char name,int period,int times)
		// Insert a new task entry in SMARTS context array [ ]
{
	if (currentTask < MaxTask-1) 
	{
		context[totalTasks++].declare(code, userTaskEnd, name,period,times);
		++activeTasks;
		return true;
	}
	else	
		return false;
}

void Parallelism::setPriorityToOriginal(int task) {
	if(context[task].priority!=context[task].originalPriority[0])
	{
		int counter=0;
		int flag=0;
		int level=9;
		for(int i=MaxTask-1;i>=0;i--){
		// cprintf(",%d", context[task].originalPriority[i]);
			if(context[task].originalPriority[i]==-1)	
				level=i-1;
		}
		// context[task].originalPriority[level]=-1;
		// level--;
		// cprintf("level %d", level);
		// cprintf(" task %d", task);
		for(int j=level;j>=0;j--){
			for (int k=0; k<SMARTS.getTotalTasks()-1; k++)
			{
				if(SMARTS.context[k].taskOwner==task&&context[task].originalPriority[j]==SMARTS.context[k].priority){
					counter++;
					if(counter==2){
					context[task].priority=context[task].originalPriority[j];
					cprintf("priority %d", context[task].priority);
					cprintf("k %d", k);
					SMARTS.setPriorityToOriginal(k);
					context[task].originalPriority[j]=-1;
					SMARTS.context[k].taskOwner=-1;
					j=-1;
					k=SMARTS.getTotalTasks();
					flag=1;
					}
				}
			}
		}
		if(flag==0)
		{
			context[task].priority=context[task].originalPriority[0];
			cprintf("priority %d", context[task].priority);

		}
		
	}
	else{
		cprintf("+++++");
		cprintf("priority %d", context[task].priority);

	}
}

int Parallelism::declareTaskP(void far *code, char name,int period,int times, int priority)
		// Insert a new task entry in SMARTS context array [ ]
{
	if (currentTask < MaxTask-1) 
	{
		context[totalTasks++].declareP(code, userTaskEnd, name,period,times, priority, -1);
		++activeTasks;
		return true;
	}
	else	
		return false;
}

void Parallelism::runTheTasks()
		// Start running all tasks declared in SMARTS.		
{
	context[totalTasks].status = READY;	//for task 'runTheTasks' (IDLE task)
	context[totalTasks].priority = MAXINT;
	context[totalTasks].currentPriority = MAXINT;

	currentTask = totalTasks;

	asm	cli;		// forbids interrupts (clear interrupts) while changing the interrupt vect
		// saves the original BIOS userInt in our variable 'userIntAddress' to be restored at the end
	userIntAddress = getvect(userInt);	// BIOS userInt 0x60  (unused by PC)
				// puts the normal BIOS timerInt into the unused userInt address
	setvect(userInt,getvect(timerInt));	// timerInt 0x08 
	
	// sets our SMARTS external function 'timerInterruptHandler' as the new PC hard interrupt time handler
	setvect(timerInt,timerInterruptHandler);
	asm	sti;	// allows back interrupts (set interrupts)

	    // waits for end of runTheTasks (end of all the tasks)
	while(true)
	{

		if (deadlock)
		{
			textcolor(RED);
			cprintf("\n\n\rExit : deadlock");
			break;
		}
		if (activeTasks==0)
		{
			cprintf("\n\n\rExit : finish");
			break;
		}
	}

		// restore the original BIOS 'interrupt vector' at the end before returning to regular DOS
	asm	cli;	// no interrupts
	setvect(timerInt, getvect(userInt));	// restore original BIOS time handler
	setvect(userInt, userIntAddress);	// restore original BIOS userInt
	asm	sti;	// yes interrupts
}

void Parallelism::callScheduler( )
	// Return the control to the scheduler, this sets ON the software interrupt ProgInt flag
{
	// cprintf("call scheduler \n");
	setProgInt( );
	asm int timerInt;
}

void Parallelism::restoreSchedStack( )
	// Restore the scheduler stack
{
	for (int i=MaxStack-1; i >= (MaxStack-14); i--)
		contextSched.stack[i] = schedCopy[i];
}

int Parallelism::getCurrentTask( )
{	
	return currentTask;
}

void Parallelism::setCurrentTask(int taskNum)
	// Sets the next task to be run
{	
	if (taskNum <= totalTasks)
		currentTask = taskNum;
}

int Parallelism::getTotalTasks( )
	// Gets total tasks declared
{	
	return totalTasks;
}

int Parallelism::getDeadlock( )
{	
	return deadlock;
}

void Parallelism::setDeadlock( )
{	
	deadlock = true;
}

int Parallelism::contextSwitchOn( )
				// flag which enables context switch
{
	if (endOfTimeSlice) //is current time slice finished ?
	{	
		endOfTimeSlice = false;
		context[currentTask].contextSwitchFlag = true;
			// cprintf("t");
		callScheduler();	// return control to the scheduler
		return 1;
	}
	context[currentTask].contextSwitchFlag = true;
	return 0;
}

void Parallelism::contextSwitchOff( )
				// Disable context switch
{	
		context[currentTask].contextSwitchFlag = false;
}

int Parallelism::getContextSwitch( )
{	
	return context[currentTask].contextSwitchFlag;
}

void Parallelism::setProgInt( )
			// flag indicates to the extern function 'timerInterruptHandler' 
			// that this is an internal SMARTS software interrupt call, 
			// and the original BIOS function has no to be called.
{	
	progInt = true;
}

void Parallelism::resetProgInt()
{	
	progInt = false;
}

int Parallelism::getProgInt( )
{
	return progInt;
}

void Parallelism::setEndOfTimeSlice( )
		// flag indicates that when 'context switch' will be enabled, 
		// it must also return the control to the scheduler.
{	
	endOfTimeSlice = true;
}

char Parallelism::getName(int taskNum)	// returns name found or ' ' if not
{	
	return (taskNum <= totalTasks)? context[taskNum].name : ' ';
}

char Parallelism::getCurrentName( )
{	
	return context[currentTask].name;
}

int Parallelism::getTaskPeriodTillEnd(int taskNum){
	return context[taskNum].currentPeriod;
}
int Parallelism::getTaskPeriod(int taskNum){
	return context[taskNum].period;
}

taskStatus Parallelism::getStatus(int taskNum)  	
	// returns status or undefined if not found
{
	return (taskNum <= totalTasks)? context[taskNum].status : UNDEFINED;
}

taskStatus Parallelism::getCurrentStatus( )
{	
	return context[currentTask].status;
}

void Parallelism::resume(int taskNum)
{
	if (taskNum < totalTasks)
		context[taskNum].status = READY;
}

void Parallelism::resume(char taskName)
{
	for(int i=0;i<totalTasks;++i)
		if (context[i].name == taskName)
			context[i].status = READY;
}


void Parallelism::setCurrentNotActive()
{
	context[currentTask].status = NOT_ACTIVE;
	//	--activeTasks;
}
void Parallelism::suspended()
{	
	context[currentTask].status = SUSPENDED;
	callScheduler();
}

void Parallelism::incrPriority(int taskNum )
{
	if (taskNum < totalTasks)
		context[taskNum].incrPriority();
}
void Parallelism::setOriginalPriority(int taskNum )
{
	if (taskNum < totalTasks)
		context[taskNum].setOriginalPriority();
}

void Parallelism::setCurrentOriginalPriority()
{
	context[currentTask].setOriginalPriority();
}

Event *Parallelism::getExpectedEvent(int taskNum)    
// returns *Event  or  NULL  if not found
{	
	return (taskNum <= totalTasks)? context[taskNum].expectedEvent : NULL;
}

Event *Parallelism::getCurrentExpectedEvent( )
{	
	return context[currentTask].expectedEvent;
}

void Parallelism::setCurrentExpectedEvent(Event *expectedEvent)
{	
	context[currentTask].expectedEvent = expectedEvent;
}

void Parallelism::sleep(int t)
// Current task sleeps for 't' milliseconds
{
	if (t < MAXINT) 
	{
		context[currentTask].sleepCount = t/55+1;
		context[currentTask].status = SLEEP;
		++sleepTasks;
		callScheduler();		// return control to scheduler
	}
}

void Parallelism::sleepDecr(int taskNum)
{
	if (taskNum < totalTasks)
		context[taskNum].sleepDecr();
}

void Parallelism::getCurrentStack(unsigned &StackSeg, unsigned &StackPtr)
	// Load current task stack pointer
{   
	StackSeg = context[currentTask].stackSeg;
	StackPtr = context[currentTask].stackPtr;
}

void Parallelism::setCurrentStack(unsigned StackSeg, unsigned StackPtr)
	// Save current task stack pointer
{
	context[currentTask].stackSeg = StackSeg;
	context[currentTask].stackPtr = StackPtr;
}

void Parallelism::getSchedStack(unsigned &StackSeg, unsigned &StackPtr)
					// Load scheduler  stack pointer
{   
	StackSeg = contextSched.stackSeg;
	StackPtr = contextSched.stackPtr;
}

void Parallelism::handleTimers()
	// handling of the sleep status mode
{ 
	timeCounter ++ ;
	for (int i=0; i<getTotalTasks(); i++)
	{
			context[i].currentPeriod--;
		if(context[i].currentPeriod==0 && getStatus(i)==NOT_ACTIVE && context[i].times > 1)
		{
			context[i].reDeclareP(context[i].code,context[i].taskEnd,context[i].name,context[i].period,context[i].times, context[i].priority,context[i].taskOwner);
		}
		else {
			if(context[i].currentPeriod==0 && context[i].times > 1){
				contextSwitchOff();
				cout << "error, task not done";
				for(int i=0; i<getTotalTasks(); i++)
					context[i].status = NOT_ACTIVE;
				activeTasks = 0;
				contextSwitchOn();
			}
			else {
				if(context[i].currentPeriod == -1 && getStatus(i) == NOT_ACTIVE){
					activeTasks--;
				}
			}
		}
		
		if(getStatus(i)==SLEEP)
		{
			sleepDecr(i);
			if (getStatus(i) == READY)
				--sleepTasks;
		}
	}

}

void Parallelism::taskEnd( )
	// This function is called after the last operation of a task, instead of function return
{
	SMARTS.setCurrentNotActive();
	SMARTS.callScheduler();	// return the control to the scheduler to run a next task
}

/**********    class body:  Task     **********/
Task::Task()
{
	stack[MaxStack-14]=_BP;
	stack[MaxStack-13]=_DI;
	stack[MaxStack-12]=_SI;
	stack[MaxStack-11]=_DS;
	stack[MaxStack-10]=_ES;
	stack[MaxStack-9]=_DX;
	stack[MaxStack-8]=_CX;
	stack[MaxStack-7]=_BX;
	stack[MaxStack-6]=_AX;
	stackSeg=FP_SEG(&stack[MaxStack-14]);
	stackPtr=FP_OFF(&stack[MaxStack-14]);
	status = NOT_ACTIVE;
	sleepCount = 0;
	currentPriority=priority=originalPriority[0]=0;
	contextSwitchFlag=true;
}
//-----------------------------------------------------
void Task::declare(void far *code, void far *userTaskEnd, char name,int period,int times)
{
	stack[MaxStack-5]=FP_OFF(code);
	stack[MaxStack-4]=FP_SEG(code);
	stack[MaxStack-3]=_FLAGS;
	stack[MaxStack-2]=FP_OFF(userTaskEnd);
	stack[MaxStack-1]=FP_SEG(userTaskEnd);
	this->name= name;
	this->period=period;
	this->times=times;
	this->currentPeriod=period;
	this->code=code;
	this->taskEnd=userTaskEnd;
	status = READY;
}
//-----------------------------------------------------
void Task::declareP(void far *code, void far *userTaskEnd, char name,int period,int times, int priority,int taskOwner)
{
	stack[MaxStack-5]=FP_OFF(code);
	stack[MaxStack-4]=FP_SEG(code);
	stack[MaxStack-3]=_FLAGS;
	stack[MaxStack-2]=FP_OFF(userTaskEnd);
	stack[MaxStack-1]=FP_SEG(userTaskEnd);
	this->name= name;
	this->period=period;
	this->times=times;
	this->currentPeriod=period;
	this->code=code;
	this->taskEnd=userTaskEnd;
	this->priority=priority;
	this->taskOwner=taskOwner;
	status = READY;
	this->originalPriority[0]=priority;
	for(int i=1;i<MaxTask;i++){
		this->originalPriority[i]=-1;
	}
}
//----------------------------------------------------
void Task::reDeclare(void far *code, void far *taskEnd, char name,int period, int times)
{
	stack[MaxStack-14]=_BP;
	stack[MaxStack-13]=_DI;
	stack[MaxStack-12]=_SI;
	stack[MaxStack-11]=_DS;
	stack[MaxStack-10]=_ES;
	stack[MaxStack-9]=_DX;
	stack[MaxStack-8]=_CX;
	stack[MaxStack-7]=_BX;
	stack[MaxStack-6]=_AX;
	stackSeg=FP_SEG(&stack[MaxStack-14]);
	stackPtr=FP_OFF(&stack[MaxStack-14]);
	status = NOT_ACTIVE;
	sleepCount = 0;
	currentPriority=priority=0;
	contextSwitchFlag=true;
	this->declare( code, taskEnd, name, period,  times-1);
}
//----------------------------------------------------
void Task::reDeclareP(void far *code, void far *taskEnd, char name,int period, int times, int priority, int taskOwner)
{
	stack[MaxStack-14]=_BP;
	stack[MaxStack-13]=_DI;
	stack[MaxStack-12]=_SI;
	stack[MaxStack-11]=_DS;
	stack[MaxStack-10]=_ES;
	stack[MaxStack-9]=_DX;
	stack[MaxStack-8]=_CX;
	stack[MaxStack-7]=_BX;
	stack[MaxStack-6]=_AX;
	stackSeg=FP_SEG(&stack[MaxStack-14]);
	stackPtr=FP_OFF(&stack[MaxStack-14]);
	status = NOT_ACTIVE;
	sleepCount = 0;
	currentPriority=priority=0;
	contextSwitchFlag=true;
	this->declareP( code, taskEnd, name, period,  times-1, priority,taskOwner);
}
//----------------------------------------------------
void Task::incrPriority( )
{
 	--currentPriority;
}
//----------------------------------------------------
void Task::setOriginalPriority( )
{
	currentPriority = priority;
}
//----------------------------------------------------
void Task::sleepDecr( )
	// Decrements the sleep counter and update the task status accordingly
{	
	if (status==SLEEP)
	{
		if (sleepCount > 0)
			--sleepCount;
		if (!sleepCount)
			status = READY;
	}
}