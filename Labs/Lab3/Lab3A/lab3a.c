#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>
#include <getopt.h>
#include "ext2_fs.h"

int fd;
struct ext2_super_block superblock;
struct ext2_group_desc desc_g;
struct ext2_dir_entry dir_entry;
struct ext2_inode inode;
unsigned int block_size;
const int indirection_level_1 = 1;
const int indirection_level_2 = 2;
const int indirection_level_3 = 3;

void die(char *msg) {
  perror(msg);
  exit(1);
}

//DONE
//Parham
void print_superblock() { 
  if (pread(fd, &superblock, sizeof(superblock), 1024) < 0)
    die("Failed to read\n");
  
  fprintf(stdout, "SUPERBLOCK,");
  fprintf(stdout, "%d,", superblock.s_blocks_count);
  fprintf(stdout, "%d,", superblock.s_inodes_count);
  fprintf(stdout, "%d,", block_size);
  fprintf(stdout, "%d,", superblock.s_inode_size);
  fprintf(stdout, "%d,", superblock.s_blocks_per_group);
  fprintf(stdout, "%d,", superblock.s_inodes_per_group);
  fprintf(stdout, "%d\n", superblock.s_first_ino);
}

//DONE
//John
void dirPrint(unsigned int inode_og, unsigned int nBytes){
  fprintf(stdout, "DIRENT,");
  fprintf(stdout, "%d,", inode_og);
  fprintf(stdout, "%d,", nBytes);
  fprintf(stdout, "%d,", dir_entry.inode);
  fprintf(stdout, "%d,", dir_entry.rec_len);
  fprintf(stdout, "%d,", dir_entry.name_len);
  fprintf(stdout, "'%s'\n", dir_entry.name);
}

//DONE
//John
void indirect1(struct ext2_inode inode,char type, unsigned int nInode) {
  unsigned int i;
  unsigned int *block_ptrs = malloc(block_size);
  unsigned int num_ptrs = block_size / sizeof(unsigned int);
  unsigned long offset, indir_offset = 1024 + (inode.i_block[12] - 1) * block_size;
  if (pread(fd, block_ptrs, block_size, indir_offset) < 0)
    die("Failed to read\n");
  
  for (i = 0; i < num_ptrs; i++) {
    if (block_ptrs[i] != 0) {
      if (type == 'd') {
	offset = 1024 + (block_ptrs[i] - 1) * block_size;
	while(block_ptrs[i] < block_size) {
	  if (pread(fd, &dir_entry, sizeof(dir_entry), offset + block_ptrs[i]) < 0)
	    die("Failed to read\n");
	  if (dir_entry.inode != 0) {
	    dirPrint(nInode, block_ptrs[i]);
	  }
	  block_ptrs[i] += dir_entry.rec_len;
	}
      }
      fprintf(stdout, "INDIRECT,");
      fprintf(stdout, "%d,", nInode);
      fprintf(stdout, "%d,", indirection_level_1);
      fprintf(stdout, "%d,", i+12);
      fprintf(stdout, "%d,", inode.i_block[12]);
      fprintf(stdout, "%d\n", block_ptrs[i]);
    }
  }
}

