#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include "linked_list.h"
#define CLERKS 5

Node * business_info = NULL;
Node * economy_info = NULL;
Node * queue0 = NULL;
Node * queue1 = NULL;
int avaliability[] = {0,0,0,0,0};// flag for clerk thread
pthread_mutex_t lock; //mutex declaration
pthread_cond_t convar = PTHREAD_COND_INITIALIZER; // condvar declaration
static struct timeval start_time; 
double wait_economy = 0;
double wait_business = 0;
int size_economy = 0;
int size_business = 0;

void process_file(char* filename);
void simulate();
void * customer_entry(void *);
void * clerk_1(void *); //Clerk1
void * clerk_2(void *); //Clerk2
void * clerk_3(void *); //Clerk3
void * clerk_4(void *); //Clerk4
void * clerk_5(void *); //Clerk5


/*
 * Function:  process_file
 * --------------------
 *  get the customer data from the input file
 *
 *  input: 
 *  filename: filename of the file
 * 
 *  returns: 
 */
void process_file(char* filename){
    char customer_num[4] = {};
    char customer[4000];
    char file_address[50];
    char c;
    char *token;
    int index = 0;
    int num;
    sprintf(file_address, "%s", filename);// modify the user input

    // Read file
    FILE *acs_info;
    
    acs_info = fopen(file_address, "r");
    if(acs_info == NULL){// check if file exists
        printf("file does not exist\n");
        exit(EXIT_FAILURE);
    }


    // Find total customer number    
    while ((c = getc(acs_info)) != '\n'){
        customer_num[index] = c;
        index++;
    }

    num = atoi(customer_num);
    
    // Find customer info
    index = 0;
    while ((c = getc(acs_info)) != EOF){
        customer[index] = c;
        index++;
    }
    fclose(acs_info);
    



    token = strtok(customer, ":");
   
    // Get formatted data
    while( token != NULL ) {
        int id = atoi(token);
    
        token = strtok(NULL, ",");
        int class = atoi(token);

        token = strtok(NULL, ",");
        int arrive_time = atoi(token);
        if(arrive_time <= 0){
            printf("invalid file: arrving time need to be positive integer\n");
            exit(EXIT_FAILURE);
        }

        token = strtok(NULL, "\n");
        int service_time = atoi(token);
        if(service_time <= 0){
            printf("invalid file: service time need to be positive integer\n");
            exit(EXIT_FAILURE);
        }

        if(class == 1){
            business_info = add_newNode(business_info, id, class, arrive_time, service_time);
        }
        else {
            economy_info = add_newNode(economy_info, id, class, arrive_time, service_time);
        }
    
        token = strtok(NULL, ":");
    }

    // record customer amount
    size_business = get_size(business_info);
    size_economy = get_size(economy_info);


    if(num != size_business + size_economy) {
        printf("Invalid file: customer number does not match");
    }

}


/*
 * Function:  simulate
 * --------------------
 *  Simulate the queue of the air check system
 *
 *  input: 
 * 
 *  returns: 
 */
void simulate() {
    pthread_t clerks[CLERKS];
    pthread_t customer;

    gettimeofday(&start_time, NULL);

    if (pthread_mutex_init(&lock, NULL) != 0){ //mutex initialization
        printf("\n mutex init failed\n");
        exit(EXIT_FAILURE);
    }

    // Thread declaration
    if(pthread_create(&customer, NULL, customer_entry, NULL ) != 0){
        printf("Unable to initialize customers queue\n");	
        exit(EXIT_FAILURE);
    }

    if(pthread_create(&clerks[0], NULL, clerk_1, NULL ) != 0){
        printf("Unable to initialize clerk 1\n");	
        exit(EXIT_FAILURE);
    }
    
    if(pthread_create(&clerks[1], NULL, clerk_2, NULL ) != 0){
        printf("Unable to initialize clerk 2\n");
        exit(EXIT_FAILURE);

    }
    
    if(pthread_create(&clerks[2], NULL, clerk_3, NULL ) != 0){
        printf("Unable to initialize clerk 3\n");	
        exit(EXIT_FAILURE);

    }

    if(pthread_create(&clerks[3], NULL, clerk_4, NULL ) != 0){
        printf("Unable to initialize clerk 4\n");	
        exit(EXIT_FAILURE);

    }
    
    if(pthread_create(&clerks[4], NULL, clerk_5, NULL ) != 0){
        printf("Unable to initialize clerk 5\n");	
        exit(EXIT_FAILURE);
    }
    
    // Join the finished thread
    for(int i = 0; i < 5; i++) {
        pthread_join(clerks[i], NULL);
    }

    pthread_join(customer, NULL);

    // Destroy mutex and conditional variable
    if(pthread_mutex_destroy(&lock) != 0){
        printf("Unable to destroy mutex lock\n");	
        exit(EXIT_FAILURE);
    }

	if(pthread_cond_destroy(&convar) != 0){
        printf("Unable to destroy condition variable\n");
        exit(EXIT_FAILURE);
    }


    // Compute and display the average waiting time statisics
    printf("The average waiting time for all customers in the system is: %.2f seconds. \n", 
        (wait_business + wait_economy) / (size_business + size_economy) / 10);

    printf("The average waiting time for all business-class customers is: %.2f seconds. \n", 
        (wait_business) / (size_business) / 10);

    printf("The average waiting time for all economy-class customers is: %.2f seconds. \n", 
        (wait_economy) / (size_economy) / 10);
        
    printf("Finish Simulation\n");

    return;
}

