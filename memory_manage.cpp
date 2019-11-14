#include<bits/stdc++.h>
#define pb push_back
#define ll long long
#define PGSIZE 8
#define f first
#define s second
using namespace std;
/*
1.)Queue containing not used memory in primary memory
2.)Queue used for page replacement algorithm  (LRU)
3.)Map for finding parent of all non-page-table pages in memory graph
4.)Queue for empty memory in Secondary memory
*/
queue<int> memory_queue;
queue <int> lru_pages;
map<int, pair<int, int>> parent_map;
queue<int> sec_memory_queue;
queue<int> pages_sec_memory;
map<int, int> used_process;
map<int, long long> base_page;
map<int,int> process_count;
long long main_mem[129];
ll size = 32768/2;

void make_mempry()
{
	for (int i = 1; i <=16384; i++)
	{
		if (i <= 128)
			memory_queue.push(i);
		sec_memory_queue.push(i);
	}
}
long long power(long long num , long long times)
{
	long long ans=1;
      for(int i =0;i<times;i++)
      	ans*=num;
      return ans;
}
long long find_address(long long val, long long pos)
{

	long long address = main_mem[val] % (long long)pow(2 * size, pos + 1);
	address = address / pow(2 * size, pos);

	return address;
}

void change_address(long long val, long long pos, long long new_address, bool dirty_bit)
{
	parent_map.erase(find_address(val,pos));
	main_mem[val] -= find_address(val, pos)* pow(size * 2, (long long)pos);
	main_mem[val] += new_address * pow(2 * size, pos);
	if (dirty_bit)
		main_mem[val] += pow(2 * size, pos)*size;
	else
		main_mem[val] -= pow(2 * size, pos)*size;
}
vector<long long> page_replace_lru(int mem_required)
{
	vector<long long> ans;
	while (mem_required > 0)
	{
		long long page_no = lru_pages.front();
		lru_pages.pop();
		pair<int, int> parent_of_page = parent_map[page_no];
		long long memory_adress = sec_memory_queue.front();
		sec_memory_queue.pop();
		change_address(parent_of_page.f, parent_of_page.s, memory_adress, 1);
		pages_sec_memory.push(memory_adress);
		ans.pb(page_no);
		mem_required--;
	}
	return ans;
}

vector<long long> allocate(int mem_required)
{
	vector<long long> memory_allocate;
	if (memory_queue.size() < mem_required)
	{
		memory_allocate = page_replace_lru(mem_required - memory_queue.size());
		mem_required = memory_queue.size();
	}
	while (mem_required > 0)
	{
		memory_allocate.pb((long long)memory_queue.front());
		memory_queue.pop();
		mem_required--;
	}
	return memory_allocate;
}

void make(vector< long long > &mem, int cur_level, int levels, int cur_pos, vector<int> &mem_level, bool is_used[])
{
	if (cur_level == levels - 1)
		return;
	int var_cur_pos = cur_pos;
	for (ll i = 0; i<min(mem_level[cur_level + 1], 4); i++)
	{
		while (is_used[var_cur_pos] == 1)
			var_cur_pos++;
		is_used[var_cur_pos] = 1;
		main_mem[mem[cur_pos]] += (long long)mem[var_cur_pos] * power(size * 2, i);
		if (cur_level == levels - 2)
			{
				lru_pages.push(mem[var_cur_pos]);
				parent_map[mem[var_cur_pos]] = { mem[cur_pos],i };
			}
		make(mem, cur_level + 1, levels, var_cur_pos, mem_level, is_used);
	}
	mem_level[cur_level + 1] -= min(mem_level[cur_level + 1], 4);
	return;
}

void newprocess(int pid, int size) {
	if (used_process[pid]) {
		cout << "Process id already exists. Allocation failed.";
		return;
	}
	int mem_required = size;
	int dupe = size;
	int l_count = 1;
	vector<int> mem_level;
	mem_level.pb(size);
	while (dupe > 1)
	{
		mem_required += ceil(double(dupe / double(4.0)));
		dupe = ceil(double(dupe / double(4.0)));
		mem_level.pb(dupe);
		l_count++;
	}
	cout<<"Pages process need:\n"<< mem_required<<endl;
	cout<<"Levels required for Allocation:\n"<< l_count<< endl;
	reverse(mem_level.begin(), mem_level.end());
	vector< long long> mem = allocate(mem_required);
	cout<<"Pages allocated to process:\n";
	for(int i =0;i<mem.size();i++)
		cout<<mem[i]<<" ";
	cout<<endl;
	bool is_used[mem.size()] = {0};
	is_used[0] =1;
	make(mem, 0, l_count, 0, mem_level, is_used);
	used_process[pid] = 1;
	base_page[pid] = mem[0];
}
void modify_lru_queue(int address, int to_delete)
{
	int size_queue = lru_pages.size();
	while (lru_pages.front() != address && size_queue)
	{
		lru_pages.push(lru_pages.front());
		lru_pages.pop();
		size_queue--;
	}
	lru_pages.pop();
	size_queue--;
	while (size_queue)
	{
		lru_pages.push(lru_pages.front());
		lru_pages.pop();
		size_queue--;
	}
	if( to_delete == 0){
	lru_pages.push(address);
    }
}

