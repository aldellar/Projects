#include "dining.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct dining {
  // TODO: Add your variables here
  int capacity;
  // tells us how many spots (resources) we have left to use used to moderate flow of student so they dont overfill the dining hall
  sem_t spots_left;
  // 1 means dining hall is emtpy 0 means student or cleaner is in the dining hall
  sem_t mutex;
  // 1 means there are no waiting cleaners 0 means there is a cleaner waiting and students cannot enter
  sem_t waiting_cleaner;
  //we are using this to get the value of spots left
  int val;
} dining_t;

dining_t *dining_init(int capacity) {
  // TODO: Initialize necessary variables
  dining_t *dining = malloc(sizeof(dining_t));
  dining->capacity = capacity;
  sem_init(&dining->waiting_cleaner, 0, 1);
  sem_init(&dining->spots_left, 0, capacity);
  sem_init(&dining->mutex, 0, 1);
  return dining;
}
void dining_destroy(dining_t **dining) {
  //freeing all semaphores
  sem_destroy(&(*dining)->spots_left);
  sem_destroy(&(*dining)->mutex);
  sem_destroy(&(*dining)->waiting_cleaner);
  //freeing the dining structure
  free(*dining);
  *dining = NULL;
}

void dining_student_enter(dining_t *dining) {
  // wait for him and instantly increment it back up
  sem_wait(&dining->waiting_cleaner);
  sem_post(&dining->waiting_cleaner);
  // if it is full then wait til someone leaves
  sem_wait(&dining->spots_left);
  //show the dining hall is not empty by decrementing the mutex but if another student has already done it then we should not block execution just keep running
  sem_trywait(&dining->mutex);
}

void dining_student_leave(dining_t *dining) {
  // we are done so increment the amount of spots available so another student can enter
  sem_post(&dining->spots_left);
  // if this is the last student to leave
  sem_getvalue(&dining->spots_left, &dining->val);
  if (dining->val == dining->capacity) {
    //set mutex to 1 showing that we can allow a cleaner to enter
    sem_post(&dining->mutex);
  }
}

void dining_cleaning_enter(dining_t *dining) {
  //show that there is a wainting cleaner and block students from entering
  sem_wait(&dining->waiting_cleaner);
  //wait until all students leave and dining hall is empty to eneter
  sem_wait(&dining->mutex);
  // instantly set it back to free until another student enters
  sem_post(&dining->mutex);
}

void dining_cleaning_leave(dining_t *dining) {
  // show that there is no waiting cleaner so we can enter
  sem_post(&dining->waiting_cleaner);
}
