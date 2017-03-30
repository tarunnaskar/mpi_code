/**
	Title  : Sequential Counting Sort for Characters using Pthreads
	Author : Debendranath Das
	Date   : 25.12.2016
**/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/resource.h>
#include<pthread.h>

#define NO_OF_THREADS 4
#define MAX 100000 //range of the input data set for unsigned characters..

typedef unsigned long long int ulli;
//int NO_OF_THREADS;

ulli no_of_elements_per_thread;
ulli global_count[MAX+1];  // i.e. index from 0 to 255

pthread_t tid[NO_OF_THREADS];
pthread_mutex_t mymutex[MAX+1];

void *thread_countingSort(void *);

//time_t start_t, end_t;
clock_t t;
struct rlimit x;
int *a;

int main(int argc,char *argv[])
{
	
	if (getrlimit(RLIMIT_STACK, &x) < 0)
    		perror("getrlimit");
	x.rlim_cur = RLIM_INFINITY;
	if (setrlimit(RLIMIT_STACK, &x) < 0)
    		perror("setrlimit");
	
	ulli n,i,j,l,start;
	n=atoll(argv[1]);
	printf("\nNo of elements : %llu\n",n);
	a=(int *)malloc(sizeof(int)*n);
	unsigned char k;
	for(i=0;i<n;i++)
	{
		a[i]=rand()%MAX+1;
	}
	
	printf("\nBefore Sorting : ");
	for(i=0;i<n;i++)
	{
		printf("%d ",a[i]);
	} printf("\n");
	t=clock();

	for(i=0;i<=MAX;i++)
	{
		global_count[i]=0;
	}
	for(i=0;i<=MAX;i++)
	{
		pthread_mutex_init(&mymutex[i],NULL);
	}
	
	no_of_elements_per_thread=n/NO_OF_THREADS;
	
	ulli info_size_for_each_thread=1+no_of_elements_per_thread; // 1 is added for thread id.
	//unsigned char info[NO_OF_THREADS][info_size_for_each_thread];
	size_t stacksize=1024*1024*1024;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr,stacksize);
	for(i=0;i<NO_OF_THREADS;i++)
	{
		pthread_create(&tid[i],NULL,thread_countingSort,(void *)(a+i*no_of_elements_per_thread));
	}
	for(l=0;l<NO_OF_THREADS;l++)
        {
		pthread_join(tid[l], NULL);

        }
	for(i=0;i<=MAX;i++)
	{
		pthread_mutex_destroy(&mymutex[i]);
	}
	/**
	printf("GLOBAL_COUNT\n");
	for(i=0;i<MAX;i++)
	{
		if(global_count[i]!=0)
			printf("%llu-->%llu\n",i,global_count[i]);
	}
	**/
	int sort[n]; 
	for(i=0;i<n;i++)
	{
		sort[global_count[a[i]]-1]=a[i];
		global_count[a[i]]--;
	}
	
	t=clock()-t;
	printf("\n\nAfter Sorting : ");
	for(i=0;i<n;i++)
	{
		printf("%d ",sort[i]);
	}
	double time_taken =((double)t)/CLOCKS_PER_SEC;// end_t-start_t;
	printf("\nExecution Time : %f \n\n",time_taken);
	pthread_exit(NULL);
	return 0;
}

void *thread_countingSort(void *m) {
	int *t=(int *)m;
    ulli i;
    ulli local_count[MAX+1];
    for (i=0;i<=MAX;i++)
        local_count[i] = 0;
    for (i=0;i<no_of_elements_per_thread; i++)
        local_count[*(t+i)] = local_count[*(t+i)] + 1;
    for (i=1;i<=MAX;i++)
        local_count[i] = local_count[i] + local_count[i-1];
    //lock
    for(i=0;i<=MAX;i++) {
	    pthread_mutex_lock(&mymutex[i]);
	    global_count[i]+=local_count[i];
	    pthread_mutex_unlock(&mymutex[i]);
    }
    pthread_exit((void*)0);	
}


/*void *thread_countingSort(void *m)
{
	unsigned char *t=(unsigned char *)m;
	//unsigned char p[no_of_elements_per_thread];
	ulli i;
	ulli local_count[MAX+1];
	for(i=0;i<=MAX;i++)
	{
		local_count[i]=0;
	}
	for(i=0;i<no_of_elements_per_thread;i++)
	{
		local_count[*(t+i)]++;
	}
	
	//for(i=0;i<MAX;i++)
	//{
	//	if(local_count[i]!=0)
	//		printf("%llu-->%llu\n",i,local_count[i]);
	//}
	for(i=1;i<=MAX;i++)
	{
		local_count[i]=local_count[i]+local_count[i-1];
	}
	// mutual-exclusion
	for(i=0;i<=MAX;i++)
	{
		pthread_mutex_lock(&mymutex[i]);
		global_count[i]+=local_count[i];
		pthread_mutex_unlock(&mymutex[i]);
	}
	pthread_exit((void*)0);	
}*/
