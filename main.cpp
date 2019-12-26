#include <iostream>
#include "vector.hpp"
#include <map>
#include <stack>
#include "figures.hpp"
#include <algorithm>

enum { 
    ERR, ADD,
    PRINT, DEL,
    REC, EXIT,
    UNDO, SAVE,
    LOAD,
    CENTR, AREA,
    LES_AREA,
    TRAP, RHOMB,
    SIZE, HELP
};

template <class T>
struct Action {
    int type; // 0 is adding, 1 is delliting
    int position;
    Figure<T>* store;
    Action(int in, int pos, Figure<T>* inPo): type(in), position(pos), store(inPo){}
};

template <class T>
int Save(TVector<Figure<T>*>& toS, const char* path) {
    FILE* stor = fopen(path, "wb");
    if (!stor) {
        return 0;
    }
    int s = toS.size();
    fwrite(&s, sizeof(int), 1, stor);
    
    for (int i = 0; i < s; i++) {
        fwrite(toS[i], sizeof(Figure<T>), 1, stor);
    }

    fclose(stor);
    return 1;
}

template <class T>
int Load(TVector<Figure<T>*>& vec, std::stack<Action<T>>& undo, const char* path) {
    FILE* stor = fopen(path, "rb");
    if (!stor) {
        return 0;
    }
    for (int i = vec.size() - 1; i >= 0; i--) {
        delete vec[i];
        vec.erase(vec.begin() + i);
    }
    while(!undo.empty()) {
        if (undo.top().type == 1) {
            delete undo.top().store;
        }
        undo.pop();
    }
    int s;
    fread(&s, sizeof(int), 1, stor);

    for (int i = 0; i < s; i++) {
        vec.push_back((Figure<T>*)malloc(sizeof(Figure<T>)));
        fread(vec[i], sizeof(Figure<T>), 1, stor);
    }

    fclose(stor);
    return 1;
}



void help() {
    std::cout << "\nCommands: save, load, add, del, print, area, size, quit, help, centr, less_then and undo\n";
    std::cout << "Supported Figures: rectangle, trap, rhombus\n\n";
    std::cout << "For rectangle: 2 diagonal points \n";
    std::cout << "For trap: 2 points of founding of a trap, angle and a length of a side\n";
    std::cout << "For rhombus: 2 diagonal points and a length if a side \n";
    std::cout << "Figure id can be number or target \"all\"\n\n";

}

