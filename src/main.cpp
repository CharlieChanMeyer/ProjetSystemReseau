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
    /* display of evil monkney time. (display second before evil monkey take action*/
    cout << "-------------------------------------------------------" << endl;
    cout << "EvilMonkey va kill un thread dans " <<  monitor.timeBeforeKill << " s " << endl;
    cout << "-------------------------------------------------------" << endl;
    /* display of command for user */
    cout << "Entrez 1 pour actualiser manuellement" << endl;
    cout << "Entrez 0 pour quitter le programme" << endl;
}

/* partialSum function/thread
 * does the partial sum and the report of each calculator threads
 */
void *partialSum(void *arg) {

    /* Define and use local variables for convenience */
    int start, end, len ;
    /* index of thread */
    long offset;
    /* sum partial of eaxh thread and list of number*/
    int mysum, *x;
    offset = (long)arg;
    /* len : size of the list of number*/
    len = monitor.veclen;
    /* start : first index to sum up for each thread*/
    start = offset*len;
    /* end : last index to sum up for each thread*/
    end   = start + len;
    /* local variable x for the list if number a*/
    x = monitor.a;
    /* initialisation of sumpartial of each thread*/
    mysum = 0;
    /* initialisation of currentTimeReport and currentTimeSum for updating sum and report of calculator thread */
    time_t startTimeReport = time(0);
    time_t startTimeSum = time(0);
    /* index in the list of number*/
    int indexSum = start;

    /* loop for each calculator thread (offset represent the ième thread)*/
    while( monitor.loop[offset] ) {

        /* currentTimeReport and currentTimeSum for updating sum and report of calculator thread */
	    time_t currentTimeReport = time(0) - startTimeReport;
        time_t currentTimeSum = time(0) - startTimeSum;

        /* report from calculator to monitor every 2 seconds*/
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
        /* partial sum realized every 1 second*/
        if( ((int) currentTimeSum) >= 1 ) {
            /* does the sum until all list of number has been sum up */   
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

/* evil monkey thread
 * creation of thread evil monkey
 * does a report every 2 second to monitor
 * kill a random thread every x second (x random)
 * */
void *evilMonkey(void *arg) { 
    /* index of thread */
    long offset = (long)arg;
    /* initialisation of startEvilMonkey and startEvilMonkeyReport for report of evil monkey thread */
    time_t startEvilMonkey = time(0);
    time_t startEvilMonkeyReport = time(0);
    time_t currentTime,currentTimeReport;
    /* initialisation timeToKill random between 1 and 100 seconds*/
    int timeToKill = rand() % 100 + 1;
    /* threadToKill : calculator thread that will "break down"*/
    int threadToKill;
    /* loop for evil monkey thread (while evil monkey works) */
    while( monitor.loop[offset] ) {
        /* currentTime for kill of evil monkey thread every 'time to kill' second */
	    currentTime = time(0) - startEvilMonkey;
        /* currentTimeReport for report of evil monkey thread every 2 second */
        currentTimeReport = time(0) - startEvilMonkeyReport;
        /* report from calculator to monitor every 2 seconds*/
        if (((int) currentTimeReport) >= 2) {
            /*
            Lock a mutex prior to updating the value in the shared
            structure, and unlock it upon updating.
            */
            pthread_mutex_lock (&mutexsum);
            /* update of time to kill to monitor*/
            monitor.timeBeforeKill = timeToKill - (int) currentTime;
            pthread_mutex_unlock (&mutexsum);
            startEvilMonkeyReport = time(0);
        }
        /* random calculator thread kill every 'timeToKill' second*/
		if( ((int) currentTime) >= timeToKill ){   
            /*
            Lock a mutex prior to updating the value in the shared
            structure, and unlock it upon updating.
            */
            /* selection random calculator thread to kill*/
            threadToKill = rand() % NB_CALCULATOR;
            pthread_mutex_lock (&mutexsum);
            /* killing of thread to kill by stopping its loop */
            monitor.loop[threadToKill] = false;
            pthread_mutex_unlock (&mutexsum);
            /* reset of random time to kill*/
            timeToKill = rand() % 100 + 1;
            startEvilMonkey = time(0);
		}

		//Run other code here while not updating.
    }
    /* end pthread evil monkey*/
    pthread_exit((void*) 0);
}

/* main function 
 * start the program
 * create list of number a
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
    /* initialize list of number to sum up*/
    for (i=0; i<NB_LISTE*NB_CALCULATOR; i++) {
        a[i]=i;
    }
    /* initialize in monitor vector(list) lenght and list of number */
    monitor.veclen = NB_LISTE; 
    monitor.a = a; 

    /* setting up monitor*/
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
    /* creation of calculator threads*/
    for(i=0;i<NB_CALCULATOR;i++){
        /* Each thread works on a different set of data.
        * The offset is specified by 'i'. The size of
        * the data for each thread is indicated by NB_LISTE.
        */
        pthread_create(&callThd[i], &attr, partialSum, (void *)i); 
    }
    /* creation evil monley thread */
    pthread_create(&callThd[NB_CALCULATOR], NULL , evilMonkey, (void *) i); 
    /* name : terminal input */
    char name[20];
	time_t startTime = time(0);

    void **retval;
    /* does monitor report (report of total partial sum and calculator thread [+evil monkey thread too])  */
    report(monitor);
    /* monitorLoop for report every 3second and request from terminal input*/
    bool monitorLoop = true;
    /* monitor loop*/
	while( monitorLoop ) {
        /* currentTime for kill of evil monkey thread every 'time to kill' second */
        time_t currentTime = time(0) - startTime;
        /* monitor report every 3 seconds*/
		if( ((int) currentTime) >= 3 ) {
            /* verification that all calculator thread are running*/
            for (i = 0; i < NB_CALCULATOR; i++){
                try
                {
                    int exist = pthread_tryjoin_np(callThd[i], retval);
                    /* if thread still running update timeExec thread*/
                    if (exist == 16) { 
                        monitor.timeExec[i] = (int) (time(0) - monitor.timeStart[i]);
                    } else {
                        throw i;
                    }
                    
                }
                /* initialization of new calculator thread if thread is not running anymore*/
                catch(long i)
                {
                    monitor.timeExec[i] = 0;
                    monitor.timeStart[i] = time(0);
                    monitor.sum[i] = 0;
                    monitor.loop[i] = true;
                    pthread_create(&callThd[i], &attr, partialSum, (void *)i);
                }
            }
            /* report every 3 second */
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
                /* if input '1' in terminal, does a monitor report*/
                if (name[0] == '1') {
                    /* verification that all calculator thread are running*/
                    for (i = 0; i < NB_CALCULATOR; i++){
                        try
                        {
                            int exist = pthread_tryjoin_np(callThd[i], retval);
                            /* if thread still running update timeExec thread*/
                            if (exist == 16) {
                                monitor.timeExec[i] = (int) (time(0) - monitor.timeStart[i]);
                            } else {
                                throw i;
                            }
                            
                        }
                        /* initialization of new calculator thread if thread is not running anymore*/
                        catch(long i)
                        {
                            monitor.timeExec[i] = 0;
                            monitor.loop[i] = true;
                            monitor.sum[i] = 0;
                            monitor.timeStart[i] = time(0);
                            pthread_create(&callThd[i], &attr, partialSum, (void *)i);
                        }
                    }
                    /* monitor report from input in terminal */
                    report(monitor);
                    timeout.tv_usec = 0;
                /*  if input '0' in terminal : */ 
                } else if (name[0] == '0') {
                    /*stop all threads by stopping every loop thread */
                    for (i = 0; i < NB_CALCULATOR; i++){
                        /* stop calculator loop (thread) */
                        monitor.loop[i] = false;
                    }
                    /* stop monitor loop */
                    monitorLoop = false;
                    /* stop evil monkey loop (thread) */
                    monitor.loop[NB_CALCULATOR] = false;
                }
            }
        } else {
            timeout.tv_usec = 0;    // 0 milliseconds
        }


	}
    /* end of program */
    pthread_attr_destroy(&attr);
    free (a);
    pthread_mutex_destroy(&mutexsum);
    pthread_exit(NULL);
}   

