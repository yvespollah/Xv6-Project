//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "memlayout.h"
#include "proc.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "fcntl.h"
#include "x86.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
static int
argfd(int n, int *pfd, struct file **pf)
{
	int fd;
	struct file *f;

	if(argint(n, &fd) < 0)
		return -1;
	if(fd < 0 || fd >= NOFILE || (f=myproc()->ofile[fd]) == 0)
		return -1;
	if(pfd)
		*pfd = fd;
	if(pf)
		*pf = f;
	return 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static int
fdalloc(struct file *f)
{
	int fd;
	struct proc *curproc = myproc();

	for(fd = 0; fd < NOFILE; fd++){
		if(curproc->ofile[fd] == 0){
			curproc->ofile[fd] = f;
			return fd;
		}
	}
	return -1;
}

int
sys_dup(void)
{
	struct file *f;
	int fd;

	if(argfd(0, 0, &f) < 0)
		return -1;
	if((fd=fdalloc(f)) < 0)
		return -1;
	filedup(f);
	return fd;
}

int
sys_read(void)
{
	struct file *f;
	int n;
	char *p;

	if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
		return -1;
	return fileread(f, p, n);
}

int
sys_write(void)
{
	struct file *f;
	int n;
	char *p;

	if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
		return -1;
	return filewrite(f, p, n);
}

int
sys_close(void)
{
	int fd;
	struct file *f;

	if(argfd(0, &fd, &f) < 0)
		return -1;
	myproc()->ofile[fd] = 0;
	fileclose(f);
	return 0;
}

int
sys_fstat(void)
{
	struct file *f;
	struct stat *st;

	if(argfd(0, 0, &f) < 0 || argptr(1, (void*)&st, sizeof(*st)) < 0)
		return -1;
	return filestat(f, st);
}

int sys_chmod(void)
{
	char* path;
	int mode;

	if(argstr(0, &path) < 0 || argint(1,&mode)<0)
		return -1;

	struct proc* currProc=myproc();
	struct inode* ip=namei(path);
	if(currProc->uid!=ip->uid && currProc->uid!=ROOT)
		return -1;

	ilock(ip);
	ip->mode=mode;
	iunlock(ip);
	return 1;
}

void changeOwnerGroup(int uid, int gid, struct inode* ip)
{
	ilock(ip);
	ip->uid=uid;
	ip->gid=gid;
	iunlock(ip);
}

int sys_chown(void)
{
	char* path;
	int uid, gid;

	if(argstr(0, &path) < 0 || argint(1, &uid)<0 || argint(2, &gid)<0)
		return -1;

	struct proc* currProc=myproc();
	struct inode* ip=namei(path);
	if(currProc->uid!=ROOT)
		return -1;

	changeOwnerGroup(uid, gid, ip);
	return 1;
}

int sys_updateDirOwner(void)
{
	char* path;
	int uid, gid;

	if(argstr(0, &path) < 0 || argint(1, &uid)<0 || argint(2, &gid)<0)
		return -1;

	struct inode* dp=namei(path);
	changeOwnerGroup(uid, gid, dp);											// change the uid and gid of homedir dir

	int off;
	struct dirent de;
	for(off=2*sizeof(de); off<dp->size; off+=sizeof(de)){					// change the uid and gid of homedir files
		if(readi(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
			panic("updateDirOwner");

		struct inode* ip=namei(de.name);
		changeOwnerGroup(uid, gid, ip);
	}

	return 1;
}

// Create the path new as a link to the same inode as old.
int
sys_link(void)
{
	char name[DIRSIZ], *new, *old;
	struct inode *dp, *ip;

	if(argstr(0, &old) < 0 || argstr(1, &new) < 0)
		return -1;

	begin_op();
	if((ip = namei(old)) == 0){
		end_op();
		return -1;
	}

	ilock(ip);
	if(ip->type == T_DIR){
		iunlockput(ip);
		end_op();
		return -1;
	}

	ip->nlink++;
	iupdate(ip);
	iunlock(ip);

	if((dp = nameiparent(new, name)) == 0)
		goto bad;
	ilock(dp);
	if(dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0){
		iunlockput(dp);
		goto bad;
	}
	iunlockput(dp);
	iput(ip);

	end_op();

	return 0;

bad:
	ilock(ip);
	ip->nlink--;
	iupdate(ip);
	iunlockput(ip);
	end_op();
	return -1;
}

// Is the directory dp empty except for "." and ".." ?
static int
isdirempty(struct inode *dp)
{
	int off;
	struct dirent de;

	for(off=2*sizeof(de); off<dp->size; off+=sizeof(de)){
		if(readi(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
			panic("isdirempty: readi");
		if(de.inum != 0)
			return 0;
	}
	return 1;
}

int
sys_unlink(void)
{
	struct inode *ip, *dp;
	struct dirent de;
	char name[DIRSIZ], *path;
	uint off;

	if(argstr(0, &path) < 0)
		return -1;

	begin_op();
	if((dp = nameiparent(path, name)) == 0){
		end_op();
		return -1;
	}

	ilock(dp);

	// Cannot unlink "." or "..".
	if(namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
		goto bad;

	if((ip = dirlookup(dp, name, &off)) == 0)
		goto bad;
	ilock(ip);

	if(ip->nlink < 1)
		panic("unlink: nlink < 1");
	if(ip->type == T_DIR && !isdirempty(ip)){
		iunlockput(ip);
		goto bad;
	}

	memset(&de, 0, sizeof(de));
	if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
		panic("unlink: writei");
	if(ip->type == T_DIR){
		dp->nlink--;
		iupdate(dp);
	}
	iunlockput(dp);

	ip->nlink--;
	iupdate(ip);
	iunlockput(ip);

	end_op();

	return 0;

bad:
	iunlockput(dp);
	end_op();
	return -1;
}

int validateWrite(char* path)
{
	int currUser=myproc()->uid;
	char s[20];
	struct inode* iParent=nameiparent(path, s);

	if(!strncmp(s,"passwd", 6))						// replacment for setuid bit
		return 1;
	if(!strncmp(s,"group", 5))
		return 1;

	ilock(iParent);
	int uidParent=iParent->uid;
	int modeParent=iParent->mode;
	iunlock(iParent);
	
	/*int setuidBit=SETUID << 9;					// if file has setuidBit, it has access
	if(modeChild & setuidBit)
		return 1;*/
	
	int userWrite=WRITE << 6;						// does owner have access?
		if((modeParent & userWrite) && (uidParent == currUser))
			return 1;

	int otherWrite=WRITE;							// do other members have access?
		if(modeParent & otherWrite)
			return 1;

	return -1;
}

struct inode*
create(char *path, short type, short major, short minor)
{
	struct inode *ip, *dp;
	char name[DIRSIZ];

	int currUser=myproc()->uid;
	if(currUser!=ROOT)					// if user isn't root, validate permisions
		if(validateWrite(path)<0)
			return 0;

	if((dp = nameiparent(path, name)) == 0)
		return 0;
	ilock(dp);

	if((ip = dirlookup(dp, name, 0)) != 0){
		iunlockput(dp);
		ilock(ip);
		if((type == T_FILE && ip->type == T_FILE) || ip->type == T_DEV)
			return ip;
		iunlockput(ip);
		return 0;
	}

	if((ip = ialloc(dp->dev, type)) == 0)
		panic("create: ialloc");

	ilock(ip);
	ip->major = major;
	ip->minor = minor;
	ip->nlink = 1;
	ip->mode = 0644;
	iupdate(ip);

	if(type == T_DIR){  // Create . and .. entries.
		dp->nlink++;  // for ".."
		iupdate(dp);
		// No ip->nlink++ for ".": avoid cyclic ref count.
		if(dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
			panic("create dots");
	}

	if(dirlink(dp, name, ip->inum) < 0)
		panic("create: dirlink");

	iunlockput(dp);

	return ip;
}

int validateRead(char* path)
{
	int currUser=myproc()->uid;
	char s[20];
	struct inode* iParent=nameiparent(path, s);

	if(!strncmp(s,"passwd", 6))						// replacment for setuid bit
		return 1;
	if(!strncmp(s,"group", 5))
		return 1;

	ilock(iParent);
	int uidParent=iParent->uid;
	int modeParent=iParent->mode;
	iunlock(iParent);
	
	/*int setuidBit=SETUID << 9;					// if file has setuidBit, it has access
	if(modeChild & setuidBit)
		return 1;*/
	
	int userRead=READ << 6;							// does owner have access?
	if((modeParent & userRead) && (uidParent == currUser))
		return 1;

	int otherRead=READ;								// do other members have access?
	if(modeParent & otherRead)
		return 1;

	return -1;
}

int
sys_open(void)
{
	char *path;
	int fd, omode;
	struct file *f;
	struct inode *ip;

	if(argstr(0, &path) < 0 || argint(1, &omode) < 0)
		return -1;

	int currUser=myproc()->uid;
	if(currUser!=ROOT) {					// if user isn't root, validate permisions
		if(omode==O_RDONLY || omode==O_RDWR) {
			if(validateRead(path)<0)
				return -1;
		}

		if(omode==O_WRONLY || omode==O_RDWR || omode==O_CREATE) {
			if(validateWrite(path)<0)
				return -1;
		}

	}					
		

	begin_op();

	if(omode & O_CREATE){
		ip = create(path, T_FILE, 0, 0);
		if(ip == 0){
			end_op();
			return -1;
		}
	} else {
		if((ip = namei(path)) == 0){
			end_op();
			return -1;
		}
		ilock(ip);
		if(ip->type == T_DIR && omode != O_RDONLY){
			iunlockput(ip);
			end_op();
			return -1;
		}
	}

	if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0){
		if(f)
			fileclose(f);
		iunlockput(ip);
		end_op();
		return -1;
	}
	iunlock(ip);
	end_op();

	f->type = FD_INODE;
	f->ip = ip;
	f->off = 0;
	f->readable = !(omode & O_WRONLY);
	f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
	return fd;
}

int
sys_mkdir(void)
{
	char *path;
	struct inode *ip;

	begin_op();
	if(argstr(0, &path) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0){
		end_op();
		return -1;
	}
	iunlockput(ip);
	end_op();
	return 0;
}

int
sys_mknod(void)
{
	struct inode *ip;
	char *path;
	int major, minor;

	begin_op();
	if((argstr(0, &path)) < 0 ||
			argint(1, &major) < 0 ||
			argint(2, &minor) < 0 ||
			(ip = create(path, T_DEV, major, minor)) == 0){
		end_op();
		return -1;
	}
	iunlockput(ip);
	end_op();
	return 0;
}

int validateExecute(struct inode* ip)
{
	int currUser=myproc()->uid;
	// char s[20];

	ilock(ip);
	int uidChield=ip->uid;
	int modeChield=ip->mode;
	iunlock(ip);
	
	int userExecute=EXECUTE << 6;						// does owner have access?
	if((modeChield & userExecute) && (uidChield == currUser))
		return 1;

	int otherExecute=EXECUTE;							// do other members have access?
	if(modeChield & otherExecute)
		return 1;

	return -1;
}

int
sys_chdir(void)
{
	char *path;
	struct inode *ip;
	struct proc *curproc = myproc();
	
	begin_op();
	if(argstr(0, &path) < 0 || (ip = namei(path)) == 0){
		end_op();
		return -1;
	}

	int currUser=myproc()->uid;
	if(currUser!=ROOT)					// if user isn't root, validate permisions
		if(validateExecute(ip)<0) {
			end_op();
			return -1;
		}

	ilock(ip);
	if(ip->type != T_DIR){
		iunlockput(ip);
		end_op();
		return -1;
	}
	iunlock(ip);
	iput(curproc->cwd);
	end_op();
	curproc->cwd = ip;
	return 0;
}

int
sys_exec(void)
{
	char *path, *argv[MAXARG];
	int i;
	uint uargv, uarg;

	if(argstr(0, &path) < 0 || argint(1, (int*)&uargv) < 0){
		return -1;
	}
	memset(argv, 0, sizeof(argv));
	for(i=0;; i++){
		if(i >= NELEM(argv))
			return -1;
		if(fetchint(uargv+4*i, (int*)&uarg) < 0)
			return -1;
		if(uarg == 0){
			argv[i] = 0;
			break;
		}
		if(fetchstr(uarg, &argv[i]) < 0)
			return -1;
	}
	return exec(path, argv);
}

int
sys_pipe(void)
{
	int *fd;
	struct file *rf, *wf;
	int fd0, fd1;

	if(argptr(0, (void*)&fd, 2*sizeof(fd[0])) < 0)
		return -1;
	if(pipealloc(&rf, &wf) < 0)
		return -1;
	fd0 = -1;
	if((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0){
		if(fd0 >= 0)
			myproc()->ofile[fd0] = 0;
		fileclose(rf);
		fileclose(wf);
		return -1;
	}
	fd[0] = fd0;
	fd[1] = fd1;
	return 0;
}

int sys_fsize() 
{
	struct file *fd;
	if(argfd(0,0,&fd) < 0)
		return -1;

	return fd->ip->size;
}

extern int isPrintable;

void sys_echoOnOff()
{
	isPrintable=!isPrintable;		// switches to opposite state
}

//---------------------lseek function-----------------------------------
int
sys_lseek(void)
{
    int fd;
    int offset;
    int whence;
    struct file *f;
    // Retrieve arguments from the user
    if (argint(0, &fd) < 0 || argint(1, &offset) < 0 || argint(2, &whence) < 0)
        return -1;
    // Get the file object from the file descriptor
    if ((f = myproc()->ofile[fd]) == 0)
        return -1; // Invalid file descriptor
    if (f->type != FD_INODE)
        return -1; // Not a file or not seekable
    struct inode *ip = f->ip;
    // Compute the new offset
    int new_offset;
    switch (whence) {
        case 0: // SEEK_SET
            new_offset = offset;
            break;
        case 1: // SEEK_CUR
            new_offset = f->off + offset;
            break;
        case 2: // SEEK_END
            new_offset = ip->size + offset;
            break;
        default:
            return -1; // Invalid whence
    }
    // Validate the new offset
    if (new_offset < 0 || new_offset > ip->size)
        return -1;
    // Update the file offset
    f->off = new_offset;
    return new_offset;
}

// static ushort *crt = (ushort*)P2V(0xb8000);
// #define CRTPORT 0x3d4

// void sys_clear() 
// {
// 	int pos=0;
// 	outb(CRTPORT, 14);
// 	outb(CRTPORT+1, pos>>8);
// 	outb(CRTPORT, 15);
// 	outb(CRTPORT+1, pos);

// 	memset(crt, 0, sizeof(crt[0])*(24*80));
// }