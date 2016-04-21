#include <stdio.h>
#include <string.h>
#include <time.h>
#include "glfs.h"
#include <stdlib.h>
#include "glfs-handles.h"
#include <sys/time.h>

char *s;
char *decoded;
/*
void warm_up(glfs_t *fs){
	glfs_fd_t *fd;
	int i;
	fd = glfs_creat(fs, "/yk3", O_RDWR, 0644);
	for (i = 0; i < 2 ;i++)
		glfs_write(fd, s, L, 0);
	glfs_fsync(fd);
	glfs_close(fd);
}
*/
static int seed = 2;
inline char fastrand(){
	seed = (214013 * seed + 2531011);
	return (seed>>16) & 0x7f;
}
const int size_count = 1;

int main(int argc, char *argv[])
{
	int i,j;
	seed = time(0);
	srand(time(0));
//	int L = 1<<30;
	int L = rand()%16384;
//	int L = 16;
//	printf("%d\n",L);
	s=calloc(L,sizeof(char));
	for(i=0;i<L;i++)
		s[i] = fastrand();
	decoded=calloc(L*size_count,sizeof(char));
	glfs_t *fs2 = NULL;
	glfs_fd_t *fd = NULL;	
	struct timeval start;
	struct timeval end;
	struct timeval res;
	struct timeval now;

	//memset(s,'a',L);

	fs2 = glfs_new("disperse-12-4");
	glfs_set_volfile_server(fs2, "tcp", "tfs00", 24007);
	glfs_set_logging(fs2, "/dev/null", 3);
	//	glfs_set_logging(fs2, "/dev/stderr", 1);
	glfs_init(fs2);


	//warm_up(fs2);

	const char * filePath = argc==1?"/yk3":argv[1];

	fd = glfs_creat(fs2, filePath, O_RDWR, 0644);
	printf("glfs:%p fd:%p\n",fs2,fd),

	gettimeofday(&start,NULL);
	for (i = 0; i < size_count; i++) {
		gettimeofday(&now,NULL);
		printf("begin write %d. Timestamp: %u.%.6u\n", i,now.tv_sec,now.tv_usec);
		int errno = glfs_write(fd, s, L, 0);
		printf("Write %d bytes.\n",errno);
		printf("end write %d\n", i);
	}
	gettimeofday(&end,NULL);
	timersub(&end,&start,&res);
	printf("%u.%.6u cost for commit data.\n",res.tv_sec,res.tv_usec);
	glfs_fsync(fd);
	glfs_close(fd);
// 	fs2 = glfs_new("disperse-24-8");
//	glfs_init(fs2);
	
	fd = glfs_open(fs2,filePath,0644);

	printf("Read %d bytes.\n",glfs_read(fd,decoded,L * size_count,0644));
	printf("Finish Read\n");
	glfs_close(fd);
	gettimeofday(&end,NULL);

	for(i=0;i<size_count;i++)
		printf("%d: %s\n",i,memcmp(s,decoded+L*i,L)==0?"ok":"error");
	

	glfs_fini(fs2);
	return 0;
}
