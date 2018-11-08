
/*
Júlia Pessoa Souza 15/0133294
Lucas Machado 15/0137303
Guilherme Baldissera 14/0142002
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>

#define MIN_STUDENT 3
#define MAX_STUDENT 40
#define ASSISTENT_TIMES 3

// Define threads
void * studentThread(void * arg);
void * assistentThread();
void showHeader();
void showActualState();
void showFooter();
int generateWaitTime();

// Define counts
int studentCount = 0;
int chairCount = 0;
int chairFree = 0;
int * assistentQueue;
int assistentIndex = 0;
int assistantState = 0;

// Pthreads needed
pthread_t assistent;
pthread_t * students;
pthread_mutex_t globalLocker;
pthread_mutex_t * threadLockers;
sem_t sem;

int main(int argc, char const *argv[]) {

  // Creating random numbers for students and chairs
  srand(time(NULL));
  studentCount = rand() % (MAX_STUDENT - MIN_STUDENT) + MIN_STUDENT;
  chairCount = studentCount / 2;
  chairFree = chairCount;

  // Initializing assistent queue
  assistentQueue = (int *) malloc((studentCount * ASSISTENT_TIMES) * sizeof(int));
  for (int i = 0; i < sizeof(assistentQueue); i++) {
    assistentQueue[i] = 0;
  }

  showHeader();

  // Thread for assistent
  pthread_create(&assistent, NULL, assistentThread, NULL);
  pthread_mutex_init(&globalLocker, NULL);

  // Thread for students
  students = (pthread_t *) malloc(studentCount * sizeof(pthread_t));
  threadLockers = (pthread_mutex_t *) malloc(studentCount * sizeof(pthread_mutex_t));
  for (int i = 0; i < studentCount; i++) {
    pthread_mutex_init(&threadLockers[i], NULL);
    pthread_mutex_lock(&threadLockers[i]);
    pthread_create(&students[i], NULL, studentThread, (void *) i);
  }

  // Wait Threads
  pthread_join(assistent, NULL);
  for (int i = 0; i < studentCount; i++) {
    pthread_join(students[i], NULL);
  }

  showFooter();
  return 0;
}

// Function to control student
void * studentThread(void * arg) {
  int studentIndex = (int *) arg;
  int studentCode = studentIndex + 1;

  int assistentTimes = ASSISTENT_TIMES;

  while (assistentTimes > 0) {
    pthread_mutex_lock(&globalLocker);
    if (chairFree > 0) {
      // Have chair empty, wait to be assisted by assitent
      assistentQueue[assistentIndex + (chairCount - chairFree)] = studentCode;
      chairFree--;
      showActualState();
      pthread_mutex_unlock(&globalLocker);
      pthread_mutex_lock(&threadLockers[studentIndex]);
      assistentTimes--;
    } else {
      // All chairs are occuped, back to work
      pthread_mutex_unlock(&globalLocker);
      sleep(generateWaitTime());
    }
  }

  pthread_exit(0);
}

// Function to control assistent
void * assistentThread() {

  int assistentsNeeded = (studentCount) * ASSISTENT_TIMES;
  assistentIndex = 0;
  assistantState = 0;

  while (assistentIndex < assistentsNeeded) {
    pthread_mutex_lock(&globalLocker);
    if (chairFree < chairCount) {
      // Helping some student
      assistantState = assistentQueue[assistentIndex];
      assistentIndex++;
      showActualState();
      chairFree++;
      pthread_mutex_unlock(&globalLocker);
      sleep(generateWaitTime());
      pthread_mutex_unlock(&threadLockers[assistantState - 1]);
    } else {
      // No students to help, sleep
      assistantState = 0;
      showActualState();
      pthread_mutex_unlock(&globalLocker);
      sleep(generateWaitTime());
    }
  }
  pthread_exit(0);
}

// Function to generate random numbers for second sleep.
int generateWaitTime() {
  srand(time(NULL));
  return rand() % 5 + 1;
}

// Function to show header
void showHeader() {
  // Print default header with informations
  printf("\n-----------------------------\n");
  printf("----- Inicio da monitoria -----\n");
  printf("-------------------------------\n");
  printf("------- Estudantes: %.2d ------\n", studentCount);
  printf("-------- Cadeiras: %.2d -------\n", chairCount);
  printf("-------------------------------\n");

}

// Function to show state of assistent and chairs
void showActualState() {
  if(assistantState != 00){
    printf("Assitente: Ajudando estudante [%.2d].\n", assistantState);
    printf("Estudante  [%.2d]: Sendo ajudado.\n", assistantState);
  } else{
    printf("Assitente dormindo.\n", assistantState);
  }
  printf("Cadeiras:");
  for (int i = 0; i < chairCount; i++) {
    int index = i + assistentIndex;
    if(assistentQueue[index] <= MAX_STUDENT){
      printf(" [%.2d]", assistentQueue[index]);
    }
  }
  printf("\n-----------------------------------------------------------------------\n");
}

void showFooter() {
  // FOOTER
  printf("----------------------------------------\n");
  printf("Todos os estudantes foram atendidos!\n");
  printf("----------------------------------------\n\n");
}
