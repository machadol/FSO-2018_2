#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

#define MIN_STUDENT 3
#define MAX_STUDENT 10
#define ASSISTENT_TIMES 3

// Define threads
void * studentThread(void * arg);
void * assistentThread();
int generateWaitTime();

// Define counts
int studentCount = 0;
int chairCount = 0;
int chairFree = 0;

// Pthreads needed
pthread_t assistent;
pthread_t * students;
// pthread_mutex_t mutex;
sem_t sem;

int main(int argc, char const *argv[]) {
  
  // Creating random numbers for students and chairs
  srand(time(NULL));
  studentCount = rand() % (MAX_STUDENT - MIN_STUDENT) + MIN_STUDENT;
  chairCount = studentCount / 2;
  chairFree = chairCount;

  // HEADER
  printf("----------------------------------------\n");
  printf("Starting assistance\n");
  printf("----------------------------------------\n");
  printf("Number of students: %d \n", studentCount);
  printf("Number of chairs: %d \n", chairCount);
  printf("----------------------------------------\n");

  // Create semaphore
  sem_init(&sem, 0, chairCount);

  int value = 10;
  if (sem_getvalue(&sem, &value) == 0) {
    printf("sucesso!: value: %d\n", value);
  } else {
    printf("erro!: value: %d\n", value);
  }

  // Thread for assistent
  pthread_create(&assistent, NULL, assistentThread, NULL);

  // Thread for students
  students = (pthread_t *) malloc(studentCount * sizeof(pthread_t));
  for (int i = 0; i < studentCount; i++) {
    pthread_create(&students[i], NULL, studentThread, (void *) i+1);
  }

  // Wait Threads
  pthread_join(assistent, NULL);
  for (int i = 0; i < studentCount; i++) {
    pthread_join(students[i], NULL);
  }

  // FOOTER
  printf("----------------------------------------\n");
  printf("Show, todos os alunos foram atendidos!!!\n");
  return 0;
}

// Function to control student
void * studentThread(void * arg) {
  int studentCode = (int) arg;
  int assistentTimes = ASSISTENT_TIMES;
  // printf("Thread of student %d \n", studentCode);

  while (assistentTimes > 0) {
    if (chairFree > 0) {
      // Have chair empty, wait to be assisted by assitent
      chairFree--;
      assistentTimes--;
      sem_wait(&sem);
      printf("Student %d waiting for assistent\n", studentCode);
    } else {
      // All chairs are occuped, back to work
      sleep(generateWaitTime());
    }
  }

  printf("||||||| Student %d concluded |||||||\n", studentCode);
  pthread_exit(0);
}

// Function to control assistent
void * assistentThread() {
  int status = 0; // 0 to sleep, 1 to assist
  // printf("Thread of assistend\n");

  int assistents = (studentCount - 1) * ASSISTENT_TIMES;
  while (assistents > 0) {

    if (chairFree <= 0) {
      // Helping some student
      printf("Assisting...\n");
      chairFree++;
      assistents--;
      sem_post(&sem);
      sleep(generateWaitTime());
    } else {
      // No one students to help, sleep
      printf("Assistent sleeping...\n");
      sleep(generateWaitTime());
    }
    // printf("Assistencias: %d", assistents);
  }
  pthread_exit(0);
}

int generateWaitTime() {
  srand(time(NULL));
  return rand() % 5 + 1;
}
