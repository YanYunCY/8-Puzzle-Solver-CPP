#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "PuzzleState.h"
#include "Solver.h"

using namespace std;

vector<int> parseState(const string& s) {
    vector<int> state;
    stringstream ss(s);
    string token;
    while (getline(ss, token, ',')) {
        state.push_back(stoi(token));
    }
    return state;
}

void printPathAsJSON(const vector<PuzzleState>& path, int nodesExpanded, double timeUsed) {
    cout << "{\n";
    cout << "  \"success\": true,\n";
    cout << "  \"steps\": " << (path.size() - 1) << ",\n";
    cout << "  \"nodesExpanded\": " << nodesExpanded << ",\n";
    cout << "  \"timeUsed\": " << timeUsed << ",\n";
    cout << "  \"path\": [\n";
    for (size_t i = 0; i < path.size(); i++) {
        cout << "    [";
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                cout << path[i].getBoard(row, col);
                if (row * 3 + col < 8) cout << ",";
            }
        }
        cout << "]";
        if (i < path.size() - 1) cout << ",";
        cout << "\n";
    }
    cout << "  ]\n";
    cout << "}\n";
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <initial_state> <goal_state> <algorithm>\n";
        return 1;
    }

    vector<int> initial = parseState(argv[1]);
    vector<int> goal = parseState(argv[2]);
    string algorithm = argv[3];

    PuzzleState::setGoalState(goal);
    PuzzleState start(initial);

    if (!start.isSolvable()) {
        cout << "{ \"success\": false, \"error\": \"无解\" }\n";
        return 0;
    }

    SolveResult result;
    if (algorithm == "bfs") {
        BFSSolver solver(start);
        result = solver.solve();
    } else if (algorithm == "ida") {
        IDAStarSolver solver(start);
        result = solver.solve();
    } else {
        AStarSolver solver(start);
        result = solver.solve();
    }

    if (result.success) {
        printPathAsJSON(result.path, result.nodesExpanded, result.timeUsed);
    } else {
        cout << "{ \"success\": false, \"error\": \"求解失败\" }\n";
    }
    return 0;
}