bool check_dirty_bit(long long address)
{
    return address/(size);
} 

void call(long long base_address)
{
	if (parent_map.find(base_address) != parent_map.end()) {
		cout<<base_address<<" ";
		modify_lru_queue(base_address, 0);
		return;
	}
	for (int i = 0; i < 4; i++)
	{
		long long address = find_address(base_address, i);
		if (address == 0)
			break;
		if(check_dirty_bit(address))
		{
	        vector< long long> new_memory = allocate(1);
			main_mem[base_address] -= address * pow(2*size, i);
			main_mem[base_address] += new_memory[0] * pow(2*size, i);
			parent_map[new_memory[0]] = {base_address, i};
			lru_pages.push(new_memory[0]);
			call(new_memory[0]);
			
		}
		else{

			call(address);
		}
	}
}
void call_process(int pid)
{
	if (!used_process[pid])
	{
		cout << "Process doesn't exists. Process Call failed. :(\n";
		return;
	}
	long long base_address = base_page[pid];
	cout<<" Base Page tabel stored at: ";
	cout<<base_address<<"\n";
	
	cout<<"Pages Process used:\n";
	call(base_address);
	cout<<endl;
}
void call_to_delete(long long base_address)
{
	if (parent_map.find(base_address) != parent_map.end()) {
		modify_lru_queue(base_address, 1);
		memory_queue.push(base_address);
	    main_mem[base_address] =0;
	    parent_map.erase(base_address);
		return;
	}
	for (int i = 0; i < 4; i++)
	{
		long long address = find_address(base_address, i);
		if (address == 0)
			continue;
		if(check_dirty_bit(address))
		{
			continue;
		}
		else
			call_to_delete(address);
	}
	 memory_queue.push(base_address);
	 main_mem[base_address] =0;
}
void deallocate(int pid)
{
    
	if (!used_process[pid])
	{
		cout << "Process doesn't exists. Process Deallocation failed. :(\n";
		return;
	}
	long long base_address = base_page[pid];
	cout<<" Base Page tabel stored at: ";
	cout<<base_address<<"\n";
	call_to_delete(base_address);
	cout<<endl;
	used_process[pid] = 0;

} 
void print_lru()
{
	int n = lru_pages.size();
	for(int i=0;i<n;i++)
	{
		cout<<lru_pages.front()<<" ";
		lru_pages.push(lru_pages.front());
		lru_pages.pop();
	}
	cout<<endl;
}
void address_translate(long long base_address, long long number,int &val, int num2)
{
	if (parent_map.find(base_address) != parent_map.end()) 
	{
		val++;
		if(val  == number)
		{
			cout<<"Following byte is located at "<<base_address*4+num2<<" address"<<endl;
		}
		return;
	}
	for(int i =0;i<4;i++)
	{
		long long address = find_address(base_address, i);
		if (address == 0)
			continue;
		if(check_dirty_bit(address))
		{
			val++;
			if(val  == number)
			{	
				cout<<"The byte is stored in Secondary memory and the address of page is: ";
				cout<<base_address*4+num2<<endl;
			}
			return;
		}
		else
			address_translate(address, number, val, num2);
	}
}
void address_translation(int pid, int number, int num2)
{
	 
	if (!used_process[pid])
	{
		cout << "Process doesn't exists. Address Translation failed. :(\n";
		return;
	}
	long long base_address = base_page[pid];
	cout<<" Base Page tabel stored at: ";
	cout<<base_address<<endl;
	int val =0;
	address_translate(base_address, number, val, num2);
}
int main()
{
	
	while(1)
	{
		cout<<"What does the system want to do? \n1.) Allocate a new process  2.) Call Process which is already allocated \n3.) Address Translation of a byte in process  4.) Deallocation of process  \n5.) Exit\n";
		int choice;
		cin>>choice;
		if( choice == 1)
		{
			cout<<"Tell Process ID \n";
			int pid;
			cin>>pid;
			cout<<"Pages Process Require\n";
			int size;
			cin>>size;
			newprocess(pid, size);
			cout<<"Current LRU:\n";
			print_lru();
			process_count[pid] = size;

		}
		else if( choice == 2)
		{
			cout<<"Tell Process ID \n";
			int pid;
			cin>>pid;
			cout<<"LRU at starting:\n";
			print_lru();
			call_process(pid);
			cout<<"LRU after Calling:\n";
			print_lru();

		}
		else if( choice == 3)
		{
			cout<<"Tell Process ID \n";
			int pid;
			cin>>pid;
			int page_num;
			cout<<"Address of which byte :\n";
			cin>>page_num;
			int page_num1 = ceil((double)page_num/4.0);
			if( page_num1 > process_count[pid])
			{
				cout<<"Process doesn't contain these many process\n";
			    continue;
			}
			address_translation(pid, page_num1, page_num%4+1);

		}
		else if( choice == 4)
		{
			cout<<"Tell Process ID \n";
			int pid;
			cin>>pid;
			cout<<"LRU at starting:\n";
			print_lru();
			deallocate(pid);
			cout<<"LRU after Calling:\n";
			print_lru();
		}
		else
			break;
	}
  
}