#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t pid;
  int fd[2];
  pipe(fd);

  pid = fork();
  if (pid == 0) {
    close(fd[0]);
    dup2(fd[1], STDOUT_FILENO);
    close(fd[1]);

    execlp("ls", "ls", "-1", (char *)NULL);
    perror("execlp");
    exit(1);
  } else {
    close(fd[1]);
    dup2(fd[0], STDIN_FILENO);
    close(fd[0]);

    char buffer[1024];
    int count = 0;
    while (read(STDIN_FILENO, buffer, 1024) > 0) {
      printf("%s\n", buffer);
      count++;
      if (count == 3)
        break;
    }
    wait(NULL);
  }

  char cwd[1024];
  getcwd(cwd, sizeof(cwd));
  printf("Current working directory: %s\n", cwd);

  return 0;
}
