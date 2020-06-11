


//Function to find GCD.

int gcd(int a,int b){
	if(b==0){
		return a;
	}

	return gcd(b,a%b);
}



  
void genperiodicschedulefile(struct frame *f, int numofframes){
	/*
		Structure of the file.
		Number of Frames in the schdedule.
		Number of Jobs	Remaining Slack.
		Task Number		Job Number 		Arrival time of Job		Deadline of Job		execution time of Job
	*/
	FILE *writer;
	writer = fopen("periodicSchedule.txt","w");
	fprintf(writer,"%d\n",numofframes);
	for(int i =0 ;i<numofframes;i++){
		fprintf(writer,"%d %0.2f\n",f[i].numberofjobs,f[i].slack);
		struct job *firstjob = f[i].firstjob;
		while(firstjob!=NULL){
			fprintf(writer,"%d %d %0.2f %0.2f %0.2f\n",firstjob->oftask,firstjob->job,firstjob->arrival,firstjob->end,firstjob->exe);
			firstjob = firstjob->next;
		}	
	}
	fclose(writer);
}


int totaljobs(int hyp,struct tasks *t,int n){
	int sum = 0;
	for(int i=0;i<n;i++){
		sum = sum + hyp/t[i].period;
	}

	return sum;
}


int numjobs(int hyp,struct tasks t){
	return hyp/t.period;
}

void tasksort(struct tasks *t,int n){
	float min;
	int pos;
	for(int i=0;i<n-1;i++){
		min = t[i].period;
		pos = i;
		for(int j=i+1;j<n;j++){
			if(min>t[j].period){
				min = t[j].period;
				pos = j;
			}
		}
		struct tasks a;
		a = t[i];
		t[i] = t[pos];
		t[pos] = a;
	}
}

int numframes(int hyp,int framesize){
	int ans = hyp/framesize;
	if(hyp%framesize!=0)
		ans++;
	return ans;
}

struct job * createjoblist(struct tasks *t,int n, int total, int hyp){
	struct job * s = (struct job *)malloc(total*sizeof(struct job));
	int k =0;
	for(int i = 0;i<n;i++){
		for(int j =0;j<numjobs(hyp,t[i]);j++){
			s[k].oftask = t[i].tasknum;
			s[k].job = j+1;
			s[k].exe = t[i].execution;
			s[k].arrival = j*t[i].period;
			s[k].end =  s[k].arrival+t[i].deadline;
			s[k].next = NULL;
			s[k].prev = NULL;
			k++;
		}
	}

	return s;
}

int lcmn(int *arr, int n){

	int lcm = arr[0];

	for(int i=0;i<n;i++){
		lcm = (arr[i]*lcm)/(gcd(arr[i],lcm));
	}

	return lcm;

}

// Find the maximum number in the array.

int findMax(int *p, int n){
	int max = p[0];
	for(int i=1;i<n;i++){
		if(max<p[i]){
			max = p[i];
		}
	}

	return max;
}


struct frame * initframes(int fra, int hyp){
	int numofframes = hyp/fra,i;
	struct frame *st;
	if(hyp%fra != 0)
		numofframes++;
	st = (struct frame *)malloc(numofframes*sizeof(struct frame));
	for(i=0;i<numofframes;i++){
		st[i].numberofjobs = 0;
		st[i].framenum = i;
		st[i].framesize = fra;
		st[i].slack = fra;
		st[i].starttime = i*fra;
		st[i].endtime = i*fra+fra;
		st[i].firstjob = NULL;
		st[i].lastjob = NULL;
		st[i].afirstjob = NULL;
		st[i].alastjob = NULL;
		st[i].sfirstjob = NULL;
		st[i].slastjob = NULL;
	}
	return st;
}



void printschedule(struct frame* f, int numofframes){
	for(int i = 0;i<numofframes;i++){
		printf("Frame Number = %d with number of jobs = %d\n",f[i].framenum,f[i].numberofjobs);
		struct job *firstjob = f[i].firstjob;
		while(firstjob!=NULL){
			printf("J%d%d\t",firstjob->oftask,firstjob->job);
			firstjob = firstjob->next;
		}
		printf("\n");
	}
}


