#include <pthread.h>
#include <stdio.h>

typedef struct {
  int number;
  int is_prime;
} number_t;

void *is_prime_thread(void *arg) {
  number_t *num = (number_t *)arg;
  int i;

  if (num->number <= 1) {
    num->is_prime = 0;
    return NULL;
  }

  for (i = 2; i * i <= num->number; i++) {
    if (num->number % i == 0) {
      num->is_prime = 0;
      return NULL;
    }
  }

  num->is_prime = 1;
  return NULL;
}

int main() {
  int numbers[] = {12, 43, 49};
  int num_elements = sizeof(numbers) / sizeof(numbers[0]);
  pthread_t threads[num_elements];
  number_t nums[num_elements];
  int i;

  for (i = 0; i < num_elements; i++) {
    nums[i].number = numbers[i];
    nums[i].is_prime = -1;
    pthread_create(&threads[i], NULL, is_prime_thread, &nums[i]);
  }

  for (i = 0; i < num_elements; i++) {
    pthread_join(threads[i], NULL);
    if (nums[i].is_prime == 1) {
      printf("%d angka prima\n", nums[i].number);
    } else {
      printf("%d bukan angka prima\n", nums[i].number);
    }
  }

  return 0;
}