/*
 * Function:  customer_entry
 * --------------------
 *  Thread for simulating customer queues
 *
 *  input: 
 * 
 *  returns: 
 */
void * customer_entry(void * cus_info) {
    struct timeval current_time;
    double cur_secs, init_secs;
    pthread_mutex_lock(&lock);
    int wait;
    while(get_size(economy_info) + get_size(business_info) != 0) {
        
        init_secs = (start_time.tv_sec + (double) start_time.tv_usec / 1000000);
        gettimeofday(&current_time, NULL);
        cur_secs = (current_time.tv_sec + (double) current_time.tv_usec / 1000000);
        int simp = ((cur_secs - init_secs) * 100);


        // wait until next customer arrives
        if(get_size(business_info) == 0) {
            wait = peekNode(economy_info)->arrive_time - simp / 10;
            usleep(wait * 100000);
        }
        else if(get_size(economy_info) == 0){
            wait = peekNode(business_info)->arrive_time - simp / 10;
            usleep(wait * 100000);
        }
        else {
            if(peekNode(economy_info)->arrive_time < peekNode(business_info)->arrive_time){
                wait = peekNode(economy_info)->arrive_time - simp / 10;

                usleep(wait * 100000);

            }
            else {
                wait = peekNode(business_info)->arrive_time - simp / 10;
                usleep(wait * 100000);
            }
        }
        

        // Find avaliable clerk for the waiting customer
        while ((get_size(economy_info) != 0 && peekNode(economy_info)->arrive_time == simp / 10) 
        || (get_size(business_info) != 0 && peekNode(business_info)->arrive_time == simp / 10)){

            gettimeofday(&current_time, NULL);
            cur_secs = (current_time.tv_sec + (double) current_time.tv_usec / 1000000);
            simp = ((cur_secs - init_secs) * 100);

            Node * cur = NULL;
            Node * vip = NULL;

            if(get_size(economy_info) != 0){
                cur = peekNode(economy_info);
            }

            if(get_size(business_info) != 0){
                vip = peekNode(business_info);
            }

            if(cur != NULL && cur->arrive_time == simp / 10) {
                queue0 = add_back(queue0, cur->id, cur->class, cur->arrive_time, cur->service_time);
                economy_info = removeFront(economy_info);
                printf("A customer arrives: customer ID %2d. \n", cur->id);
                printf("A customer enters a queue: the queue ID %1d, and length of the queue %2d. \n", 0, get_size(queue0));

            }
            else if(vip != NULL && vip->arrive_time == simp / 10){
                queue1 = add_back(queue1, vip->id, vip->class, vip->arrive_time, vip->service_time);
                business_info = removeFront(business_info);
                printf("A customer arrives: customer ID %2d. \n", vip->id);
                printf("A customer enters a queue: the queue ID %1d, and length of the queue %2d. \n", 1, get_size(queue1));
            }
            else if(vip != NULL 
                && cur != NULL 
                && cur->arrive_time == simp / 10 
                && vip->arrive_time == simp / 10){
                 
                queue0 = add_back(queue0, cur->id, cur->class, cur->arrive_time, cur->service_time);
                economy_info = removeFront(economy_info);
                printf("A customer arrives: customer ID %2d. \n", cur->id);
                printf("A customer enters a queue: the queue ID %1d, and length of the queue %2d. \n", 0, get_size(queue0));
                queue1 = add_back(queue1, vip->id, vip->class, vip->arrive_time, vip->service_time);
                business_info = removeFront(business_info);
                printf("A customer arrives: customer ID %2d. \n", vip->id);
                printf("A customer enters a queue: the queue ID %1d, and length of the queue %2d. \n", 1, get_size(queue1));
            }

        }

        // Assign customer to an avaliable customer.
                    
        if(get_size(economy_info) + get_size(business_info) == 0){

                pthread_mutex_unlock(&lock);
                pthread_exit(NULL);
                return NULL;
        }
        for(int i = 0; i < 5; i++){
            if(avaliability[i] == 0){
                
                pthread_cond_wait(&convar, &lock);

            }
        }
        
    }

    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
    return NULL;

}

