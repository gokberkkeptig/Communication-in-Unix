/*Yusuf Gökberk Keptiğ 2079366*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <wait.h>
#include <string.h>

int * insertionSort(int arr[], int n );

int * selectionSort(int arr[], int n);
void swap(int *xp, int *yp);

static int signalIdentifier;
static struct tm * receiveTime;
void sig_handler(int sig);
int cmpfunc (const void * a, const void * b);


struct value
{		
	char signalTime[50];
	char signalName[50];
	int* numbers;
	double executionTime;
	char strNums[100];
};
	
	

int main(int argc, char *argv[]){

	int pi_d ,pid,readNumber = 0,n,i, *arr, childStatus, numberCounter ;
	int* numbers;
	char readFile[30];
	
    	struct sigaction sa;
    	sa.sa_handler = sig_handler;
    	sa.sa_flags = SA_RESTART;
    	sigemptyset(&sa.sa_mask);
    	
    	int fd[2],fd1[2],fd2[2];
    	if(pipe(fd) == -1){
    		printf ("error with the pipe. \n");
    		return 1;
    	}
    	
	sigaction(SIGUSR1, &sa , NULL);
    	sigaction(SIGUSR2, &sa , NULL);
    	
	pid_t *pidArray;
	srand(time(NULL));
	
	printf("Enter n:");
	scanf("%d",&n);
	
	struct value *values;
	values = (struct value*)malloc(sizeof(struct value)*n); 	 
	struct value *recievedValues;
	recievedValues = (struct value*)malloc(sizeof(struct value)*n);   	
	
	pidArray = (pid_t *)malloc(sizeof(pid_t)*n); 	
	double *randomNumberGenerate = (double *)malloc(sizeof(double)*n); 
	for(i = 0; i<n; i++){
		
		randomNumberGenerate[i]= (rand()%7+1);
		pidArray[i] = fork();	
		if(pidArray[i] == 0){// Child Process
		
			close(fd[0]);
			
	  		printf("\nNumber: %d Child Process id:\npid :%d\nppid:%d\n",i,getpid(),getppid());
	  		numberCounter = 0;
	  		sprintf(readFile, "input%d.txt",i+1);
	  		printf("\n%s is ok!",readFile);
	  		FILE *file  = fopen (readFile, "r");
	  		while(fscanf(file, "%d", &readNumber) == 1){
	  			numberCounter++;
	  		}
	  			
			fseek( file, 0, SEEK_SET );
			values[i].numbers = (int*)malloc(sizeof(int)*numberCounter); 
			int j = 0;
				
	  		while (fscanf(file, "%d", &readNumber) == 1){  
			      values[i].numbers[j]= readNumber; 
			      j++;
			    }   
			
			fclose(file);
			
			clock_t begin = clock();   
			if(pidArray[i]%2 == 0){
				values[i].numbers = insertionSort(values[i].numbers,numberCounter);			
			}
			else{
				values[i].numbers = selectionSort(values[i].numbers,numberCounter);
			}
			
			
			
			clock_t end = clock();
			double timeSpent = (double)(end - begin) / CLOCKS_PER_SEC;
			timeSpent = timeSpent + randomNumberGenerate[i];
			sleep(randomNumberGenerate[i]);
			values[i].executionTime = timeSpent;
			
			printf("\nExecuted in %lf seconds",values[i].executionTime);
			
			
			if(signalIdentifier == 1)
				strcpy(values[i].signalName, "SIGUSR1");
				
			
			else if(signalIdentifier == 2)
				strcpy(values[i].signalName, "SIGUSR2");
				
				
			printf("\nSignal nAme:  %s ",values[i].signalName);	
				
				
			char buffer[26];
			strftime(buffer, 26, "%H:%M:%S", receiveTime);
			strcpy(values[i].signalTime, buffer);
			printf("\nSignal time:  %s ",values[i].signalTime);	
			
			//Creating intermidiate Files
			
	  		int l;
	  		
	  		for (l = 0 ; l < numberCounter ; ++l)
				{
				
				    values[i].strNums[l] = values[i].numbers[l]+'0';
				
				}
	  		
        	
	  		if(write(fd[1],&values[i],sizeof(struct value)) == -1){
	  			printf("error with the pipe 3. \n");
	  			return 3;
	  		}
	  		
	  	
	  		//close(fd[1]);
	  		
	  		
	  		
	  		//close(fd[1]);
			
	  		exit(0);
		}
		else{ // parent process
			
			//close(fd[1]);
			if(pidArray[i]%2 == 0){
				kill(pidArray[i],SIGUSR1);
				sleep(1);
						
			}
			else{
			
				kill(pidArray[i],SIGUSR2);
				sleep(1);
			}
			wait(&childStatus);
	  		
				 
	  	
			int mysize = 0;
			
			// parent process reads from pipe
			if (read(fd[0], &recievedValues[i], sizeof(struct value)) == -1) // note variable reference here
			{
			    printf("error with the pipe. \n");
			    return 5;
			}
			
			//close(fd[0]);
            	
    			wait(NULL);
	  		
		}	
	}
	
    		int z;
    		
		//Sorting struct value
    		qsort(recievedValues, n, sizeof(struct value), cmpfunc);
    		
		FILE *final= fopen("output.txt","w");
		for(z=0; z < n ; z++){
			fprintf(final,"%lf %s %s %s\n",recievedValues[z].executionTime,recievedValues[z].strNums,recievedValues[z].signalName,recievedValues[z].signalTime);
			
		}
			
		fclose(final);
		
		
		
    		
	return 0;
}

//FUNCTIONS

//Sorting struct value based on their execution time by descending order
int cmpfunc (const void * a, const void * b) {
	struct value *e1 = (struct value *)a;
      	struct value *e2 = (struct value *)b;
      	
      	
      	if (e1->executionTime < e2->executionTime)
    		return 1;
  	else if (e1->executionTime > e2->executionTime)
    		return -1;
  	else
    		return 0;  
}	
		

void sig_handler(int sig){
	//These are neccesary for getting time information
	time_t rawtime;
	time(&rawtime);
  	receiveTime = localtime(&rawtime);
  	
    if(sig == SIGUSR1){
    	printf("\nChild:Signal 1 is recieved from parent!");
    	signalIdentifier = 1;
    }
    else{
    	printf("\nChild:Signal 2 is recieved from parent!");
    	signalIdentifier = 2;
    }
}



int* insertionSort(int arr[], int n)
{
    int i, key, j;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;
 
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
    
    return arr;
}
 


void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}
  
int* selectionSort(int arr[], int n)
{
    int i, j, min_idx;
  
    for (i = 0; i < n-1; i++)
    {
        min_idx = i;
        for (j = i+1; j < n; j++)
          if (arr[j] < arr[min_idx])
            min_idx = j;
  
        swap(&arr[min_idx], &arr[i]);
    }
    
    return arr;
}
  