//DONE
//Parham
void indirect2(struct ext2_inode inode,unsigned int inode_num) {
  unsigned int *block_ptrs, indir_offset, indir2_offset, num_ptrs, *indir_block_ptrs, k = 0;
  indir_block_ptrs = malloc(block_size);
  num_ptrs = block_size / sizeof(uint32_t);
  indir2_offset =1024 + (inode.i_block[13] - 1) * block_size;
  if (pread(fd, indir_block_ptrs, block_size, indir2_offset) < 0)
    die("Failed to read\n");
  int sum = 256 + 12;
  unsigned long offset;

  unsigned int j;
  for (j = 0; j < num_ptrs; j++) {
    if (indir_block_ptrs[j] != 0) {
      fprintf(stdout, "INDIRECT,");
      fprintf(stdout, "%d,", inode_num);
      fprintf(stdout, "%d,", indirection_level_2);
      fprintf(stdout, "%d,", sum + j);
      fprintf(stdout, "%d,", inode.i_block[13]);
      fprintf(stdout, "%d\n", indir_block_ptrs[j]);
      
      block_ptrs = malloc(block_size);
      indir_offset = 1024 + (indir_block_ptrs[j] - 1) * block_size;
      if (pread(fd, block_ptrs, block_size, indir_offset) < 0)
	die("Failed to read\n");
      
      for (; k < num_ptrs; k++) {
	if ((block_ptrs[k] ^ 0) == 0) {
	  offset = 1024 + (block_ptrs[k] - 1) * block_size;
	  while(block_ptrs[k] < block_size) {
	    if (pread(fd, &dir_entry, sizeof(dir_entry), offset + block_ptrs[k]) < 0)
	      die("Failed to read\n");
	    if ((dir_entry.inode ^ 0) == 0) {
	      dirPrint(inode_num, block_ptrs[k]);
	    }
	    block_ptrs[k] += dir_entry.rec_len;
	  }
	      
	  fprintf(stdout, "INDIRECT,");
	  fprintf(stdout, "%d,", inode_num);
	  fprintf(stdout, "%d,", indirection_level_1);
	  fprintf(stdout, "%d,", sum + k);
	  fprintf(stdout, "%d,", indir_block_ptrs[j]);
	  fprintf(stdout, "%d,", block_ptrs[k]);
	}
      }
    }
  }
}

//DONE
//John
void indirect3(struct ext2_inode inode,unsigned int inode_num) {
  unsigned int *indir2_block_ptrs, num_ptrs, j=0, *indir_block_ptrs, k=0, p=0;
  indir2_block_ptrs = malloc(block_size);
  num_ptrs = block_size / sizeof(uint32_t);
  
  unsigned long indir2_offset, offset, indir3_offset = 1024 + (inode.i_block[14] - 1) * block_size;
  if (pread(fd, indir2_block_ptrs, block_size, indir3_offset) < 0)
    die("Failed to read\n");
  int sum = 65536 + 256 + 12;

  while(j < num_ptrs){
    if (indir2_block_ptrs[j] != 0) {
      fprintf(stdout, "INDIRECT,");
      fprintf(stdout, "%d,", inode_num);
      fprintf(stdout, "%d,", indirection_level_3);
      fprintf(stdout, "%d,", sum + j);
      fprintf(stdout, "%d,", inode.i_block[14]);
      fprintf(stdout, "%d\n", indir2_block_ptrs[j]);
      
      indir_block_ptrs = malloc(block_size);
      indir2_offset = 1024 + (indir2_block_ptrs[j] - 1) * block_size;
      if (pread(fd, indir_block_ptrs, block_size, indir2_offset) < 0)
	die("Failed to read\n");
      
      while(k < num_ptrs) {
	if (indir_block_ptrs[k] != 0) {
	  fprintf(stdout, "INDIRECT,");
	  fprintf(stdout, "%d,", inode_num);
	  fprintf(stdout, "%d,", indirection_level_2);
	  fprintf(stdout, "%d,", sum + k);
	  fprintf(stdout, "%d,", indir2_block_ptrs[j]);
	  fprintf(stdout, "%d\n", indir_block_ptrs[k]);

	  uint32_t *block_ptrs = malloc(block_size);
	  unsigned long indir_offset = 1024 + (indir_block_ptrs[k] - 1) * block_size;
	  if (pread(fd, block_ptrs, block_size, indir_offset) < 0)
	    die("Failed to read\n");
	      
	      
	  while (p < num_ptrs) {
	    if (block_ptrs[p] != 0) {
	      offset = 1024 + (block_ptrs[p] - 1) * block_size;
	      while(block_ptrs[p] < block_size) {
		if (pread(fd, &dir_entry, sizeof(dir_entry), offset + block_ptrs[p]) < 0)
		  die("Failed to read\n");
		if (dir_entry.inode != 0) {
		  dirPrint(inode_num, block_ptrs[p]);
		}
		block_ptrs[p] += dir_entry.rec_len;
	      }
	    }
	    fprintf(stdout, "INDIRECT,");
	    fprintf(stdout, "%d,", inode_num);
	    fprintf(stdout, "%d,", 1);
	    fprintf(stdout, "%d,", sum + p);
	    fprintf(stdout, "%d,", indir_block_ptrs[k]);
	    fprintf(stdout, "%d\n", block_ptrs[p]);
	            
	    p++;
	  }
	}
	k++;
      }
    }
    j++;
  }
}

