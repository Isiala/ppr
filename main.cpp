#include <iostream>
#include <stack>
#include <fstream>
#include <stdlib.h>
#include <vector>

using namespace std;
typedef unsigned int uint;


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

int main() {
    //Declaration of needed variables
    ifstream matrixFile;
    string temp;
    int numOfVertexes;
    stack< vector<int> > myStack;

    vector<int> colored;
    vector<int> coloredBestSolution;
    vector<int> solution;
    vector<int> currentColored;
    vector<int> currentSolution;
    vector<int> bestSolution;
    bool firstSolution = true;
    bool firstRun = true;
    int sumOfOnes = 0;

    //Open .txt file
    matrixFile.open("graf.txt");
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
    int k = sumOfOnes / numOfVertexes;
    uint lowerBound = numOfVertexes/(k+1);

//-------------------------------------------WHILE-----------------------------------------------------------------
    while (true) {

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
        if (firstRun) firstRun = false;

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
                    /*NOT SURE*/        if (bestSolution.size() <= lowerBound) break;        //Lower bound for solution

                } else if (solution.size() < bestSolution.size()) {
                    bestSolution = solution;
                    /*NOT SURE*/        if (bestSolution.size() <= lowerBound) break;        //Lower bound for solution

                }
            }
            continue;
        }

        currentColored = colored;
        currentSolution = solution;

        //This part is pushing vertexes to stack. (if they are not colored already)
        for(int i = 0; i < numOfVertexes; i++) {
            if (isNotColored(i,colored)) {
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
getchar();
    return 0;
}