void insertintoframe(struct job *j,struct frame *f){
	if(f->firstjob == NULL){
		f->firstjob = j;
		f->lastjob = j;
		j->next=NULL;
		j->prev=NULL;
	}else{
		f->lastjob->next = j;
		f->lastjob = j;
		j->prev = f->lastjob;
		j->next = NULL;
	}
}

int createschedule(struct frame *f,struct job *j, int total, int numofframes){
	//For each job we iterate through all the jobs.
	for(int i =0 ;i<total;i++){
		int flag = 0;
		for(int k=0;k<numofframes;k++){
			//First check if the frame satisfies the deadline of the job.
			// To satisfy the start time of the job must be before or equal to that of the frames starttime.
			// To satify the end time of the frame must be equal or less than the end time(deadline) of the job.
			//To satisfy that the frame has more or equal slack than the execution time of the job.
			// Overall the system will follow a rate monotonic method.
			// The insertintofuction is O(1) complexity. So the whole algo should be O(n^2). After this if time permits will improve the
			// implememntation.
			//printf("for i = %d job = J%d%d  job arrival = %f, frame start time = %f,job end = %f, frame end = %f, frame slack = %f, job execution = %f\n",i,j[i].oftask,j[i].job,j[i].arrival,f[k].starttime,j[i].end,f[k].endtime,f[k].slack,j[i].exe);
			if((j[i].arrival <= f[k].starttime) && (j[i].end >= f[k].endtime ) && (j[i].exe <= f[k].slack)){
				f[k].slack = f[k].slack - j[i].exe;
				f[k].numberofjobs++;
				struct job* in = (struct job*)malloc(sizeof(struct job));
				in->arrival = j[i].arrival;
				in->end = j[i].end;
				in->exe = j[i].exe;
				in->executed = j[i].executed;
				in->inframestart = j[i].inframestart;
				in->inframestop = j[i].inframestop;
				in->job = j[i].job;
				in->next = j[i].next;
				in->oftask = j[i].oftask;
				in->prev = j[i].prev;
				insertintoframe(in,f+k);
				flag = 1;
				break;
			} 
		}
		if(flag == 0){
			return -1;
		}
	}
	return 1; // 1 means schedule was done properly.

}

int calcframesize(int *e, int n,int hyp,int *p, int* D,int prevframe){

	// Finding the maximun execution time.
	int maxe = findMax(e,n);
	int lcm = lcmn(p,n);
	int framesize=lcm;
	
	if(prevframe != 0)
		framesize = prevframe-1;

	while(framesize >= maxe){
		
		if(lcm%framesize !=0){
			framesize--;
			continue;
		}
		int doesdivide = 0;
		for(int i=0;i<n;i++){
			if(p[i]%framesize == 0 )
			{
				doesdivide = 1;
				break;
			}
		}
		
		if(doesdivide == 1){
			int b = 1;
			for(int i =0;i<n;i++){
				//printf("FrameSize = %d, gcd = %d, D[%d] = %d\n",framesize,gcd(p[i],framesize),i,D[i]);
				if(2*framesize-gcd(p[i],framesize)>D[i]){
					
					b =0;
				}
			}
			if(b == 1)
				break;
		}
		framesize--;
	}

	if(framesize<maxe){
		return -1;
	}
	return framesize;
}



void createsigmamatrix(struct frame *f,int n){
    int i,j;
    arr = (float **)malloc(n * sizeof(float *)); 
    for (i=0; i<n; i++) 
         arr[i] = (float *)malloc(n * sizeof(float));
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            if(i<=j){
                float ans = 0;
                for(int k = i;k<=j;k++){
                    ans = ans + f[k].slack;
                }
                arr[i][j] = ans;
            } 
        }
    }
}

void printer(int n){
    int i,j;
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            printf("%f ",arr[i][j]); 
        }
        printf("\n");
    }
}
