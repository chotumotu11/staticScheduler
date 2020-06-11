// Contaiins all the code to schedule all sporadic jobs properly.
// The sporadic jobs contains arrival time, deadline, execution.


// First step is to read the sporadic job from the file.




float sigma(int a,int b,int f,int hyp){
    int F = (hyp/f);
    if(a<F && b<F){
       return arr[a][b];
    }
    int j = a/F+1;
    int i = a%F;
    int jdash = b/F+1;
    int k = b%F;
    //printf("F = %d, j = %d, i = %d, jdash = %d, k = %d\n",F,j,i,jdash,k);
    return arr[i][F-1]+arr[0][k]+(jdash-j-1)*arr[0][F-1];
}
struct sporadicjobs findMaxDeadlineSporadic(struct sporadicjobs *sj){
    float max  = 0; 
    int pos = -1;
    for(int i =0;i<n;i++){
        if(sj[i].deadline>max){
            max = sj[i].deadline;
            pos = i;
        }
    }
    return sj[pos];
} 



void insertsporadicjobintoqueue(struct sporadicjobs *new1){
    if(accepted==NULL){
        accepted = new1;
        new1->next=NULL;
        new1->prev = NULL;
    }else{
        
        struct sporadicjobs *nextone = accepted;
        if((nextone->deadline)>(new1->deadline)){
            
            new1->next = nextone;
            nextone->prev = new1;
            new1->prev = NULL;
            accepted = new1;
            return;
        }
    
        while(nextone->next!=NULL){
            
            struct sporadicjobs *test = nextone->next; 
            if((test->deadline)>(new1->deadline)){
            
                new1->next = test;
                new1->prev = test->prev;
                (test->prev)->next = new1;
                test->prev = new1; 
                return;
            }
            nextone=nextone->next;
        }
        nextone->next = new1;
        new1->prev = nextone;
        new1->next = NULL;
    }
}


void removesporadicjobfromqueue(struct sporadicjobs *new1){
    if(accepted == NULL){
        accepted = NULL; //nothing to remove.
    }else if(accepted->next == NULL){
        //free(accepted);
        accepted = NULL;
    }else{
        struct sporadicjobs * helper = accepted;
        if(accepted == new1){
            accepted = accepted->next;
            accepted->prev = NULL;
            //free(helper);
            return;
        }
        
        while(helper!=new1){
            helper=helper->next;
        }
        if(helper->next== NULL){
            (helper->prev)->next = NULL;
            //free(helper);
            return;
        }
        (helper->next)->prev = helper->prev;
        (helper->prev)->next = helper->next;
        //free(helper);
    }
}

// The below two function gives the freaking time.
int startframe(struct sporadicjobs  t, int f){
    return ((int)(t.arrival/(f+1)))*f;
}

int endframe(struct sporadicjobs t,int f){
    return ((int)(t.deadline/f))*f;
}


void insertintoframesporadic(struct sporadicjobs *j,struct frame *f){
	if(f->sfirstjob == NULL){
		f->sfirstjob = j;
		f->slastjob = j;
		j->next=NULL;
		j->prev=NULL;
	}else{
		f->slastjob->next = j;
		f->slastjob = j;
		j->prev = f->slastjob;
		j->next = NULL;
	}
}


void printschedulewithsporadic(struct frame* f, int numofframes){
	for(int i = 0;i<numofframes;i++){
		printf("Frame Number = %d with number of jobs = %d\n",f[i].framenum,f[i].numberofjobs);
		struct job *firstjob = f[i].firstjob;
        printf("Periodic jobs \n");
		while(firstjob!=NULL){
			printf("J%d%d\t",firstjob->oftask,firstjob->job);
			firstjob = firstjob->next;
		}
		printf("\n");
        printf("Sporadic Jobs \n");
        struct sporadicjobs *sfirstjob = f[i].sfirstjob;
        while(sfirstjob!=NULL){
			printf("J%d\t",sfirstjob->jobnum);
			sfirstjob = sfirstjob->next;
		}
		printf("\n");

	}
}

float exeminusek(struct sporadicjobs * all){
    float ans = 0;
    struct sporadicjobs *aa = accepted;
    while(aa!=NULL){
        if((aa->deadline)<=all->deadline && aa!=all){
            ans = ans +  (aa->execution - aa->e);
        }
        aa=aa->next;
    }
    return ans;
}

int allaccepedok(float deadline,float execution){
    struct sporadicjobs *aa = accepted;
    while(aa!=NULL){
        if((aa->deadline)>deadline){
            if((aa->slack)<execution){
                return -1;
            }
        }
        aa=aa->next;
    }
    while(aa!=NULL){
        if((aa->deadline)>deadline){
            aa->slack = aa->slack - execution;
        }
        aa=aa->next;
    }
    return 1;
}

