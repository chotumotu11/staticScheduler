

struct aperiodicjobs* readaperiodicjobs(){
    FILE *fp;
    fp = fopen("aperiodic.txt","r");
    if(fp == NULL)
	{
		printf("No file for aperiodic jobs found\n");
		return 0;
	}
    fscanf(fp,"%d",&n1);
    //printf("Number of aperiodic jobs %d\n",n1);
    struct aperiodicjobs *aj = (struct aperiodicjobs*)malloc(n1*sizeof(struct aperiodicjobs));
    for(int i =0;i<n1;i++){
        fscanf(fp,"%f %f",&aj[i].arrival,&aj[i].execution);
        aj[i].jobnum = i;
        aj[i].executedinframe = 0;
    }

    return aj;
    fclose(fp);
}


void sortaperiodicjobonarrival(struct aperiodicjobs *aj,int numofjobs){
    float min;
	int pos;
	for(int i=0;i<n1-1;i++){
		min = aj[i].arrival;
		pos = i;
		for(int j=i+1;j<n1;j++){
			if(min>aj[j].arrival){
				min = aj[j].arrival;
				pos = j;
			}
		}
		struct aperiodicjobs a;
		a = aj[i];
		aj[i] = aj[pos];
		aj[pos] = a;
	}
}

void insert(struct aperiodicjobs *aj){
    if(apj==NULL){
        apj=aj;
        apj->next = NULL;
        apj->prev = NULL;
    }else{
        struct aperiodicjobs * helper = apj;
        while(helper->next!=NULL){
            helper=helper->next;
        }
        helper->next = aj;
        aj->prev = helper;
        aj->next = NULL;
    }
}


void removeaperiodic(struct aperiodicjobs *aj){
    if(apj==NULL){
        return;
    }else if(apj->next == NULL){
        apj = NULL;
    }else{
        struct aperiodicjobs *helper=apj;
        if(apj == aj){
            apj = apj->next;
            apj->prev = NULL;
            return;
        }
        while(helper!=aj){
            helper = helper->next;
        }
        if(helper->next == NULL){
            (helper->prev)->next = NULL;
            return;
        }
        (helper->next)->prev = helper->prev;
        (helper->prev)->next = helper->next;
    }
}

void inserttoqueue(struct aperiodicjobs * aj)
{   
    if(aj == NULL){
        return;
    }
    for(int i =0;i<n1;i++){
        insert(aj+i);
    }
}


void printaperiodic(struct aperiodicjobs *aj){
    printf("The aperiodic jobs are\n");
    for(int i =0;i<n1;i++){
        printf("%f %f\n",aj[i].arrival,aj[i].execution);
    }

}


void printeraperiodic(){
    struct aperiodicjobs * helper = apj;
    while(helper!=NULL){
        printf("Job num aperiodic =  %d\n",helper->jobnum);
        helper = helper->next;
    }
}


void insertintoframesaperiodic(struct aperiodicjobs *j,struct frame *f){
	if(f->afirstjob == NULL){
		f->afirstjob = j;
		f->alastjob = j;
		j->next=NULL;
		j->prev=NULL;
	}else{
		f->alastjob->next = j;
		f->alastjob = j;
		j->prev = f->alastjob;
		j->next = NULL;
	}
}

