/********************************************************************/
/* A Small Real Time System for the Real-Time laboratory                 */
/* built by: A.Teitelbaum on an idea of H.G.Mendelbaum                  */
/* Jerusalem College of Technology, 5759-64 (1999)                            */
/* SchedAlgo64.CPP, short-term scheduler algorithms                */
/****************************************************************/


#include "smarts77.h"

int roundRobin( )
// Round Robin short-term algorithm 
{
	int count;
	int nextTask = (SMARTS.getCurrentTask()+1)%SMARTS.getTotalTasks( );
	for (count=0;SMARTS.getStatus(nextTask)!=READY && count<SMARTS.getTotalTasks( );count++)
	     nextTask=++nextTask%SMARTS.getTotalTasks( );
	if (count == SMARTS.getTotalTasks( ))	// no found READY task to run
		nextTask = SMARTS.getTotalTasks( );
	return nextTask;
} 

int EDF()
{
	int count;
	int taskNum = 0;
	int minTimeToEnd = SMARTS.getTaskPeriodTillEnd(0);
	for(count = 1; count < SMARTS.getTotalTasks(); count ++){
		if(SMARTS.getTaskPeriodTillEnd(count)< minTimeToEnd && SMARTS.getStatus(count) == READY){
			minTimeToEnd = SMARTS.getTaskPeriodTillEnd(count);
			taskNum = count;
		} else {
			if (taskNum == 0 && SMARTS.getStatus(0) != READY && SMARTS.getStatus(count) == READY){
				minTimeToEnd = SMARTS.getTaskPeriodTillEnd(count);
			taskNum = count;
			}
		}
	}
	if(taskNum == 0 && SMARTS.getStatus(0) != READY)
		taskNum = SMARTS.getTotalTasks();
	return taskNum;
}

int RMS()
{
		int count;
	int taskNum = 0;
	int minPeriod = SMARTS.getTaskPeriod(0);
	for(count = 1; count < SMARTS.getTotalTasks(); count ++){
		if(SMARTS.getTaskPeriod(count)< minPeriod && SMARTS.getStatus(count) == READY){
			minPeriod = SMARTS.getTaskPeriod(count);
			taskNum = count;
		} else {
			if (taskNum == 0 && SMARTS.getStatus(0) != READY && SMARTS.getStatus(count) == READY){
				minPeriod = SMARTS.getTaskPeriod(count);
			taskNum = count;
			}
		}
	}
	if(taskNum == 0 && SMARTS.getStatus(0) != READY)
		taskNum = SMARTS.getTotalTasks();
	return taskNum;
}


int FPS()
{
	int count;
	int taskNum = 0;
	int minPriority = SMARTS.getPriorityOfTask(0);
	for(count = 1; count < SMARTS.getTotalTasks(); count ++){
		if(SMARTS.getPriorityOfTask(count) > minPriority && SMARTS.getStatus(count) == READY){
			minPriority = SMARTS.getPriorityOfTask(count);
			taskNum = count;
		} else {
			if (taskNum == 0 && SMARTS.getStatus(0) != READY && SMARTS.getStatus(count) == READY){
				minPriority = SMARTS.getPriorityOfTask(count);
			taskNum = count;
			}
		}
	}
	if(taskNum == 0 && SMARTS.getStatus(0) != READY)
		taskNum = SMARTS.getTotalTasks();
	return taskNum;
}

