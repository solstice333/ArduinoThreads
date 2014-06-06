/*
 * ext2reader.h
 *
 *  Created on: Jun 3, 2014
 *      Author: knavero
 */

#ifndef EXT2READER_H_
#define EXT2READER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ext2.h"
#include "globals.h"
#include "SdReader.h"

#define DEFAULT_SIZE 32
#define BLOCK_SIZE 512
#define ISDIR_SHIFT 14
#define ISFILE_SHIFT 15

/*
 * Finds the root directory inside ext2 filesystem 
 * and returns a uint32_t pointer pointing to the beginning of the 12
 * direct block pointers associated to the |dir| inode. Client is responsible
 * for freeing the dynamically allocated ext2_dir_entry.
 */
uint32_t *get_root();

/*
 * List entries inside some directory whose directory entries are pointed
 * to by |blocks|. |blocks| associates to a directory inode's 12 direct
 * pointers. Use find_dir() to get the direct block pointers associated to a
 * directory inode then pass them into list_entries() or dump_file()
 */
void list_entries(uint32_t *blocks);

/*
 * Dumps contents of file |file_dump| given that |file_dump| is a valid file
 * inside the directory associated to |blocks|. |blocks| is obtained using
 * find_dir() and specifies the 12 direct block pointers belonging to the
 * inode found using find_dir()
 */
void dump_file(uint32_t *blocks, char *file_dump);

#endif /* EXT2READER_H_ */
