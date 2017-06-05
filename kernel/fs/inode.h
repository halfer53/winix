#ifndef _FS_INODE_H_
#define _FS_INODE_H_

#define INODE_NUM   496
#define NR_TZONES   7

typedef unsigned char nlink_t;
typedef unsigned long zone_t;
typedef unsigned long ino_t;

typedef struct _inode {
  mode_t i_mode;		/* file type, protection, etc. */
  nlink_t i_nlinks;		/* how many links to this file */
  uid_t i_uid;			/* user id of the file's owner */
  gid_t i_gid;			/* group number */
  off_t i_size;			/* current file size in bytes */
  time_t i_atime;		/* time of last access (V2 only) */
  time_t i_mtime;		/* when was file data last changed */
  time_t i_ctime;		/* when was inode itself changed (V2 only)*/
  zone_t i_zone[NR_TZONES]; /* zone numbers for direct, ind, and dbl ind */
  
  /* The following items are not present on the disk. */
  //dev_t i_dev;			/* which device is the inode on */
  ino_t i_num;			/* inode number on its (minor) device */
  int i_count;			/* # times inode used; 0 means slot is free */
  int i_ndzones;		/* # direct zones (Vx_NR_DZONES) */
  int i_nindirs;		/* # indirect zones per indirect block */
  //struct super_block *i_sp;	/* pointer to super block for inode's device */
  char i_dirt;			/* CLEAN or DIRTY */
  char i_pipe;			/* set to I_PIPE if pipe */
  //char i_mount;			/* this bit is set if file mounted on */
  char i_seek;			/* set on LSEEK, cleared on READ/WRITE */
  char i_update;		/* the ATIME, CTIME, and MTIME bits are here */
}inode_t;

#define NIL_INODE (struct _inode *) 0	/* indicates absence of inode slot */

#endif
