#include "mpi.h"
#include <iostream>
#include <stack>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <unistd.h>


#define LENGTH 100

#define CHECK_MSG_AMOUNT  100

#define MSG_WORK_REQUEST 1000
#define MSG_WORK_SENT    1001
#define MSG_WORK_NOWORK  1002
#define MSG_TOKEN        1003
#define MSG_FINISH       1004



using namespace std;
typedef unsigned int uint;




bool canColorVertex(int vertex, int numOfVertexes, int rank, int numOfProcessors){
  	if(numOfProcessors >= numOfVertexes){
		return rank == vertex;
	}else{
		return true ; //TODO
	}


}


bool isNotColored(int vertex, vector<int> vec) {
    for(uint i = 0; i < vec.size(); i++) {
        if (vec[i] == vertex) {
            return false;
        }
    }
    return true;
}

void printColoredVertexes(int vertex, vector<int> *colored, vector< vector<int> > graph) {
    size_t vSize = graph[vertex].size();
    int first = 1;
    colored->push_back(vertex);
    for(uint i = 0; i < vSize; i++) {
        if (graph[vertex][i] == 1) {

            if (isNotColored(i, *colored)){
                //cout << i;
                colored->push_back(i);
                if(first == 1){
                    cout << i;
                    first = 0;
                }else{
                    cout << ", " << i;
                }

            }
        }
    }
}


void printVectorSimple(vector<int> vec) {
    for(uint i = 0; i < vec.size(); i++) {
        cout << "--- Krok " << (i+1) << ": " << "Hrac " << ((i%2) ? 2 : 1)  << ": Oznaceny uzel: " << vec[i];
        cout << "---------------------" << endl;
    }
    cout << endl;
}



void printVector(vector<int> vec, vector<int> *colored, vector< vector<int> > graph) {
    for(uint i = 0; i < vec.size(); i++) {
        cout << "Krok " << (i+1) << ": " << "Hrac " << ((i%2) ? 2 : 1)  << ": Oznaceny uzel: " << vec[i];
        cout << " Automaticky obarvene: (";
        printColoredVertexes(vec[i], colored, graph);
        cout << ")" << endl;
        cout << "---------------------" << endl;
    }
    cout << endl;
}


void colorVertexes(int vertex, vector<int> *colored, vector< vector<int> > graph) {
    size_t vSize = graph[vertex].size();

    colored->push_back(vertex);
    for(uint i = 0; i < vSize; i++) {
        if (graph[vertex][i] == 1) {
            if (isNotColored(i, *colored)) colored->push_back(i);
        }
    }
}





bool isAllColored(uint sizeOfGraph, vector<int> colored) {
    return (sizeOfGraph == colored.size() - 1) ? true : false;
}

void getRidofMinusOne(vector<int> *vec) {
    if (vec->at(0) == -1) vec->erase(vec->begin());
}