//DONE
//Parham
void reading_inode(unsigned int inode_table_id, unsigned int index, unsigned int inode_num) {
  unsigned long offset = 1024 + (inode_table_id - 1) * block_size + index * sizeof(inode); 
  if (pread(fd, &inode, sizeof(inode), offset) < 0)
    die("Failed to read\n");
  
  if (inode.i_mode == 0 || inode.i_links_count == 0) {
    return;
  }
  
  char type;
  unsigned int file_val = (inode.i_mode >> 12) << 12;
  switch (file_val)
    {
    case 0xa000:
      type = 's';
    case 0x8000:
      type = 'f';
    case 0x4000:
      type = 'd';
    }
  
  unsigned int num_blocks = indirection_level_2 * (inode.i_blocks / (indirection_level_2 << superblock.s_log_block_size));
  
  fprintf(stdout, "INODE,");
  fprintf(stdout, "%d,", inode_num);
  fprintf(stdout, "%c,", type);
  fprintf(stdout, "%o,", inode.i_mode & 0xFFF);
  fprintf(stdout, "%d,", inode.i_uid);
  fprintf(stdout, "%d,", inode.i_gid);
  fprintf(stdout, "%d,", inode.i_links_count);
 
  char ctime[20], mtime[20], atime[20];
  time_t rawtime = inode.i_ctime;
  struct tm* time_info = gmtime(&rawtime);
  strftime(ctime, 32, "%m/%d/%y %H:%M:%S", time_info);
  rawtime = inode.i_mtime;
  time_info = gmtime(&rawtime);
  strftime(mtime, 32, "%m/%d/%y %H:%M:%S", time_info);
  rawtime = inode.i_atime;
  time_info = gmtime(&rawtime);
  strftime(atime, 32, "%m/%d/%y %H:%M:%S", time_info);
  fprintf(stdout, "%s,%s,%s,", ctime, mtime, atime);
  
  fprintf(stdout, "%d,", inode.i_size);
  fprintf(stdout, "%d", num_blocks);
  
  unsigned int i;
  for (i = 0; i < 15; i++) { 
    fprintf(stdout, ",%d", inode.i_block[i]);
  }
  fprintf(stdout, "\n");
  
  for (i = 0; i < 12; i++) { 
    if (inode.i_block[i] != 0 && type == 'd') {
      unsigned long offset2 = 1024 + (inode.i_block[i] - 1) * block_size;
      unsigned int nBytes = 0;
      while(nBytes < block_size) {
	if (pread(fd, &dir_entry, sizeof(dir_entry), offset2 + nBytes) < 0)
	  die("Failed to read\n");
	if (dir_entry.inode != 0) {
	  dirPrint(inode_num,nBytes);
	}
	nBytes += dir_entry.rec_len;
      }
    }
  }
  
  indirect1(inode, type, inode_num);
  indirect2(inode, inode_num);  
  indirect3(inode, inode_num);  
}

