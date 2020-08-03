/********************************************************************/
/* A Small Real Time System for the Real-Time laboratory            */
/* built by: A.Teitelbaum on an idea of H.G.Mendelbaum              */
/* Jerusalem College of Technology, 5759-64 (1999-03)               */
/* update  Tishrey   5777                                           */
/* APP77.CPP, an application to demonstrate SMARTS77  functioning   */
/********************************************************************/
#include "smarts77.h"
// #include <ctime>    

Mutex Mutex1;
Mutex Mutex2;
Mutex Mutex3;
Mutex Mutex4;
Mutex Mutex5;

Event e1;
Event e2;
char source;
// user Functions
void a( )
{
	for (long j=0; j<600000; j++);
	Mutex1.acquire();
	cout << "\nA entered\n";
	e1.send('B', NULL, 0);
	e1.send('E', NULL, 0);
		for(long k=0;k<10;k++){
			for (long i=0; i <600000; i++);
			cout<<'A';
		}
		cout<<"\nA done\n";
  Mutex1.release();
}

void b( )
{
	e1.wait(source);
	for (long j=0; j<600000; j++);
	int aTime=SMARTS.timeCounter;
	Mutex1.acquire();
	cprintf("\n B suspendeed time: %d", SMARTS.timeCounter- aTime);
	cout << "\nB entered\n";
		for(long k=0;k<10;k++){
			for (long i=0; i <600000; i++);
			cout<<'B';
		}
		cout<<"\nB done\n";
  Mutex1.release();
}

void c( )
{
	for (long j=0; j<600000; j++);
	Mutex2.acquire();
	cout << "\nC entered\n";
		e2.send('D', NULL, 0);
	e2.send('F', NULL, 0);
		for(long k=0;k<10;k++){
			for (long i=0; i <600000; i++);
			cout<<'C';
		}
		cout<<"\nC done\n";
  Mutex2.release();
}
void d( )
{
	e2.wait(source);
	for (long j=0; j<600000; j++);
	int aTime=SMARTS.timeCounter;
	Mutex2.acquire();
	cprintf("\n D suspendeed time: %d", SMARTS.timeCounter- aTime);
	cout << "\nD entered\n";
		for(long k=0;k<10;k++){
			for (long i=0; i <600000; i++);
			cout<<'D';
		}
		cout<<"\nD done\n";
  Mutex2.release();
}	
void e( )
{
	e1.wait(source);
	for (long j=0; j<600000; j++);
	Mutex3.acquire();
	cout << "\nE entered\n";
		for(long k=0;k<10;k++){
			for (long i=0; i <600000; i++);
			cout<<'E';
		}
		cout<<"\nE done\n";
  Mutex3.release();
}	
void f( )
{
		e2.wait(source);
	for (long j=0; j<600000; j++);
	Mutex4.acquire();
	cout << "\nF entered\n";
		for(long k=0;k<10;k++){
			for (long i=0; i <600000; i++);
			cout<<'F';
		}
		cout<<"\nF done\n";
  Mutex4.release();
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
	SMARTS.declareTaskP(f,'F',70, 1,5);
	SMARTS.declareTaskP(d,'D',70, 1,6);
	SMARTS.declareTaskP(c,'C',70, 1,4);
	SMARTS.declareTaskP(e,'E',70, 1,2);
	SMARTS.declareTaskP(b,'B',70,1,3);
	SMARTS.declareTaskP(a,'A',70,1,1);
	// not working well
	// SMARTS.declareTask(a,'A',90,3);
	// SMARTS.declareTask(b,'B',90,4);
	// SMARTS.declareTask(c,'C',25,5);
	SMARTS.runTheTasks();
	Mutex5.acquire();
	cout << "\n *************   Main   *********************\n";
	Mutex5.release();
	while(true);
}



