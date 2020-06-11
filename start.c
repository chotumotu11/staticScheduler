#include<stdio.h>
#include<stdlib.h>
#include "datastr.h"
#include "period.h"
#include "aperiod.h"
#include  "spor.h"


int main(int argc,char **argv){
	if(argc !=2){
		printf("Invalid argument type\n");
		return 0;
	}
	FILE *fptr;
	int n;
	int *p,*e,*D;
	fptr = fopen(argv[1],"r");
	if(fptr == NULL)
	{
		printf("Please set the tasks in file names tasks.txt\n");
		return 0;
	}
	fscanf(fptr,"%d",&n);
	p = (int *)malloc(sizeof(int)*n);
	e = (int *)malloc(sizeof(int)*n);
	D = (int *)malloc(sizeof(int)*n);

	// The first line contains the number of tasks
	// period execution time and deadline
	// if deadline is equal to period then put deadline as 0.
	int i=0;
	struct tasks *t=(struct tasks*)malloc(n*sizeof(struct tasks)); 
	while(!feof(fptr)){
		fscanf(fptr,"%d %d %d",&p[i],&e[i],&D[i]);
		if(D[i] == 0){
			D[i] = p[i];
		}
		t[i].tasknum = i+1;
		t[i].period = p[i];
		t[i].deadline = D[i];
		t[i].execution = e[i];
		i++;
	}
	// Now we will sort the tasks based on 
	tasksort(t,n);
	int hyp = lcmn(p,n);
	int total = totaljobs(hyp,t,n);
	printf("The Hyperperiod of the periodic tasks is : %d\n",hyp);


	int fra = calcframesize(e,n,hyp,p,D,0);
	if(fra == -1){
		printf("Valid frame not found\n");
		return 0;
	}
	int prevfram = 0;
	int success;
	struct frame *st;
	do{
		
		int numofframes = numframes(hyp,fra);
		st = initframes(fra,hyp);
		struct job * jobs = createjoblist(t,n,total,hyp);
		success = createschedule(st,jobs,total,numofframes);
		if(success == -1){
			printf("Cannot be scheduled with frame size %d\n",fra);
		}else{
			//printschedule(st,numofframes);
			genperiodicschedulefile(st,numofframes);
			createsigmamatrix(st,numofframes);
		}
		prevfram = fra;
		fra = calcframesize(e,n,hyp,p,D,fra);
	}while(success == -1 && fra!=-1);
	accepted = NULL;
	int framenum = numframes(hyp,prevfram);
	struct sporadicjobs *sj =  readsporadicjobs();
	
	sortsporadicjobonarrivaltime(sj,numberofsporadicjobs());
	
	checksporadicjobsatzero(sj,prevfram,hyp);
	
	struct sporadicjobs max  = findMaxDeadlineSporadic(sj);
	int framesinsporadicschedule = endframe(max,prevfram)/prevfram;
	framesinsporadicschedule = framenum > framesinsporadicschedule ? framenum : framesinsporadicschedule;
	struct frame *frameset = createframeset(st,framesinsporadicschedule,hyp,prevfram);
	createsporadicshedule(frameset,sj,framesinsporadicschedule,prevfram,hyp);
	
	printuncheduled();
	apj = readaperiodicjobs();
	sortaperiodicjobonarrival(apj,n1);
	
	inserttoqueue(apj);
	scheduleaperiodicjobs(frameset,framesinsporadicschedule);
	
	printtimelinefinal(frameset,framesinsporadicschedule);
	

	fclose(fptr);
	return 0;
}