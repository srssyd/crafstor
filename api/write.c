#include <stdio.h>
#include <string.h>
#include <time.h>
#include "glfs.h"
#include <stdlib.h>
#include "glfs-handles.h"
#include <sys/time.h>
#define L (1*1024 * 1024 * 1024 )

char *s;


void warm_up(glfs_t *fs){
	glfs_fd_t *fd;
	int i;
	fd = glfs_creat(fs, "/yk3", O_RDWR, 0644);
	for (i = 0; i < 2 ;i++)
		glfs_write(fd, s, L, 0);
	glfs_fsync(fd);
	glfs_close(fd);
}

const int size_count = 10;

int main(int argc, char *argv[])
{
	s=calloc(L,sizeof(char));
	glfs_t *fs2 = NULL;
	glfs_fd_t *fd = NULL;	
	int i;
	struct timeval start;
	struct timeval end;
	struct timeval res;
	struct timeval now;

	//memset(s,'a',L);

	fs2 = glfs_new("disperse-24-8");
	glfs_set_volfile_server(fs2, "tcp", "tfs06", 24007);
	glfs_set_logging(fs2, "/dev/null", 3);
	//	glfs_set_logging(fs2, "/dev/stderr", 1);
	glfs_init(fs2);


	//warm_up(fs2);


	if(argc==1)
		fd = glfs_creat(fs2, "/yk8", O_RDWR, 0644);
	else
		fd = glfs_creat(fs2,argv[1],O_RDWR, 0644);
	gettimeofday(&start,NULL);
	for (i = 0; i < size_count; i++) {
		gettimeofday(&now,NULL);
		printf("begin write %d. Timestamp: %u.%.6u\n", i,now.tv_sec,now.tv_usec);
		glfs_write(fd, s, L, 0);
	//	glfs_fsync(fd);
		printf("end write %d\n", i);
	}
	gettimeofday(&end,NULL);
	timersub(&end,&start,&res);
	printf("%u.%.6u cost for commit data.\n",res.tv_sec,res.tv_usec);
	glfs_fsync(fd);

	glfs_close(fd);
	gettimeofday(&end,NULL);
	timersub(&end,&start,&res);

	printf("%u.%u s for writring data with %d*%f GB\n",res.tv_sec,res.tv_usec,size_count,L/(1024*1024*1024.0));
	

	glfs_fini(fs2);
	return 0;
}
