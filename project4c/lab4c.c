#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#ifdef BEAGLEBONE
#include <mraa/aio.h>
#include <mraa/gpio.h>
#endif

#include <getopt.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <math.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>

char id[10] = {0,0,0,0,0,0,0,0,0,0};
char* host = NULL;
FILE* file = NULL;
int port = -1;
int tcp = 0;
int volatile period = 1;
char volatile scale = 'F';
int volatile go = 1;
int volatile interrupt = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
SSL* sslClient = NULL;

void* readfd(void* fdpointer){
    char buffer[100];
    char* leftover = NULL;
    int fd = *(int*)fdpointer;
    int leftoverCount = 0;
    while(interrupt == 0){
        int output = 0;
        if(tcp == 0){
            output = SSL_read(sslClient, buffer, 99);
        } else {
            output = read(fd, buffer, 99);
        }
        if(output == 0){
            continue;
        } else if(output < 0){
            if(errno == EAGAIN){
                continue;
            }
            pthread_mutex_lock(&lock);
            fprintf(stderr, "read: %s\n", strerror(errno));
            pthread_mutex_unlock(&lock);
            if(leftover != NULL){
                free(leftover);
            }
            fclose(file);
            exit(2);
        } else if(output == 0){
            if(tcp == 0){
                fprintf(stderr, "SSL read failed\n");
                exit(2);
            }
            continue;
        }
        if(leftover == NULL){
            leftover = malloc((output+1)*sizeof(char));
            if(leftover == NULL){
                pthread_mutex_lock(&lock);
                fprintf(stderr, "malloc: %s\n", strerror(errno));
                pthread_mutex_unlock(&lock);
                if(leftover != NULL){
                    fclose(file);
                }
                exit(2);
            }
        } else {
            leftover = realloc(leftover, (output+leftoverCount+1)*sizeof(char));
            if(leftover == NULL){
                pthread_mutex_lock(&lock);
                fprintf(stderr, "realloc: %s\n", strerror(errno));
                pthread_mutex_unlock(&lock);
                free(leftover);
                fclose(file);
                exit(2);
            }
        }
        int i = 0;
        for(i = 0; i < output; i++){
            if(buffer[i] == '\n'){
                leftover[leftoverCount] = '\0';
                if(leftoverCount < 3){
                    pthread_mutex_lock(&lock);
                    fprintf(stderr, "Invalid input: %s\n", leftover);
                    fprintf(file, "%s\n", leftover);
                    fflush(file);
                    pthread_mutex_unlock(&lock);
                    leftoverCount = 0;
                    continue;
                }
                //parse 3 length commands
                if(leftover[0] == 'O' && leftover[1] == 'F' && leftover[2] == 'F'){
                    if(leftoverCount != 3){
                        pthread_mutex_lock(&lock);
                        fprintf(stderr, "Invalid input: %s\n", leftover);
                        fprintf(file, "%s\n", leftover);
                        fflush(file);
                        pthread_mutex_unlock(&lock);
                        leftoverCount = 0;
                        continue;
                    } else {
                        pthread_mutex_lock(&lock);
                        fprintf(file, "OFF\n");
                        fflush(file);
                        pthread_mutex_unlock(&lock);
                    }
                    interrupt = 1;
                    free(leftover);
                    return NULL;
                } else if(leftover[0] == 'L' && leftover[1] == 'O' && leftover[2] == 'G'){
                    pthread_mutex_lock(&lock);
                    fprintf(file, "%s\n", leftover);
                    fflush(file);
                    pthread_mutex_unlock(&lock);
                } else if(leftoverCount < 4){
                    pthread_mutex_lock(&lock);
                    fprintf(stderr, "Invalid input: %s\n", leftover);
                    fprintf(file, "%s\n", leftover);
                    fflush(file);
                    pthread_mutex_unlock(&lock);
                    leftoverCount = 0;
                    continue;
                }
                //parse 4 length commands
                else if(leftover[0] == 'S' && leftover[1] == 'T' && leftover[2] == 'O' && leftover[3] == 'P'){
                    go = 0;
                    pthread_mutex_lock(&lock);
                    fprintf(file, "STOP\n");
                    fflush(file);
                    pthread_mutex_unlock(&lock);
                } else if(leftoverCount < 5){
                    pthread_mutex_lock(&lock);
                    fprintf(stderr, "Invalid input: %s\n", leftover);
                    fprintf(file, "%s\n", leftover);
                    fflush(file);
                    pthread_mutex_unlock(&lock);
                    leftoverCount = 0;
                    continue;
                }
                //parse 5 length commands
                else if(leftover[0] == 'S' && leftover[1] == 'T' && leftover[2] == 'A' && leftover[3] == 'R' && leftover[4] == 'T'){
                    go = 1;
                    pthread_mutex_lock(&lock);
                    fprintf(file, "START\n");
                    fflush(file);
                    pthread_mutex_unlock(&lock);
                } else if(leftoverCount < 6){
                    pthread_mutex_lock(&lock);
                    fprintf(stderr, "Invalid input: %s\n", leftover);
                    fprintf(file, "%s\n", leftover);
                    fflush(file);
                    pthread_mutex_unlock(&lock);
                    leftoverCount = 0;
                    continue;
                }
                //parse 6 length commands
                else if(leftover[0] == 'S' && leftover[1] == 'C' && leftover[2] == 'A' && leftover[3] == 'L' && leftover[4] == 'E' && leftover[5] == '='){
                    if(leftoverCount != 7){
                        pthread_mutex_lock(&lock);
                        fprintf(stderr, "Invalid input: %s\n", leftover);
                        fprintf(file, "%s\n", leftover);
                        fflush(file);
                        pthread_mutex_unlock(&lock);
                        leftoverCount = 0;
                        continue;
                    } else {
                        if(leftover[6] == 'F' || leftover[6] == 'C'){
                            scale = leftover[6];
                            pthread_mutex_lock(&lock);
                            fprintf(file, "%s\n", leftover);
                            fflush(file);
                            pthread_mutex_unlock(&lock);
                        } else {
                            pthread_mutex_lock(&lock);
                            fprintf(stderr, "Invalid input: %s\n", leftover);
                            fprintf(file, "%s\n", leftover);
                            fflush(file);
                            pthread_mutex_unlock(&lock);
                            leftoverCount = 0;
                            continue;
                        }
                    }
                } else if(leftoverCount < 7){
                    pthread_mutex_lock(&lock);
                    fprintf(stderr, "Invalid input: %s\n", leftover);
                    fprintf(file, "%s\n", leftover);
                    fflush(file);
                    pthread_mutex_unlock(&lock);
                    leftoverCount = 0;
                    continue;
                }
                //parse 7 length commands
                else if(leftover[0] == 'P' && leftover[1] == 'E' && leftover[2] == 'R' && leftover[3] == 'I' && leftover[4] == 'O' && leftover[5] == 'D' && leftover[6] == '='){
                    if(leftoverCount < 8){
                        pthread_mutex_lock(&lock);
                        fprintf(stderr, "Invalid input: %s\n", leftover);
                        fprintf(file, "%s\n", leftover);
                        fflush(file);
                        pthread_mutex_unlock(&lock);
                        leftoverCount = 0;
                        continue;
                    }
                    int failed = 0;
                    int j = 7;
                    for(j = 7; j < leftoverCount; j++){
                        if(leftover[j] < '0' || leftover[j] > '9'){
                            pthread_mutex_lock(&lock);
                            fprintf(stderr, "Invalid input: %s\n", leftover);
                            fprintf(file, "%s\n", leftover);
                            fflush(file);
                            pthread_mutex_unlock(&lock);
                            leftoverCount = 0;
                            failed = 1;
                            break;
                        }
                    }
                    if(failed == 1){
                        continue;
                    }
                    period = atoi(leftover+7);
                    pthread_mutex_lock(&lock);
                    fprintf(file, "%s\n", leftover);
                    fflush(file);
                    pthread_mutex_unlock(&lock);
                } else {
                    pthread_mutex_lock(&lock);
                    fprintf(stderr, "Invalid input: %s\n", leftover);
                    fprintf(file, "%s\n", leftover);
                    fflush(file);
                    pthread_mutex_unlock(&lock);
                    leftoverCount = 0;
                    continue;
                }
                leftoverCount = 0;
            }
            else{
                leftover[leftoverCount] = buffer[i];
                leftoverCount++;
            }
        }
    }
    if(leftover != NULL){
        free(leftover);
    }
    if(close(0) == -1){
        pthread_mutex_lock(&lock);
        fprintf(stderr, "close: %s\n", strerror(errno));
        pthread_mutex_unlock(&lock);
        exit(2);
    }
    return NULL;
}

