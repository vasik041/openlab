
#include "main.h"
#include "data.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>


#define RECV_FLAGS	0


int get_header(int clientSocket,char *strHeader,int nMaxLen)
{
	int nLen=0,state=0,nBytesReceived=0;

	while (nLen < nMaxLen && state != 4) {
		nBytesReceived=recv(clientSocket,&strHeader[nLen],1,RECV_FLAGS);
		if (nBytesReceived <= 0) {
			return -1;
		}

		if  ((strHeader[nLen] == '\r') && ((state == 0) || (state == 2))) state++;
		else if ((strHeader[nLen] == '\n') && ((state == 1) || (state == 3))) state++;
		else state = 0;
		nLen++;
	}
	strHeader[nLen]='\0';
	return nLen;
}


void send_header(int sock,int result,const char *resmsg,int clen,const char *mime)
{
char str[256] = {0};
	int n = snprintf(str,sizeof(str),"HTTP/1.1 %d %s\r\n"
				"Content-Type: %s\r\n"
				"Connection: close\r\n"
				"Content-Length: %d\r\n"
				"EXT:\r\n"
				"Server: LZs\r\n"
				"\r\n",
				result,
				resmsg,
				mime,
				clen);
	send(sock,str,n,0);
}


char *load_file(char *fnm1,int *size)
{
FILE *f;
char *p;
char fnm[256];
	snprintf(fnm,sizeof(fnm),"web/%s",fnm1);
	f = fopen(fnm,"rb+");
	if(!f) return NULL;
	fseek(f,0,SEEK_END);
	*size = ftell(f);
	fseek(f,0,SEEK_SET);
	p = (char *)malloc(*size);
	if(p) {
		fread(p,1,*size,f);
	}
	fclose(f);
	return p;
}


typedef struct {
	const char *ext;
	const char *mime;
} mime_t;


mime_t mime[] = {
	{ ".htm", "text/html" },
	{ ".jpg", "image/jpeg" }
};


int ends_with(const char *str,const char *pat)
{
int slen = strlen(str);
int plen = strlen(pat);
	if(plen > slen) return 0;
	return (strncmp(str+slen-plen,pat,plen) == 0) ? 1 : 0;
}


const char *get_mime(const char *strFile)
{
int i;
	for(i=0; i < NELEM(mime); i++) {
		if(ends_with(strFile,mime[i].ext)) return mime[i].mime;
	}
	return "text/plain";
}


void send_error(int sock,int errnum,const char *errmsg)
{
	int msglen = strlen(errmsg);
	send_header(sock,errnum,errmsg,msglen,"text/plain");
	send(sock,errmsg,msglen,0);
}


void send_file(int sock,char *strHeader)
{
char strFile[256] = "index.htm";

	if(strncmp(strHeader,"GET /",5) == 0) {
		strHeader += 5;
		char *end = strchr(strHeader,' ');
		if(end) *end = '\0';
		if(*strHeader)
			strncpy(strFile,strHeader,sizeof(strFile));
	}

	int sz = 0;
	char *p = load_file(strFile,&sz);
	if(!p) {
		send_error(sock,404,"File not found");
		return;
	}
	send_header(sock,200,"OK",sz,get_mime(strFile));
	send(sock,p,sz,0);
	free(p);
}


void process_request(int sock)
{
char strHeader[4096] = {0};
char resp[4096] = {0};
int rc;
	rc = get_header(sock,strHeader,sizeof(strHeader));
	if(rc > 0) {
		int len = parse_rest(strHeader,resp,sizeof(resp));
		if(len > 0) {
			send_header(sock,200,"OK",len,"application/json");
			send(sock,resp,len,0);
		} else {
			send_file(sock,strHeader);
		}
	}
	close(sock);
}


//DWORD WINAPI process_impl(LPVOID p)
void *process_impl(void *p)
{
int *ps = (int *)p;
int s = *ps;
	free(ps);
	process_request(s);
	return NULL;
}


void process(int s)
{
//HANDLE h;
//DWORD id;
	int *ps = (int *)malloc(sizeof(int));
	if(ps) {
		*ps = s;
//		h = CreateThread(NULL,0,process_impl,ps,0,&id);
//		if(h == INVALID_HANDLE_VALUE) {
//			printf("Can\'t create thread %d",GetLastError());
//		}

		pthread_t tid;
		if(pthread_create(&tid, NULL, process_impl, ps)) {
		    fprintf(stderr, "Error creating processing thread\n");
		}
	}
}

//=============================================================================

int server_socket=-1;


int server()
{
int server_socket;
struct sockaddr_in address;
socklen_t addrlen = sizeof(address);
 
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket < 0) {
		fprintf(stderr,"server socket error %d %s\n",errno,strerror(errno));
		return -1;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(8080);

	if (bind(server_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
		fprintf(stderr,"can\'t bind %d %s\n",errno,strerror(errno));
		return -1;
	}

	for(;;) {
	    int client_socket;
	    if(listen(server_socket, 5) < 0) {
		return -1;
	    }
	    if((client_socket = accept(server_socket, (struct sockaddr *) &address, &addrlen)) < 0) {    
		return -1;
	    }
	    process(client_socket);
	}
	close(server_socket);
}

//=============================================================================

//DWORD WINAPI start_impl(LPVOID p)
void *start_impl(void *p)
{
	server();
	return NULL;
}

//WSADATA wsaData;

void start_server(void)
{
//HANDLE h;
//DWORD id;
//int iResult;

	// Initialize Winsock
//	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
//	if (iResult != 0) {
//		printf("WSAStartup failed: %d\n", iResult);
//		return;
//	}

//	h = CreateThread(NULL,0,start_impl,NULL,0,&id);
//	if(h == INVALID_HANDLE_VALUE) {
//		printf("Can\'t create thread %d",GetLastError());
//	}

	pthread_t tid;
	if(pthread_create(&tid, NULL, start_impl, NULL)) {
	    fprintf(stderr, "Error creating server thread\n");
	}
}


void stop_server()
{
    close(server_socket);
}