void scheduleaperiodicjobs(struct frame *f,int numofframes){
    for(int i =0;i<numofframes;i++){
        struct aperiodicjobs * helper = apj;
        struct aperiodicjobs *test1;
        if(f[i].slack!=0){
            while(helper!=NULL){
                int flag = 0;
                // This is if the aperiodic job arrives at the middle of a frame.
                if(helper->arrival <= f[i].endtime && helper->arrival >= f[i].starttime && f[i].slack!=0){
                    //printf("Job num aperiodic = %d at frame number = %d job arrival = %f, frame start = %f\n",helper->jobnum,i,helper->arrival,f[i].starttime);
                    if(helper->execution<=f[i].slack){
                        if((f[i].endtime-helper->arrival)>=helper->execution){
                            f[i].slack -= helper->execution;
                            f[i].numberofjobs++;
                            struct aperiodicjobs * in = (struct aperiodicjobs*)malloc(sizeof(struct aperiodicjobs));
                            in->arrival = helper->arrival;
                            in->executedinframe = helper->execution;
                            in->execution = helper->execution;
                            in->jobnum = helper->jobnum;
                            in->next = helper->next;
                            in->prev = helper->prev;
                        
                            insertintoframesaperiodic(in,f+i);
                            flag = 1;
                            test1 = helper->next;
                            removeaperiodic(helper);
                        }else{
                            f[i].numberofjobs++;
                            helper->execution -= (f[i].endtime-helper->arrival);
                            struct aperiodicjobs * in = (struct aperiodicjobs*)malloc(sizeof(struct aperiodicjobs));
                            in->arrival = helper->arrival;
                            in->executedinframe = (f[i].endtime-helper->arrival);
                            in->execution = helper->execution;
                            in->jobnum = helper->jobnum;
                            in->next = helper->next;
                            in->prev = helper->prev;
                            f[i].slack -= (f[i].endtime-helper->arrival);
                            insertintoframesaperiodic(in,f+i);
                        }
                    }else{
                        if((f[i].endtime-helper->arrival)>=helper->execution){
                            helper->execution -= f[i].slack;
                            f[i].numberofjobs++;
                            struct aperiodicjobs * in = (struct aperiodicjobs*)malloc(sizeof(struct aperiodicjobs));
                            in->arrival = helper->arrival;
                            in->execution = helper->execution;
                            in->executedinframe = f[i].slack;
                            in->jobnum = helper->jobnum;
                            in->next = helper->next;
                            in->prev = helper->prev;
                            f[i].slack = 0;
                            insertintoframesaperiodic(in,f+i);
                        }else{
                            if((f[i].endtime-helper->arrival)<=f[i].slack){
                                helper->execution -= (f[i].endtime - helper->arrival);
                                f[i].numberofjobs++;
                                struct aperiodicjobs * in = (struct aperiodicjobs*)malloc(sizeof(struct aperiodicjobs));
                                in->arrival = helper->arrival;
                                in->execution = helper->execution;
                                in->executedinframe = (f[i].endtime - helper->arrival);
                                in->jobnum = helper->jobnum;
                                in->next = helper->next;
                                in->prev = helper->prev;
                                f[i].slack -= (f[i].endtime-helper->arrival);
                                insertintoframesaperiodic(in,f+i);
                            }else{
                                helper->execution -= f[i].slack;
                                f[i].numberofjobs++;
                                struct aperiodicjobs * in = (struct aperiodicjobs*)malloc(sizeof(struct aperiodicjobs));
                                in->arrival = helper->arrival;
                                in->executedinframe = f[i].slack;
                                in->execution = helper->execution;
                                in->jobnum = helper->jobnum;
                                in->next = helper->next;
                                in->prev = helper->prev;
                                f[i].slack = 0;
                                insertintoframesaperiodic(in,f+i);
                            }
                        }
                    }
                }else if(helper->arrival <= f[i].starttime && f[i].slack!=0){
                    // This section if any remaining job arrives before a frame ha started.
                    //printf("Job num aperiodic = %d at frame number = %d job arrival = %f, frame start = %f , frame slack = %f\n",helper->jobnum,i,helper->arrival,f[i].starttime,f[i].slack);
                    if(helper->execution<=f[i].slack){
                        //printf("Job num aperiodic = %d at frame number = %d job arrival = %f, frame start = %f , frame slack = %f, job exectution = %f\n",helper->jobnum,i,helper->arrival,f[i].starttime,f[i].slack,helper->execution);
                        f[i].slack -= helper->execution;
                        f[i].numberofjobs++;
                        struct aperiodicjobs * in = (struct aperiodicjobs*)malloc(sizeof(struct aperiodicjobs));
                        in->arrival = helper->arrival;
                        in->execution = helper->execution;
                        in->executedinframe = helper->execution;
                        in->jobnum = helper->jobnum;
                        in->next = helper->next;
                        in->prev = helper->prev;                       
                        insertintoframesaperiodic(in,f+i);
                        flag = 1;
                        test1 = helper->next; 
                        removeaperiodic(helper);
                    }else{
                        helper->execution -= f[i].slack;
                        f[i].numberofjobs++;
                        struct aperiodicjobs * in = (struct aperiodicjobs*)malloc(sizeof(struct aperiodicjobs));
                        in->arrival = helper->arrival;
                        in->execution = helper->execution;
                        in->executedinframe = f[i].slack;
                        in->jobnum = helper->jobnum;
                        in->next = helper->next;
                        in->prev = helper->prev;
                        f[i].slack = 0;
                        insertintoframesaperiodic(in,f+i);
                    }
                }
                if(flag == 0){
                    helper=helper->next;
                }
                else{
                    helper = test1;
                }
            }
        }
    }
} 



