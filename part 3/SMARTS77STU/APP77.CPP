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
	e1.wait(source);
		cout << "\nA entered\n";
e1.send('F', NULL, 0);
	// e1.send('G', NULL, 0);
	// e1.send('H', NULL, 0);
	

		int aTime=SMARTS.timeCounter;
	for (long j=0; j<600000; j++);
	Mutex1.acquire();
		cprintf("\n A suspendeed time: %d", SMARTS.timeCounter- aTime);
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
	
	int aTime=SMARTS.timeCounter;
	for (long j=0; j<600000; j++);
	Mutex1.acquire();
	e1.send('A', NULL, 0);
	cout << "\nB entered\n";
	Mutex2.acquire();
		cprintf("\n B suspendeed time: %d", SMARTS.timeCounter- aTime);
		for(long k=0;k<10;k++){
			for (long i=0; i <600000; i++);
			cout<<'B';
		}
	Mutex2.release();
		cout<<"\nB done\n";
  Mutex1.release();
}

void c( )
{
	e1.wait(source);
	int aTime=SMARTS.timeCounter;
	for (long l=0; l<600000; l++);
	Mutex2.acquire();
	e1.send('B', NULL, 0);
	cout << "\nC entered\n";
	for (long j=0; j<600000; j++);
//  cout << "\nC befor acquire\n";
	Mutex3.acquire();
		cprintf("\n C suspendeed time: %d", SMARTS.timeCounter- aTime);
		for(long k=0;k<10;k++){
			for (long i=0; i <600000; i++);
			cout<<'C';
		}
	Mutex3.release();
		cout<<"\nC done\n";
  Mutex2.release();
}
void d( )
{
	e1.wait(source);
	int aTime=SMARTS.timeCounter;
	for (long j=0; j<600000; j++);
	Mutex3.acquire();
	e1.send('C', NULL, 0);
	cout << "\nD entered\n";
		Mutex4.acquire();
			cprintf("\n D suspendeed time: %d", SMARTS.timeCounter- aTime);
		for(long k=0;k<10;k++){
			for (long i=0; i <600000; i++);
			cout<<'D';
		}
	Mutex4.release();
		cout<<"\nD done\n";
  Mutex3.release();
}	
void e( )
{
	for (long j=0; j<600000; j++);
	int aTime=SMARTS.timeCounter;
	for (long l=0; l <600000; l++);
	Mutex4.acquire();
	cprintf("\n E suspendeed time: %d", SMARTS.timeCounter- aTime);
	e1.send('D', NULL, 0);
	cout << "\nE entered\n";
		for(long k=0;k<10;k++){
			for (long i=0; i <600000; i++);
			cout<<'E';
		}
		cout<<"\nE done\n";
  Mutex4.release();
}	
void f( )
{
		e1.wait(source);

	for (long j=0; j<600000; j++);
	cout << "\nF entered\n";
		for(long k=0;k<10;k++){
			for (long i=0; i <600000; i++);
			cout<<'F';
		}
		cout<<"\nF done\n";
}	
void g( )
{
		// e1.wait(source);
	for (long i=0; i <600000; i++);
	cout << "\nG entered\n";
		for(long k=0;k<10;k++){
			for (long i=0; i <600000; i++);
			cout<<'G';
		}
		cout<<"\nG done\n";
}	
void h( )
{
		// e1.wait(source);

	for (long i=0; i <600000; i++);
	cout << "\nH entered\n";
		for(long k=0;k<10;k++){
			for (long i=0; i <600000; i++);
			cout<<'H';
		}
		cout<<"\nH done\n";
}	

void main( )
{
	clrscr();
		// SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, EDF);
	// SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, roundRobin);
	// SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, RMS);
	SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, FPS);
		SMARTS.declareTaskP(a,'A',700,1,5);
	SMARTS.declareTaskP(b,'B',700,1,4);
	SMARTS.declareTaskP(c,'C',700, 1,3);
	SMARTS.declareTaskP(d,'D',700, 1,2);
	SMARTS.declareTaskP(e,'E',700, 1,1);
	SMARTS.declareTaskP(f,'F',700, 1,3);
	SMARTS.declareTaskP(g,'G',700, 1,3);
	SMARTS.declareTaskP(h,'H',700, 1,2);
	// not working well
	// SMARTS.declareTask(a,'A',90,3);
	// SMARTS.declareTask(b,'B',90,4);
	// SMARTS.declareTask(c,'C',25,5);
	SMARTS.runTheTasks();
	Mutex5.acquire();
				for (long i=0; i <600000; i++);
	cout << "\n *************   Main   *********************\n";
	Mutex5.release();
	while(true);
}