//DONE
//Parham
void reading_bitmap(int group, int block, int inode_table_id) {
  int nBytes = superblock.s_inodes_per_group / 8;
  char* bit_mapping = (char*) malloc(nBytes);
  unsigned long offset = 1024 + (block - 1) * block_size;
  unsigned int changing = group * superblock.s_inodes_per_group + 1;
  //unsigned int original = changing;
  if (pread(fd, bit_mapping, nBytes, offset) < 0)
    die("Failed to read\n");
  
  int i=0, j=0;
  while(i < nBytes) {
    char x = bit_mapping[i];
    for (j=0; j < 8; j++) {
      if (x) {
	reading_inode(inode_table_id, changing - (group*superblock.s_inodes_per_group+1), changing);
      } else { 
	fprintf(stdout, "IFREE,%d\n", changing);
      }
      x = x >> 1;
      changing++;
    }
    i++;
  }
}

//DONE
//John
void read_group(int group, int total_groups) {
  int checked;
  unsigned int i=0, j;
  unsigned int inode_bitmap, inode_table, descblock=block_size==1024?2:1;
  unsigned long offset = block_size * descblock + 32 * group;
  if (pread(fd, &desc_g, sizeof(desc_g), offset) < 0)
    die("Failed to read\n");
  
  unsigned int num_blocks_in_group = superblock.s_blocks_per_group;
  if (group == total_groups - 1) {
    num_blocks_in_group = superblock.s_blocks_count - (superblock.s_blocks_per_group * (total_groups - 1));
  }
  
  unsigned int num_inodes_in_group = superblock.s_inodes_per_group;
  if (group == total_groups - 1) {
    num_inodes_in_group = superblock.s_inodes_count - (superblock.s_inodes_per_group * (total_groups - 1));
  }
  
  fprintf(stdout, "GROUP,");
  fprintf(stdout, "%d,",group);
  fprintf(stdout, "%d,", num_blocks_in_group);
  fprintf(stdout, "%d,", num_inodes_in_group);
  fprintf(stdout, "%d,", desc_g.bg_free_blocks_count);
  fprintf(stdout, "%d,", desc_g.bg_free_inodes_count);
  fprintf(stdout, "%d,", desc_g.bg_block_bitmap);
  fprintf(stdout, "%d,", desc_g.bg_inode_bitmap);
  fprintf(stdout, "%d\n", desc_g.bg_inode_table);
  
  unsigned int block_bitmap = desc_g.bg_block_bitmap;
  //free_blocks(group, block_bitmap);
  char pj, *bytes = (char*) malloc(block_size);
  if (bytes == NULL)
    die("Failed to allocate memory\n");
  unsigned long offset2 = 1024 + (block_bitmap - 1) * block_size;
  unsigned int counter = superblock.s_first_data_block + group * superblock.s_blocks_per_group;
  if (pread(fd, bytes, block_size, offset2) < 0)
    die("Failed to read\n");
  
  while (i < block_size) {
    pj = bytes[i];
    for (j = 8; j > 0; j--) {
      checked =  1 & pj;
      if (!checked) {
        fprintf(stdout, "BFREE,%d\n", counter);
      }
      pj >>= 1;
      counter++;
    }
    i++;
  }

  inode_bitmap = desc_g.bg_inode_bitmap;
  inode_table = desc_g.bg_inode_table;
  reading_bitmap(group, inode_bitmap, inode_table);
}


//DONE
//Parham
int main (int argc, char* argv[]) {
  if (argc != 2)
    die("Incorrect number of arguments\n");

  int i;
  
  if ((fd = open(argv[1], O_RDONLY)) == -1) {
    die("Failed to open\n");
  }
  block_size = EXT2_MIN_BLOCK_SIZE << superblock.s_log_block_size;
  print_superblock();
  double num_groups = superblock.s_blocks_count / superblock.s_blocks_per_group;
  double pergro= (double) superblock.s_blocks_count / superblock.s_blocks_per_group;
  
  num_groups<pergro?num_groups++:num_groups;
  i=0;
  while (i<num_groups) {
    read_group(i, num_groups);
    i++;
  }
  exit(0);
}