int main(int argc, char* const argv[]){
    if(argc < 5){
        fprintf(stderr, "Invalid number of arguments: %d\n", argc);
        return 1;
    }
    
    opterr = 0;
    struct option options[4];
    
    options[0].name = "id";
    options[0].has_arg = 1;
    int flag0[1] = {0};
    options[0].flag = flag0;
    options[0].val = 1;
    
    options[1].name = "host";
    options[1].has_arg = 1;
    int flag1[1] = {0};
    options[1].flag = flag1;
    options[1].val = 1;
    
    options[2].name = "log";
    options[2].has_arg = 1;
    int flag2[1] = {0};
    options[2].flag = flag2;
    options[2].val = 1;
    
    options[3].name = 0;
    options[3].has_arg = 0;
    options[3].flag = 0;
    options[3].val = 0;
    
    int longInd = -1;
    int output = getopt_long(argc, argv, "-:", options, &longInd);
    while(output != -1){
        if(output == 1){
            int len = strlen(optarg);
            int i = 0;
            for(i = 0; i < len; i++){
                if(optarg[i] < '0' || optarg[i] > '9'){
                    fprintf(stderr, "Invalid id argument: %s\n", optarg);
                    return 1;
                }
            }
            port = atoi(optarg);
        } else if(output == '?'){
            if(optind < argc && strlen(argv[optind]) > 2 && argv[optind][0] == '-' && argv[optind][1] != '-' && longInd == -1){
                fprintf(stderr, "Invalid input: %s\n", argv[optind]);
            } else {
                fprintf(stderr, "Invalid input: %s\n", argv[optind-1]);
            }
            return 1;
        } else if(output == ':'){
            fprintf(stderr, "Argument requires output: %s\n", argv[optind-1]);
            return 1;
        }else if(longInd == 0){
            //id
            int len = strlen(optarg);
            if(len != 9){
                fprintf(stderr, "Invalid id argument: %s\n", optarg);
                return 1;
            }
            int i = 0;
            for(i = 0; i < len; i++){
                if(optarg[i] < '0' || optarg[i] > '9'){
                    fprintf(stderr, "Invalid id argument: %s\n", optarg);
                    return 1;
                }
            }
            strcpy(id, optarg);
        } else if(longInd == 1){
            //host
            if(host != NULL){
                free(host);
            }
            host = malloc((strlen(optarg)+1)*sizeof(char));
            strcpy(host, optarg);
        } else if(longInd == 2){
            //log
            if(file != NULL){
                output = fclose(file);
                if(output == EOF){
                    fprintf(stderr, "fclose: %s\n", strerror(errno));
                    return 2;
                }
            }
            file = fopen(optarg, "a");
            if(file == NULL){
                fprintf(stderr, "fopen: %s\n", strerror(errno));
                return 1;
            }
        }
        longInd = -1;
        output = getopt_long(argc, argv, "-:", options, &longInd);
    }
    
    if(strstr(argv[0], "lab4c_tls")){
        tcp = 0;
    } else if(strstr(argv[0], "lab4c_tcp")){
        tcp = 1;
    } else {
        fprintf(stderr, "Neither tls nor tcp\n");
        return 1;
    }
    
    if(port == -1 || strcmp(id, "") == 0 || host == NULL || file == NULL){
        fprintf(stderr, "Invalid Input. Usage: %s --port=# --id=# --host=HOSTNAME --log=FILE", argv[0]);
        return 1;
    }
    
    int sockfd = -1;
    
    struct sockaddr_in serv_addr; //encode the ip address and the port for the remote
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        fprintf(stderr, "sockfd: %s\n", strerror(errno));
        return 2;
    }
    // AF_INET: IPv4, SOCK_STREAM: TCP connection
    struct hostent *server = gethostbyname(host);
    if(server == NULL){
        fprintf(stderr, "gethostbyname error\n");
        return 2;
    }
    // convert host_name to IP addr
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET; //address is Ipv4
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    //copy ip address from server to serv_addr 
    serv_addr.sin_port = htons(port); //setup the port
    output = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));  //initiate the connection to server
    if(output == -1){
        fprintf(stderr, "connect: %s\n", strerror(errno));
        return 2;
    }
    
    if(tcp == 0){
        //do tls
        SSL_CTX* newContext = NULL;
        SSL_library_init();
        //Initialize the error message
        SSL_load_error_strings();
        OpenSSL_add_all_algorithms();
        //TLS version: v1, one context per server.
        newContext = SSL_CTX_new(TLSv1_client_method());
        if(newContext == NULL){
            fprintf(stderr, "SSL Context initialization failed\n");
            return 2;
        }
        sslClient = SSL_new(newContext);
        if(sslClient == NULL){
            fprintf(stderr, "SSL Client initialization failed\n");
            return 2;
        }
        output = SSL_set_fd(sslClient,sockfd);
        if(output == 0){
            fprintf(stderr, "SSL set fd failed\n");
            return 2;
        }
        output = SSL_connect(sslClient);
        if(output < 0){
            fprintf(stderr, "SSL connect failed\n");
            return 2;
        } else if(output == 2){
            fprintf(stderr, "SSL connect shut down\n");
            return 2;
        }
    }
    
