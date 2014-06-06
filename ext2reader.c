/*
 * ext2reader.c
 *
 *  Created on: Jun 3, 2014
 *      Author: knavero
 */

#include "ext2reader.h"

#define DEBUG_SB_BGDT 1
#define DEBUG_INO 0
#define DEBUG_DENTRY 0

/*
 * This is called by parse_blocks() for recursion, and is not to be
 * directly called by the client application. |blocks| points to the beginning
 * of an inode's i_block field. The inode must of type regular file (0x8000)
 * inside i_mode. |opt| can be either "d" for direct, "i" for indirect, or
 * "I" for doubly indirect.
 */
/*
static void parse_blocks_recurs(uint32_t *blocks, char *opt) {
   int i, j, k;
   char data[BLOCK_SIZE];

   if (!strcmp(opt, "d")) {
      for (i = 0; blocks[i] && i < 12; i++) {
         sdReadData(blocks[i] * 2, 0, data, BLOCK_SIZE);
         for (j = 0; data[j] && j < BLOCK_SIZE; j++)
            printf("%c", data[j]);
      }

      if (blocks[i - 1])
         parse_blocks_recurs(blocks, "i");
   }
   else if (!strcmp(opt, "i")) {
      uint32_t direct_blocks[BLOCK_SIZE];

      sdReadData(blocks[12] * 2, 0, direct_blocks, BLOCK_SIZE);
      for (i = 0; direct_blocks[i] && i < 256; i++) {
         sdReadData(direct_blocks[i] * 2, 0, data, BLOCK_SIZE);
         for (j = 0; data[j] && j < BLOCK_SIZE; j++)
            printf("%c", data[j]);
      }

      if (direct_blocks[i - 1])
         parse_blocks_recurs(blocks, "I");
   }
   else if (!strcmp(opt, "I")) {
      uint32_t indirect_blocks[BLOCK_SIZE];
      uint32_t direct_blocks[BLOCK_SIZE];

      sdReadData(blocks[13] * 2, 0, indirect_blocks, BLOCK_SIZE);
      for (i = 0; i < 256; i++) {
         sdReadData(indirect_blocks[i] * 2, 0, direct_blocks, BLOCK_SIZE);
         for (j = 0; j < 256; j++) {
            sdReadData(direct_blocks[j] * 2, 0, data, BLOCK_SIZE);
            for (k = 0; data[k] && k < BLOCK_SIZE; k++)
               printf("%c", data[k]);
         }
      }
   }
   else
      fprintf(stderr, "\nError: invalid opt\n");
}
*/

/*
 * Parse block pointers and dump the data to screen. The intended use of this
 * function is such that |blocks| points to the beginning of an inode's
 * i_block field. The inode must be of type regular file (0x8000) inside mode.
 * This function is called within dump_file() and shouldn't be called directly
 * inside the client application
 */
/*
static void parse_blocks(uint32_t *blocks) {
   parse_blocks_recurs(blocks, "d");
}
*/

uint32_t *get_root() {
   int i;

   // get superblock
   uint8_t data_sb[sizeof(ext2_super_block)];
   sdReadDataSafe(2, 0, data_sb, sizeof(ext2_super_block));
   ext2_super_block *sb = (ext2_super_block *) data_sb; 

   // get bgdt, root dir entry list, and an array of the first
   // 12 blocks in inode 2
   uint8_t data_bgdt[BLOCK_SIZE];
   sdReadDataSafe(4, 0, data_bgdt, BLOCK_SIZE);
   ext2_group_desc *bgdt = (ext2_group_desc *)data_bgdt;

// test superblock and bgdt contents
#if DEBUG_SB_BGDT
   set_cursor(1, 1);
   print_string("blocks per group: ");
   print_int32(sb->s_blocks_per_group);
   set_cursor(2, 1);
   print_string("inode table index: ");
   print_int32(bgdt[0].bg_inode_table);
   exit(1);
#endif

   // No longer printing after this block for some reason. More memory issues?
   uint8_t data_ino[sizeof(ext2_inode)];
   sdReadDataSafe(bgdt[0].bg_inode_table * 2, sizeof(ext2_inode), data_ino, 
    sizeof(ext2_inode));
   ext2_inode *ino = (ext2_inode *) data_ino;

// test root inode contents
#if DEBUG_INO
   set_cursor(1, 1);
   print_string("mode: ");
   print_hex(ino->i_mode);
   set_cursor(2, 1);
   print_string("blocks count: ");
   print_int32(ino->i_blocks);
   exit(1);
#endif

   // copy all the direct block pointers to an array
   uint32_t *blocks = malloc(13 * sizeof(uint32_t));
   for (i = 0; ino->i_block[i] && i < 12; i++)
      blocks[i] = ino->i_block[i];
   blocks[i] = 0;

// test trying to print first directory entry name inside root
#if DEBUG_DENTRY
   char name[DEFAULT_SIZE];
   
   // set dir and dir_next directory entries
   uint8_t data_dir[BLOCK_SIZE];
   sdReadDataSafe(blocks[0] * 2, 0, data_dir, BLOCK_SIZE);
   ext2_dir_entry *dir_next = (ext2_dir_entry *)data_dir;

   strncpy(name, dir_next->name, dir_next->name_len);
   name[dir_next->name_len] = NULL;

   print_string(name);
   exit(1);
#endif

   return blocks;
}

