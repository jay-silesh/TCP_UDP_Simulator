#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define PORT "21350" //  the port of the Warehouse
#define MAXDATASIZE 100 

/* PORT NOs FOR THE STORE3  TO USE AND SEND */
char port_read[2][100]={"13350","15350"};
char port_write[2][100]={"17350","19350"};
char port_self[2][100]={"14350","16350"};

/* STRUCTURE DEFINATION WHICH IS LIKE THE TRUCK(CONCEPTUALLY) */
typedef struct elements {
    int camera;
	int laptop;
	int printer;
	int storeno;
} allelem;

/* THIS FUNCTION IS USED TO OBTAIN THE IPADDRESS BEING USED */
/* INPUT PARAMETER IS THE SOCKET NO */

//get the port no of IPV4 or IPV6 
in_port_t get_in_port(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return (((struct sockaddr_in*)sa)->sin_port);
    }

    return (((struct sockaddr_in6*)sa)->sin6_port);
}
void staticupdate(allelem *q,allelem * ele)
{/* ASSIGNS THE STUCTURE'S PARAMTERS WHICH THE STRUCT PASSED AS "q" */
	ele->camera=ele->camera+q->camera;
	ele->laptop=ele->laptop+q->laptop;
	ele->printer=ele->printer+q->printer;
}

void update_ele(allelem *q,allelem * ele)
{/* UPDATES THE STUCTURE'S (ele'S) PARAMTERS WHICH THE STRUCT PASSED AS "q" */
	
	ele->camera=q->camera;
	ele->laptop=q->laptop;
	ele->printer=q->printer;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) 
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


/* THIS FUNCTION IS USED TO OBTAIN THE PORT NO ON WHICH THE SOCKET IS BEING USED */
/* INPUT PARAMETER IS THE SOCKET NO */
int sock_to_port(int sock)
{

/**************************************************/
	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	if (getsockname(sock, (struct sockaddr *)&sin, &len) == -1)
    perror("getsockname");
	else
    return ntohs(sin.sin_port);
	//printf("port number %d\n", ntohs(sin.sin_port));
	
	/***************************************************/
}


void initstruct(allelem *e)
{
	/* INITIALIZING THE STRUCTURE IN THE WAREHOUSE */
	e->camera=0;
	e->laptop=0;
	e->printer=0;
	e->storeno=0;
}

