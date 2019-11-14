# Memory-Management-Unit
This algorithm implements the Page Based Memory allocation which has features of Heirarchical Page Tables, LRU page replacement policy and virtual to physical address translation  


## Specifications

### Memory available
* PAGE SIZE = FRAME SIZE = 8 Bytes
* Main Memory = 128 Frames = 1024 Bytes
* Secondary Memory = 2^14 Frames = 131,072 Bytes

### Page Tables
* One page table is stored in one frame
* Each page table stores 4 entries
* Each page table entry consists of 1 dirty bit (to know whether page is in MM or SM) and 14-bit address 

![pagetable pic](https://github.com/Hrishabh-yadav/Memory-Management-Unit/blob/master/Pagetable.PNG)

## Functions available

1. **New process allocation** - Initializes page tables and stores the respective pages in MM. If there isn't enough space in MM, the pages in MM are replaced according to LRU page replacement policy, and the replaced pages are sent to secondary memory
2. **Call Process** - Calls all pages of the process to the MM. Follows LRU Page replacement policy.
3. **Address Translation** - Given the process id, the page number and the offset (Virtual address), this function tells the Physical address of the particular byte.
4. **Deallocate process** - This deletes all the page tables and pages allocated to a particular process.

## How is this implemented?

Each entry of the Page Table is of Long Long Integer type. Each of these is divided into 4 parts of 15 bit each. The Page tables are implemented like Trees, and the last level of the tree consists of the actual pages allocated to the process. Whenever we need to work with functions like call_process, deallocate and translate_address, DFS is used to traverse the Tree.

## File descriptions:
* memory_manage.cpp   -   Supports multi-level paging
* mmu_2.cpp           -   Supports 2-level paging
* dryRun.txt          -   Sample Output on running mmu_2.cpp
