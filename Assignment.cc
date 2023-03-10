//This Program is to schedule threads with Rate Monotonic

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/types.h>

#define gettid() syscall(__NR_gettid)
#define SCHED_DEADLINE  3
#define INNERLOOP 1000
#define OUTERLOOP 2000
#define NPERIODICTASKS 3
#define NAPERIODICTASKS 0
#define NTASKS NPERIODICTASKS + NAPERIODICTASKS

#ifdef __x86_64__
#define __NR_sched_setattr      314
#define __NR_sched_getattr      315
#endif

//code of periodic tasks
void task1_code( );
void task2_code( );
void task3_code( );



//characteristic function of the thread, only for timing and synchronization
//periodic tasks
void *task1( void *);
void *task2( void *);
void *task3( void *);



struct timeval next_arrival_time[NTASKS];

double WCET[NTASKS];
int missed_deadlines[NTASKS];


struct sched_attr {
     __u32 size;

     __u32 sched_policy;
     __u64 sched_flags;

     // SCHED_NORMAL, SCHED_BATCH 
     __s32 sched_nice;

     // SCHED_FIFO, SCHED_RR 
     __u32 sched_priority;
     // SCHED_DEADLINE (nsec)
     __u64 sched_runtime;
     __u64 sched_deadline;
     __u64 sched_period;
};

int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags)
 {
     return syscall(__NR_sched_setattr, pid, attr, flags);
 }

int sched_getattr(pid_t pid, struct sched_attr *attr, unsigned int size, unsigned int flags)
 {
     return syscall(__NR_sched_getattr, pid, attr, size, flags);
 }


struct sched_attr attr[NTASKS];
void *run_deadline1(void*data)
{
	
	
	    cpu_set_t cset;
	    CPU_ZERO (&cset);
	    CPU_SET(0, &cset);
	    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cset);
	int x=0, ret;
	unsigned int flags=0;
	printf("deadline thread 1 start %ld\n",
	    gettid());
	struct timespec waittime;
	waittime.tv_sec=0;
	waittime.tv_nsec=attr[0].sched_period;
	ret=sched_setattr(0,&attr[0],flags);
	if(ret<0)
	 {
	   perror("sched_setattr");
	   exit(-1);
        }
	int i=0;
	  for(i=0;i<100;i++)
	  {
		fflush(stdout);
//To execute the specific code of Application
		task1_code();
		fflush(stdout);
		struct timeval ora;
		struct timezone zona;
		gettimeofday(&ora, &zona);
//After the execution compute the time to beginning of next period
	long int timetowait=1000*((next_arrival_time[0].tv_sec-ora.tv_sec)*1000000+(next_arrival_time[0].tv_usec-ora.tv_usec));
	if(timetowait < 0) 
	  missed_deadlines[0]++;
	waittime.tv_sec=timetowait/1000000000;
	waittime.tv_nsec=timetowait%1000000000;
//Suspend the task until beginning of next period
//use nanosleep
	nanosleep(&waittime, NULL);
//The task is ready and now set the next arrival time for each task
	long int periods_micro=attr[0].sched_period/1000;
	next_arrival_time[0].tv_sec=next_arrival_time[0].tv_sec+periods_micro/1000000;
	next_arrival_time[0].tv_usec=next_arrival_time[0].tv_usec+periods_micro%1000000;

    	}
	printf("deadline thread 1 dies");
	return NULL;
}