void insertintonunscheduled(struct sporadicjobs *sj){
    //printf("The job num is %d\n",sj->jobnum);
    struct sporadicjobs *t = (struct sporadicjobs*)malloc(sizeof(struct sporadicjobs));
    t->jobnum = sj->jobnum;
    t->arrival = sj->arrival;
    t->deadline = sj->deadline;
    t->e = sj->e;
    t->execution = sj->execution;
    t->next = sj->next;
    t->prev = sj->prev;
    t->slack = sj->slack;
    if(unscheduled == NULL){
        unscheduled = t;
        unscheduled->next=NULL;
        unscheduled->prev=NULL;
    }else{
        struct sporadicjobs * helper = unscheduled;
        while(helper->next!=NULL){
            helper = helper->next;
        }
        helper->next = t;
        t->prev = helper;
        t->next = NULL;
    }

}

void checksporadicjobsatzero(struct sporadicjobs * all,int f,int hyp){
    for(int i = 0;i<n;i++){
        if(all[i].arrival == 0){
            int startf  = startframe(all[i],f)/f;
            int endf = endframe(all[i],f)/f-1;
            float sigmac = sigma(startf,endf,f,hyp);
            //printf("%d %d %f %f %d\n",startf,endf,sigmac,exeminusek(&all[i]),allaccepedok(all[i].deadline,all[i].execution));
            float kk = sigmac - exeminusek(&all[i])-all[i].execution;
            if(kk>=0 && allaccepedok(all[i].deadline,all[i].execution)!=-1){
                all[i].slack = kk;
                insertsporadicjobintoqueue(&all[i]);
            }else{
                insertintonunscheduled(&all[i]);
            }
        }else{
            break;
        }
    }
}

int numberofsporadicjobs(){
    return n;
}

int numberofaperiodicjobs(){
    return n1;
}

struct job * deepinsert(struct job* lp){
    if(lp==NULL){
        return NULL;
    }
    struct job* helper = lp;
    int i=0;
    while(helper!=NULL){
        i++;
        helper=helper->next;
    }
    struct job* deep =(struct job*)malloc(i*sizeof(struct job));
    for(int k =0;k<i;k++){
        deep[k]=lp[k];
    } 
    return deep;
}


struct frame * createframeset(struct frame * frames,int numofframe,int hyp,int fra){
    int actual = hyp/fra;
    struct frame *frameset = (struct frame*)malloc(numofframe * sizeof(struct frame));
    for(int i =0 ;i<numofframe;i++){
        frameset[i].afirstjob = frames[i%actual].afirstjob;
        frameset[i].alastjob = frames[i%actual].alastjob;
        frameset[i].endtime = i/actual*hyp + frames[i%actual].endtime;
        frameset[i].firstjob = deepinsert(frames[i%actual].firstjob);
        frameset[i].framenum = i;
        frameset[i].framesize = frames[i%actual].framesize;
        frameset[i].lastjob = deepinsert(frames[i%actual].lastjob);
        frameset[i].numberofjobs = frames[i%actual].numberofjobs;
        frameset[i].sfirstjob = frames[i%actual].sfirstjob;
        frameset[i].slack = frames[i%actual].slack;
        frameset[i].slastjob = frames[i%actual].slastjob;
        frameset[i].starttime = i/actual*hyp + frames[i%actual].starttime;
    }
    return frameset;
}


void sortsporadicjobonarrivaltime(struct sporadicjobs *sj, int numofjobs){
    float min;
	int pos;
	for(int i=0;i<n-1;i++){
		min = sj[i].arrival;
		pos = i;
		for(int j=i+1;j<n;j++){
			if(min>sj[j].arrival){
				min = sj[j].arrival;
				pos = j;
			}
		}
		struct sporadicjobs a;
		a = sj[i];
		sj[i] = sj[pos];
		sj[pos] = a;
	}
}



struct sporadicjobs* readsporadicjobs(){
    FILE *fp;
    fp = fopen("sporadic.txt","r");
    if(fp == NULL)
	{
		printf("No file for sporadic jobs found\n");
		return 0;
	}
    fscanf(fp,"%d",&n);
    //printf("Number of sporadic jobs %d\n",n);
    struct sporadicjobs *sj = (struct sporadicjobs*)malloc(n*sizeof(struct sporadicjobs));
    for(int i =0;i<n;i++){
        fscanf(fp,"%f %f %f",&sj[i].arrival,&sj[i].deadline,&sj[i].execution);
        sj[i].e=0;
        sj[i].jobnum = i;
    }

