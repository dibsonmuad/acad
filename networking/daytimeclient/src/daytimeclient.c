#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
extern int errno;
int main(int argc, char* argv[]) {
    int sockfd, n;
    char recvline[100];
    struct sockaddr_in servaddr;

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        exit(-1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    if (getaddrinfo(argv[1], 13, ))
    struct hostent* hp = gethostbyname(argv[1]);
    for (int i=0; i< (hp->h_length > 0); i++) {
        printf("%s\n", inet_ntoa( *( struct in_addr* ) (hp->h_addr_list[i]) ));
    }

    int ret = inet_pton(AF_INET, inet_ntoa( *( struct in_addr* )(hp->h_addr_list[0])), &servaddr.sin_addr);

    if (ret < 0) {
        printf("inet_pton failed: %d", ret);
        exit(-2);
    }

    ret = connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
    if (ret < 0) {
        perror("connect failed");
        exit(-3);
    }
    while( (n = read(sockfd, recvline, 100)) > 0 ) {
        recvline[n] = 0;
        printf("%s\n", recvline);
    }
    if (n < 0)
        exit(-4);
    exit(0);
}