void *run_deadline2(void*data)
{
	
	int x = 0, ret;
	unsigned int flags = 0;
	    cpu_set_t cset;
	    CPU_ZERO (&cset);
	    CPU_SET(0, &cset);
	    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cset);
	
	printf("deadline thread 2 start %ld\n",
	    gettid());
	struct timespec waittime;
	waittime.tv_sec=0;
	waittime.tv_nsec=attr[1].sched_period;
	ret=sched_setattr(0,&attr[1],flags);
	if(ret<0) {

	   perror("sched_setattr");
	   exit(-1);
        }
	int i=0;
	  for(i=0;i<100;i++)
	  {
		fflush(stdout);
//execute application specific code
		task2_code();
		fflush(stdout);
		struct timeval ora;
		struct timezone zona;
		gettimeofday(&ora, &zona);
//Time to beginning of the next 
	long int timetowait=1000*((next_arrival_time[1].tv_sec-ora.tv_sec)*1000000+(next_arrival_time[1].tv_usec-ora.tv_usec));
	if(timetowait<0)
	  missed_deadlines[1]++;

	waittime.tv_sec=timetowait/1000000000;
	waittime.tv_nsec=timetowait%1000000000;
//Suspend the task until beginning of next period
//use nanosleep
	nanosleep(&waittime, NULL);
	long int periods_micro=attr[1].sched_period/1000;
	next_arrival_time[1].tv_sec=next_arrival_time[1].tv_sec+periods_micro/1000000;
	next_arrival_time[1].tv_usec=next_arrival_time[1].tv_usec+periods_micro%1000000;
	
    	}
	printf("deadline thread 2 dies");
	return NULL;
}
void *run_deadline3(void*data)
{
	
	
	    cpu_set_t cset;
	    CPU_ZERO (&cset);
	    CPU_SET(0, &cset);
	    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cset);
	int x=0, ret;
	unsigned int flags=0;
	printf("deadline thread 3 start %ld\n",
	    gettid());
	struct timespec waittime;
	waittime.tv_sec=0;
	waittime.tv_nsec=attr[2].sched_period;
	ret=sched_setattr(0,&attr[2],flags);
	if(ret<0) {
	   perror("sched_setattr");
	   exit(-1);
        }
	int i=0;
	  for(i=0;i<100;i++)
	  {
		fflush(stdout);
		task3_code();
		fflush(stdout);
		struct timeval ora;
		struct timezone zona;
		gettimeofday(&ora, &zona);
	long int timetowait=1000*((next_arrival_time[2].tv_sec-ora.tv_sec)*1000000+(next_arrival_time[2].tv_usec-ora.tv_usec));
	if(timetowait<0)
	  missed_deadlines[2]++;

	waittime.tv_sec=timetowait/1000000000;/*seconds*/
	waittime.tv_nsec=timetowait%1000000000;/*nanoseconds*/
//Suspend the task until beginning of next period
//use nanosleep
	nanosleep(&waittime, NULL);
//same as task1 compute time to beginning of next period
	long int periods_micro=attr[2].sched_period/1000;
	next_arrival_time[2].tv_sec=next_arrival_time[2].tv_sec+periods_micro/1000000;
	next_arrival_time[2].tv_usec=next_arrival_time[2].tv_usec+periods_micro%1000000;

    	}
	printf("deadline thread 3 dies");
	return NULL;
}
		
