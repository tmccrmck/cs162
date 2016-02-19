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
    if (!strcmp(path, "/") || !strcmp(path, "/.superblock")) {
        return -ENOENT;
    }
    return lstat(path, stbuf);
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
  return 0;
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

static int kvfs_access(const char *path, int mask)
{
  const char* superblock_file = "/.superblock";
  char fname[strlen(mountparent) + strlen(superblock_file)];
  strcpy(fname, mountparent);
  strcat(fname,superblock_file);

  return access(path, mask);

}

static int kvfs_rename(const char *path, const char *newpath){

  const char* superblock_file = "/.superblock";
  char fname[strlen(mountparent) + strlen(superblock_file)];
  char fnewpath[strlen(mountparent) + strlen(superblock_file)];
  strcpy(fname, mountparent);
  strcat(fname,superblock_file);
  strcpy(fnewpath, mountparent);
  strcat(fnewpath, superblock_file);

  return rename(fname, fnewpath);

}

static int kvfs_mknod(const char *path, mode_t mode, dev_t rdev)
{
  int ret = mknod(path, mode ,rdev);
  ret += 0;
  return 0;
}

struct fuse_operations kvfs_oper = {
    .getattr    = kvfs_getattr,
    .truncate   = kvfs_truncate,
    .init       = kvfs_init,
    .open       = kvfs_open,
    .write      = kvfs_write,
    .read       = kvfs_read,
    .access     = kvfs_access,
    .rename     = kvfs_rename,
    .mknod      = kvfs_mknod
    /*
    .mv         = kvfs_mv,
    .fsync      = kvfs_fysnc  
    */
};


