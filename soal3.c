#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define MAX_NUMBERS 100

int numbers[MAX_NUMBERS];
int count = 0;

void handle_client(int new_socket) {
  char buffer[1024] = {0};
  read(new_socket, buffer, sizeof(buffer));

  if (strncmp(buffer, "POST", 4) == 0) {
    int number = atoi(buffer + 5);
    if (count < MAX_NUMBERS) {
      numbers[count++] = number;
      send(new_socket, "Penambahan angka berhasil\n",
           strlen("Penambahan angka berhasil\n"), 0);
    }
  } else if (strncmp(buffer, "GET", 3) == 0) {
    char response[1024] = {0};
    for (int i = 0; i < count; ++i) {
      char num_str[12];
      sprintf(num_str, "%d ", numbers[i]);
      strcat(response, num_str);
    }
    strcat(response, "\n");
    send(new_socket, response, strlen(response), 0);
  } else {
    send(new_socket, "Bad request\n", strlen("Bad request\n"), 0);
  }
  close(new_socket);
}

void *run_server(void *arg) {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("setsockopt");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 3) < 0) {
    perror("listen");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  printf("Server listening on port %d\n", PORT);

  while ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                              (socklen_t *)&addrlen)) >= 0) {
    handle_client(new_socket);
  }

  if (new_socket < 0) {
    perror("accept");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  return NULL;
}

void send_request(const char *message) {
  int sock = 0;
  struct sockaddr_in serv_addr;
  char buffer[1024] = {0};

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\n Socket creation error \n");
    return;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    printf("\nInvalid address/ Address not supported \n");
    close(sock);
    return;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("\nConnection Failed \n");
    close(sock);
    return;
  }

  send(sock, message, strlen(message), 0);
  read(sock, buffer, sizeof(buffer));
  printf("%s", buffer);

  close(sock);
}

void run_client() {
  char command[256];

  while (1) {
    printf("Enter command (POST <number> or GET or EXIT): ");
    fgets(command, sizeof(command), stdin);
    command[strcspn(command, "\n")] = '\0'; // Remove trailing newline

    if (strcmp(command, "EXIT") == 0) {
      break;
    } else if (strncmp(command, "POST", 4) == 0 ||
               strcmp(command, "GET") == 0) {
      send_request(command);
    } else {
      printf("Invalid command. Please use POST <number> or GET or EXIT.\n");
    }
  }
}

int main() {
  pthread_t server_thread;

  // Start the server in a separate thread
  pthread_create(&server_thread, NULL, run_server, NULL);

  // Run the client in the main thread
  run_client();

  // Wait for the server thread to finish (it won't, since it's a loop)
  pthread_join(server_thread, NULL);

  return 0;
}
