#include "kvfs.h"
#include <fcntl.h>

#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite()/utimensat() */
#define _XOPEN_SOURCE 700
#endif

kvfs_t* kvfs;

static void* kvfs_init(struct fuse_conn_info *conn) {

    const char* superblock_file = "/.superblock";
    char fname[strlen(mountparent) + strlen(superblock_file)];
    strcpy(fname, mountparent);
    strcat(fname,superblock_file);
    int fd;
    if (access(fname, F_OK) == -1) {
        kvfs = calloc(1, sizeof(kvfs_t));
        kvfs->size = 0;
        int size = 0;
        fd = open(fname, O_CREAT | O_WRONLY, 0644);
        write(fd, &size, sizeof(size));
        lseek(fd, SUPERBLOCK_SIZE - 1, SEEK_SET);
        write(fd, "\0", 1);
        close(fd);
    }
    fd = open(fname, O_RDWR);
    if (fd == -1) {
        printf("kv_fs_init(): open(): %s\n", strerror(errno));
        exit(1);
    }
    return NULL;
}


static int kvfs_getattr(const char *path, struct stat *stbuf)
{
    if (!strcmp(path, "/")) {
        return lstat(mountparent, stbuf);
    }
    return stat(mountparent, stbuf);
}

static int kvfs_truncate(const char *path, off_t size)
{
    return 0;
}

static int kvfs_open(const char *path, struct fuse_file_info *fi)
{  
  int fd;
  const char* superblock_file = "/.superblock";
  char fname[strlen(mountparent) + strlen(superblock_file)];
  strcpy(fname, mountparent);
  strcat(fname,superblock_file);
  
  if(!strcmp(path, "/")){
    return -ENOENT;
  }

  fd = open(fname, fi->flags);
  fi->fh = fd;
  return fd;
}

static int kvfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    const char* superblock_file = "/.superblock";
    char fname[strlen(mountparent) + strlen(superblock_file)];
    strcpy(fname, mountparent);
    strcat(fname,superblock_file);
    
    if(!strcmp(path, "/")){
      return -ENOENT;
    }

    return pwrite(fi->fh, buf, size, offset);
}

static int kvfs_read(const char *path, char *buf,size_t size, off_t offset, struct fuse_file_info *fi)
{
  if(!strcmp(path, "/")){
    return -ENOENT;
  }
  
  return pread(fi->fh, buf, size, offset);
}

struct fuse_operations kvfs_oper = {
    .getattr    = kvfs_getattr,
    .truncate   = kvfs_truncate,
    .init       = kvfs_init,
    .open       = kvfs_open,
    .write      = kvfs_write,
    .read       = kvfs_read
    /*
    .access     = kvfs_access,
    .mv         = kvfs_mv,
    .fsync      = kvfs_fysnc  
    */
};