#ifdef BEAGLEBONE
    //setup temperature pin
    mraa_aio_context thermometer;
    thermometer = mraa_aio_init(1);
    if(thermometer == NULL){
        fprintf(stderr, "Thermometer initialization failed\n");
        return 2;
    }
#endif
    
    char idline[] = "ID=#########\n";
    int i = 0;
    for(i = 0; i < 9; i++){
        idline[3+i] = id[i];
    }
    int len = 13;
    int count = 0;
    while(count < len){
        int temp = 0;
        if(tcp == 0){
            temp = SSL_write(sslClient, idline+count, len-count);
        } else {
            temp = write(sockfd, idline+count, len-count);
        }
        if(temp < 0){
            fprintf(stderr, "write: %s\n", strerror(errno));
            return 2;
        } else if(temp == 0){
            if(tcp == 0){
                fprintf(stderr, "ssl write error\n");
                return 2;
            }
        } else {
            count += temp;
        }
    }
    fprintf(file, "%s", idline);
    
    //setup pthread
    pthread_t thread;
    output = pthread_create(&thread, NULL, readfd, (void*)&sockfd);
    if(output != 0){
        fprintf(stderr, "Pthread start error: %s\n", strerror(output));
        return 2;
    }
    
    while(interrupt == 0){
#ifdef BEAGLEBONE
        //read temperature
        int reading = mraa_aio_read(thermometer);
        double calc = 1023.0/(float)reading - 1.0;
        calc = 100000.0*calc;
        double temp = 1.0/(log(calc/100000.0)/4275 + 1/298.15) - 273.15;
        if(scale == 'F'){
            temp = (temp*9)/5 + 32;
        }
        time_t now = time(NULL);
        struct tm* timespec = localtime(&now);
        if(timespec == NULL){
            pthread_mutex_lock(&lock);
            fprintf(stderr, "localtime error\n");
            pthread_mutex_unlock(&lock);
            return 2;
        }
        pthread_mutex_lock(&lock);
        fprintf(file, "%.2d:%.2d:%.2d %.1f\n", (timespec->tm_hour%12), timespec->tm_min, timespec->tm_sec, temp);
        fflush(file);
        char readstring[100];
        len = sprintf(readstring, "%.2d:%.2d:%.2d %.1f\n", (timespec->tm_hour%12), timespec->tm_min, timespec->tm_sec, temp);
        if(len < 0){
            fprintf(stderr, "sprintf error\n");
            return 2;
        }
        count = 0;
        while(count < len){
            int temp = 0;
            if(tcp == 0){
                temp = SSL_write(sslClient, readstring+count, len-count);
            } else {
                temp = write(sockfd, readstring+count, len-count);
            }
            if(temp < 0){
                fprintf(stderr, "write: %s\n", strerror(errno));
                return 2;
            } else if(temp == 0){
                if(tcp == 0){
                    fprintf(stderr, "SSL write failure\n");
                    return 2;
                }
            } else {
                count += temp;
            }
        }
        pthread_mutex_unlock(&lock);
#else
        //print temperature and time
        pthread_mutex_lock(&lock);
        fprintf(file, "Temperature measured\n");
        fflush(file);
        pthread_mutex_unlock(&lock);
#endif
        sleep(period);
        while(go == 0 && interrupt == 0){
            continue;
        }
    }
    
#ifdef BEAGLEBONE
    //close pin
    mraa_aio_close(thermometer);
#endif
    
    if(tcp == 0){
        output = SSL_shutdown(sslClient);
        while(output == 2){
            output = SSL_shutdown(sslClient);
        }
        if(output == -1){
            fprintf(stderr, "SSL shutdown failed\n");
            return 2;
        }
        SSL_free(sslClient);
    }
    
    time_t now = time(NULL);
    struct tm* timespec = localtime(&now);
    if(timespec == NULL){
        fprintf(stderr, "localtime error\n");
        return 1;
    }
    fprintf(file, "%.2d:%.2d:%.2d SHUTDOWN\n", (timespec->tm_hour%12), timespec->tm_min, timespec->tm_sec);
    fflush(file);
    if(output == EOF){
        fprintf(stderr, "fclose: %s\n", strerror(errno));
        return 1;
    }
    
    free(host);
    output = fclose(file);
    if(output == EOF){
        fprintf(stderr, "fclose: %s\n", strerror(errno));
        return 2;
    }
	return 0;
}