int main() {
    using Point = PairWIO<int,int>;
    using T = int;
    Point tmpP1, tmpP2;
    std::string comId, figType;
    //Rectangle* tmpV;
    int id;
    int area_key;
    double length, angle, overallArea;
    int status = 1;
    TVector<Figure<T>*> vec;
    std::stack<Action<T>> undo;
    std::map <std::string, int> command;

    command["add"] = ADD;       command["h"] = HELP;
    command["print"] = PRINT;   command["p"] = PRINT;
    command["del"] = DEL;       command["rec"] = REC;
    command["rectangle"] = REC; command["quit"] = EXIT;
    command["q"] = EXIT;        command["centr_of"] = CENTR;
    command["centr"] = CENTR;   command["area_of"] = AREA;
    command["area"] = AREA;     command["size_of"] = AREA;
    command["less_then"] = LES_AREA;    command["less"] = LES_AREA;
    command["size"] = SIZE;     command["trap"] = TRAP;
    command["rhomb"] = RHOMB;   command["undo"] = UNDO;
    command["u"] = UNDO;        command["help"] = HELP;
    command["save"] = SAVE;     command["load"] = LOAD;
    command["s"] = SAVE;        command["l"] = LOAD;
    



    help();
    while (status) {
        
        std::cout << "Enter command: ";
        std::cin >> comId;
        switch (command[comId]) {
            case ADD:
                std::cin >> figType;
                switch (command[figType]) {
                    case REC:
                        vec.push_back(Factory<T>::createFigure(Factory<T>::Rectangle));
                        undo.push(Action<T>(0, vec.size() - 1, vec[vec.size()]));
                        std::cout << "Rectangle added\n";
                        break;
                    case RHOMB:
                        vec.push_back(Factory<T>::createFigure(Factory<T>::Rhombus));
                        undo.push(Action<T>(0, vec.size() - 1, vec[vec.size()]));
                        std::cout << "Rhombus added\n";
                        break;
                    case TRAP:
                        vec.push_back(Factory<T>::createFigure(Factory<T>::Trap));
                        undo.push(Action<T>(0, vec.size() - 1, vec[vec.size()]));
                        std::cout << "Trap added\n";
                        break;
                    case ERR:
                        std::cout << "Unknown figure\n";
                        break;
                }
                break;
            case PRINT:
                std::cin >> comId;
                if (comId == "all") {
                    int counter = 0;
                    std::for_each(vec.begin(), vec.end(), [&counter](auto& k){
                        std::cout << counter++ << ": \n";
                        printCoor(*k);
                        putchar('\n');
                    });
                } else {
                    try {
                        id = std::stoi(comId);
                    } catch (std::invalid_argument) {
                        std::cout << "Invalid figure ID\n";
                        break;
                    }
                    if (id > ((int)vec.size() - 1) || id < 0) {
                        std::cout << "Invalid figure ID\n";
                        break;
                    }
                    printCoor(*(vec[id]));
                }
                break;
                
            case CENTR:
                if (!(std::cin >> id)) {
                    std::cout << "Invalid figure ID\n";
                    break;
                }
                if (id > ((int)vec.size() - 1) || id < 0) {
                    std::cout << "Invalid figure ID\n";
                    break;
                }
                std::cout << centr(*(vec[id])) << '\n';
                break;
            case AREA:
                std::cin >> comId;
                if (comId == "all") {
                    overallArea = 0;
                    for (int i = 0; i < vec.size(); i++) {
                        overallArea += area(*vec[i]);
                    }
                    std::cout << overallArea << '\n';
                } else {
                    try {
                        id = std::stoi(comId);
                    } catch (std::invalid_argument) {
                        std::cout << "Invalid figure ID\n";
                        break;
                    }
                    if (id > ((int)vec.size() - 1) || id < 0) {
                        std::cout << "Invalid figure ID\n";
                        break;
                    }
                    std::cout << area(*vec[id]) << '\n';
                }
                break;
            case LES_AREA:
                std::cin >> area_key;
                std::cout << std::count_if(vec.begin(), vec.end(), [area_key](auto& k) {
                    return area_key > area(*k);
                    }) << '\n';
                break;
            case DEL:
                if (!(std::cin >> id)) {
                    std::cout << "Invalid figure ID\n";
                    break;
                }
                if (id > ((int)vec.size() - 1) || id < 0) {
                    std::cout << "Invalid figure ID\n";
                    break;
                }
                undo.push(Action<T>(1, id, vec[id]));
                //delete vec[id];
                vec.erase(vec.begin() + id);
                std::cout << "Deleted\n";
                break;
            case SIZE:
                std::cout << vec.size() << "\n";
                break;
            case UNDO:
                if (undo.empty()) {
                    std::cout << "Nothing to undo\n";
                    break;
                }
                if (undo.top().type == 0) {
                    delete vec[vec.size() - 1];
                    vec.erase(vec.end() - 1);
                    undo.pop();
                } else if (undo.top().type == 1) {
                    vec.insert(vec.begin() + undo.top().position, undo.top().store);
                    undo.pop();
                }
                std::cout << "Done\n";
                break;
            case HELP:
                help();
                break;
            case ERR:
                std::cout << "Invalid command\n";
                break;
            case SAVE:
                std::cout << "to file:\n";
                std::cin >> comId;
                if(Save(vec, comId.c_str())) {
                    std::cout << "Done\n";
                } else {
                    std::cout << "Error\n";
                }
                break;
            case LOAD:
                std::cout << "from file:\n";
                std::cin >> comId;
                if(Load(vec, undo, comId.c_str())) {
                    std::cout << "Done\n";
                } else {
                    std::cout << "Error\n";
                }
                break;
            case EXIT:
                for (int i = 0; i < vec.size(); i++) {
                    delete vec[i];
                }
                while(!undo.empty()) {
                    if (undo.top().type == 1) {
                        delete undo.top().store;
                    }
                    undo.pop();
                }
                return 0;
                break;
        }
        while(getchar() != '\n');
        std::cin.clear();
    }
    return 0;
}
