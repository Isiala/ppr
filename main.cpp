#include <iostream>
#include <stack>
#include <fstream>
#include <stdlib.h>
#include <vector>

using namespace std;
typedef unsigned int uint;

void printVector(vector<int> vec) {
    for(uint i = 0; i < vec.size(); i++) {
        cout << vec[i] << " ";
    }
    cout << endl;
}

bool isNotColored(int vertex, vector<int> vec) {

    for(uint i = 0; i < vec.size(); i++) {
        if (vec[i] == vertex) {
            return false;
        }
    }
    return true;
}

void colorVertexes(int vertex, vector<int> *colored, vector< vector<int> > graph) {

    size_t vSize = graph[vertex].size();

    colored->push_back(vertex);
    for(uint i = 0; i < vSize; i++) {
        if (graph[vertex][i] == 1) {
            //cout << "IS 1 ... i=" << i << " vertex is " << vertex << endl;
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


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

int main() {
    //Declaration of needed variables
    ifstream matrixFile;
    string temp;
    int numOfVertexes;
    stack< vector<int> > myStack;

    vector<int> colored;
    vector<int> solution;
    vector<int> currentColored;
    vector<int> currentSolution;
    vector<int> bestSolution;
    bool firstSolution = true;
    bool firstRun = true;

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
        }
    }

//-------------------------------------------------------------------------------
    /*
        //PRINT number of Vertexes
        cout << numOfVertexes << endl;

        //PRINT WHOLE GRAPH
        for(int i = 0; i < numOfVertexes; i++) {
            for(int j = 0; j < numOfVertexes; j++) {
                cout << graph[i][j] << " ";
            }
            cout << endl;
        }
        //END OF PRINTING
    */
//-------------------------------------------------------------------------------

    //WE push back value -1 for future testing of END OF STATE SPACE
    solution.push_back(-1);
    colored.push_back(-1);

    myStack.push(solution);
    myStack.push(colored);

    while (true) {
        //POP LAST STACK ITEM -- there are two of them (solution and colored)
        //WE wanna keep -1 so if it is a furst run of loop then we do not pop anything
        if (!firstRun) {
            colored = myStack.top();
            myStack.pop();

            solution = myStack.top();
            myStack.pop();
        }

        // STOP of WHILE, when we searched whole state space
        if ((solution.back() == -1) && (!firstRun)) break;
        if (firstRun) firstRun = false;


        //IF we already colored everything then if its First player who drawed, then we check this solution
        //for improvment of solution that we already have and if its better then we declared this solution as best.
        if (isAllColored(numOfVertexes,colored)) {
            if (solution.size() % 2 == 0) {
                //cout << "SOM V DOBREJ CASTI...NASIEL SOM VYHOVUJUCE RIESENIE" << endl;
                if (firstSolution) {
                    bestSolution = solution;
                    firstSolution = false;
                } else if (solution.size() < bestSolution.size()) {
                    bestSolution = solution;
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

                currentColored = colored;
                currentSolution = solution;
            }
        }
    }

    getRidofMinusOne(&bestSolution);
    printVector(bestSolution);

    return 0;
}
