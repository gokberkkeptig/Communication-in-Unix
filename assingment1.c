/*Yusuf Gökberk Keptiğ 2079366*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <wait.h>

int * insertionSort(int arr[], int n );

int * selectionSort(int arr[], int n);
void swap(int *xp, int *yp);

static int signalIdentifier;
static struct tm * receiveTime;
void sig_handler(int sig);

struct value
{		
	char signalTime[250];
	char numbers[250];
	int index;
	double executionTime;
};
	
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
		

int main(int argc, char *argv[]){

	int pi_d ,pid,readNumber = 0,n,i, *arr, childStatus, numberCounter ;
	int* numbers;
	char readFile[30],addFileName[30],readFileName[30];
	FILE *oFile;
	FILE *outputFile;	
	FILE *getFile;
  	
    	struct sigaction sa;
    	sa.sa_handler = sig_handler;
    	sa.sa_flags = SA_RESTART;
    	sigemptyset(&sa.sa_mask);
    	
    	
	sigaction(SIGUSR1, &sa , NULL);
    	sigaction(SIGUSR2, &sa , NULL);
    	
	pid_t *pidArray;
	srand(time(NULL));
	
	printf("Enter n:");
	scanf("%d",&n);
	pidArray = (pid_t *)malloc(sizeof(pid_t)*n); 	
	double *randomNumberGenerate = (double *)malloc(sizeof(double)*n); 
	for(i = 0; i<n; i++){
		randomNumberGenerate[i]= (rand()%7+1);
		pidArray[i] = fork();	
		if(pidArray[i] == 0){//Child Process
	  		printf("\nNumber: %d Child Process id:\npid :%d\nppid:%d\n",i,getpid(),getppid());
	  		numberCounter = 0;
	  		sprintf(readFile, "input%d.txt",i+1);
	  		printf("\n%s is ok!",readFile);
	  		FILE *file  = fopen (readFile, "r");
	  		while(fscanf(file, "%d", &readNumber) == 1){
	  			numberCounter++;
	  		}
	  			
			fseek( file, 0, SEEK_SET );
			numbers = (int*)malloc(sizeof(int)*numberCounter); 
			int j = 0;
				
	  		while (fscanf(file, "%d", &readNumber) == 1){  
			      numbers[j]= readNumber; 
			      j++;
			         
			    }   
			fclose(file);
			
			clock_t begin = clock();   
			if(pidArray[i]%2 == 0){
				numbers = insertionSort(numbers,numberCounter);			
			}
			else{
				numbers = selectionSort(numbers,numberCounter);
			}
			clock_t end = clock();
			double timeSpent = (double)(end - begin) / CLOCKS_PER_SEC;
			timeSpent = timeSpent + randomNumberGenerate[i];
			sleep(randomNumberGenerate[i]);
			printf("\nExecuted in %lf seconds",timeSpent);
			
			//Creating intermidiate Files
			sprintf(addFileName,"additional%d.txt",i+1);
	  		FILE *addFile  = fopen(addFileName, "w");
	  		int k;
	  		for(k = 0; k<numberCounter; k++){
	  			fprintf(addFile,"%d-",numbers[k]);
	  		}
			fprintf(addFile,"\n%lf",timeSpent);
			
			if(signalIdentifier == 1)
				fprintf(addFile,"\nSIGUSR1-");
			
			else if(signalIdentifier == 2)
				fprintf(addFile,"\nSIGUSR2-");
				
			char buffer[26];
			
			//Here I convert time recieved into a format
			strftime(buffer, 26, "%H:%M:%S", receiveTime);
			fprintf(addFile,"%s",buffer);
			printf("\n%s is created!",addFileName);
			fclose(addFile);
			//free(numbers);
			
	  		exit(0);
		}
		else{ //Parent
			
			//Sending Signals
			if(pidArray[i]%2 == 0){
				kill(pidArray[i],SIGUSR1);
				sleep(1);
						
			}
			else{
			
				kill(pidArray[i],SIGUSR2);
				sleep(1);
			}
			wait(&childStatus);
			
			
			
		}	
	}
	
	for(int i=0;i<n;i++)
    		wait(NULL);
    		
    	
    	int lineCounter = 0;
    	struct value *values;
    	values = (struct value*)malloc(sizeof(struct value)*n); 
    	
    	char line[256];
    	for(int i=0;i<n;i++) {
		sprintf(readFileName,"additional%d.txt",i+1);
		getFile  = fopen(readFileName, "r");
		
		int lineCounter = 0;
		values[i].index = i;
		while(fgets(line, sizeof(line), getFile)){
			if(lineCounter == 0){
				sscanf(line, "%s", &values[i].numbers);
				lineCounter++; 
			}
			else if(lineCounter == 1){
				sscanf(line, "%lf", &values[i].executionTime);
				lineCounter++; 
			}
			else if(lineCounter == 2){
				sscanf(line, "%s", &values[i].signalTime);
				lineCounter++; 
			}
		}
	
		
    		}
    		
    		int z;
    		
		//Sorting struct value
    		qsort(values, n, sizeof(struct value), cmpfunc);
    		
		FILE *final= fopen("output.txt","w");
		for(z=0; z < n ; z++)
			fprintf(final,"%lf %s %s\n",values[z].executionTime,values[z].numbers,values[z].signalTime);
		
		fclose(final);
		
    		
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
  


