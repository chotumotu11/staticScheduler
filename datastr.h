float **arr;


struct frame{
	int framesize,framenum,numberofjobs;
	float starttime,endtime,slack;
	struct job *firstjob,*lastjob;
    struct sporadicjobs *sfirstjob,*slastjob;
    struct aperiodicjobs *afirstjob,*alastjob;
};


struct tasks{
	int tasknum;
	float period,deadline,execution;
};

// This is for periodic job.
struct job{
	int job,oftask,executed;
	float exe,arrival,end,inframestart,inframestop;
	struct job *next,*prev;
};



int n,n1;


struct aperiodicjobs{
	int jobnum,executed;
    float arrival,execution,executedinframe,inframestart,inframestop;
    struct aperiodicjobs * prev, *next;
};

struct sporadicjobs{
    int jobnum,executed;
    float arrival,deadline,execution,e,slack,executedinframe,inframestart,inframestop;
    struct sporadicjobs * prev,*next;
};

struct sporadicjobs * accepted;
struct sporadicjobs * unscheduled;
struct aperiodicjobs *apj;


void calalldata(struct frame* f,int numofframes,int hyp,int fra,int tasks){
	//First calculate the context swiches.
	int context = 0;
	for(int i = 0;i<numofframes;i++){
		struct sporadicjobs * sp = f[i].sfirstjob;
    	struct job * pj = f[i].firstjob;
    	struct aperiodicjobs* ap = f[i].afirstjob;
		while(sp!=NULL){
			context++;
			sp=sp->next;
		}
		while(pj!=NULL){
			context++;
			pj=pj->next;
		}
		while(ap!=NULL){
			context++;
			ap=ap->next;
		}
	}
	printf("The context switch is %d\n",context);
	int numf = hyp/fra;
	float *max =(float *)malloc((tasks+1)*sizeof(float));
	float *min =(float *)malloc((tasks+1)*sizeof(float));  
	for(int i =0 ;i<numf;i++){
		struct job * pj = f[i].firstjob;
		printf("Frame = %d\n",f[i].framenum);
		while(pj!=NULL){
			printf("Task =%d inframestop = %f, arrival = %f\n",pj->oftask,pj->inframestop,pj->arrival);
			if(max[pj->oftask]<(pj->inframestop-pj->arrival)){
				max[pj->oftask]=(pj->inframestop-pj->arrival);
			}
			if(min[pj->oftask]>(pj->inframestop-pj->arrival)){
				min[pj->oftask]=(pj->inframestop-pj->arrival);
			}
			pj=pj->next;
		}
	}
	for(int i =1;i<=tasks;i++){
		printf("Realtive response time of task %d = %f\n",i,max[i]-min[i]);
	}
}