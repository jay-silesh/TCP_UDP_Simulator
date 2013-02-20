#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#define PORT "21350" // the port STORES will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold

/* PORT NOs FOR THE WAREHOUSE TO TAKE */
char port_self[2][100]={"31350","32350"};
char port_write[1][100]={"5350"};


/* STRUCTURE DEFINATION WHICH IS LIKE THE TRUCK(CONCEPTUALLY) */
typedef struct elements {
     int camera;
	 int laptop;
	 int printer;
	 int storeno;
} allelem;


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
	/***************************************************/
}


/* THIS FUNCTION IS USED TO OBTAIN THE IPADDRESS BEING USED */
/* INPUT PARAMETER IS THE SOCKET NO */

in_port_t get_in_port(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return (((struct sockaddr_in*)sa)->sin_port);
    }

    return (((struct sockaddr_in6*)sa)->sin6_port);
}

void initstruct(allelem *e)
{
	/* INITIALIZING THE STRUCTURE IN THE WAREHOUSE */
	e->camera=0;
	e->laptop=0;
	e->printer=0;
	e->storeno=0;
}


void staticupdate(allelem *q,allelem * ele)
{
	/* ASSIGNS THE STUCTURE'S PARAMTERS WHICH THE STRUCT PASSED AS "q" */
	ele->camera=ele->camera+q->camera;
	ele->laptop=ele->laptop+q->laptop;
	ele->printer=ele->printer+q->printer;
}



void update_ele(allelem *q,allelem * ele)
{
	/* UPDATES THE STUCTURE'S (ele'S) PARAMTERS WHICH THE STRUCT PASSED AS "q" */
	
	ele->camera=q->camera;
	ele->laptop=q->laptop;
	ele->printer=q->printer;
}

char *getipadd(int ss)
{
	socklen_t len;
	struct sockaddr_storage addr;
	char *ipstr=malloc(1000);
	int port;
	len = sizeof addr;
	getpeername(ss, (struct sockaddr*)&addr, &len);
	struct sockaddr_in *s = (struct sockaddr_in *)&addr;
	port = ntohs(s->sin_port);
	inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
	
	return ipstr;
}

/* HANDLES THE CRTL+C SIGNAL TO THE PROCESS BY KILLING THE PROCESS AND NOT LEAVING PROCESS WITH THE PROTNO*/
void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
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


