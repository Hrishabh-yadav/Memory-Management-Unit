/*
	- Different version of the MMU
	- Assumes a process requests some set of pages from secondary memory as and when required
	- Supports LRU swapping within that process
	- Maintains a Virtual Adress Map for each page currently in the main memory

	@author: bmsohwinc
*/



#include<bits/stdc++.h>
using namespace std;


 
#define MM_SIZE 1024
#define PAGE_SIZE 8
#define MAX_PROCESSES 8
#define MAX_FRAMES 128
#define MAX_PTABLES 32

int mainMemory[MM_SIZE][2]; // data - recentness
int recentness[MAX_PROCESSES];
bool isCurrentProcess[MAX_PROCESSES];
int numberOfActiveProcesses;
queue<int> availablePages;
queue<int> availablePageTables;
vector<string> addressTable[MM_SIZE];
int pagesUsed[MAX_PROCESSES];

// grows at max 8 elements to the right
vector<int> pageTablesInPageTableIndex[MAX_PROCESSES];
vector<int> pagesInPageTables[MAX_PTABLES];


void allocate(int processID, int numberOfPages) {

	int i,j;
	processID--;
	// int numberOfPageTables = ceil(1.0 * numberOfPages / 8);
	if (isCurrentProcess[processID]) {
		// if (numberOfPageTables < availablePageTables.size()) {

		// }

		// int remainingPages = availablePages.size() - numberOfPages; 

		int count = 0;
		int f = 0;

		for (i=0;i<pageTablesInPageTableIndex[processID].size();i++) {

			int pageTable = pageTablesInPageTableIndex[processID][i];

			if (pagesInPageTables[pageTable].size() < 8) {

				while (pagesInPageTables[pageTable].size() < 8) {

					int page = availablePages.front();
					pagesInPageTables[pageTable].push_back(page);	
					availablePages.pop();

					// update recentness
					recentness[processID]++;
					mainMemory[page][1] = recentness[processID];

					count++;

					// updated address table
					int index = pagesInPageTables[pageTable][pagesInPageTables[pageTable].size() - 1];
					addressTable[page].push_back(to_string(processID));
					addressTable[page].push_back(to_string(pageTable));
					addressTable[page].push_back(to_string(pagesInPageTables[pageTable][index]));

					pagesUsed[processID]++;

					if (count == numberOfPages) {
						f = 1;
						break;
					}
				}
			}
		}

		if (f) {
			printf("[I] Alloc done\n");
		}
		else {
			int rem = numberOfPages - count;

			while (rem) {

				int pageTable = availablePageTables.front();
				availablePageTables.pop();

				pageTablesInPageTableIndex[processID].push_back(pageTable);

				while (rem and pagesInPageTables[pageTable].size() < 8) {

					int page = availablePages.front();
					pagesInPageTables[pageTable].push_back(page);
					availablePages.pop();

					recentness[processID]++;
					mainMemory[page][1] = recentness[processID];

					// updated address table
					int index = pagesInPageTables[pageTable][pagesInPageTables[pageTable].size() - 1];
					addressTable[page].push_back(to_string(processID));
					addressTable[page].push_back(to_string(pageTable));
					addressTable[page].push_back(to_string(pagesInPageTables[pageTable][index]));
					
					pagesUsed[processID]++;

					rem--;
				}

			}
		}

	}
	else {

		numberOfActiveProcesses++;
		isCurrentProcess[processID] = true;

		while (numberOfPages) {

			int pageTable = availablePageTables.front();
			availablePageTables.pop();

			pageTablesInPageTableIndex[processID].push_back(pageTable);

			while (numberOfPages and pagesInPageTables[pageTable].size() < 8) {

				int page = availablePages.front();
				pagesInPageTables[pageTable].push_back(page);
				availablePages.pop();

				recentness[processID]++;
				mainMemory[page][1] = recentness[processID];

				// updated address table
				int index = pagesInPageTables[pageTable][pagesInPageTables[pageTable].size() - 1];
				addressTable[page].push_back(to_string(processID));
				addressTable[page].push_back(to_string(pageTable));
				addressTable[page].push_back(to_string(pagesInPageTables[pageTable][index]));

				pagesUsed[processID]++;
				
				numberOfPages--;
			}
		}	

		printf("[I] Alloc done\n");	
	}

}


int leastRecentPage(int processID) {

	int leastRecent = INT_MAX;
	int thatPage;
	int i,j;

	for (i=0; i<pageTablesInPageTableIndex[processID].size(); i++) {

		int pageTable = pageTablesInPageTableIndex[processID][i];

		for (j=0; j<pagesInPageTables[pageTable].size(); j++) {
			
			int page = pagesInPageTables[pageTable][j];

			// /leastRecent = min(leastRecent, mainMemory[page][1]);

			if (mainMemory[page][1] < leastRecent) {
				leastRecent = mainMemory[page][1];
				thatPage = page;
			}

		}
	}

	return thatPage;
}

