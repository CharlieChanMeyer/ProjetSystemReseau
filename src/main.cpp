#include "main.hpp"

using namespace std;

/*  report function : 
 *  does a report of the monitor :
 *  report :
 *      the number of calculator threads;
 *      the total partiel sum of all the calculator;
 *      + does a report of each calculator ( thread name + thread partial sum + thread time of execution) 
 * */
void static report(clc monitor){
    system("clear");
    /* calculation of the total sum of the thread's partial sums */
    int sumTot= 0;
    for (size_t i = 0; i < NB_CALCULATOR ; i++) {
        sumTot += monitor.sum[i];
    }
    /* display report : number of calculator threads + total sum  */
    cout << "Nombre de processus : " << NB_CALCULATOR << "\nSomme totale : " << sumTot << endl;
    /* display of each thread report */
    cout << "\n         Nom    | Somme partielle | Temps d'execution " << endl;
    cout << "-------------------------------------------------------" << endl;
    for (size_t i = 0; i < NB_CALCULATOR; i++) {
        if (monitor.loop[i])
        {
            cout << "       Thread "<< i << " |        " << monitor.sum[i]  << "        |           "   << monitor.timeExec[i] << " s"   << endl;
        }
        else
        {
           cout << "       Thread "<< i << " a planter. Redemarrage en cours...   " << endl;
        }
        
    }
    cout << "-------------------------------------------------------" << endl;
    cout << "EvilMonkey va kill un thread dans " <<  monitor.timeBeforeKill << " s " << endl;
    cout << "-------------------------------------------------------" << endl;
    cout << "Entrez 1 pour actualiser manuellement" << endl;
    cout << "Entrez 0 pour quitter le programme" << endl;
}

/* partialSum function
 * does the partial sum of each calculator threads
 */
void *partialSum(void *arg) {

    /* Define and use local variables for convenience */
    int start, end, len ;
    long offset;
    int mysum, *x;
    offset = (long)arg;
     
    len = monitor.veclen;
    start = offset*len;
    end   = start + len;
    x = monitor.a;
    mysum = 0;
    time_t startTimeReport = time(0);
    time_t startTimeSum = time(0);
    int indexSum = start;
    while( monitor.loop[offset] ) {
	    time_t currentTimeReport = time(0) - startTimeReport;
        time_t currentTimeSum = time(0) - startTimeSum;

		if( ((int) currentTimeReport) >= 2 ){   
            /*
            Lock a mutex prior to updating the value in the shared
            structure, and unlock it upon updating.
            */
            pthread_mutex_lock (&mutexsum);
            monitor.sum[offset] = mysum;
            pthread_mutex_unlock (&mutexsum);
            startTimeReport = time(0);
		}

        if( ((int) currentTimeSum) >= 1 ) {   
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

void *evilMonkey(void *arg) { 
    long offset = (long)arg;
    time_t startEvilMonkey = time(0);
    time_t startEvilMonkeyReport = time(0);
    time_t currentTime,currentTimeReport;
    int timeToKill = rand() % 100 + 1;
    int threadToKill;
    while( monitor.loop[offset] ) {
	    currentTime = time(0) - startEvilMonkey;
        currentTimeReport = time(0) - startEvilMonkeyReport;
        if (((int) currentTimeReport) >= 2)
        {
            pthread_mutex_lock (&mutexsum);
            monitor.timeBeforeKill = timeToKill - (int) currentTime;
            pthread_mutex_unlock (&mutexsum);
            startEvilMonkeyReport = time(0);
        }
        
		if( ((int) currentTime) >= timeToKill ){   
            /*
            Lock a mutex prior to updating the value in the shared
            structure, and unlock it upon updating.
            */
            threadToKill = rand() % NB_CALCULATOR;
            pthread_mutex_lock (&mutexsum);
            monitor.loop[threadToKill] = false;
            pthread_mutex_unlock (&mutexsum);
            timeToKill = rand() % 100 + 1;
            startEvilMonkey = time(0);
		}

		//Run other code here while not updating.
    }
    pthread_exit((void*) 0);
}

/* main function 
 *
 */
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
    for (i = 0; i < NB_CALCULATOR; i++){
        monitor.sum[i]=0;
        monitor.loop[i]=true;
        monitor.timeStart[i] = time(0);
    }
    monitor.loop[NB_CALCULATOR]=true;

    pthread_mutex_init(&mutexsum, NULL);
            
    /* Create threads to perform the partialSum  */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(i=0;i<NB_CALCULATOR;i++){
        /* Each thread works on a different set of data.
        * The offset is specified by 'i'. The size of
        * the data for each thread is indicated by NB_LISTE.
        */
        pthread_create(&callThd[i], &attr, partialSum, (void *)i); 
    }
    pthread_create(&callThd[NB_CALCULATOR], NULL , evilMonkey, (void *) i); 

    char name[20];
	time_t startTime = time(0);

    void **retval;
    report(monitor);
    bool monitorLoop = true;
	while( monitorLoop ) {
		
        time_t currentTime = time(0) - startTime;
        
		if( ((int) currentTime) >= 3 ) {
            
            for (i = 0; i < NB_CALCULATOR; i++){
                try
                {
                    int exist = pthread_tryjoin_np(callThd[i], retval);
                    if (exist == 16) {
                        monitor.timeExec[i] = (int) (time(0) - monitor.timeStart[i]);
                    } else {
                        throw i;
                    }
                    
                }
                catch(long i)
                {
                    monitor.timeExec[i] = 0;
                    monitor.timeStart[i] = time(0);
                    monitor.sum[i] = 0;
                    monitor.loop[i] = true;
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
                if (name[0] == '1') {
                    for (i = 0; i < NB_CALCULATOR; i++){
                        try
                        {
                            int exist = pthread_tryjoin_np(callThd[i], retval);
                            if (exist == 16)
                            {
                                monitor.timeExec[i] = (int) (time(0) - monitor.timeStart[i]);
                            } else {
                                throw i;
                            }
                            
                        }
                        catch(long i)
                        {
                            monitor.timeExec[i] = 0;
                            monitor.loop[i] = true;
                            monitor.sum[i] = 0;
                            monitor.timeStart[i] = time(0);
                            pthread_create(&callThd[i], &attr, partialSum, (void *)i);
                        }
                    }
                    report(monitor);
                    timeout.tv_usec = 0;
                } else if (name[0] == '0') {
                    for (i = 0; i < NB_CALCULATOR; i++)
                    {
                        monitor.loop[i] = false;
                    }
                    monitorLoop = false;
                    monitor.loop[NB_CALCULATOR] = false;
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