int main(void)
{
	allelem ele;
	
	int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p,*selfinfo;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	char s1[INET6_ADDRSTRLEN];
	int rv;
	int x=0;
	
	initstruct(&ele);
	
	/*INITIALIZING THE MEMBERS OF THE COMPLETE STRUCTURE*/
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP
	if ((rv = getaddrinfo("localhost", PORT, &hints, &servinfo)) != 0)  
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	
	
	// loop through all the results and bind to the first we can
		/* CREATING A SOCKET */
		if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,servinfo->ai_protocol)) == -1) 
		{
			perror("server: socket");
			//continue;
		}
		/* ERROR CHECKING FOR THE SOCKET */
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1)
		{
			perror("setsockopt");
			exit(1);
		}
	
		/* BINDING THE SOCKET TO A PORT NO ON WHICH IT HAS TO WAIT FOR THE STOREs TO CONNECT */
		if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("server: bind");
			//continue;
		}
	
	if (servinfo == NULL)
	{
		fprintf(stderr,"server: failed to bind\n");
		return 2;
	}
	/*******************************************************************************************/
		void *addr;
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)servinfo->ai_addr;
		addr = &(ipv4->sin_addr);
		inet_ntop(servinfo->ai_family, addr, s, sizeof s);

	/*******************************************************************************************/
	printf("\nPhase 1: The central warehouse has TCP port number %d and IP address %s ",sock_to_port(sockfd),  s  );
	
	
	freeaddrinfo(servinfo); // all done with this structure
	if (listen(sockfd, BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	/* HANDLES THE CRTL+C SIGNAL AS TO WHAT ACTION IS TO BE TAKEN*/
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}

	int wc=4;
	while(wc!=0)
	{	
		sin_size = sizeof their_addr;
		/* WAITING FOR A STORE TO CONNECT HERE ... */
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1)
		{
			perror("Accept");
			wc--;
			continue;
		}

		inet_ntop(their_addr.ss_family,
		get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
		
		int numbytes;
		void *recvstruct=malloc(256);
		/* TRYING TO RECIEVE THE DATA SENT FROM THE STOREs */
		if ((numbytes = recv(new_fd,recvstruct,256, 0)) == -1) 
		{
			perror("recv");
			exit(1);
		}
		
		allelem *temp_ele=(allelem *)recvstruct;
		staticupdate(temp_ele,&ele);
		printf("\nPhase 1: The central warehouse received information from store%d",temp_ele->storeno);
		close(new_fd);
		wc--;
	}
	close(sockfd); 
	printf("\nEnd of Phase 1 for the central warehouse");
	
	
	/******************************************************************************************************************/
		allelem tempvals;
		/* INITIALIZING THE TRUCK VECTORE FROM THE WAREHOUSE TO BE SENT TO THE STORES */
		
		if(ele.camera < 0)
			ele.camera= ele.camera*-1;
		else
			ele.camera=0;
		
		if(ele.laptop < 0)
			ele.laptop= ele.laptop*-1;
		else
			ele.laptop=0;
	
		if(ele.printer < 0)
			ele.printer= ele.printer*-1;
		else
			ele.printer=0;
	
//		update_ele(&tempvals,&ele);
	
/**********************************************************************************************************/
/*					sending the truck vector to the first client...										 */

	/*INITIALIZING THE MEMBERS OF THE COMPLETE STRUCTURE*/
	
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
	//hints.ai_flags = AI_PASSIVE;
	/* HERE THE port_write IS THE PORT TO WHICH PORT WE WANT TO WRITE TO.. */
    if ((rv = getaddrinfo("localhost", port_write[0], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
	
	/* HERE THE port_self IS THE PORT TO WHICH PORT WE WANT TO ASSIGN FOR  THE SOCKET.. */
	if ((rv = getaddrinfo("localhost", port_self[0], &hints, &selfinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
		/*CREATING THE SOCKET */
        if ((sockfd = socket(selfinfo->ai_family, selfinfo->ai_socktype,selfinfo->ai_protocol)) == -1) 
		{
            perror("talker: socket");
         //   continue;
        }
		/* WE ARE BINDING THE self_port AND THE SOCKET */
		if (bind(sockfd, selfinfo->ai_addr, selfinfo->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("server: bind");
			//continue;
		}
		
    
    if (servinfo == NULL) 
	{
        fprintf(stderr, "talker: failed to bind socket\n");
        return 2;
    }
	
	void *addr1;
	struct sockaddr_in *ipv41 = (struct sockaddr_in *)selfinfo->ai_addr;
	addr1 = &(ipv41->sin_addr);
	inet_ntop(selfinfo->ai_family, addr1, s1, sizeof s1);

	
	printf("\nPhase 2: The central warehouse has UDP port number %d and IP address %s",ntohs(get_in_port((struct sockaddr *)selfinfo->ai_addr)),s1);
	printf("\nPhase 2: Sending the truck-vector to outlet store store_1. The truck vector value is <%d,%d,%d>.",ele.camera,ele.laptop,ele.printer);
	
	int numbytes;
	if ((numbytes = sendto(sockfd,&ele,256, 0,servinfo->ai_addr, servinfo->ai_addrlen)) == -1) 
	{
        perror("talker: sendto");
        exit(1);
    }
	
	freeaddrinfo(selfinfo);
    freeaddrinfo(servinfo);
	
    close(sockfd);	
	/*******************************************************************************************************************/
	//struct sockaddr_storage their_addr;
    socklen_t addr_len;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

	/* HERE THE port_self IS THE PORT TO WHICH PORT WE WANT TO ASSIGN FOR  THE SOCKET.. */
	
    if ((rv = getaddrinfo(NULL, port_self[1], &hints, &servinfo)) != 0)
	{
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
		/*CREATING THE SOCKET */
        if ((sockfd = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol)) == -1) 
		{
            perror("SERVER: socket");
          
        }
		/* WE ARE BINDING THE self_port AND THE SOCKET */
		
        if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) 
		{
            close(sockfd);
            perror("SERVER: bind");
          
        }

    if (servinfo == NULL) 
	{
        fprintf(stderr, "SERVER: failed to bind socket\n");
        return 2;
    }
	printf("\nPhase 2: The central warehouse has UDP port number %d and IP address %s",ntohs(get_in_port((struct sockaddr *)servinfo->ai_addr)),getipadd(sockfd));
	
	addr_len = sizeof their_addr;
	void *recele=malloc(256);
	/* TRYING TO RECIEVE THE DATA SENT FROM THE STORE4 */
		
    if ((numbytes = recvfrom(sockfd,recele, 256 , 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) 
	{
        perror("recvfrom");
        exit(1);
    }
	allelem *temp_ele=(allelem *)recele;
	/* UPDATES THE STRUCTURE OF THE WAREHOUSE */
	update_ele(temp_ele,&ele);
	
  	printf("\nPhase 2: The final truck-vector is received from the outlet store store_4, the truck-vector value is: <%d,%d,%d>",ele.camera,ele.laptop,ele.printer);
	printf("\nEnd of Phase 2 for the central warehouse\n\n");
	/* TEARING THE CONNECTION COMPLETELY */
	close(sockfd);
	freeaddrinfo(servinfo);
	
	return 0;
}