void printGraph(vector< vector<int> > *graph, int numOfVertexes) {
    cout << numOfVertexes << endl;
    for(int i = 0; i < numOfVertexes; i++) {
        for(int j = 0; j < numOfVertexes; j++) {
            cout << (graph->at(i)).at(j) << " ";
        }
        cout << endl;
    }
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

int main(int argc, char **argv) {
    //Declaration of needed variables
 int my_rank;
  int p;

    ifstream matrixFile;
    string temp;
    int numOfVertexes;
    stack< vector<int> > myStack;
int tag=1;
    vector<int> colored;
    vector<int> coloredBestSolution;
    vector<int> solution;
    vector<int> currentColored;
    vector<int> currentSolution;
    vector<int> bestSolution;
    bool firstSolution = true;
    bool firstRun = true;
    int sumOfOnes = 0;
MPI_Status status;
int counter = 0;

int aaa = 0;

/* start up MPI */
  MPI_Init( &argc, &argv );

  /* find out process rank */
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  /* find out number of processes */
  MPI_Comm_size(MPI_COMM_WORLD, &p);




if (my_rank == 0) {

    //Open .txt file
    matrixFile.open("graf5_12u.txt");
    getline(matrixFile,temp);
    numOfVertexes = strtol(temp.c_str(),NULL,10);

    vector< vector<int> > graph(numOfVertexes, vector<int>(numOfVertexes));

    //Copy values from .txt to my GRAPH representation
    for(int i = 0; i < numOfVertexes; i++) {
        getline(matrixFile,temp);
        for(int j = 0; j < numOfVertexes; j++) {
            graph[i][j] = (int) temp[j] - 48;
            if (graph[i][j] == 1) sumOfOnes++;
        }
    }


// sent number of nodes to all processors 
for (int dest=1;dest<p;dest++){
      MPI_Send(&numOfVertexes, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
}

// sent data to all processors 
for (int dest=1;dest<p;dest++){
	
	for(int j = 0; j < numOfVertexes; j++){
MPI_Send(&(graph[j]).front(), numOfVertexes, MPI_INT, dest, 1, MPI_COMM_WORLD);
	}

     
}





//-------------------------------------------------------------------------------

    printGraph(&graph, numOfVertexes);

//-------------------------------------------------------------------------------

    //WE push back value -1 for future testing of END OF STATE SPACE
    solution.push_back(-1);
    colored.push_back(-1);

    myStack.push(solution);
    myStack.push(colored);

    int tempN = 0;      //tempN just calculate number of PUSHES -- only for optimalization purposes

    //OUR lowerBound (maybe >D )

    double k = sumOfOnes / (double)numOfVertexes;
    uint lowerBound = numOfVertexes/(k+1);

//-------------------------------------------WHILE-----------------------------------------------------------------
    while (true) {

counter++;

if ((counter % CHECK_MSG_AMOUNT)==0)
  {
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
    if (flag)
    {
      //prisla zprava, je treba ji obslouzit
      //v promenne status je tag (status.MPI_TAG), cislo odesilatele (status.MPI_SOURCE)
      //a pripadne cislo chyby (status.MPI_ERROR)
      switch (status.MPI_TAG)
      {
         case MSG_WORK_REQUEST : // zadost o praci, prijmout a dopovedet
                                 // zaslat rozdeleny zasobnik a nebo odmitnuti MSG_WORK_NOWORK
                                 break;
         case MSG_WORK_SENT : // prisel rozdeleny zasobnik, prijmout
                              // deserializovat a spustit vypocet
                              break;
         case MSG_WORK_NOWORK : // odmitnuti zadosti o praci
                                // zkusit jiny proces
                                // a nebo se prepnout do pasivniho stavu a cekat na token
                                break;
         case MSG_TOKEN : //ukoncovaci token, prijmout a nasledne preposlat
                          // - bily nebo cerny v zavislosti na stavu procesu
                          break;
         case MSG_FINISH : //konec vypoctu - proces 0 pomoci tokenu zjistil, ze jiz nikdo nema praci
                           //a rozeslal zpravu ukoncujici vypocet
                           //mam-li reseni, odeslu procesu 0
                           //nasledne ukoncim spoji cinnost
                           //jestlize se meri cas, nezapomen zavolat koncovou barieru MPI_Barrier (MPI_COMM_WORLD)
                           MPI_Finalize();
                           exit (0);
                           break;
         default : chyba("neznamy typ zpravy"); break;
      }
    }
  }





        //POP LAST STACK ITEM -- there are two of them (solution and colored)
        //WE wanna keep -1 so if it is a first run of loop then we do not pop anything
        if (!firstRun) {
            colored = myStack.top();
            myStack.pop();

            solution = myStack.top();
            myStack.pop();
        }


        // STOP of WHILE, when we searched whole state space
        if ((solution.back() == -1) && (!firstRun)) break;
        

        //IF we have better solution already, then we cut that branch
        if ((solution.size() >= bestSolution.size()) && (!firstSolution)) continue;
        /*NOT SURE*/    if ((int) solution.size() > numOfVertexes - k) continue;      //Upper bound for solution

        //IF we already colored everything then if its First player who drawed, then we check this solution
        //for improvement of solution that we already have and if its better then we declared this solution as best.
        if (isAllColored(numOfVertexes,colored)) {
            if (solution.size() % 2 == 0) {
                if (firstSolution) {
                    bestSolution = solution;
                    firstSolution = false;
                    if (bestSolution.size() <= lowerBound) break;
                            //Lower bound for solution

                } else if (solution.size() < bestSolution.size()) {
                    bestSolution = solution;
                      if (bestSolution.size() <= lowerBound) break;        //Lower bound for solution

                }
            }
            continue;
        }

        currentColored = colored;
        currentSolution = solution;

        //This part is pushing vertexes to stack. (if they are not colored already)
        for(int i = 0; i < numOfVertexes; i++) {
            if (isNotColored(i,colored)) {
		if(!firstRun || canColorVertex(i, my_rank, p)){
			aaa++;
		        currentSolution.push_back(i);
		        colorVertexes(i,&currentColored,graph);


		        myStack.push(currentSolution);
		        myStack.push(currentColored);
		        tempN++;

		        currentColored = colored;
		        currentSolution = solution;
		}
            }
        }


// var firstRUn is usefull in for cycle above
if (firstRun) firstRun = false;


    }
//----------------------------------------------END OF WHILE--------------------------------------

    if (bestSolution.size() > 0) {
        getRidofMinusOne(&bestSolution);
        cout << endl;
        cout << "Best solution:" << endl;
        cout << "---------------------" << endl;
        cout << "---------------------" << endl;
        printVector(bestSolution, &coloredBestSolution, graph);
        cout << endl;
        cout << tempN << endl;
    } else {
        cout << "NO SOLUTUION" << endl;
    }

   
}


if(my_rank != 0){

cout << "proces: " << my_rank << endl;

vector<int> tmp;
int numOfVertexes;


cout << my_rank << " - cekam na velikost dat " << endl;
MPI_Recv(&numOfVertexes, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
   
cout << my_rank << " - prisla velikost: " << numOfVertexes << endl;

vector< vector<int> > graph(numOfVertexes, vector<int>(numOfVertexes));


tmp.resize(numOfVertexes);

for(int i = 0; i < numOfVertexes; i++){
  MPI_Recv(&tmp.front(), numOfVertexes, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

  for(int k = 0; k <  numOfVertexes; k++){
	graph[i][k] = tmp[k];
  }
  
}


 sleep(my_rank);
cout << my_rank << " dosla data" << endl;
    printGraph(&graph, numOfVertexes);



   /*
    sleep(2);
cout << "1 - budicek :))" << endl;
*/
}





MPI_Finalize();

 return 0;


}
