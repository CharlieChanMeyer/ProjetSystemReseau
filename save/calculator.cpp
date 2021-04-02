#include "calculator.hpp"

void *partialSum(void *i) {
    time_t startTimeReport = time(0);
    int threadNumber = *((int *) i);
    cout << "Bonjour, je suis le thread " << threadNumber << endl;
    cout << monitor[threadNumber].name << " | " << monitor[threadNumber].timeExec << endl;
    
    time_t startTimeSum = time(0);
    int partialSum = 0;
    int indexSum = 0;
    while( true )
	{
		time_t currentTimeReport = time(0) - startTimeReport;
        time_t currentTimeSum = time(0) - startTimeSum;

		if( ((int) currentTimeReport) >= 2 )
		{   
            monitor[threadNumber].partialSum = partialSum;
            cout << "report thread "<< threadNumber << endl;
            startTimeReport = time(0);
		}

        if( ((int) currentTimeSum) >= 1 )
		{   
            cout << monitor[threadNumber].list[indexSum] << endl;
            partialSum = partialSum + monitor[threadNumber].list[indexSum];
            cout << "sum thread "<< threadNumber << " : " << partialSum << endl;
            indexSum++;
            startTimeSum = time(0);
		}

		//Run other code here while not updating.

    }
}
