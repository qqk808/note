#include <stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#include<linux/ioctl.h>
int fd;
int i=0;
char buff[4196];

#define HELLO_MAGIC 'k'  //当然我们也可以定义一个相应的头文件，把ioctl的cmd放进里面，然后再include进 来
#define SPI_CMD1      _IO(HELLO_MAGIC,0x1a)
#define HELLO_CMD2    _IO(HELLO_MAGIC,0x1b)

typedef struct IOCT_BAG
{
	unsigned int cmd;
	unsigned int send_len;
	unsigned int rev_len;
	unsigned char data[4196];
}*IOCT_BAG_P;
struct IOCT_BAG iobag;

int ioct_data_change(unsigned int cmd ,unsigned int sendlen,unsigned int revlen ,unsigned char *data)
{
	int i,ret;
	if((sendlen>4150)||(revlen>4150))
	{
		printf("data change len err!!!\n");
		return -1;
	}
	iobag.cmd=cmd;
	iobag.send_len=sendlen;
	iobag.rev_len=revlen;
	for(i=0;i<sendlen;i++)
		iobag.data[i]=data[i];
	ret = ioctl( fd, SPI_CMD1,&iobag);
	if ( ret == -1)
	{
		printf("ioctrl err!!!\n");
	}
	return ret;
}

void ioct_testsend()
{
	int i;
	unsigned char buff[4096];
	for(i=0;i<4096;i++)
	{
		buff[i]=i;
	}
	ioct_data_change(1,4096,4096,buff);

	//for(i=4090;i<4096;i++)
	{
		//printf("%d ",iobag.data[i]);
	}
	//printf("\nup is get data \n");
}

void main()
{
	int ret;
	printf("main run!\n");
	fd=open("/dev/chardev",O_RDWR,0);
	if(fd<=0)
	{
		printf("open err!");
		return ;
	}
	printf("open success!\n");
	while(1)
	{
		ioct_testsend();
		usleep(1000);
	}

	while(1)
	{
		sprintf(buff,"count i:%d",i++);
		
		ret=write(fd,buff,20);
		printf("Write:%s ret is :%d\n",buff,ret);
		sprintf(buff,"---------------------");
		ret=read(fd,buff,20);
		printf("Read:%s ret is :%d\n",buff,ret);
		sleep(1);
	} 
	
	printf("read over!\n");
	
}