/*Computations for the truck that is being recieved and the truck that has to be sent */
void update_struct(allelem *q,allelem * ele)
{
	if(ele->camera > 0)
	{
		q->camera+=ele->camera;
		ele->camera=0;
	}
	else if(q->camera > 0 && ele->camera< 0)
	{	
		int real=ele->camera;
		int temp=q->camera;
		if((real*-1)==(temp))
		{
			q->camera=0;
			ele->camera=0;
		}
		else if( (-1*real) < temp)
		{
			ele->camera=0;
			q->camera+=real;
		}
		else if((-1*real) > temp)
		{
			q->camera=0;
			ele->camera+=temp;			
		}
	}
	
	
	if(ele->laptop > 0)
	{
		q->laptop+=ele->laptop;
		ele->laptop=0;
	}
	else if(q->laptop > 0 && ele->laptop< 0)
	{	
		int real=ele->laptop;
		int temp=q->laptop;
		if((real*-1)==(temp))
		{
			q->laptop=0;
			ele->laptop=0;
		}
		else if( (-1*real) < temp)
		{
			ele->laptop=0;
			q->laptop+=real;
		}
		else if((-1*real) > temp)
		{
			q->laptop=0;
			ele->laptop+=temp;			
		}
	}
	
	if(ele->printer > 0)
	{
		q->printer+=ele->printer;
		ele->printer=0;
	}
	else if(q->printer > 0 && ele->printer< 0)
	{	
		int real=ele->printer;
		int temp=q->printer;
		if((real*-1)==(temp))
		{
			q->printer=0;
			ele->printer=0;
		}
		else if( (-1*real) < temp)
		{
			ele->printer=0;
			q->printer+=real;
		}
		else if((-1*real) > temp)
		{
			q->printer=0;
			ele->printer+=temp;			
		}
	}
	
	
	

}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;
	char buf[2000];
	struct addrinfo hints, *servinfo, *p,*selfinfo;
	int rv;
	char s[INET6_ADDRSTRLEN];
	allelem ele;
	initstruct(&ele);
	char s1[INET6_ADDRSTRLEN];
	char s2[INET6_ADDRSTRLEN];	
	
	/*INITIALIZING THE MEMBERS OF THE COMPLETE STRUCTURE*/	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;
	if ((rv = getaddrinfo("localhost", PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		/* CREATING A SOCKET */
		
		if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
		{
			perror("client: socket");
			continue;
		}	
		/* connectin to the socket of the server's */
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
		{
			close(sockfd);
			perror("client: connect");
			continue;
		}
	
	
		break;
	}
	
	if (p == NULL) 
	{
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}
	void *addr;
	struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
	addr = &(ipv4->sin_addr);
	inet_ntop(p->ai_family, addr, s, sizeof s);
	printf("\nPhase 1: store_3 has TCP port number %d and IP address %s",sock_to_port(sockfd),s);
	
		/* freeing the structures */
	freeaddrinfo(servinfo);
	freeaddrinfo(p);
	
	
	/* Opening the file for reading the store's input data... */
	/* the files that should be opened for the store3 */
	FILE *fd;
	fd = fopen("Store-3.txt", "r");
	if (fd == NULL)
	{
		fprintf(stderr, "Cannot open for reading.\n");
		exit(0);
	}
	
	
	int cit=0;
	/* get the complete information from the file */
	while(fgets(buf, 10000, fd)!=NULL)
	{	
			char  c;
			int is=0,id=0;
			char dst[1000],dstv[1000];
			char *qq,*ww;
			qq=strtok(buf," ");
			ww=strtok(NULL,"\n");
			cit++;
			if ( cit==1) 
			{
				ele.camera=atoi(ww);
			}
			else if ( cit==2) 
			{
				ele.laptop=atoi(ww);
			}
			else if ( cit==3) 
			{
				ele.printer=atoi(ww);
			
			}
			
	}
	ele.storeno=3;
	printf("\nPhase 1: The outlet vector<%d,%d,%d> for store_3 has been sent to the central warehouse",ele.camera,ele.laptop,ele.printer);
	
	if (send(sockfd,&ele,256, 0) == -1)
			perror("send");
	
	close(sockfd);
	fclose(fd);
printf("\nEnd of Phase 1 for store_3");
/**********************************************************************************************************/
/*					waiting for the truck vector from the server...										 */
int comp_counter=0;



while(comp_counter!=2)
{
   struct sockaddr_storage their_addr;
    socklen_t addr_len;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
	/* HERE THE port_write IS THE PORT TO WHICH PORT WE WANT TO READ FROM.. */
   
    if ((rv = getaddrinfo("localhost", port_read[comp_counter], &hints, &servinfo)) != 0)
	{
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

        if ((sockfd = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol)) == -1) 
		{
            perror("Client 1: socket");
          
        }
		/* WE ARE BINDING THE self_port AND THE SOCKET */
        if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) 
		{
            close(sockfd);
            perror("Client 1: bind");
          
        }

    if (servinfo == NULL) 
	{
        fprintf(stderr, "Client 2: failed to bind socket\n");
        return 2;
    }

	void *addr1;
	struct sockaddr_in *ipv41 = (struct sockaddr_in *)servinfo->ai_addr;
	addr1 = &(ipv41->sin_addr);
	inet_ntop(servinfo->ai_family, addr1, s1, sizeof s1);

	
   printf("\nPhase 2: Store_3 has UDP port %d and IP address %s",sock_to_port(sockfd),s1);
   
    addr_len = sizeof their_addr;
	void *recele=malloc(256);
    if ((numbytes = recvfrom(sockfd,recele, 256 , 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) 
	{
        perror("recvfrom");
        exit(1);
    }
	allelem *temp_ele=(allelem *)recele;
	printf("\nPhase 2: Store_3 received the truck-vector<%d,%d,%d> from the Store_2",temp_ele->camera,temp_ele->laptop,temp_ele->printer);
   
	update_struct(temp_ele,&ele);
	
   close(sockfd);
	freeaddrinfo(servinfo);

/**********************************************************************************************************/
/*					sending the truck vector to the second client...										 */
	memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
	//hints.ai_flags = AI_PASSIVE;
	 
    if ((rv = getaddrinfo("localhost", port_write[comp_counter], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
	/* HERE THE port_self IS THE PORT TO WHICH PORT WE WANT TO ASSIGN FOR  THE SOCKET.. */
	
	if ((rv = getaddrinfo("localhost",port_self[comp_counter], &hints, &selfinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
	
	 if ((sockfd = socket(selfinfo->ai_family, selfinfo->ai_socktype,selfinfo->ai_protocol)) == -1) 
	 {
		perror("talker: socket");
   	 }
	 	 /*BINDING THE self_port TO THE SOCKET */
	if (bind(sockfd, selfinfo->ai_addr, selfinfo->ai_addrlen) == -1)
	{
		close(sockfd);
		perror("server: bind");
		//continue;
	}
	
	void *addr2;
	struct sockaddr_in *ipv42 = (struct sockaddr_in *)selfinfo->ai_addr;
	addr2 = &(ipv42->sin_addr);
	inet_ntop(selfinfo->ai_family, addr2, s2, sizeof s2);

	
	printf("\nPhase 2: Store_3 has UDP port %d and IP address %s",sock_to_port(sockfd),s2);
	printf("\nPhase 2: The updated truck-vector<%d,%d,%d> has been sent to store_4",temp_ele->camera,temp_ele->laptop,temp_ele->printer);
	printf("\nPhase 2: Store_3 updated outlet-vector is <%d,%d,%d>",ele.camera,ele.laptop,ele.printer);
	
	
	if ((numbytes = sendto(sockfd,temp_ele,256, 0,servinfo->ai_addr, servinfo->ai_addrlen)) == -1) 
	{
        perror("talker: sendto");
        exit(1);
    }
	/* TEARIN DOWN THE CONNECTION COMPLETELY */
    freeaddrinfo(servinfo);
	freeaddrinfo(selfinfo);
    close(sockfd);	
	comp_counter++;
	
}
printf("\nEnd of Phase 2 for store_3\n\n");
	return 0;
}