/*
 * Function:  clerk1
 * --------------------
 *  Thread for simulating clerk1
 *
 *  input: 
 * 
 *  returns: 
 */
void * clerk_1(void * arg){
    
    //Initial thread and get mutex
    struct timeval current_time;
    double cur_secs, init_secs;
    pthread_mutex_lock(&lock);
    avaliability[0] = 1;

    int wait;
    Node * cur_customer;
    
    // Service the customer and wait
    while(get_size(economy_info) + get_size(business_info) != 0 || get_size(queue0) + get_size(queue1) != 0){
        
        init_secs = (start_time.tv_sec + (double) start_time.tv_usec / 1000000);
        gettimeofday(&current_time, NULL);
        cur_secs = (current_time.tv_sec + (double) current_time.tv_usec / 1000000);
        double simp = ((cur_secs - init_secs) * 100);

        // Truncate the error of waiting time
        if(get_size(queue0) + get_size(queue1) != 0){ // If there is customer on queue
            if(get_size(queue1) != 0){
                cur_customer = peekNode(queue1);
                queue1 = removeFront(queue1);
                wait_business += (int) simp / 10 - cur_customer->arrive_time;

            }
            else if(get_size(queue0) != 0){
                cur_customer = peekNode(queue0);
                queue0 = removeFront(queue0);
                wait_economy += (int) simp / 10 - cur_customer->arrive_time;

            }

            printf("A clerk starts serving a customer: start time %.2f, the customer ID %2d, the clerk ID %1d. \n", 
                simp / 100, cur_customer->id, 1);


            wait = cur_customer->service_time * 100000;
            pthread_cond_signal(&convar);
            
            pthread_mutex_unlock(&lock);
            usleep(wait);

            gettimeofday(&current_time, NULL);
            cur_secs = (current_time.tv_sec + (double) current_time.tv_usec / 1000000);
            simp = ((cur_secs - init_secs) * 100);


            printf("-->>> A clerk finishes serving a customer: end time %.2f, the customer ID %2d, the clerk ID %1d. \n", 
                simp / 100, cur_customer->id, 1);
            avaliability[0] = 0;

        }
        else{// If there is no customer on queue
            pthread_cond_signal(&convar);
            pthread_mutex_unlock(&lock);
            avaliability[0] = 0;
        }

        pthread_mutex_lock(&lock);
        avaliability[0] = 1;

        
    }

    // Finish task
    pthread_mutex_unlock(&lock);
    avaliability[0] = -1;

    pthread_exit(NULL);
    return NULL;
    

}

