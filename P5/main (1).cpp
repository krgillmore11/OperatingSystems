//Kevin Gillmore OS P5
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>
#include "arch.hpp"
#include <stdint.h>
#include <string.h>

using namespace std;

int main(int argc, char * argv[]) {
	/*    Machine Architecture

    Pages are 2048 bytes long         --- 11 bits
    VA Space is 32 pages              ---  5 bits
    Virtual Addresses are therefore   --- 16 bits
    Physical memory can fit           ---  8 pages
    PFN in bits                       ---  3 bits
	*/

	const uint32_t PAGE_BITS    = 11;
	const uint32_t PAGE_SIZE    = (1 << PAGE_BITS);
	const uint32_t PFN_BITS     = 3;
	const uint32_t VPN_BITS     = 5;
	const uint32_t VRT_PAGES    = (1 << VPN_BITS);
	const uint32_t PHYS_PAGES   = (1 << PFN_BITS);
	const uint32_t PHYS_SIZE    = PHYS_PAGES * PAGE_SIZE;

	const uint32_t VA_SIZE_BITS = VPN_BITS + PAGE_BITS;
	const uint32_t VA_SIZE      = 1 << (VA_SIZE_BITS);

	struct PTE {//Page table entries, number after : specify bitfield size
		uint32_t dirty : 1;
		uint32_t referenced : 1;        // UNUSED
		uint32_t present : 1;
		uint32_t valid : 1;
		uint32_t rw : 1;                // UNUSED
		uint32_t pfn : PFN_BITS;//3
	};

	struct PMME{//Physical frames
	PMME() = default;
	bool in_use = 0;
	uint32_t VPN;
	uint32_t PFN;
	};
	PTE PT[VRT_PAGES]; //Virtual Page Table

	for(unsigned int i = 0; i < VRT_PAGES; i++){//Initialize used variables
		PT[i].dirty = 0;
		PT[i].present = 0;
		PT[i].valid = 1;
		PT[i].pfn = 0;
	}
	PMME MME[PHYS_PAGES]; //Physical Frame Table

	if (argc == 1 || strcmp(argv[1], "-f") != 0){//checks to see if -f was given
		cerr << "Must specify file name with -f" << endl;
		return 1;
	}

	string iPath = argv[2];
	ifstream iFile (iPath);

	if (!iFile.is_open()){//Check to see if file has been opened succesfully
		cerr << "Failed to open: " << argv[2] << endl;
		perror("Cause: ");
		return 1;  
	}

	cout << "Machine Architecture:" << endl;
	cout << setw(28) << left << "Page Size (bits):" << PAGE_BITS << endl;
	cout << setw(28) << left << "Page Size (bytes):" << PAGE_SIZE << endl;
	cout << setw(28) << left << "VA Size (bits):" << VA_SIZE_BITS << endl;
	cout << setw(28) << left << "VA Size (bytes):" << VA_SIZE << endl;
	cout << setw(28) << left << "Physical Memory (bytes):" << PHYS_SIZE << endl;
	cout << setw(28) << left << "Physical Pages:" << PHYS_PAGES << endl;


	uint32_t nextPage = 0;
	string line;
	while (getline(iFile, line)){//read in file line by line
		stringstream ss(line);
		getline(ss, line, ' ');//delimits for spaces (comments and args)
		if (line == "Read" || line == "Write"){
			cout << line;
			bool isWrite = false;//output for read and write is very simlar so we us this
			if (line == "Write"){
				isWrite = true;
			}
			getline(ss, line, '\t');//delimits by tabs because VV
			//getline(ss, line, ' ');//Well aparently tabs aren't the same as spaces according to getline char delimiters
			cout << " " << line << endl;
			uint32_t oldVPN;
			uint32_t page = stoi(line) / PAGE_SIZE;//gives virtual page number
			if (PT[page].present){
				cout << "VPN: " << page << " VA: " << line << " SUCCESSFUL TRANSLATION TO PFN: " << page;
			//VPN: 0 VA: 100 SUCCESSFUL TRANSLATION TO PFN----------
			}
			else{//Page fault if not present
				cout << "VPN: " << page << " VA: " << line << " PAGE FAULT" << endl;
				//VPN: 0 VA: 0 PAGE FAULT
				string outHldr = "PFN";//saved for later output between PFN or PHYSICAL FRAME
				string outHldrTwo = "PFN";//saved for later output between PFN or ""
				if (MME[nextPage].in_use){//swap time
					outHldrTwo = "";
					oldVPN = MME[nextPage].VPN;//The vpn of the entry chosen to be ejected from Physical ememory
					cout << "VPN: " << oldVPN << " SELECTED TO EJECT";
					//VPN: 0 SELECTED TO EJECT ---------
					if (PT[oldVPN].dirty){														
						cout << " DIRTY" << endl;
						//DIRTY
						cout << "VPN: " << oldVPN << " WRITING BACK";
						//VPN: 0 WRITING BACK -----------
					}
					cout << endl;
					outHldr = "PHYSICAL FRAME";
					PT[oldVPN].present = 0;//set ejected entry present bit to false
				}
				MME[nextPage].VPN = page;	//updating: frame present VPN
				MME[nextPage].in_use = 1;	//in_use bool to true
				PT[page].pfn = nextPage;	//entry pfn bits
				PT[page].dirty = 0;			//dirty bit to false
				PT[page].valid = 1;			//valid to true
				PT[page].present = 1;		//present to true
				cout << "VPN: " << page << " VA: " << line << " ASSIGNING TO " << outHldrTwo << ": " << nextPage << endl;
				//VPN: 0 VA: 0 ASSIGNING TO PFN: 0
				cout << "VPN: " << page << " VA: " << line << " SWAPPING IN TO " << outHldr << " : " << nextPage;
				//VPN: 0 VA: 0 SWAPPING IN TO -PfnStr- : 0 --------------
				nextPage++;
				nextPage = nextPage % PHYS_PAGES;//nextPage resets when it reaches PHYS_PAGES
			}
			if (isWrite){
				if (PT[page].dirty){
					cout << " REPEAT WRITE" << endl;
					//REPEAT WRITE	
				}
				else{
					cout << " NEWLY DIRTY" << endl;
					//NEWLY DIRTY
					PT[page].dirty = 1;//dirty now since it is a write
				}
			}
			else{
				cout << endl;//read case
			}
		}
		if (line == "DUMP_MMU"){
		cout << "MMU:" << endl;
		for(unsigned int i = 0; i < PHYS_PAGES;i++){
			cout << "[" << setw(3) << right << i;
			cout << "] USED VPN: " << setw(4) << right << MME[i].VPN << endl;
		}
		/*
		[  0] USED VPN:    7
		[  1] USED VPN:   23
		[  2] USED VPN:   16
		[  3] USED VPN:   28
		[  4] USED VPN:   22
		[  5] USED VPN:   13
		[  6] USED VPN:   15
		[  7] USED VPN:    9*/
		}
		if (line == "DUMP_PT"){
			cout << "PAGE TABLE:" << endl;
			for(unsigned int i = 0; i < VRT_PAGES;i++){
				if(PT[i].present){
					cout << "[" << setw(3) << right << i;
					cout << "] ";
					if(PT[i].dirty){
						cout << "DIRTY";
					}
					else{
						cout << "CLEAN";
					}
					cout << " PRES IN PFN: " << setw(4) << right << PT[i].pfn << endl;
				}
		}
			/*
			[  7] CLEAN PRES IN PFN:    0 
			[  9] CLEAN PRES IN PFN:    7 
			[ 13] CLEAN PRES IN PFN:    5 
			[ 15] DIRTY PRES IN PFN:    6 
			[ 16] CLEAN PRES IN PFN:    2 
			[ 22] CLEAN PRES IN PFN:    4 
			[ 23] CLEAN PRES IN PFN:    1 
			[ 28] CLEAN PRES IN PFN:    3 */
		}
	}
return 0;//THE END
}
