//Kevin Gillmore OS P4
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

class Allocator{
private:
	vector<char*> freeSlabs;
	vector<char*> allocSlabs;
	vector<char*> gulps;

public:
	vector<char**> Allocate(int numSlabs){
		vector<char**> addresses;
		for (int j = 0; j < numSlabs;j++){
			//Check if gulp is ncessary VV
			if(freeSlabs.size() == 0){
				cout << "Gulping" << endl;
				char *gulp;
				gulp = (char*) malloc(65536);
				gulps.push_back(gulp);
				int offset = 512;
				for(int i = 0;i < 128;i++){
					gulp += i*offset;
					freeSlabs.push_back(gulp);
				}
			}
			allocSlabs.push_back(freeSlabs[freeSlabs.size() -1]);
			addresses.push_back(&(freeSlabs[freeSlabs.size() -1]));
			freeSlabs.pop_back();
			cout << "Allocating Slab" << setw(5) << freeSlabs.size() << endl;
		}
		return addresses;
	}

	bool FreeSlab(int numSlabs, bool byAddress, char* ptr = nullptr){//Can either free a number of slabs or a single one by address
		if(!byAddress){
			for(int i = 0; i < numSlabs; i++){
				freeSlabs.push_back(allocSlabs[allocSlabs.size() -1]);
				allocSlabs.pop_back();
			}
			return true;
		}
		else{//Returns false on nullptr, free too many and freeing a nonexistent or already freed slab
			if(ptr == nullptr || allocSlabs.size() == 0){
				return false;
			}
			for(unsigned int i = 0; i < allocSlabs.size(); i++){//manages cases of nonexistent or already free by searching allocated
				if(ptr == allocSlabs[i]){
					freeSlabs.push_back(allocSlabs[i]);
					allocSlabs.erase(allocSlabs.begin() + i);
					return true;
				}
			}
		}
		return false;
	}	

	int GetSlabsAvailable(){
		return freeSlabs.size();
	}

	~Allocator(){
		for(unsigned int i = 0; i < gulps.size();i++){
			free(gulps[i]);
			cout << "Releasing Gulp" << endl;
		}
	}
};

int main(int argc, char * argv[]) {
	Allocator allocator;

	allocator.Allocate(256);

	//1. Tests if correct number of slabs were allocated
	if (allocator.GetSlabsAvailable() == 0){
		cout << "Number of Available Slabs should be 0. Is: " << allocator.GetSlabsAvailable() << " (Correct)" << endl;
	}
	else{
		cout << "Number of Available Slabs should be 0. Is: " << allocator.GetSlabsAvailable() << " (Wrong)" << endl;
		return 1;
	}

	cout << "Releasing all Allocated Buffers" << endl;
	allocator.FreeSlab(256, false);

	//2. Tests if correct amount of slabs were freed
	if (allocator.GetSlabsAvailable() == 256){
		cout << "Number of Available Slabs should be 256. Is: " << allocator.GetSlabsAvailable() << " (Correct)" << endl;
	}
	else{
		cout << "Number of Available Slabs should be 256. Is: " << allocator.GetSlabsAvailable() << " (Wrong)" << endl;
		return 1;
	}

	vector<char**> firstAlloc = allocator.Allocate(1);
	allocator.FreeSlab(1, false);
	vector<char**> secondAlloc = allocator.Allocate(1);
	allocator.FreeSlab(1, false);

	//3., 4., 5. Test if free addresses are equal
	if(firstAlloc[0] == secondAlloc[0]){
		cout << "Alloc / Free / Alloc Test succeeded" << endl;
	}
	else{
		cout << "Alloc / Free / Alloc Test failed" << endl;
		return 1;
	}
	
	//6.Test nullptr for free argument
	if(!allocator.FreeSlab(0, true, nullptr)){
		cout << "Correctly caught error: attempting to free nullptr" << endl;
	}
	else{
		cout << "Did Not Catch Attempt to Free NULL" << endl;
		return 1;
	}

	//7. Test too many frees
	if(!allocator.FreeSlab(0, true, *firstAlloc[0])){
		cout << "Correctly caught error: free'd too many slabs" << endl;
	}
	else{
		cout << "Did Not Catch Freeing of Too Many Buffers" << endl;
		return 1;
	}

	vector<char**> twoAllocs = allocator.Allocate(2);
	allocator.FreeSlab(0, true, *twoAllocs[1]);

	//8. Double free
	if(!allocator.FreeSlab(0, true, *twoAllocs[1])){
			cout << "Correctly caught error: attempting double free" << endl;
		}
		else{
			cout << "Did Not Catch Attempt to Double Free" << endl;
			return 1;
		}

	char *a = new char('a');

	//I'm aware I should put an address of 12345 here, however my method takes a pointer
	//Still, the pointer delcared above will not be within one of the gulps.  Test functions almost as intended
	//I hope this is okay

	//9. bad address check
	if(!allocator.FreeSlab(0, true, a)){
			cout << "Correctly caught error: attempting to free location not in any gulp" << endl;
		}
		else{
			cout << "Did Not Catch Attempt to Free BAD Address" << endl;
			return 1;
		}

	cout << "Program Ending - Destructors should now run" << endl;

	return 0;
}