void * clerk_2(void * arg){

    //Initial thread and get mutex
    struct timeval current_time;
    double cur_secs, init_secs;
    pthread_mutex_lock(&lock);
    avaliability[1] = 1;

    int wait;
    Node * cur_customer;
    
    // Service the customer and wait
    while(get_size(economy_info) + get_size(business_info) != 0 || get_size(queue0) + get_size(queue1) != 0 ){

        // Truncate the error of waiting time
        if(get_size(queue0) + get_size(queue1) != 0){// If there is customer on queue

            init_secs = (start_time.tv_sec + (double) start_time.tv_usec / 1000000);
            gettimeofday(&current_time, NULL);
            cur_secs = (current_time.tv_sec + (double) current_time.tv_usec / 1000000);
            double simp = ((cur_secs - init_secs) * 100);

            if(get_size(queue1) != 0){
                cur_customer = peekNode(queue1);
                queue1 = removeFront(queue1);
                wait_business += (int) simp / 10 - cur_customer->arrive_time;


            }
            else if(get_size(queue0) != 0){
                cur_customer = peekNode(queue0);
                queue0 = removeFront(queue0);
                wait_economy += (int) simp / 10 - cur_customer->arrive_time;

            }

            pthread_cond_signal(&convar);
            pthread_mutex_unlock(&lock);

            printf("A clerk starts serving a customer: start time %.2f, the customer ID %2d, the clerk ID %1d. \n", 
                 simp / 100, cur_customer->id, 2);


            wait = cur_customer->service_time * 100000;
            usleep(wait);

            gettimeofday(&current_time, NULL);
            cur_secs = (current_time.tv_sec + (double) current_time.tv_usec / 1000000);
            simp = ((cur_secs - init_secs) * 100);

            printf("-->>> A clerk finishes serving a customer: end time %.2f, the customer ID %2d, the clerk ID %1d. \n", 
                 simp / 100, cur_customer->id, 2);
            avaliability[1] = 0;

        
        }
        else{// If there is no customer on queue
            pthread_cond_signal(&convar);
            pthread_mutex_unlock(&lock);
            avaliability[1] = 0;
        }

        pthread_mutex_lock(&lock);
        avaliability[1] = 1;

        
    }
    
    // Finish task
    pthread_mutex_unlock(&lock);
    avaliability[1] = -1;

    pthread_exit(NULL);
    return NULL;
    
}
void * clerk_3(void * arg){

    //Initial thread and get mutex
    struct timeval current_time;
    double cur_secs, init_secs;
    pthread_mutex_lock(&lock);
    avaliability[2] = 1;

    int wait;
    Node * cur_customer;
    
    // Service the customer and wait
    while(get_size(economy_info) + get_size(business_info) != 0 || get_size(queue0) + get_size(queue1) != 0 ){


        init_secs = (start_time.tv_sec + (double) start_time.tv_usec / 1000000);
        gettimeofday(&current_time, NULL);
        cur_secs = (current_time.tv_sec + (double) current_time.tv_usec / 1000000);
        double simp = ((cur_secs - init_secs) * 100);
        
        // Truncate the error of waiting time
        if(get_size(queue0) + get_size(queue1) != 0){// If there is customer on queue
            if(get_size(queue1) != 0){
                cur_customer = peekNode(queue1);
                queue1 = removeFront(queue1);
                wait_business += (int) simp / 10 - cur_customer->arrive_time;


            }
            else if(get_size(queue0) != 0){
                cur_customer = peekNode(queue0);
                queue0 = removeFront(queue0);
                wait_economy += (int) simp / 10 - cur_customer->arrive_time;

            }

            pthread_cond_signal(&convar);
            pthread_mutex_unlock(&lock);

            printf("A clerk starts serving a customer: start time %.2f, the customer ID %2d, the clerk ID %1d. \n", 
                simp / 100, cur_customer->id, 3);


            wait = cur_customer->service_time * 100000;
            usleep(wait);


            gettimeofday(&current_time, NULL);
            cur_secs = (current_time.tv_sec + (double) current_time.tv_usec / 1000000);
            simp = ((cur_secs - init_secs) * 100);

            printf("-->>> A clerk finishes serving a customer: end time %.2f, the customer ID %2d, the clerk ID %1d. \n", 
                 simp / 100, cur_customer->id, 3);
            avaliability[2] = 0;
            
        }
        else{// If there is no customer on queue
            pthread_cond_signal(&convar);
            pthread_mutex_unlock(&lock);
            avaliability[2] = 0;
        }

        pthread_mutex_lock(&lock);
        avaliability[2] = 1;

        
    }

    // Finish task
    pthread_mutex_unlock(&lock);
    avaliability[2] = -1;

    pthread_exit(NULL);
    return NULL;

}
void * clerk_4(void * arg){

    //Initial thread and get mutex
    struct timeval current_time;
    double cur_secs, init_secs;
    pthread_mutex_lock(&lock);
    avaliability[3] = 1;

    int wait;
    Node * cur_customer;
    
    // Service the customer and wait
    while(get_size(economy_info) + get_size(business_info) != 0 || get_size(queue0) + get_size(queue1) != 0 ){

        
        init_secs = (start_time.tv_sec + (double) start_time.tv_usec / 1000000);
        gettimeofday(&current_time, NULL);
        cur_secs = (current_time.tv_sec + (double) current_time.tv_usec / 1000000);
        double simp = ((cur_secs - init_secs) * 100);

        // Truncate the error of waiting time
        if(get_size(queue0) + get_size(queue1) != 0){// If there is customer on queue

            if(get_size(queue1) != 0){
                cur_customer = peekNode(queue1);
                queue1 = removeFront(queue1);
                wait_business += (int) simp / 10 - cur_customer->arrive_time;
            }
            else if(get_size(queue0) != 0){
                cur_customer = peekNode(queue0);
                queue0 = removeFront(queue0);
                wait_economy += (int) simp / 10 - cur_customer->arrive_time;
            }

            pthread_cond_signal(&convar);
            pthread_mutex_unlock(&lock);

            printf("A clerk starts serving a customer: start time %.2f, the customer ID %2d, the clerk ID %1d. \n", 
                 simp / 100, cur_customer->id, 4);


            wait = cur_customer->service_time * 100000;
            usleep(wait);
            
            init_secs = (start_time.tv_sec + (double) start_time.tv_usec / 1000000);
            gettimeofday(&current_time, NULL);
            cur_secs = (current_time.tv_sec + (double) current_time.tv_usec / 1000000);
            simp = ((cur_secs - init_secs) * 100);


            printf("-->>> A clerk finishes serving a customer: end time %.2f, the customer ID %2d, the clerk ID %1d. \n", 
                simp / 100, cur_customer->id, 4);
            avaliability[3] = 0;

        }
        else {// If there is no customer on queue
            pthread_cond_signal(&convar);
            pthread_mutex_unlock(&lock);
            avaliability[3] = 0;
        }

        pthread_mutex_lock(&lock);
        avaliability[3] = 1;
        
    }

    // Finish task
    pthread_mutex_unlock(&lock);
    avaliability[3] = -1;


    pthread_exit(NULL);
    return NULL;

}
void * clerk_5(void * arg){

    //Initial thread and get mutex
    struct timeval current_time;
    double cur_secs, init_secs;
    pthread_mutex_lock(&lock);
    avaliability[4] = 1;
    Node * cur_customer;
    int wait;
    
    // Service the customer and wait
    while(get_size(economy_info) + get_size(business_info) != 0 || get_size(queue0) + get_size(queue1) != 0 ){
        
        init_secs = (start_time.tv_sec + (double) start_time.tv_usec / 1000000);
        gettimeofday(&current_time, NULL);
        cur_secs = (current_time.tv_sec + (double) current_time.tv_usec / 1000000);
        double simp = ((cur_secs - init_secs) * 100);

        // Truncate the error of waiting time
        if(get_size(queue0) + get_size(queue1) != 0){// If there is customer on queue
            if(get_size(queue1) != 0){
                cur_customer = peekNode(queue1);
                queue1 = removeFront(queue1);
                wait_business += (int) simp / 10 - cur_customer->arrive_time;
            }
            else if(get_size(queue0) != 0){
                cur_customer = peekNode(queue0);
                queue0 = removeFront(queue0);
                wait_economy += (int) simp / 10 - cur_customer->arrive_time;
            }

            pthread_cond_signal(&convar);
            pthread_mutex_unlock(&lock);

            printf("A clerk starts serving a customer: start time %.2f, the customer ID %2d, the clerk ID %1d. \n", 
                simp / 100, cur_customer->id, 5);


            wait = cur_customer->service_time * 100000;
            usleep(wait);

            init_secs = (start_time.tv_sec + (double) start_time.tv_usec / 1000000);
            gettimeofday(&current_time, NULL);
            cur_secs = (current_time.tv_sec + (double) current_time.tv_usec / 1000000);
            simp = ((cur_secs - init_secs) * 100);

            printf("-->>> A clerk finishes serving a customer: end time %.2f, the customer ID %2d, the clerk ID %1d. \n", 
                simp / 100, cur_customer->id, 5);
            avaliability[4] = 0;
        
        }
        else{// If there is no customer on queue
            pthread_cond_signal(&convar);
            pthread_mutex_unlock(&lock);
            avaliability[4] = 0;

        }


        pthread_mutex_lock(&lock);
        avaliability[4] = 1;
        
    }

    // Finish task
    pthread_mutex_unlock(&lock);
    avaliability[4] = -1;

    pthread_exit(NULL);
    return NULL;


}

// Main function
int main() {

    // User prompt
    char user_input[50];
    printf("ACS ");
    scanf("%s" , user_input);

    // Process file
    process_file(user_input);

    // Simulate
    simulate();
    return 0;
}