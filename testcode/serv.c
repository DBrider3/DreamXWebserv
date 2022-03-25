#define BUF_SIZE 1000
#define HEADER_FMT "HTTP/1.1 %d %s\nContent-Length: %ld\nContent-Type: %s\n\n"

#define NOT_FOUND_CONTENT       "<h1>404 Not Found</h1>\n"
#define SERVER_ERROR_CONTENT    "<h1>500 Internal Server Error</h1>\n"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>


/*
    @func   assign address to the created socket lsock(sd)
    @return bind() return value
*/
int bind_lsock(int lsock, int port) {
    struct sockaddr_in sin;

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);

    return bind(lsock, (struct sockaddr *)&sin, sizeof(sin));
}

/*
    @func   format HTTP header by given params
    @return 
*/
void fill_header(char *header, int status, long len, char *type) {
    char status_text[40];
    switch (status) {
        case 200:
            strcpy(status_text, "OK"); break;
        case 404:
            strcpy(status_text, "Not Found"); break;
        case 500:
        default:
            strcpy(status_text, "Internal Server Error"); break;
    }
    //status_text에 상태번호에 따른 문자열을 넣어줌
    sprintf(header, HEADER_FMT, status, status_text, len, type);
    //header에 문자열 서식(HEADER_FMT)에 따라 status, status_text, len, type를 넣어줌
}

/*
    @func   find content type from uri
    @return 
*/
void find_mime(char *ct_type, char *uri) {
    //Content-Type을 찾는 함수
    char *ext = strrchr(uri, '.');
    if (!strcmp(ext, ".html")) 
        strcpy(ct_type, "text/html");
    else if (!strcmp(ext, ".jpg") || !strcmp(ext, ".jpeg")) 
        strcpy(ct_type, "image/jpeg");
    else if (!strcmp(ext, ".png"))
        strcpy(ct_type, "image/png");
    else if (!strcmp(ext, ".css"))
        strcpy(ct_type, "text/css");
    else if (!strcmp(ext, ".js"))
        strcpy(ct_type, "text/javascript");
    else strcpy(ct_type, "text/plain");
}

/*
    @func handler for not found
    @return
*/
void handle_404(int asock) {
    char header[BUF_SIZE];
    fill_header(header, 404, sizeof(NOT_FOUND_CONTENT), "text/html");

    write(asock, header, strlen(header));
    write(asock, NOT_FOUND_CONTENT, sizeof(NOT_FOUND_CONTENT));
}

/*
    @func handler for internal server error
    @return
*/
void handle_500(int asock) {
    char header[BUF_SIZE];
    fill_header(header, 500, sizeof(SERVER_ERROR_CONTENT), "text/html");

    write(asock, header, strlen(header));
    write(asock, SERVER_ERROR_CONTENT, sizeof(SERVER_ERROR_CONTENT));
}

/*
    @func main http handler; try to open and send requested resource, calls error handler on failure
    @return
*/
void http_handler(int asock) {
    char header[BUF_SIZE];
    char buf[BUF_SIZE];

    if (read(asock, buf, BUF_SIZE) < 0) {
        //request message를 read함
        perror("[ERR] Failed to read request.\n");
        handle_500(asock); return;
    }

    char *method = strtok(buf, " ");
    char *uri = strtok(NULL, " ");
    if (method == NULL || uri == NULL) {
        perror("[ERR] Failed to identify method, URI.\n");
        handle_500(asock); return;
    }

    printf("[INFO] Handling Request: method=%s, URI=%s\n", method, uri);

    char safe_uri[BUF_SIZE];
    char *local_uri;
    struct stat st;

    strcpy(safe_uri, uri);
    if (!strcmp(safe_uri, "/"))
        strcpy(safe_uri, "/index.html");
    
    local_uri = safe_uri + 1; // '/'을 무시하기 위함
    if (stat(local_uri, &st) < 0) {
        //stat : 파일의 상태나 파일의 정보를 얻는 함수
        perror("[WARN] No file found matching URI.\n");
        handle_404(asock); return;
    }

    int fd = open(local_uri, O_RDONLY);
    if (fd < 0) {
        perror("[ERR] Failed to open file.\n");
        handle_500(asock); return;
    }

    int ct_len = st.st_size;
    char ct_type[40];
    find_mime(ct_type, local_uri);
    fill_header(header, 200, ct_len, ct_type);
    write(asock, header, strlen(header));
    //response의 헤더를 작성한 후 asock에게 보냄

    int cnt;
    while ((cnt = read(fd, buf, BUF_SIZE)) > 0)
        write(asock, buf, cnt);
    //body. 이미 길이가 알려진 단일 파일로 구성된 단일-리소스 본문을 전송(헤더 두개(Content-Type와 Content-Length)로 정의)
}

int main(int argc, char **argv) {
    int port, pid;
    int lsock, asock;

    struct sockaddr_in remote_sin;
    socklen_t remote_sin_len;

    if (argc < 2) {
        printf("Usage: \n");
        printf("\t%s {port}: runs mini HTTP server.\n", argv[0]);
        exit(0);
    }

    port = atoi(argv[1]);
    printf("[INFO] The server will listen to port: %d.\n", port);
    
    lsock = socket(AF_INET, SOCK_STREAM, 0);
    if (lsock < 0) {
        perror("[ERR] failed to create lsock.\n");
        exit(1);
    }

    if (bind_lsock(lsock, port) < 0) {
        perror("[ERR] failed to bind lsock.\n");
        exit(1);
    }

    if (listen(lsock, 10) < 0) {
        perror("[ERR] failed to listen lsock.\n");
        exit(1);
    }

    // to handle zombie process
    signal(SIGCHLD, SIG_IGN);

    while (1) {
        printf("[INFO] waiting...\n");
        asock = accept(lsock, (struct sockaddr *)&remote_sin, &remote_sin_len);
        if (asock < 0) {
            perror("[ERR] failed to accept.\n");
            continue;
        }

        pid = fork();
        if (pid == 0) {
            //자식 프로세스 (보통 자식 프로세스에서 작업)
            close(lsock); //서버의 소켓은 필요없으니 close함
            http_handler(asock); 
            close(asock); //다 사용한 클라이언트 소켓은 close함
            exit(0);
        }

        if (pid != 0)   { close(asock); } //부모 프로세스
        if (pid < 0)    { perror("[ERR] failed to fork.\n"); }
    }
}