    return sj;
    fclose(fp);
}



void printuncheduled(){
    struct sporadicjobs *acc = unscheduled;
    printf("Printing uncheduled sproadic jobs \n");
    while(acc!=NULL){
        printf("Job Number = %d  ,\n",acc->jobnum);
        acc=acc->next;
    } 
}


void printaccepted(){
    struct sporadicjobs *acc = accepted;
    printf("Printing sporadic jobs \n");
    while(acc!=NULL){
        printf("Job Number = %d , arrival time = %f, deadline = %f, execution = %f \n",acc->jobnum,acc->arrival,acc->deadline,acc->execution);
        acc=acc->next;
    } 
}

// 1.Now we have to sort both based on the arrival times.
// 2. Sort accepted list based on deadlines so that EDF is implemented.
// 3. find the last frame needed based on the maximum deadline of the sporadic job.

int createsporadicshedule(struct frame *frames,struct sporadicjobs *all,int numberofframes,int f,int hyp){
    for(int i =0 ;i<numberofframes;i++){
        struct sporadicjobs *test = accepted;
        struct sporadicjobs *test1;
        while(test!=NULL){
            int flag = 0;
            //printf("The jobnum = %d\n",test->jobnum);
            //printf("Job arrival = %f , Job deadline = %f, frame starttime = %f, frame endtime = %f\n",test->arrival,test->deadline,frames[i].starttime,frames[i].endtime);
            if((test->deadline)>=frames[i].endtime && test->arrival <= frames[i].starttime){
                if(frames[i].slack!=0){
                    if(test->execution<=frames[i].slack){
                        frames[i].slack -= test->execution;
                        frames[i].numberofjobs++;
                        test->executedinframe = test->execution;
                        struct sporadicjobs * in = (struct sporadicjobs*)malloc(sizeof(struct sporadicjobs));
                        in->arrival = test->arrival;
                        in->deadline = test->deadline;
                        in->e = test->e;
                        in->executedinframe = test->executedinframe;
                        in->execution = test->execution;
                        in->jobnum = test->jobnum;
                        in->next = test->next;
                        in->prev = test->prev;
                        in->slack = test->slack;
                        
                        insertintoframesporadic(in,frames+i);
                        flag = 1;
                        test1 = test->next;
                        removesporadicjobfromqueue(test);
                    }else{
                        test->execution -= frames[i].slack;
                        test->e += test->e+frames[i].slack;
                        test->executedinframe = frames[i].slack;
                        frames[i].numberofjobs++;
                        struct sporadicjobs * in = (struct sporadicjobs*)malloc(sizeof(struct sporadicjobs));
                        in->arrival = test->arrival;
                        in->deadline = test->deadline;
                        in->e = test->e;
                        in->executedinframe = test->executedinframe; 
                        in->execution = test->execution;
                        in->jobnum = test->jobnum;
                        in->next = test->next;
                        in->prev = test->prev;
                        in->slack = test->slack;
                        frames[i].slack = 0;
                        insertintoframesporadic(in,frames+i);
                    }
                }
            }
            if(flag == 0){
                test=test->next;
            }
            else{
                test = test1;
            }
        }


        //Code for accepting jobs that come during the running of the presently selected frame.
        //printf("The value of n is %d\n",n);
        for(int j = 0;j<n;j++){
            //printf("jon arrival = %f , frame start = %f , frame end = %f\n",all[j].arrival,frames[i].starttime,frames[i].endtime);
            if(all[j].arrival > frames[i].starttime && all[j].arrival <= frames[i].endtime){
                //printf("Goru Gadha \n");
                int startf  = startframe(all[j],f)/f;
                int endf = endframe(all[j],f)/f-1;
                float sigmac = sigma(startf,endf,f,hyp);
                //printf("%d %d %f %f %d\n",startf,endf,sigmac,exeminusek(&all[j]),allaccepedok(all[j].deadline,all[j].execution));
                float kk = sigmac - exeminusek(&all[j])-all[j].execution;
                if(kk>=0 && allaccepedok(all[j].deadline,all[j].execution)!=-1){
                    all[j].slack = kk;
                    insertsporadicjobintoqueue(&all[j]);
                }else{
                    //printf("The jon unchedued is %d\n",all[j].jobnum);
                    //printf("jon arrival = %f , frame start = %f , frame end = %f\n",all[j].arrival,frames[i].starttime,frames[i].endtime);
                    insertintonunscheduled(&all[j]);
                }
            }
        }

    }
}
