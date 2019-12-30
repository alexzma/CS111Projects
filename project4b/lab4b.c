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
#include <fcntl.h>
#include <pthread.h>
#include <math.h>

int volatile period = 1;
char volatile scale = 'F';
FILE* volatile file = NULL;
int volatile go = 1;
int volatile interrupt = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* readstdin(void* arg){
    char buffer[100];
    char* leftover = arg;
    leftover = NULL;
    int leftoverCount = 0;
    while(interrupt == 0){
        int output = read(0, buffer, 99);
        if(output == 0){
            continue;
        } else if(output == -1){
            if(errno == EAGAIN){
                continue;
            }
            pthread_mutex_lock(&lock);
            fprintf(stderr, "read: %s\n", strerror(errno));
            pthread_mutex_unlock(&lock);
            if(leftover != NULL){
                free(leftover);
            }
            if(file != NULL){
                fclose(file);
            }
            exit(1);
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
                exit(1);
            }
        } else {
            leftover = realloc(leftover, (output+leftoverCount+1)*sizeof(char));
            if(leftover == NULL){
                pthread_mutex_lock(&lock);
                fprintf(stderr, "realloc: %s\n", strerror(errno));
                pthread_mutex_unlock(&lock);
                free(leftover);
                if(file != NULL){
                    fclose(file);
                }
                exit(1);
            }
        }
        int i = 0;
        for(i = 0; i < output; i++){
            if(buffer[i] == '\n'){
                leftover[leftoverCount] = '\0';
                if(leftoverCount < 3){
                    pthread_mutex_lock(&lock);
                    fprintf(stderr, "Invalid input: %s\n", leftover);
                    if(file != NULL){
                        fprintf(file, "%s\n", leftover);
                        fflush(file);
                    }
                    pthread_mutex_unlock(&lock);
                    leftoverCount = 0;
                    continue;
                }
                //parse 3 length commands
                if(leftover[0] == 'O' && leftover[1] == 'F' && leftover[2] == 'F'){
                    if(leftoverCount != 3){
                        pthread_mutex_lock(&lock);
                        fprintf(stderr, "Invalid input: %s\n", leftover);
                        if(file != NULL){
                            fprintf(file, "%s\n", leftover);
                            fflush(file);
                        }
                        pthread_mutex_unlock(&lock);
                        leftoverCount = 0;
                        continue;
                    } else {
                        pthread_mutex_lock(&lock);
                        if(file != NULL){
                            fprintf(file, "OFF\n");
                            fflush(file);
                        }
                        pthread_mutex_unlock(&lock);
                    }
                    interrupt = 1;
                    free(leftover);
                    return NULL;
                } else if(leftover[0] == 'L' && leftover[1] == 'O' && leftover[2] == 'G'){
                    pthread_mutex_lock(&lock);
                    if(file != NULL){
                        fprintf(file, "%s\n", leftover);
                        fflush(file);
                    }
                    pthread_mutex_unlock(&lock);
                } else if(leftoverCount < 4){
                    pthread_mutex_lock(&lock);
                    fprintf(stderr, "Invalid input: %s\n", leftover);
                    if(file != NULL){
                        fprintf(file, "%s\n", leftover);
                        fflush(file);
                    }
                    pthread_mutex_unlock(&lock);
                    leftoverCount = 0;
                    continue;
                }
                //parse 4 length commands
                else if(leftover[0] == 'S' && leftover[1] == 'T' && leftover[2] == 'O' && leftover[3] == 'P'){
                    go = 0;
                    pthread_mutex_lock(&lock);
                    if(file != NULL){
                        fprintf(file, "STOP\n");
                        fflush(file);
                    }
                    pthread_mutex_unlock(&lock);
                } else if(leftoverCount < 5){
                    pthread_mutex_lock(&lock);
                    fprintf(stderr, "Invalid input: %s\n", leftover);
                    if(file != NULL){
                        fprintf(file, "%s\n", leftover);
                        fflush(file);
                    }
                    pthread_mutex_unlock(&lock);
                    leftoverCount = 0;
                    continue;
                }
                //parse 5 length commands
                else if(leftover[0] == 'S' && leftover[1] == 'T' && leftover[2] == 'A' && leftover[3] == 'R' && leftover[4] == 'T'){
                    go = 1;
                    pthread_mutex_lock(&lock);
                    if(file != NULL){
                        fprintf(file, "START\n");
                        fflush(file);
                    }
                    pthread_mutex_unlock(&lock);
                } else if(leftoverCount < 6){
                    pthread_mutex_lock(&lock);
                    fprintf(stderr, "Invalid input: %s\n", leftover);
                    if(file != NULL){
                        fprintf(file, "%s\n", leftover);
                        fflush(file);
                    }
                    pthread_mutex_unlock(&lock);
                    leftoverCount = 0;
                    continue;
                }
                //parse 6 length commands
                else if(leftover[0] == 'S' && leftover[1] == 'C' && leftover[2] == 'A' && leftover[3] == 'L' && leftover[4] == 'E' && leftover[5] == '='){
                    if(leftoverCount != 7){
                        pthread_mutex_lock(&lock);
                        fprintf(stderr, "Invalid input: %s\n", leftover);
                        if(file != NULL){
                            fprintf(file, "%s\n", leftover);
                            fflush(file);
                        }
                        pthread_mutex_unlock(&lock);
                        leftoverCount = 0;
                        continue;
                    } else {
                        if(leftover[6] == 'F' || leftover[6] == 'C'){
                            scale = leftover[6];
                            pthread_mutex_lock(&lock);
                            if(file != NULL){
                                fprintf(file, "%s\n", leftover);
                                fflush(file);
                            }
                            pthread_mutex_unlock(&lock);
                        } else {
                            pthread_mutex_lock(&lock);
                            fprintf(stderr, "Invalid input: %s\n", leftover);
                            if(file != NULL){
                                fprintf(file, "%s\n", leftover);
                                fflush(file);
                            }
                            pthread_mutex_unlock(&lock);
                            leftoverCount = 0;
                            continue;
                        }
                    }
                } else if(leftoverCount < 7){
                    pthread_mutex_lock(&lock);
                    fprintf(stderr, "Invalid input: %s\n", leftover);
                    if(file != NULL){
                        fprintf(file, "%s\n", leftover);
                        fflush(file);
                    }
                    pthread_mutex_unlock(&lock);
                    leftoverCount = 0;
                    continue;
                }
                //parse 7 length commands
                else if(leftover[0] == 'P' && leftover[1] == 'E' && leftover[2] == 'R' && leftover[3] == 'I' && leftover[4] == 'O' && leftover[5] == 'D' && leftover[6] == '='){
                    if(leftoverCount < 8){
                        pthread_mutex_lock(&lock);
                        fprintf(stderr, "Invalid input: %s\n", leftover);
                        if(file != NULL){
                            fprintf(file, "%s\n", leftover);
                            fflush(file);
                        }
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
                            if(file != NULL){
                                fprintf(file, "%s\n", leftover);
                                fflush(file);
                            }
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
                    if(file != NULL){
                        fprintf(file, "%s\n", leftover);
                        fflush(file);
                    }
                    pthread_mutex_unlock(&lock);
                } else {
                    pthread_mutex_lock(&lock);
                    fprintf(stderr, "Invalid input: %s\n", leftover);
                    if(file != NULL){
                        fprintf(file, "%s\n", leftover);
                        fflush(file);
                    }
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
        exit(1);
    }
    return NULL;
}

void handler(int sig){
    if(sig == SIGINT){
        interrupt = 1;
    }
}

void buttonHandler(){
    interrupt = 1;
}

int main(int argc, char* const argv[]){
    opterr = 0;
    struct option options[4];
    
    options[0].name = "period";
    options[0].has_arg = 1;
    int flag0[1] = {0};
    options[0].flag = flag0;
    options[0].val = 1;
    
    options[1].name = "scale";
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
            fprintf(stderr, "Invalid input: %s\n", argv[optind-1]);
            return 1;
        } else if(output == '?'){
            if(optind < argc && argv[optind][0] == '-' && longInd == -1){
                fprintf(stderr, "Invalid input: %s\n", argv[optind]);
            } else {
                fprintf(stderr, "Invalid input: %s\n", argv[optind-1]);
            }
            return 1;
        } else if(output == ':'){
            fprintf(stderr, "Argument requires output: %s\n", argv[optind-1]);
            return 1;
        }else if(longInd == 0){
            //period
            int len = strlen(optarg);
            int i = 0;
            for(i = 0; i < len; i++){
                if(optarg[i] < '0' || optarg[i] > '9'){
                    fprintf(stderr, "Invalid period argument: %s\n", optarg);
                    return 1;
                }
            }
            period = atoi(optarg);
        } else if(longInd == 1){
            //scale
            if(strlen(optarg) != 1){
                fprintf(stderr, "Invalid scale argument: %s\n", optarg);
                return 1;
            }
            if(optarg[0] != 'C' && optarg[0] != 'F'){
                fprintf(stderr, "Invalid scale argument: %s\n", optarg);
                return 1;
            }
            scale = optarg[0];
        } else if(longInd == 2){
            //log
            file = fopen(optarg, "a");
            if(file == NULL){
                fprintf(stderr, "open: %s\n", strerror(errno));
                return 1;
            }
        }
        longInd = -1;
        output = getopt_long(argc, argv, "-:", options, &longInd);
    }
    
    int flags = fcntl(0, F_GETFL, 0);
    if(flags == -1){
        fprintf(stderr, "fcntl: %s\n", strerror(errno));
    }
    output = fcntl(0, F_SETFL, flags | O_NONBLOCK);
    if(output == -1){
        fprintf(stderr, "fcntl: %s\n", strerror(errno));
    }
    
    if(signal(SIGINT, handler) == SIG_ERR){
        fprintf(stderr, "signal: %s\n", strerror(errno));
        return 1;
    }
    
#ifdef BEAGLEBONE
    //setup temperature and button pins
    mraa_gpio_context button;
    button = mraa_gpio_init(60);
    mraa_gpio_dir(button, MRAA_GPIO_IN);
    mraa_gpio_isr(button, MRAA_GPIO_EDGE_FALLING, &buttonHandler, NULL);
    mraa_aio_context thermometer;
    thermometer = mraa_aio_init(1);
#endif
    
    //setup pthread
    pthread_t thread;
    output = pthread_create(&thread, NULL, readstdin, NULL);
    if(output != 0){
        fprintf(stderr, "Pthread start error: %s\n", strerror(output));
        return 1;
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
            return 1;
        }
        pthread_mutex_lock(&lock);
        fprintf(stdout, "%.2d:%.2d:%.2d %04.1f\n", (timespec->tm_hour%12), timespec->tm_min, timespec->tm_sec, temp);
        if(file != NULL){
            fprintf(file, "%.2d:%.2d:%.2d %04.1f\n", (timespec->tm_hour%12), timespec->tm_min, timespec->tm_sec, temp);
            fflush(file);
        }
        fflush(stdout);
        pthread_mutex_unlock(&lock);
#else
        //print temperature and time
        pthread_mutex_lock(&lock);
        fprintf(stdout, "Temperature measured\n");
        if(file != NULL){
            fprintf(file, "Temperature measured\n");
            fflush(file);
        }
        fflush(stdout);
        pthread_mutex_unlock(&lock);
#endif
        sleep(period);
        while(go == 0 && interrupt == 0){
            continue;
        }
    }
    
    //pthread_join
    output = pthread_join(thread, NULL);
    if(output != 0){
        fprintf(stderr, "Pthread end error: %s\n", strerror(output));
        return 1;
    }
    
#ifdef BEAGLEBONE
    //close pins
    mraa_gpio_close(button);
    mraa_aio_close(thermometer);
#endif
    
    time_t now = time(NULL);
    struct tm* timespec = localtime(&now);
    if(timespec == NULL){
        fprintf(stderr, "localtime error\n");
        return 1;
    }
    fprintf(stdout, "%.2d:%.2d:%.2d SHUTDOWN\n", (timespec->tm_hour%12), timespec->tm_min, timespec->tm_sec);
    if(file != NULL){
        fprintf(file, "%.2d:%.2d:%.2d SHUTDOWN\n", (timespec->tm_hour%12), timespec->tm_min, timespec->tm_sec);
        fflush(file);
        output = fclose(file);
        if(output == EOF){
            fprintf(stderr, "fclose: %s\n", strerror(errno));
            return 1;
        }
    }
	return 0;
}