void printschedulefinal(struct frame* f, int numofframes){
	for(int i = 0;i<numofframes;i++){
		printf("Frame Number = %d with number of jobs = %d\n",f[i].framenum,f[i].numberofjobs);
		struct job *firstjob = f[i].firstjob;
        printf("Periodic jobs \n");
		while(firstjob!=NULL){
			printf("J%d%d (%f) \t",firstjob->oftask,firstjob->job,firstjob->exe);
			firstjob = firstjob->next;
		}
		printf("\n");
        printf("Sporadic Jobs \n");
        struct sporadicjobs *sfirstjob = f[i].sfirstjob;
        while(sfirstjob!=NULL){
			printf("J%d (%f) \t",sfirstjob->jobnum,sfirstjob->executedinframe);
			sfirstjob = sfirstjob->next;
		}
		printf("\n");
        printf("Aperiodic Jobs \n");
        struct aperiodicjobs *afirstjob = f[i].afirstjob;
        while(afirstjob!=NULL){
			printf("J%d (%f) \t",afirstjob->jobnum,afirstjob->executedinframe);
			afirstjob = afirstjob->next;
		}
        printf("\n");

	}
}


void printtimelinefinal(struct frame *f,int numofframes){
    FILE *writer;
	writer = fopen("finalSchedule.txt","w");
    for(int i=0;i<numofframes;i++){
        printf("Frame = %d\n",i);
        fprintf(writer,"Frame = %d\n",i);
        float frametime = f[i].starttime;
        struct sporadicjobs * sp = f[i].sfirstjob;
        struct job * pj = f[i].firstjob;
        struct aperiodicjobs* ap = f[i].afirstjob;
        while(sp!=NULL || pj!=NULL || ap!=NULL){

            if(ap!=NULL && ap->arrival <= frametime ){
                ap->inframestart = frametime;
                ap->inframestop = frametime+ ap->executedinframe;
                frametime = ap->inframestop;
                ap->executed=1;
                printf("AJ%d (%f,%f) \t",ap->jobnum,ap->inframestart,ap->inframestop);
                fprintf(writer,"AJ%d (%f,%f) \t",ap->jobnum,ap->inframestart,ap->inframestop);
                ap=ap->next;
                continue;
            }
            
            if(sp!=NULL){
                sp->inframestart = frametime;
                sp->inframestop = frametime+sp->executedinframe;
                frametime=sp->inframestop;
                printf("SJ%d (%f,%f) \t",sp->jobnum,sp->inframestart,sp->inframestop);
                fprintf(writer,"SJ%d (%f,%f) \t",sp->jobnum,sp->inframestart,sp->inframestop);
                sp=sp->next;
                continue;
            }

            if(pj!=NULL){
                pj->inframestart = frametime;
                pj->inframestop = frametime+pj->exe;
                frametime=pj->inframestop;
                printf("PJ%d%d (%f,%f) \t",pj->oftask,pj->job,pj->inframestart,pj->inframestop);
                fprintf(writer,"PJ%d%d (%f,%f) \t",pj->oftask,pj->job,pj->inframestart,pj->inframestop);
                pj=pj->next;
            }
        }
        
    printf("\n");
    fprintf(writer,"\n");
    }
    fclose(writer);
}