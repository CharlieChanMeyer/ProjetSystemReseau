/* include librairies */
#include "main.hpp"

using namespace std;

void static report(clc monitor[]){
    system("clear");
    int sumTot= 0;
    for (size_t i = 0; i < NB_CALCULATOR ; i++) {
        sumTot += monitor[i].partialSum;
    }
    cout << "Nombre de processus : " << NB_CALCULATOR << "\nSomme totale : " << sumTot << endl;
    cout << "\n         Nom    | Somme partielle | Temps d'execution " << endl;
    for (size_t i = 0; i < NB_CALCULATOR; i++) {
        cout << "       "<<monitor[i].name << " |        " << monitor[i].partialSum  << "        |           " << monitor[i].timeExec << " s" << endl;
    }    
}



int main(void) {
    fd_set          input_set;
    struct timeval  timeout;
    int             ready_for_reading = 0;
    int             read_bytes = 0;

    int listNb[NB_LISTE]; 
    for (size_t i = 1; i <= NB_LISTE; i++){
        listNb[i-1] = i;
    }
    
    for (int i = 0; i < NB_CALCULATOR; i++){
        monitor[i].name = "thread_" +to_string(i);
        monitor[i].partialSum = 0;
        monitor[i].timeExec = 0;
        cout << monitor[i].name << endl;
        for (size_t j = 0; j < NB_LISTE/NB_CALCULATOR; j++) {
            monitor[i].list[j] = listNb[j+i*NB_LISTE/NB_CALCULATOR];
            cout << monitor[i].list[j] << endl;
        }
    }

    pthread_t thread;
    for ( int i = 0; i < NB_CALCULATOR; i++ ) {
        int *arg = (int *) malloc(sizeof(*arg));
        if ( arg == NULL ) {
            fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
            exit(EXIT_FAILURE);
        }

        *arg = i;
        cout << arg << endl;
        pthread_create(&thread, 0, partialSum, arg);
    }


    char name[20];
	time_t startTime = time(0);
    time_t startCalculators = time(0);
    report(monitor);
	while( true )
	{
		time_t currentTime = time(0) - startTime;

		if( ((int) currentTime) >= 3 )
		{   
            for (int i = 0; i < NB_CALCULATOR; i++){
                monitor[i].timeExec = (int) (time(0) - startCalculators);
            }
			report(monitor);
            cout << "3 sec auto" << endl;
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
                        monitor[i].timeExec = (int) (time(0) - startCalculators);
                    }
                    report(monitor);
                    cout << "COUCOU C'EST MOI" << endl;
                    timeout.tv_usec = 0;
                }
            }
        } else {
            cout << "3 secs read" << endl;
            timeout.tv_usec = 0;    // 0 milliseconds
        }


	}


	return 0;

    
}
