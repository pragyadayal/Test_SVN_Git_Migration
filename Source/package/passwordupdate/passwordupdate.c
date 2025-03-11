#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/stat.h>
#include <fcntl.h>
#define PASSWORD_LEN 12

#define SET_ROOT_PASSWORD

static char debugOnPasswd = 0;

enum 
{
	ROOT,
	SE,
	INVALID
};

void strrev(char *mac, char *rev_mac, unsigned int len);

void strrev(char *mac, char *rev_mac, unsigned int len)
{
	unsigned char index = 0;
	len--;
	for (; len > 0;)
	{
		rev_mac[index++] = mac[--len];
// 		printf("rev_mac:%c, mac:%c, %d\n",rev_mac[index-1], mac[len], len);
	}
	rev_mac[index] = '\0';
//  printf("rev_mac:%s\n",rev_mac[0]);
}

int main(int argc, const char* argv[])
{
	int sys_ret, exit_status, ret;
	int fd,i;
	unsigned char user;
	unsigned char cmd_buffer[250];
	unsigned char mac[18], rev_mac[18];
	unsigned char product_name[20];
	
	char passwd[52] = "$AM&Bs04@2W,vYa@gm,3h@g<>{]^-9im#8";
	char actual_password[44] = {};
	char password_file[] = "/root/password.txt";
	
	struct stat stat_file;
	
	// Validate argument
	if (argv[1] == NULL || argv[2] == NULL )
	{
		printf("matrix_password: Error: Please pass some argument.\n");
		printf("matrix_password: Error: First argument should be user(se/root) and second should be MAC address in all capital.\n");
        return -EINVAL;
	}
	
	if ( 0 == strcmp(argv[1], "root"))
	{
		user = ROOT;
	}
	else if ( 0 == strcmp(argv[1], "se"))
	{
		user = SE;
	}
	else
	{
		user = INVALID;
		printf("matrix_password: Error: Please pass valid argument.\n");
	}
	
	// Copy the MAC address from arguments
    memset(mac,0,sizeof(mac));
    strcpy(mac,argv[2]);
    
    
    if(argv[3] != NULL )
    {
		memset(product_name,0,sizeof(product_name));
		strcpy(product_name,argv[3]);
	}
	else
	{
		memset(product_name,0,sizeof(product_name));
	}
	
    //Validate the mac address
    
    for (i = 0; i < 17; i++)
    {
        if (mac[i] >= 'a' && mac[i] <= 'z')
        {
            mac[i]=mac[i]-32;
        }
    }
    printf("mac addres = %s",mac);
    
    for (i = 2; i < 17; i = i+3)
    {
        if (mac[i] != ':')
        {
            printf("Matrix: MAC address not valid.\n");
            return (1);
        }
    }
    
    // go ahead with processing mac address for changing password
    
    printf("matrix_password: Info: MAC address is: %s. \n",mac);
    
    if (user == ROOT)
    {
        strrev(mac, rev_mac, sizeof(mac));
        memcpy(mac, rev_mac, sizeof(mac));
    }
		
    // Generate encrypted the mac address file.
    memset(cmd_buffer,0,sizeof(cmd_buffer));
    memcpy(passwd+34, mac, sizeof(mac));
		
    sprintf(cmd_buffer, "echo -p \"%s\" | openssl dgst -sha256 -binary | openssl base64 > %s",passwd, password_file);

    sys_ret = system(&cmd_buffer[0]);
    // Get exit status
    exit_status = WEXITSTATUS(sys_ret);
    printf("exit_status: %d\n", exit_status);
    if ( 0 !=  exit_status)
    {
        printf("openssl command Failed: %d\n", exit_status);
        return (exit_status);
    }
    
    memset(cmd_buffer,0,sizeof(cmd_buffer));
    memset(actual_password,0,sizeof(actual_password));
    memset(passwd,0,sizeof(passwd));
            
    fd = open(password_file, O_CREAT | O_RDWR);
    if (fd < 0)
    {
        printf("matrix_password: Error: while opening file: %s, %s\n", password_file, strerror(errno));
    }
    if (0 > read (fd, actual_password, sizeof(actual_password)))
    {
        printf("matrix_password: Error: while reading from file: %s, %s\n", password_file, strerror(errno));
    }
    close(fd);
		
    system("rm -f /root/password.txt");


	if(strlen(product_name) == 0)
	{
		if (user == ROOT)
		{
			sprintf(cmd_buffer, "echo -e \"%s\\n%s\" | passwd root ", actual_password, actual_password);	
		}
		else
		{
			sprintf(cmd_buffer, "echo -e \"%s\\n%s\" | passwd se ", actual_password, actual_password);
		}
	}
	else if((0 == strcmp(product_name, "pvr")) || (0 == strcmp(product_name, "wldoor")))
	{
		if (user == ROOT)
		{
			sprintf(cmd_buffer, "/usr/sbin/passwdchange.sh %s %s", "root", actual_password);
		}
		else
		{
			sprintf(cmd_buffer, "/usr/sbin/passwdchange.sh %s %s", "se", actual_password);
		}
	}
	else
	{
		printf("Not supported product name enter %s\n", product_name);
        return (-1);
	}
		
    sys_ret = system(&cmd_buffer[0]);
    // Get exit status
    exit_status = WEXITSTATUS(sys_ret);
    if ( 0 !=  exit_status)
    {
        printf("changing password for %s Failed with exit status: %d\n", argv[1], exit_status);
        return (exit_status);
    }
	return (0);
}
