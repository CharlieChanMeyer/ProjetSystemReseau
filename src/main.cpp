#include "main.hpp"

using namespace std;

void static report(clc monitor){
    //system("clear");
    int sumTot= 0;
    for (size_t i = 0; i < NB_CALCULATOR ; i++) {
        sumTot += monitor.sum[i];
    }
    cout << "Nombre de processus : " << NB_CALCULATOR << "\nSomme totale : " << sumTot << endl;
    cout << "\n         Nom    | Somme partielle | Temps d'execution " << endl;
    for (size_t i = 0; i < NB_CALCULATOR; i++) {
        cout << "       Thread "<< i << " |        " << monitor.sum[i]  << "        |           "   << monitor.timeExec[i] << " s"   << endl;
    }    
}

void *partialSum(void *arg)
{

/* Define and use local variables for convenience */

   int start, end, len ;
   long offset;
   int mysum, *x;
   offset = (long)arg;
   bool loop = true;
     
   len = monitor.veclen;
   start = offset*len;
   end   = start + len;
   x = monitor.a;
   mysum = 0;
  time_t startTimeReport = time(0);
  time_t startTimeSum = time(0);
    int indexSum = start;
    while( loop )
	{
		time_t currentTimeReport = time(0) - startTimeReport;
        time_t currentTimeSum = time(0) - startTimeSum;

		if( ((int) currentTimeReport) >= 2 )
		{   
      /*
      Lock a mutex prior to updating the value in the shared
      structure, and unlock it upon updating.
      */
        pthread_mutex_lock (&mutexsum);
        monitor.sum[offset] = mysum;
        pthread_mutex_unlock (&mutexsum);
        startTimeReport = time(0);
        if (offset == 2)
        {
            loop = false;
        }
        
		}

        if( ((int) currentTimeSum) >= 1 )
		{   
            if (indexSum <= end) {
              mysum += x[indexSum];
              indexSum++;
            }
            startTimeSum = time(0);
		}

		//Run other code here while not updating.

    }
       pthread_exit((void*) 0);
}

int main (int argc, char *argv[]) {
    fd_set          input_set;
    struct timeval  timeout;
    int             ready_for_reading = 0;
    int             read_bytes = 0;
long i;
int *a;
pthread_attr_t attr;

/* Assign storage and initialize values */

a = (int*) malloc (NB_CALCULATOR*NB_LISTE*sizeof(int));
  
for (i=0; i<NB_LISTE*NB_CALCULATOR; i++) {
  a[i]=i;
  }

monitor.veclen = NB_LISTE; 
monitor.a = a; 
for (i = 0; i < NB_CALCULATOR; i++)
{
  monitor.sum[i]=0;
}


pthread_mutex_init(&mutexsum, NULL);
         
/* Create threads to perform the partialSum  */
pthread_attr_init(&attr);
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

for(i=0;i<NB_CALCULATOR;i++)
  {
  /* Each thread works on a different set of data.
   * The offset is specified by 'i'. The size of
   * the data for each thread is indicated by NB_LISTE.
   */
   pthread_create(&callThd[i], &attr, partialSum, (void *)i); 
   }
/* Wait on the other threads */

// for(i=0;i<NB_CALCULATOR;i++) {
//   pthread_join(callThd[i], &status);
//   }
/* After joining, print out the results and cleanup */

    char name[20];
	time_t startTime = time(0);
    time_t startCalculators = time(0);
    void **retval;
    bool kill = false;
    report(monitor);
	while( true )
	{
		time_t currentTime = time(0) - startTime;
        if (!kill)
        {
            kill = true;
            int test = pthread_cancel(callThd[2]);
            cout << callThd[2] << "kill result = " << test << endl;
        }
        
        
		if( ((int) currentTime) >= 3 )
		{   
            for (int i = 0; i < NB_CALCULATOR; i++){
                try
                {
                    int exist = pthread_tryjoin_np(callThd[i], retval);
                    if (exist == 16)
                    {
                        monitor.timeExec[i] = (int) (time(0) - startCalculators);
                    } else {
                        throw i;
                    }
                    
                }
                catch(int i)
                {
                    cout << "Thread " << i << " a plante" << endl;
                    monitor.timeExec[i] = 0;
                    pthread_create(&callThd[i], &attr, partialSum, (void *)i);
                }
            }
			report(monitor);
			//Reset the timer.
			startTime = time(0);
		}

		//Run other code here while not updating.
        /* Empty the FD Set */
        FD_ZERO(&input_set );
        /* Listen to the input descriptor */
        FD_SET(0, &input_set);

        /* Waiting for some seconds */
        timeout.tv_sec = 3;    // WAIT seconds
        timeout.tv_usec = 0;    // 0 milliseconds

        /* Listening for input stream for any activity */
        ready_for_reading = select(1, &input_set, NULL, NULL, &timeout);
        /* Here, first parameter is number of FDs in the set, 
        * second is our FD set for reading,
        * third is the FD set in which any write activity needs to updated,
        * which is not required in this case. 
        * Fourth is timeout
        */

        if (ready_for_reading == -1) {
            /* Some error has occured in input */
            printf("Unable to read your input\n");
            return -1;
        } 

        if (ready_for_reading) {
            read_bytes = read(0, name, 19);
            if(name[read_bytes-1]=='\n'){
            --read_bytes;
            name[read_bytes]='\0';
            }
            if(read_bytes==0){
                printf("You just hit enter\n");
                timeout.tv_usec = 0;
            } else {
                if (name[0] == '0') {
                    for (int i = 0; i < NB_CALCULATOR; i++){
                        try
                        {
                            int exist = pthread_tryjoin_np(callThd[i], retval);
                            if (exist == 16)
                            {
                                monitor.timeExec[i] = (int) (time(0) - startCalculators);
                            } else {
                                throw i;
                            }
                            
                        }
                        catch(int i)
                        {
                            //cout << "Thread " << i << " a plante" << endl;
                            monitor.timeExec[i] = 0;
                            pthread_create(&callThd[i], &attr, partialSum, (void *)i);
                        }
                    }
                    report(monitor);
                    timeout.tv_usec = 0;
                }
            }
        } else {
            timeout.tv_usec = 0;    // 0 milliseconds
        }


	}
pthread_attr_destroy(&attr);
free (a);
pthread_mutex_destroy(&mutexsum);
pthread_exit(NULL);
}   

