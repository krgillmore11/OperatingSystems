//OS P3 - Kevin Gillmore
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>
#include <deque>

using namespace std;

struct job{//A struct for inoming processes
	string name = "";
	int q = 0;
};

void schedule(bool &idle,  vector<deque<job>> &queues, job &running);//assigns running and finds next job to be scheduled

int main(int argc, char * argv[]) {

const int NUM_QS = 4;//const number of qs
vector<deque<job>> queues(NUM_QS);//vector of queues 
vector<job> blockedJobs;//blocked jobs
string line = "";//Input file reader
bool isIdle = true;//Initialized as true at beggining
job running;//currently running job


string iPath = argv[1];
ifstream iFile (iPath);
if (argc <= 1){//checks to see if input file was given
	cerr << "Missing input file name.";
	return 1;
}
if (!iFile.is_open()){//Check to see if file has been opened succesfully
    cerr << "Input file failed to open.";  
	return 1;  
}

while (getline(iFile, line)){//read in file line by line

	stringstream ss(line);
	getline(ss, line, ',');//delimits for commas (comments and args)

	if(line.substr(0, 6) == "newjob"){//creates a job
		job newJob;
		getline(ss, line, ',');//Gets job name,  does not continue reading comments
		newJob.name = line;
		cout << "New job: " << newJob.name << " added." << endl;
		queues[0].push_back(newJob);//Adds job to queue
		if(isIdle){//If was Idle
			if(queues[0].size() > 0){
				schedule(isIdle, queues, running);//schedules newJob
			}
		}
	}

	if(line == "finish"){
		if(!isIdle){
			cout << "Job: " << running.name << " completed." << endl;
			running.name = "";
			schedule(isIdle, queues, running);//onto the next job
		}
		else{
			cout << "Error. System is idle." << endl;
		}
	}
	
	if(line == "interrupt"){//Running job has finished quantum
		if(!isIdle){
			if(running.q < NUM_QS-1){//-1 bc addition below
				running.q++;//lower priority by 1
			}
			queues[running.q].push_back(running);//back of the line for this guy
			schedule(isIdle, queues, running);//schedules next job in q
		}
		else{
			cout << "Error. System is idle." << endl;
		}
	}

	if(line == "block"){//Running job has finished quantum
		if(!isIdle){
			blockedJobs.push_back(running);
			cout << "Job: " << running.name << " blocked." << endl;
			running.name = "";
			schedule(isIdle, queues, running);//schedules next job in q
		}
		else{
			cout << "Error. System is idle." << endl;
		}
	}

	if(line.substr(0, 7) == "unblock"){
		bool unblocked =  false;//bool to see if job was unblocked
		getline(ss, line, ',');
		if(blockedJobs.size() > 0){
			for (unsigned int i = 0; i < blockedJobs.size();i++){
				if(blockedJobs[i].name == line){
					unblocked = true;
					queues[blockedJobs[i].q].push_back(blockedJobs[i]);//returns it to appropriate q
					blockedJobs.erase(blockedJobs.begin() + i);//removes it from list
					cout << "Job: " << line << " has unblocked." << endl;
				}
			}
		}
		if(unblocked){
			if(!isIdle){//in case unblocked is higher priority
				queues[running.q].push_back(running);
			}
			schedule(isIdle, queues, running);//runs if idle
		}
		else{//given job name was not in blocked vector
			cout << "Error. Job: " << line << " not blocked." << endl;
		}
	}

	if(line == "runnable"){
		cout << "Runnables:" << endl;
		if(isIdle){
			cout << "None" << endl;
		}
		else{
			cout << "NAME    QUEUE   " << endl;
			for(int i = 0; i < NUM_QS; i++){
				for(unsigned int j = 0; j < queues[i].size(); j++){
					cout << setfill(' ') << setw(8) << left << queues[i][j].name;
					cout << setfill(' ') << setw(8) << left << queues[i][j].q << endl;
				}
			}
		}
	}

	if (line == "running"){
		cout << "Running:" << endl;
		if (isIdle){
			cout << "None" << endl;
		}
		else{
			cout << "NAME    QUEUE   " << endl;
			cout << setfill(' ') << setw(8) << left << running.name;
			cout << setfill(' ') << setw(8) << left << running.q << endl;
		}
	}

	if(line == "blocked"){
		cout << "Blocked:" << endl;
		if (blockedJobs.size() <= 0){
			cout << "None" << endl;
		}
		else{
			cout << "NAME    QUEUE   " << endl;
			for(unsigned int i = 0;i < blockedJobs.size(); i++){
				cout << setfill(' ') << setw(8) << left << blockedJobs[i].name;
				cout << setfill(' ') << setw(8) << left << blockedJobs[i].q << endl;
			}
		}
	}

	if (line == "epoch"){
		for(int i = 1; i < NUM_QS; i++){
			while (queues[i].size() > 0){//changes pri and moves from old to new q
				cout << "Job: " << queues[i][0].name <<" lifted up." << endl;
				queues[i][0].q = 0;
				queues[0].push_back(queues[i][0]);
				queues[i].pop_front();
			}
		}
		if(!isIdle){//no need to move these
			cout << "Job: " << running.name <<" lifted up." << endl;
			running.q = 0;
		}
		if(blockedJobs.size() > 0){
			for (unsigned int i = 0; i < blockedJobs.size(); i++){
				cout << "Job: " << blockedJobs[i].name <<" lifted up." << endl;
				blockedJobs[i].q = 0;
			}
		}
	}

}//End file read

return 0;
}//End main

void schedule(bool &idle,  vector<deque<job>> &queues, job &running){

		idle = false;

		for(unsigned int i = 0; i < queues.size(); i++){//first q with a job is scheduled
			if (queues[i].size() > 0){
				running = queues[i].at(0);
				queues[i].pop_front();//removes from q
				cout << "Job: " << running.name << " scheduled." << endl;
				return;
			}
		}

		idle = true;//only gets here if no jobs are in the qs
		cout << "System is idle." << endl;
		running.name = "";
}