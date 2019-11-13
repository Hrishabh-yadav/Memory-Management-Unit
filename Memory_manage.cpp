#include<bits/stdc++.h>
#define pb push_back
#define ll long long
#define PGSIZE 8
#define f first
#define s second
using namespace std;
bool mem_used[129];
bool is_pt[129];
 long long main_mem[129];
int sec_mem[16385];
ll size = 32768/2;
map<int, int> st_pro;
queue<int> memory_queue;
queue <int> lru_pages;
map<int, pair<int, int> > parent_map;
queue<int> sec_memory_queue;
queue<int> pages_sec_memory;
map<int, int> used_process;
map<int, long long> base_page;

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
   cout<<mem[cur_pos]<<" ";

mem_level[cur_level + 1] -= min(mem_level[cur_level + 1], 4);
return;
}

void newprocess(int pid, int size) {
if( size > 90)
{
  cout<<"Can't allocate these many pages\n";
  return;
 }
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
cout<<"Pages process need (along with page_tables): "<< mem_required<<endl;
cout<<"Levels required for Allocation: "<< l_count<< endl;
reverse(mem_level.begin(), mem_level.end());
vector< long long> mem = allocate(mem_required);
cout<<"Pages allocated to process:\n";
for(int i =0;i<mem.size();i++)
cout<<mem[i]<<" ";
cout<<endl;
bool is_used[mem.size()];
for(int i=0;i<mem.size();i++)
    is_used[i]=0;
is_used[0] =1;
cout<<"Page table page frames:\n";
make(mem, 0, l_count, 0, mem_level, is_used);
cout<<endl;
if(size==1){
lru_pages.push(mem[0]);
parent_map[mem[0]]={ 0,0};
}
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
cout << "Process doesn't exists. Process Call unsuccessful\n";
return;
}
long long base_address = base_page[pid];
cout<<" Base Page table stored at: ";
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
cout << "Process doesn't exists. Process Deallocation unsuccessful\n";
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
void address_translate(long long base_address, long long number,int &val,int offset)
{
if (parent_map.find(base_address) != parent_map.end())
{
val++;
if(val  == number)
{
cout<<"Following page is located at "<<(((base_address-1)*PGSIZE)+offset)<<" address"<<endl;
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
cout<<"The page is stored in Secondary memory and the address of page is: ";
cout<<((base_address-1)*PGSIZE)+offset<<endl;
}
return;
}
else
address_translate(address, number, val,offset);
}
}
void address_translation(int pid, int number,int offset)
{

if (!used_process[pid])
{
cout << "Process doesn't exists. Address Translation failed. :(\n";
return;
}
long long base_address = base_page[pid];
if(main_mem[base_address]==0)
{
   cout<<"Following page is located at "<<(((base_address-1)*PGSIZE)+offset)<<" address"<<endl;
   return;
}
cout<<" Base Page table stored at: ";
cout<<base_address<<endl;
int val =0;
address_translate(base_address, number, val,offset);
}
int main()
{
for (int i = 1; i <=16384; i++)
{
if (i <= 128)
memory_queue.push(i);
sec_memory_queue.push(i);
}
while(1)
{
cout<<"\nSelect you choices:\n1.) Allocation of new process\n2.) Call an existing process\n3.) Address Translation\n4.) Deallocate of process\n5.) Exit\n";
int choice;
cin>>choice;
if( choice == 1)
{
cout<<"Enter Process ID:";
int pid;
cin>>pid;
cout<<"Enter number of bytes required by Process:";
int size;
cin>>size;
int p=size/8;
p=(size%8==0)?p:p+1;
cout<<"Pages process need: "<< p<<endl;
newprocess(pid, p);
cout<<"Current LRU:\n";
print_lru();

}
else if( choice == 2)
{
cout<<"Enter Process ID:";
int pid;
cin>>pid;
cout<<"LRU initially:\n";
print_lru();
call_process(pid);
cout<<"LRU currently:\n";
print_lru();

}
else if( choice == 3)
{
cout<<"Enter Process ID:";
int pid;
cin>>pid;
int page_num;
int offset;
cout<<"Address of which page:";
cin>>page_num;
cout<<"Enter offset in page: ";
cin>>offset;
address_translation(pid, page_num,offset);

}
else if( choice == 4)
{
cout<<"Enter Process ID:";
int pid;
cin>>pid;
cout<<"LRU initially:\n";
print_lru();
deallocate(pid);
cout<<"LRU currently:\n";
print_lru();
}
else
break;
}
 
}
