/********************************************************************/
/* A Small Real Time System for the Real-Time laboratory            */
/* built by: A.Teitelbaum on an idea of H.G.Mendelbaum              */
/* Jerusalem College of Technology, 5759-64 (1999-03)               */
/* update  Tishrey   5777                                           */
/* APP77.CPP, an application to demonstrate SMARTS77  functioning   */
/********************************************************************/
#include "smarts77.h"
// #include <ctime>    

Mutex Mutexx;
Event e1;
char source;

// user Functions
void a( )
{
	int aTime=SMARTS.timeCounter;
	for (long j=0; j<600000; j++);
	Mutexx.acquire();
	// cprintf("A prio%d\n", SMARTS.getPriorityOfTask(SMARTS.getCurrentTask()));
	cprintf("A %d\n",SMARTS.getCurrentTask());
	cprintf("\n A suspendeed time: %d", SMARTS.timeCounter- aTime);
	cout << "\nA entered\n";
		e1.send('C', NULL, 0);
		e1.send('B', NULL, 0);
		for (long i=0; i <600000; i++);
		cprintf("\nAgain A \n");
  Mutexx.release();
}

void b( )
{
	cprintf("\nB entered\n");
	e1.wait(source);
	for (long i=0; i <600000; i++);
	cprintf("\nB again\n");
}

void c( )
{
	cprintf("\nC entered\n");
	e1.wait(source);
	int aTime=SMARTS.timeCounter;
	for (long j=0; j<600000; j++);
	Mutexx.acquire();
	cprintf("\n C suspendeed time: %d", SMARTS.timeCounter- aTime);	for (long i=0; i <600000; i++);
	cout << "\nC again\n";
  Mutexx.release();
}


void main( )
{
	clrscr();
		// SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, EDF);
	// SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, roundRobin);
	// SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, RMS);
	SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, FPS);
	// working well
	// SMARTS.declareTask(a,'A',70,2);
	// SMARTS.declareTask(b,'B',70,2);
	// SMARTS.declareTask(c,'C',90, 1);
	SMARTS.declareTaskP(c,'C',200, 1,3);
	SMARTS.declareTaskP(b,'B',200,1,2);
	SMARTS.declareTaskP(a,'A',200,1,1);
	// not working well
	// SMARTS.declareTask(a,'A',90,3);
	// SMARTS.declareTask(b,'B',90,4);
	// SMARTS.declareTask(c,'C',25,5);
	SMARTS.runTheTasks();
	Mutexx.acquire();
	cout << "\n *************   Main   *********************\n";
	Mutexx.release();
	while(true);
}