void fetch(int processID, int pageNumber) {

	int i,j;
	processID--;
	pageNumber--;

	if (isCurrentProcess[processID]) {

		if (addressTable[pageNumber].size() == 0) {
			
			printf("[I] Page not in main memeory... Using LRU algorithm to swap\n");

			int page = leastRecentPage(processID);

			// printf("Gere, page = %d\n", page);
			int thatProcessID, pageTable, index;
			thatProcessID = stoi(addressTable[page][0]);
			pageTable = stoi(addressTable[page][1]);
			index = stoi(addressTable[page][2]);

			// printf("Gere, page = %d, thatProcessID = %d, pageTable = %d, index = %d\n", page, thatProcessID, pageTable, index);

			if (thatProcessID != processID) {
				printf("[E] Invalid Page access. Page belongs to different process!\n");
				return;
			}


			pagesInPageTables[pageTable][index] = pageNumber;
			recentness[processID]++;
			mainMemory[pageNumber][1] = recentness[processID];

			mainMemory[page][1] = 0;
			printf("Here, page = %d, thatProcessID = %d, pageTable = %d, index = %d\n", page, thatProcessID, pageTable, index);
			printf("PageNumber = %d\n", pageNumber);


			queue<int> spare;
			while (1) {
				if (availablePages.front() != pageNumber) {
					spare.push(availablePages.front());
					availablePages.pop();
				}
				else {
					availablePages.pop();
					break;
				}
			}



			while(!spare.empty()) {
				availablePages.push(spare.front());
				spare.pop();
			}
			
			availablePages.push(page);
			addressTable[page].clear();	

			printf("[I] Page fetched from secondary memory. Recentness updated!\n");
		}
		else {
			
			int pageTable, index, thatProcessID;

			thatProcessID = stoi(addressTable[pageNumber][0]);
			pageTable = stoi(addressTable[pageNumber][1]);
			index = stoi(addressTable[pageNumber][2]);

			if (thatProcessID != processID) {
				printf("[E] Invalid Page access. Page belongs to a different process!\n");
				return;
			}

			recentness[processID]++;
			mainMemory[pageNumber][1] = recentness[processID];

			printf("[I] Page found in main memory... Fetch Success!\n");
		}

	}
	else {
		printf("[E] Process does not exit\n");
	}
}


void deallocate(int processID) {

	int i,j;
	processID--;
	if (isCurrentProcess[processID]) {
		// ?
		
		for (i=0; i<pageTablesInPageTableIndex[processID].size(); i++) {

			int pageTable = pageTablesInPageTableIndex[processID][i];

			for (j=0; j<pagesInPageTables[pageTable].size(); j++) {
				int page = pagesInPageTables[pageTable][j];

				availablePages.push(page);


				addressTable[page].clear();
				mainMemory[page][1] = 0;

			}

			pagesInPageTables[pageTable].clear();

			availablePageTables.push(pageTable);
		}

		pageTablesInPageTableIndex[processID].clear();
		isCurrentProcess[processID] = false;
		numberOfActiveProcesses--;
		pagesUsed[processID] = 0;
	}
	else {
		printf("[E] Process does not exist\n");
	}
}


void prettyPrint(int processID) {

	int i,j;
	processID--;
	for (i=0;i<pageTablesInPageTableIndex[processID].size();i++) {

		int pageTable = pageTablesInPageTableIndex[processID][i];
		printf("pageTable = %d:\n\t", pageTable+1);
		for (j=0;j<pagesInPageTables[pageTable].size();j++) {
			printf("%2d ", pagesInPageTables[pageTable][j] + 1);
		}
		printf("\n");
	}
	printf("\n");
}

void printAll() {
	for (int i=0;i<MAX_PROCESSES;i++) {

		printf("[I] PROCESS = %d and Pages Allotted: %2d\n", i+1, pagesUsed[i]);
		prettyPrint(i+1);
	}
	printf("\n");
} 


void make() {

	int i,j;
	for(i=0;i<MM_SIZE;i++) {
		if (i < MAX_PTABLES) {
			availablePageTables.push(i);
		}

		availablePages.push(i);
	}
}




void printChoices() {

	printf("\tChoose among:\n");
	printf("\t[1] - Allocate Pages\n");
	printf("\t[2] - Fetch Pages\n");
	printf("\t[3] - Deallocate Pages\n");
	printf("\t[4] - Print specific Process Page Table\n");
	printf("\t[5] - Print All Page Tables\n");
	printf("\t[6] - Exit\n");
}


int main() {

	make();

	int processID;
	int numberOfPages;
	int pageNumber;


	int choice;

	while (1) {
		
		printChoices();

		scanf("%d", &choice);

		switch (choice) {

			case 1:
				printf("Enter processID and numberOfPages: ");
				scanf("%d%d", &processID, &numberOfPages);
				
				printf("\n");
				allocate(processID, numberOfPages);
				break;

			case 2:
				printf("Enter processID and pageNumber: ");
				scanf("%d%d", &processID, &pageNumber);

				printf("\n");
				fetch(processID, pageNumber);
				break;

			case 3:
				printf("Enter processID: ");
				scanf("%d", &processID);

				printf("\n");
				deallocate(processID);
				break;

			case 4:
				scanf("%d", &processID);

				printf("\n");
				prettyPrint(processID);
				break;

			case 5:
				printf("\n");
				printAll();
				break;

			case 6:
				printf("See ya!\n");
				exit(0);
				break;
		}

		printf("\n= - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - =\n");
	}

	return 0;
}