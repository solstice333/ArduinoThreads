/*
 * ext2reader.c
 *
 *  Created on: Jun 3, 2014
 *      Author: knavero
 */

#include "ext2reader.h"

/*
 * This is called by parse_blocks() for recursion, and is not to be
 * directly called by the client application. |blocks| points to the beginning
 * of an inode's i_block field. The inode must of type regular file (0x8000)
 * inside i_mode. |opt| can be either "d" for direct, "i" for indirect, or
 * "I" for doubly indirect.
 */
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

/*
 * Parse block pointers and dump the data to screen. The intended use of this
 * function is such that |blocks| points to the beginning of an inode's
 * i_block field. The inode must be of type regular file (0x8000) inside mode.
 * This function is called within dump_file() and shouldn't be called directly
 * inside the client application
 */
static void parse_blocks(uint32_t *blocks) {
   parse_blocks_recurs(blocks, "d");
}

void print_error_msg_and_exit(int exit_value) {
   fprintf(stderr,
         "\nUsage: \n"
               "     ext2reader <image.ext2> [path]\n"
               "     ext2reader -l <image.ext2> <file_to_dump.txt>\n"
               "\n     If [path] is not specified, '/' will be used\n"
               "\nOptions:\n"
               "     -l    print to the screen the contents of <file_to_dump.txt>\n"
               "\nNotes:\n"
               "     All paths not prefixed with '/' are relative to the root directory\n");
   exit(exit_value);
}

uint32_t *get_root() {
   int i;

   // get superblock
   ext2_super_block *sb = malloc(BLOCK_SIZE);
   sdReadData(2, 0, sb, BLOCK_SIZE);

   print_string("number of inodes: ");
   print_int32(sb->s_inodes_count);
   exit(1);

   // get bgdt, root dir entry list, and an array of the first
   // 12 blocks in inode 2
   ext2_group_desc *bgdt = malloc(BLOCK_SIZE);
   ext2_inode *ino = malloc(INODE_SIZE);
   uint32_t *blocks = malloc(13 * sizeof(uint32_t));

   sdReadData(2 + TO_BGDT, 0, bgdt, BLOCK_SIZE);
   sdReadData(bgdt[0].bg_inode_table * 2, INODE_SIZE, ino, INODE_SIZE);

   // copy all the direct block pointers to a blocks array for root
   // inode (inode 2)
   for (i = 0; ino->i_block[i] && i < 12; i++)
      blocks[i] = ino->i_block[i];
   blocks[i] = 0;

   // teardown
   free(ino);
   free(bgdt);
   free(sb);
   return blocks;
}

void list_entries(uint32_t *blocks) {
   int i = 0, size;
   int sectors, offset, block_group, local_idx;
   char name[DEFAULT_SIZE];
   char type;

   // get superblock
   ext2_super_block *sb = malloc(BLOCK_SIZE);
   sdReadData(2, 0, sb, BLOCK_SIZE);

   // get bgdt and allocate space for an inode
   ext2_group_desc *bgdt = malloc(BLOCK_SIZE);
   sdReadData(4, 0, bgdt, BLOCK_SIZE);
   ext2_inode *ino = malloc(INODE_SIZE);

   // set dir and dir_next directory entries
   ext2_dir_entry *dir = malloc(BLOCK_SIZE);
   sdReadData(blocks[i] * 2, 0, dir, BLOCK_SIZE);
   ext2_dir_entry *dir_next = dir;

   /*
   while (dir_next->inode) {
      // turn directory entry name into c-string
      strncpy(name, dir_next->name, dir_next->name_len);
      name[dir_next->name_len] = NULL;

      // get the associating inode
      block_group = (dir_next->inode - 1) / sb->s_inodes_per_group;
      local_idx = (dir_next->inode - 1) % sb->s_inodes_per_group;
      sectors = local_idx * INODE_SIZE / 512;
      offset = local_idx * INODE_SIZE % 512;

      sdReadData(bgdt[block_group].bg_inode_table * 2 + sectors, offset, ino,
       INODE_SIZE);

      if (ino->i_mode >> ISDIR_SHIFT & 1)
         type = 'd';
      else if (ino->i_mode >> ISFILE_SHIFT & 1)
         type = 'f';
      else
         type = 'u';
      size = ino->i_size;

      print_string("here");
      print_string(name);
      print_string(" ");
      print_string(type);
      print_string(" ");
      print_int(size);
      exit(1);

      dir_next = ((char *) dir_next) + dir_next->rec_len;
      if ((char *) dir_next - (char *) dir >= BLOCK_SIZE) {
         sdReadData(blocks[++i] * 2, 0, dir, BLOCK_SIZE);
         dir_next = dir;
      }
   }
   */

   free(dir);
   free(ino);
   free(bgdt);
   free(sb);
}

void dump_file(uint32_t *blocks, char *file_dump) {
   int i = 0;
   ext2_super_block *sb = malloc(BLOCK_SIZE);
   ext2_group_desc *bgdt = malloc(BLOCK_SIZE);
   ext2_inode *ino = malloc(INODE_SIZE);
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

         int sectors = local_inode_index * INODE_SIZE / 512;
         int offset = local_inode_index * INODE_SIZE % 512;
         sdReadData(bgdt[block_group].bg_inode_table * 2 + sectors, offset, ino,
          INODE_SIZE);

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