int main(int argc, char**argv)
{
	pthread_t thread1, thread2, thread3;
	int j;
	    for(j=0; j<NTASKS; j++)
	{
		attr[j].size = sizeof(attr[j]);
        	attr[j].sched_flags = 0;
        	attr[j].sched_nice = 0;
        	attr[j].sched_priority = 0;
        	attr[j].sched_policy = SCHED_DEADLINE;
	 }        
	attr[0].sched_runtime = 20 * 1000 * 1000;
        attr[0].sched_period  = 100 * 1000 * 1000;
        attr[0].sched_deadline= 100 * 1000 * 1000;

	attr[1].sched_runtime = 20 * 1000 * 1000;
        attr[1].sched_period  = 50 * 1000 * 1000;
        attr[1].sched_deadline= 50* 1000 * 1000;

	attr[2].sched_runtime = 20 * 1000 * 1000;
        attr[2].sched_period  = 200 * 1000 * 1000;
        attr[2].sched_deadline= 200 * 1000 * 1000;
    
	int i;
  	    for (i =0; i < NTASKS; i++)
    	    {

      		struct timeval timeval1;
      		struct timeval timeval2;
      		struct timezone timezone1;
      		struct timezone timezone2;

		gettimeofday(&timeval1,&timezone1);
   		if(i==0)
			task1_code();
   		if(i==1)
			task2_code();
   		if(i==2)
			task3_code();
		gettimeofday(&timeval2,&timezone2);
//worst case execution
	      	WCET[i]= 1000000000*(timeval2.tv_sec - timeval1.tv_sec)
			       +(timeval2.tv_sec-timeval1.tv_sec);
      		printf("\nWorst Case Execution Time %d=%f \n", i, WCET[i]);
	    }
//now To compute U
	double U = (double)WCET[0]/attr[0].sched_period+(double)WCET[1]/attr[1].sched_period+(double)WCET[2]/attr[2].sched_period;

    	// compute Ulub by considering the fact that we have harmonic relationships between periods
	double Ulub = 1;
	//if there are no harmonic relationships, use the following formula instead
	
	
	//check the sufficient conditions: if they are not satisfied, exit  
  	if (U > Ulub)
    	{
      		printf("\n U=%lf Ulub=%lf Non schedulable Task Set", U, Ulub);
      		return(-1);
    	}
  	printf("\n U=%lf Ulub=%lf Scheduable Task Set", U, Ulub);
  	fflush(stdout);
  	sleep(5);
	printf("main thread [%ld]\n",gettid());

  	int iret[NTASKS];

	//declare variables to read the current time
	struct timeval ora;
	struct timezone zona;
	gettimeofday(&ora, &zona);
  	

  	// set the next arrival time for each task. This is not the beginning of the first
	// period, but the end of the first period and beginning of the next one. 
  	for (i = 0; i < NTASKS; i++)
    	{
		// first we encode the current time in nanoseconds and add the period 
	
		long int periods_micro = attr[i].sched_period/1000;
		//then we compute the end of the first period and beginning of the next one
		
		next_arrival_time[i].tv_sec = ora.tv_sec+periods_micro/1000000;
		next_arrival_time[i].tv_usec = ora.tv_usec+periods_micro%100000;
       		missed_deadlines[i] = 0;
    	}
	// create all threads(pthread_create)
  	iret[0] = pthread_create( &thread1, NULL, run_deadline1, NULL);
  	iret[1] = pthread_create( &thread2, NULL, run_deadline2, NULL);
  	iret[2] = pthread_create( &thread3, NULL, run_deadline3, NULL);
	sleep(10);
// join all threads (pthread_join)
  	pthread_join( thread1, NULL);
  	pthread_join( thread2, NULL);
  	pthread_join( thread3, NULL);
	printf("main dies[%ld]\n",gettid());
	return 0;
}
// application specific task_1 code
void task1_code()
{
	//print the id of the current task
  	printf(" 1[ "); fflush(stdout);

	//this double loop with random computation is only required to waste time
	int i,j;
	double uno;
  	for (i = 0; i < OUTERLOOP; i++)
    	{
      		for (j = 0; j < INNERLOOP; j++)
		{
			uno = rand()*rand()%10;
    		}
  	}



  	//print the id of the current task
  	printf(" ]1 "); fflush(stdout);
}

void task2_code()
{
	//print the id of the current task
  	printf(" 2[ "); fflush(stdout);
	int i,j;
	double uno;
  	for (i = 0; i < OUTERLOOP; i++)
    	{
      		for (j = 0; j < INNERLOOP; j++)
		{
			uno = rand()*rand()%10;
		}
    	}
	//print the id of the current task
  	printf(" ]2 "); fflush(stdout);
}
void task3_code()
{
	//print the id of the current task
  	printf(" 3[ "); fflush(stdout);
	int i,j;
	double uno;
  	for (i = 0; i < OUTERLOOP; i++)
    	{
      		for (j = 0; j < INNERLOOP; j++);		
			double uno = rand()*rand()%10;
    	}
	//print the id of the current task
  	printf(" ]3 "); fflush(stdout);
}
