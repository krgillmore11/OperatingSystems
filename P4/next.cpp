//Kevin Gillmore OS P4
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <fstream>
#include <sstream>


using namespace std;

class NextFit{
    private:
    struct block{
        int start = 0;
        int length;
    };
        int ramAmount;
        vector <block> free;
        vector <block> alloc;
    public:
            NextFit(int ram){
            unsigned int lastFit = 0;
            ramAmount = ram;
            block newBlock;
            newBlock.length = ramAmount;
            free.push_back(newBlock);
            string line;
            while (getline(cin, line)){//read in file line by line
                stringstream ss(line);
                getline(ss, line, ' ');//delimits for spaces (comments and args)

                if(line == "p"){
                    cout << endl;
                    if(free.size() == 0){
                        cout << "Free List is empty" << endl;
                    }
                    else{
                        cout << "Free List" << endl;
                        cout << " Index   Start  Length" << endl;
                        for(unsigned int i = 0; i < free.size();i++){
                            cout << "[" << setw(4) << setfill('0') << i << "]";
                            cout << setw(8) << setfill(' ') << free[i].start;
                            cout << setw(8) << free[i].length << endl;
                        }
                    }
                    cout << endl;
                    if(alloc.size() == 0){
                        cout << "Allocated List is empty" << endl;
                    }
                    else{
                        cout << "Allocated List" << endl;
                        cout << " Index   Start  Length" << endl;
                        for(unsigned int i = 0; i < alloc.size();i++){
                            cout << "[" << setw(4) << setfill('0') << i << "]";
                            cout << setw(8) << setfill(' ') << alloc[i].start;
                            cout << setw(8) << alloc[i].length << endl;
                        }
                    }
                }
                if(line == "a"){
                    getline(ss, line, ' ');
                    int size = stoi(line);
                    if(!(lastFit < free.size())){
                        lastFit = 0;
                    }
                    for(unsigned int i = lastFit; i < free.size();i++){
                        if (free[i].length >= size){
                            cout << "Next Fit Starting index: "<< lastFit << " Start: "
                            << free[lastFit].start <<" Length: " << free[lastFit].length << endl;
                            block newBlock;
                            newBlock.length = size;
                            newBlock.start = free[i].start;
                            alloc.push_back(newBlock);
                            if(size == free[i].length){
                                free.erase(free.begin() + i);
                            }
                            else{
                                free[i].start += size;
                                free[i].length -= size;
                            }
                            cout << "Next Fit Ending index: " << lastFit << " Start: "
                            << free[lastFit].start <<" Length: " << free[lastFit].length << endl;
                            cout << "Allocated: " << size << " kibibytes starting at: " << newBlock.start << endl;
                        }
                        break;
                    }

                }
                if(line == "f"){

                }
                if(line == "q"){

                }
            }
        }

};

int main(int argc, char * argv[]) {
    int size_of_ram = 512;

    int c;
    while((c = getopt(argc, argv, "k:h")) != -1){
        switch (c){
            case 'k':
                size_of_ram = stoi(optarg);
            break;

            case 'h':
                cout << "-h     prints this" << endl;
                cout << "-k     sets the maximal size of free memory" << endl;
            break;
        }
    }

    NextFit nf(size_of_ram);

    return 0;
}