/* include librairies */
#include "main.hpp"

using namespace std;

void report(clc monitor[]){
    system("clear");
    int sumTot= 0;
    for (size_t i = 0; i < NB_CALCULATOR ; i++) {
        sumTot += monitor[i].partialSum;
    }
    cout << "Nombre de processus : " << NB_CALCULATOR << "\nSomme totale : " << sumTot << endl;
    cout << "\n         Nom    | Somme partielle | Temps d'execution " << endl;
    for (size_t i = 0; i < NB_CALCULATOR; i++) {
        cout << "       "<<monitor[i].name << " |        " << monitor[i].partialSum  << "        |           " << monitor[i].timeExec << endl;
    }    
}



int main(void) {
    clc monitor[NB_CALCULATOR];
    int listNb[NB_LISTE]; 
    for (size_t i = 1; i <= NB_LISTE; i++){
        listNb[i-1] = i;
    }
    
    for (int i = 0; i < NB_CALCULATOR; i++){
        monitor[i].name = "thread_" +to_string(i);
        monitor[i].partialSum = 0;
        monitor[i].timeExec = 0;
        for (size_t j = 0; j < NB_LISTE/NB_CALCULATOR; j++) {
            monitor[i].list[j] = listNb[j+i*NB_LISTE/NB_CALCULATOR];
        }
    }
    //report(monitor);
    cout << "Entrez 0 pour actualiser le rapport du moniteur : ";
    int val;
    cin >> val;

    do {
        report(monitor);
        cout << "Entrez 0 pour actualiser le rapport du moniteur : ";
        cin >> val;
        if (val == 0)
        {
            report(monitor);
        }
        
    } while (true); 

    
}
