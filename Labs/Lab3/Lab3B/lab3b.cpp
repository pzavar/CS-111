#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <map>
using namespace std;

void die(char * msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

ifstream myfile("P3B-test_5.csv");
int total_blocks=0, total_inodes=0,linkcount;
bool bad = 0;
int temp;
int inode_num, parent_inode, block_num, indir, offset,links;
string lv;
string dn;
set <int>bfree;
set <int>ifree;
map <int,vector<int>> blocking;
map <int, int> inoding;
map <int, int> inoding_reflc;
map <int, int> inoding_lr;
map <int, int> inode_top;
map <int, string> ref_inode;
vector <int> block_r = { 0,1,2,3,4,5,6,7,64 };


vector<string> splitToWords(string line)
{
	unsigned int i = 0, j = 0,counter=0;
	while (j < line.length())
	{
		if (line[j] == ',') { i++; j++; }
		j++;
	}
	
	vector<string>words(i+1);
	i = j = 0;
	while (counter < line.length())
	{
		if (line[counter] == ',')
		{
			i++;
			j=0;
			counter++;
		}
		words[i].push_back(line[counter]);
		j++;
		counter++;
	}
	return words;
}

enum string_code {
	SUPERBLOCK,
	BFREE,
	IFREE,
	INODE,
	INDIRECT,
	DIRENT
};

string_code hash_it(string const &inString) {
	if (inString == "SUPERBLOCK") return SUPERBLOCK;
	if (inString == "BFREE") return BFREE;
	if (inString == "IFREE") return IFREE;
	if (inString == "INODE") return INODE;
	if (inString == "INDIRECT") return INDIRECT;
	if (inString == "DIRENT") return DIRENT;
}

int main(int argc, char *argv[]) {
	if (argc != 2)
		die("Incorrect number of command line arguments\n");
	int x;
	string summary_type;
	string line;
	vector <string> words;
	ifstream myfile("P3B-test_3.csv");
	if (myfile.is_open())
	{
		while (!myfile.eof())
		{
			getline(myfile, line);
			words = splitToWords(line);
			
			char type = 's';
			if (words[0] == "SUPERBLOCK")
				type = '0';
			if (words[0] == "BFREE")
				type = '1';
			if (words[0] == "IFREE")
				type = '2';
			if (words[0] == "INODE")
				type = '3';
			if (words[0] == "INDIRECT")
				type = '4';
			if (words[0] == "DIRENT")
				type = '5';

			switch (type) {
			case '0': //SUPERBLOCK
				total_blocks = stoi(words[1]);
				total_inodes = stoi(words[2]);
				break;
			case '1': //BFREE
				temp = stoi(words[1]);
				bfree.insert(temp);
				break;
			case '2': // IFREE
				temp = stoi(words[1]);
				ifree.insert(temp);
				break;
			case '3': //INODE
				inode_num = stoi(words[1]);
				inoding[inode_num] = stoi(words[6]);
				int i;
				
				for (i = 12; i < 27; i++)
				{
					block_num = stoi(words[i]);

					switch (block_num) {
					case 0:
						continue;

					case 24:
						lv = " INDIRECT";
						offset = 12;
						indir = 1;

					case 25:
						lv = " DOUBLE INDIRECT";
						offset = 268;
						indir = 2;

					case 26:
						lv = " TRIPLE INDIRECT";
						offset = 65804;
						indir = 3;

					default:
						lv = "";
						offset = indir = 0;
					}

					if (block_num < 0 || block_num>total_blocks)
					{
						cout << "INVALID" << lv << " BLOCK " << block_num << " IN INODE " <<
							inode_num << " AT OFFSET " << offset << endl;
						bad = 1;
					}
					else if ((block_num != 0) && (find(block_r.begin(), block_r.end(),
						block_num) != block_r.end()))
					{
						//element found
						cout << "RESERVED" << lv << " BLOCK " << block_num << " IN INODE " << inode_num << " AT OFFSET " << offset << endl;
						bad = 1;
					}
					else if (blocking.find(block_num) == blocking.end())
					{
						vector<int>tempV = { inode_num,offset,indir };
						blocking[block_num].push_back(inode_num);
						blocking.insert(pair<int, vector<int>>(block_num, tempV));
					}
					else {
						vector<int>tempV = { inode_num,offset,indir };
						blocking.insert(pair<int, vector<int>>(block_num, tempV));
					}
				}
				break;

			case '4': //INDIRECT
				block_num = stoi(words[5]);
				inode_num = stoi(words[1]);
				indir = stoi(words[2]);

				switch (indir) {
				case 1:
					lv = "INDIRECT";
					offset = 12;
					break;

				case 2:
					lv = "DOUBLE INDIRECT";
					offset = 268;
					break;

				case 3:
					lv = "TRIPLE INDIRECT";
					offset = 65804;

				}


				if (block_num < 0 || block_num > total_blocks) {
					cout << "INVALID " << lv << " BLOCK " << block_num << " IN INODE " << inode_num << " AT OFFSET " << offset << endl;
					bad = 1;
				}
				else if ((block_num != 0) && (find(block_r.begin(), block_r.end(),
					block_num) != block_r.end())) {
					cout << "RESERVED " << lv << " BLOCK " << block_num << " IN INODE " << inode_num << " AT OFFSET " << offset << endl;
					bad = 1;
				}
				else if (blocking.find(block_num) == blocking.end())
				{
					vector<int>tempV = { inode_num,offset,indir };
					blocking[block_num].push_back(inode_num);
					blocking.insert(pair<int, vector<int>>(block_num, tempV));
				}
				else {
					vector<int>tempV = { inode_num,offset,indir };
					blocking.insert(pair<int, vector<int>>(block_num, tempV));
				}
				break;

			case '5': //DIRENT
				dn = words[6];
				parent_inode = stoi(words[1]);
				inode_num = stoi(words[3]);

				ref_inode.insert(pair<int, string>(inode_num, dn));

				if (inoding_reflc.find(inode_num) == inoding_reflc.end())
					inoding_reflc.insert(pair<int, int>(inode_num, 1));

				else {
					int maptemp = inoding_reflc[inode_num] + 1;
					inoding_reflc[inode_num]++;
				}

				if (inode_num < 1 || inode_num > total_inodes) {
					cout << "DIRECTORY INODE " << parent_inode << " NAME " << dn.substr(0, dn.length() - 1) << " INVALID INODE " << inode_num << endl;
					bad = 1;
					continue;
				}
				else if (dn.substr(0, 3) == "'.'") continue;
				else if (dn.substr(0, 4) == "'..'")
				{
					inode_top.insert(pair<int, int>(parent_inode, inode_num));
				}
				else {
					inoding_lr.insert(pair<int, int>(inode_num, parent_inode));
				}
				break;
			default:
				break;
			}

		     
		
		}
	}
	for (x = 1; x < total_blocks + 1; x++) 
		if ((bfree.find(x) == bfree.end()) && (find(block_r.begin(), block_r.end(),
			x)) == block_r.end() && !blocking.count(x))
		{
			cout << "UNREFERENCED BLOCK " << x << endl;
			bad = 1;
		}
		else if (bfree.find(x) != bfree.end() && blocking.count(x))
		{
			cout << "ALLOCATED BLOCK " << x << " ON FREELIST " << endl;
			bad = 1;
		}
	x = 0;
	for (x = 0; x < inode_top.size(); x++)
	{
		if (inode_top[x] == 2 && x == 2)continue;
		if (x == 2)
		{
			cout << "DIRECTORY INODE 2 NAME '..' LINK TO INODE " << inode_top[x] << " SHOULD BE 2" << endl;
			bad = 1;
		}
		else if (inode_top[x] != inoding_lr[x])
		{
			cout << "DIRECTORY INODE " << x << " NAME '..' LINK TO INODE " << x << " SHOULD BE " << inoding_lr[x] << endl;
			bad = 1;
		}
		x++;
	}
	
	for (x = 0; x < inoding.size(); x++)
	{
		linkcount = inoding[x];

		if (inoding_reflc.find(x) != inoding_reflc.end()) {
			links = inoding_reflc[x];
		}
		else {
			links = 0;
		}
		if (links != linkcount)
		{
			cout << "INODE " << x << " HAS " << links << " LINKS BUT LINKCOUNT IS " << linkcount << endl;
			bad = 1;
		}
		x++;
	}
	
	
	for (x = 0; x < ref_inode.size(); x++)
	{
		
		string p = ref_inode[x];
		if (ifree.find(x) != ifree.end() && inoding_lr.find(x) != inoding_lr.end())
		{
			parent_inode = inoding_lr[x];
			dn = ref_inode[x];
			cout << "DIRECTORY INODE " << parent_inode << " NAME " << dn.substr(0, dn.length() - 2) << " ' UNALLOCATED INODE " << x << endl;
			bad = 1;
		}
		x++;
	}

	myfile.close();
	if (bad) exit(2);
	exit(0);
}