void list_entries(uint32_t *blocks) {
   int i = 0, size;
   int sectors, offset, block_group, local_idx;
   char name[DEFAULT_SIZE];
   char type;

   // get superblock
   uint8_t data_sb[sizeof(ext2_super_block)];
   sdReadDataSafe(2, 0, data_sb, sizeof(ext2_super_block));
   ext2_super_block *sb = (ext2_super_block *)data_sb;

   // get bgdt and allocate space for an inode
   uint8_t data_bgdt[BLOCK_SIZE];
   sdReadDataSafe(4, 0, data_bgdt, BLOCK_SIZE);
   ext2_group_desc *bgdt = (ext2_group_desc *)data_bgdt;

   // set dir and dir_next directory entries
   uint8_t data_dir[BLOCK_SIZE];
   sdReadDataSafe(blocks[i] * 2, 0, data_dir, BLOCK_SIZE);
   ext2_dir_entry *dir_next = (ext2_dir_entry *)data_dir;

   while (dir_next->inode) {
      // turn directory entry name into c-string
      strncpy(name, dir_next->name, dir_next->name_len);
      name[dir_next->name_len] = NULL;

      print_string(name);

      dir_next = ((uint8_t *) dir_next) + dir_next->rec_len;
      if ((uint8_t *)dir_next - data_dir >= BLOCK_SIZE) {
         sdReadDataSafe(blocks[++i] * 2, 0, data_dir, BLOCK_SIZE);
         dir_next = (ext2_dir_entry *)data_dir;
      }
   }
}

/*
void dump_file(uint32_t *blocks, char *file_dump) {
   int i = 0;
   ext2_super_block *sb = malloc(BLOCK_SIZE);
   ext2_group_desc *bgdt = malloc(BLOCK_SIZE);
   ext2_inode *ino = malloc(sizeof(ext2_inode));
   ext2_dir_entry *dentry = malloc(BLOCK_SIZE);
   bool data_dumped = false;

   sdReadData(2, 0, sb, BLOCK_SIZE);
   sdReadData(2 + TO_BGDT, 0, bgdt, BLOCK_SIZE);
   sdReadData(blocks[i] * 2, 0, dentry, BLOCK_SIZE);

   ext2_dir_entry *dentry_next = dentry;

   while (dentry_next->inode && !data_dumped) {
      char dentry_name[DEFAULT_SIZE];

      strncpy(dentry_name, dentry_next->name, dentry_next->name_len);
      dentry_name[dentry_next->name_len] = NULL;

      // locate entry name and verify if a file
      if (!strcmp(file_dump, dentry_name)) {
         int block_group = (dentry_next->inode - 1) / sb->s_inodes_per_group;
         int local_inode_index = (dentry_next->inode - 1)
               % sb->s_inodes_per_group;

         int sectors = local_inode_index * sizeof(ext2_inode) / 512;
         int offset = local_inode_index * sizeof(ext2_inode) % 512;
         sdReadData(bgdt[block_group].bg_inode_table * 2 + sectors, offset, ino,
          sizeof(ext2_inode));

         // if a file, traverse through all in-use block pointers to dump
         // data
         if (ino->i_mode >> ISFILE_SHIFT & 1) {
            uint32_t *block = ino->i_block;
            parse_blocks(block);
            data_dumped = true;
            continue;
         }
      }

      dentry_next = ((char *) dentry_next) + dentry_next->rec_len;
      if ((char *) dentry_next - (char *) dentry >= BLOCK_SIZE) {
         sdReadData(blocks[++i] * 2, 0, dentry, BLOCK_SIZE);
         dentry_next = dentry;
      }
   }

   if (!data_dumped) {
      fprintf(stderr, "\nError: file %s could not be found. Exiting...\n",
            file_dump);
      exit(1);
   }

   free(sb);
   free(bgdt);
   free(ino);
   free(dentry);
}
*/
