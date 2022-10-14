// Code cited:
// scaler.com/topics/stack-in-c/
// educative.io/answers/splitting-a-string-using-strtok-in-c

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

#define CMDLINE_MAX 512
#define ARGS_MAX 15
#define ONE_IS_ENOUGH 1
#define JOB_MAX 4

struct job
{
    char *args[ARGS_MAX];
    char *filename[ONE_IS_ENOUGH];
};

struct node{
    char directory[CMDLINE_MAX];
    struct node *next;
};

struct node *head;

void display_prompt(void);
void read_cmd(char *cmdargs);
bool empty_cmdargs(char *cmdargs);
int parse_cmdargs(char *cmdargs, char **args, char **filename);
bool empty_cmd(char *cmd);
bool is_exit(char *cmd);
bool is_cd(char *cmd);
void my_cd(char *arg);
bool is_pwd(char *cmd);
void my_pwd(void);

void display_prompt(void)
{
    fprintf(stdout, "sshell@ucd$ ");
}

void read_cmd(char *cmdargs)
{
    fgets(cmdargs, CMDLINE_MAX, stdin);
}

bool empty_cmdargs(char *cmdargs)
{
    if (!strcmp(cmdargs, "\n"))
    {
        return true;
    }
    else
    {
        return false;
    }
}

int parse_cmdargs(char *cmdargs, char **args, char **filename)
{
    // printf("cmdargs = %s\n", cmdargs);
    if (cmdargs[0] == '\n')
    {
        return 2;
    }

    long unsigned int i = 0;
    int j = 0;
    int redirect_enable = 0;
    int redirect_detected = 0;

    //To save the current character
    /*
    Need to be a string to pass into strcat()
    */
    // char curr_character = '\0';
    
    // printf("--------------------------\n");
    // printf("starting parse_cmdargs\n");

    char curr_arg[CMDLINE_MAX] = "";
    while (i < strlen(cmdargs))
    {
        // printf("i = %d , j = %d\n", (int)i, j);
        if (j >= ARGS_MAX)
        {
            return 1;
        }
        // printf("args[0] = %s\n", args[0]);
        else if (cmdargs[i] == '\n')
        {
            // printf("cl detected\n");
            if ((int)strlen(curr_arg) != 0)
            {
                // printf("there is a curr_arg\n");
                if (redirect_enable == 1)
                {
                    if (j == 0)
                    {
                        return 2;
                    }
                    // printf("redirect enabled\n");
                    // printf("curr_arg = %s\n", curr_arg);
                    filename[0] = (char *)malloc(strlen(curr_arg) * sizeof(char));
                    strcpy(filename[0], curr_arg);
                    strcpy(curr_arg, "");
                    // printf("filename = %s\n", filename[0]);
                }
                else
                {
                    // printf("redirect unenabled\n");
                    // printf("args[0] = %s\n", args[0]);
                    args[j] = (char *)malloc(strlen(curr_arg)*sizeof(char));
                    strcpy(args[j], curr_arg);
                    strcpy(curr_arg, "");
                    // printf("args[%d] = %s\n", j, args[j]);
                }
            }
            // printf("args[0] = %s\n", args[0]);
            break;
        }
        else if (cmdargs[i] == '>' || cmdargs[i] == '<')
        {

            // printf("redirect enabled\n");
            redirect_detected = 1;
            redirect_enable = 1;
            if ((int)strlen(curr_arg) != 0)
            {
                // printf("there is curr_arg\n");
                args[j] = (char *)malloc(strlen(curr_arg)*sizeof(char));
                strcpy(args[j], curr_arg);
                strcpy(curr_arg, "");
                j++;
            }
            // printf("args[%d] = %s\n", j, args[j]);
            i++;
            
        }
        else if (cmdargs[i] != ' ')
        {
            // printf("character detected\n");
            strncat(curr_arg, &cmdargs[i], 1);
            // printf("curr_arg = %s\n", curr_arg);
            i++;
        }
        else if (cmdargs[i] == ' ')
        {
            // printf("whitespace detected\n");
            if ((int)strlen(curr_arg) == 0)
            {
                // printf("there is no curr_arg\n");
                i++;
            }
            else
            {
                // printf("there is a curr_arg\n");
                if (redirect_enable == 1)
                {
                    if (j == 0)
                    {
                        return 2;
                    }
                    // printf("redirect enabled\n");
                    // printf("curr_arg = %s\n", curr_arg);
                    filename[0] = (char *)malloc(strlen(curr_arg) * sizeof(char));
                    strcpy(filename[0], curr_arg);
                    strcpy(curr_arg, "");
                    redirect_enable = 0;
                    i++;
                }
                else
                {
                    // printf("redirect unenabled\n");
                    args[j] = (char *)malloc(strlen(curr_arg)*sizeof(char));
                    strcpy(args[j], curr_arg);
                    strcpy(curr_arg, "");
                    // printf("args[%d] = %s\n", j, args[j]);
                    i++;
                    j++;
                }
            }
        }
    }
    // printf("redirect_detected = %d\n", redirect_detected);
    // printf("filename = %s\n", filename[0]);
    // printf("before last check of curr_arg\n");
    // printf("strlen of curr_arg = %d\n", (int)strlen(curr_arg));
    if ((int)strlen(curr_arg) != 0)
    {
        // printf("there is a curr_arg in the end\n");
        args[j] = (char *)malloc(strlen(curr_arg)*sizeof(char));
        strcpy(args[j], curr_arg);
    }
    if (args[0] == NULL)
    {
        return 2;
    }
    if(redirect_detected == 1 && filename[0] == NULL)
    {
        return 3;
    }
    // printf("end parsing...\n");
    // printf("-------------------------------\n");
    return 0;
}

bool empty_cmd(char *cmd)
{
    if (!strcmp(cmd, ""))
    {
        fprintf(stderr, "Error: missing command\n");
        return true;
    }
    else
    {
        return false;
    }
}

bool is_exit(char *cmd)
{
    if (!strcmp(cmd, "exit"))
    {
        fprintf(stderr, "Bye...\n");
        fprintf(stderr, "+ completed 'exit' [0]\n");
        return true;
    }
    else
    {
        return false;
    }
}

bool is_cd(char *cmd)
{
    if (!strcmp(cmd, "cd"))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void my_cd(char *arg)
{
    int a = chdir(arg);
    if (a == -1)
    {
        fprintf(stderr, "Error: cannot cd into directory\n");
        fprintf(stderr, "+ completed 'cd %s' [1]\n", arg);
    }
    else if (a == 0)
    {
        fprintf(stderr, "+ completed 'cd %s' [0]\n", arg);
    }
}

bool is_pwd(char *cmd)
{
    if (!strcmp(cmd, "pwd"))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void my_pwd(void)
{
    printf("%s\n", getcwd(NULL, 0));
    printf("+ completed 'pwd' [0]\n");
}

bool is_pushd(char *cmd)
{
    if (!strcmp(cmd, "pushd"))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void my_pushd(char *arg)
{
    int a = chdir(arg);
    if (a == -1)
    {
        fprintf(stderr, "Error: no such directory\n");
        printf("+ completed 'pushd %s' [1]\n", arg);
    }
    else if (a == 0)
    {
        struct node *ptr = (struct node*)malloc(sizeof(struct node));
        if (head == NULL)
        {
            strcpy(ptr->directory, getcwd(NULL, 0));
            ptr->next = NULL;
            head = ptr;
        }
        else{
            strcpy(ptr->directory, getcwd(NULL, 0));
            ptr->next = head;
            head = ptr;
        }
        printf("+ completed 'pushd %s' [0]\n", arg);
    }
}

bool is_popd(char *cmd)
{
    if (!strcmp(cmd, "popd"))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void my_popd(void)
{
    if (head->next == NULL)
    {
        fprintf(stderr, "Error: directory stack empty\n");
        printf("+ completed 'popd' [1]\n");
    }
    else
    {
        head = head->next;
        chdir("..");
        printf("+ completed 'popd' [0]\n");
    }
}

bool is_dirs(char *cmd)
{
    if (!strcmp(cmd, "dirs"))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void my_dirs(void)
{
    if(head != NULL)
    {
        // printf("head != NULL\n");
        printf("%s\n", head->directory);
        struct node *ptr;
        ptr = head;
        while(head->next != NULL)
        {
            // printf("head->next != NULL");
            head = head->next;
            printf("%s\n", head->directory);
        }
        head = ptr;
    }
    else
    {
        // printf("head == NULL\n");
        printf("%s\n", getcwd(NULL, 0));
    }

    printf("+ completed 'dirs' [0]\n");
}

int main(void)
{
    // To save the original cwd on the stack
    struct node *ptr = (struct node*)malloc(sizeof(struct node));
    strcpy(ptr->directory, getcwd(NULL, 0));
    ptr->next = NULL;
    head = ptr;

    while (1) 
    {
        char cmdargs[CMDLINE_MAX];
        struct job *job_list[4] = {};

        // char *nl;
        // int retval;

        /* Print prompt */
        display_prompt();

        /* Get command line */
        read_cmd(cmdargs);
        
        if (!isatty(STDIN_FILENO)) 
        {
            printf("%s", cmdargs);
            fflush(stdout);
        }

        if (empty_cmdargs(cmdargs))
        {
            continue;
        }

        if (cmdargs[0] == '|')
        {
            fprintf(stderr, "Error: missing command\n");
            continue;
        }

        char saved_cmdargs[(int)strlen(cmdargs)];
        strcpy(saved_cmdargs,cmdargs);
        
        int errsig;
        int counter = 0;
        for (char *token = strtok(cmdargs, "|"); token != NULL; token = strtok(NULL, "|"))
        {
            job_list[counter] = (struct job*)malloc(sizeof(struct job));
            errsig = parse_cmdargs(token, job_list[counter]->args, job_list[counter]->filename);
            if (errsig != 0)
            {
                break;
            }
            counter++;
            // printf("token = %s\n", token);
        }
        if (errsig == 1)
        {
            fprintf(stderr, "Error: too many process arguments\n");
            continue;
        }
        else if (errsig == 2)
        {
            fprintf(stderr, "Error: missing command\n");
            continue;
        }
        else if (errsig == 3)
        {
            fprintf(stderr, "Error: no output file\n");
            continue;
        }

        // for (int i = 0; job_list[i] != NULL && i < JOB_MAX; i++)
        // {
            // printf("HERE?\n");
            // for (int j = 0; job_list[i]->args[j] != NULL && j < ARGS_MAX; j++)
            // {
                // printf("gotchu\n");
                // printf("job_list[%d].args[%d] = %s\n", i, j, job_list[i]->args[j]);
            // }
            // printf("job_list[%d].args[0] = %s\n", i, job_list[i]->args[0]);
            // printf("job_list[%d].args[1] = %s\n", i, job_list[i]->args[1]);
            // printf("job_list[%d].args[2] = %s\n", i, job_list[i]->args[2]);
            // printf("job_list[%d].args[3] = %s\n", i, job_list[i]->args[3]);
            // printf("job_list[%d].args[4] = %s\n", i, job_list[i]->args[4]);
            // printf("job_list[%d].filename[0] = %s\n", i, job_list[i]->filename[0]);
        // }

        // printf("-------------------------------\n");


        if (is_exit(job_list[0]->args[0]))
        {
            break;
        }
        else if (is_cd(job_list[0]->args[0]))
        {
            my_cd(job_list[0]->args[1]);
            continue;
        }
        else if (is_pwd(job_list[0]->args[0]))
        {
            my_pwd();
            continue;
        }
        else if (is_pushd(job_list[0]->args[0]))
        {
            my_pushd(job_list[0]->args[1]);
            continue;
        }
        else if (is_dirs(job_list[0]->args[0]))
        {
            my_dirs();
            continue;
        }
        else if (is_popd(job_list[0]->args[0]))
        {
            my_popd();
            continue;
        }

        int pids[counter];
        int pipes[counter][2];

        for (int i = 0; i < counter - 1; i++)
        {
            // printf("generating pipes\n");
            pipe(pipes[i]);
        }

        for (int i = 0; i < counter; i++)
        {
            pids[i] = fork();
            if (pids[i] == 0)
            {
                // printf("running child\nchild's args: \n");
                // printf("job_list[%d].args[0] = %s\n", i, job_list[i]->args[0]);
                // printf("job_list[%d].args[1] = %s\n", i, job_list[i]->args[1]);
                // printf("job_list[%d].args[2] = %s\n", i, job_list[i]->args[2]);
                // printf("job_list[%d].args[3] = %s\n", i, job_list[i]->args[3]);
                // printf("job_list[%d].args[4] = %s\n", i, job_list[i]->args[4]);
                // printf("job_list[%d].filename[0] = %s\n", i, job_list[i]->filename[0]);
                // printf("counter = %d\n", counter);
                if (counter > 1)
                {
                    // printf("start arrange pipes\n");
                    // for (int i = 0; i < counter - 1; i++)
                    // {
                            // printf("pipes[%d][0] = %d\n", i, pipes[i][0]);
                            // printf("pipes[%d][1] = %d\n", i, pipes[i][1]);
                    // }
                    if (i == 0)
                    {
                        // printf("arranging first pipe\n");
                        for (int j = 0; j < counter - 1; j++)
                        {
                            close(pipes[j][0]);
                            if(i != j)
                            {
                                close(pipes[j][1]);
                            }
                        }
                        dup2(pipes[i][1], STDOUT_FILENO);
                        // printf("STDOUT = %d\n", STDOUT_FILENO);
                        close(pipes[i][1]);
                    }
                    else if (i == (counter - 1))
                    {
                        // printf("arranging last pipe\n");
                        for (int j = 0; j < counter - 1; j++)
                        {
                            close(pipes[j][1]);
                            if((i - 1) != j)
                            {
                                close(pipes[j][0]);
                            }
                        }
                        dup2(pipes[i-1][0], STDIN_FILENO);
                        // printf("STDIN = %d\n", STDIN_FILENO);
                        close(pipes[i-1][0]);
                    }
                    else
                    {
                        // printf("arranging %dth pipe\n", i);
                        for (int j = 0; j < counter - 1; j++)
                        {
                            if (i != j)
                            {
                                close(pipes[j][1]);
                            }
                            if((i - 1) != j)
                            {
                                close(pipes[j][0]);
                            }
                        }
                        dup2(pipes[i-1][0], STDIN_FILENO);
                        dup2(pipes[i][1], STDOUT_FILENO);
                        // printf("STDOUT = %d\n", STDOUT_FILENO);
                        // printf("STDIN = %d\n", STDIN_FILENO);
                        close(pipes[i-1][0]);
                        close(pipes[i][1]);
                    }
                }
                if (job_list[i]->filename[0] != NULL)
                {
                    if(strchr(saved_cmdargs, '>') != NULL)
                    {
                        // TO check if mislocated ouput redirection
                        if (i != (counter - 1))
                        {
                            exit(3);
                        }
                        // printf("there is a filename\n");
                        int fd;
                        fd = open(job_list[i]->filename[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        // printf("fd = %d\n", fd);
                        if (fd == -1)
                        {
                            // printf("open() failed\n");
                            exit(2);
                        }
                        dup2(fd, STDOUT_FILENO);
                        close(fd);
                    }
                    else
                    {
                        if (i != 0)
                        {
                            exit(5);
                        }
                        // printf("there is a filename\n");
                        int fd;
                        fd = open(job_list[i]->filename[0], O_RDONLY);
                        // printf("fd = %d\n", fd);
                        if (fd == -1)
                        {
                            // printf("open() failed\n");
                            exit(4);
                        }
                        dup2(fd, STDIN_FILENO);
                        close(fd);
                    }
                }
                // if (is_exit(job_list[0]->args[0]))
                // {
                //     break;
                // }
                // else if (is_cd(job_list[0]->args[0]))
                // {
                //     my_cd(job_list[0]->args[1]);
                //     continue;
                // }
                // else if (is_pwd(job_list[0]->args[0]))
                // {
                //     my_pwd();
                //     continue;
                // }
                // else if (is_pushd(job_list[0]->args[0]))
                // {
                //     my_pushd();
                // }
                // else if (is_dirs(job_list[0]->args[0]))
                // {
                //     my_dirs();
                // }
                // printf("before exec\n");
                execvp(job_list[i]->args[0], (char *const*)job_list[i]->args);
                perror("execvp() failed");
                exit(1);
            }
            else if (pids[i] < 0)
            {
                perror("fork() failed");
                exit(1);
            }
        }
        
        for (int i = 0; i < counter - 1; i++)
        {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        int status[counter];

        for (int i = 0; i < counter; i++)
        {
            // printf("before wait()\n");
            waitpid(pids[i], &status[i], 0);
            // printf("status[%d] = %d\n", i, status[i]);
            if ((status[i] / 256) == 2)
            {
                errsig = 4;
                break;
            }
            else if((status[i] / 256) == 3)
            {
                errsig = 5;
                break;
            }
            else if((status[i] / 256) == 1)
            {
                errsig = 6;
                break;
            }
            else if((status[i] / 256) == 4)
            {
                errsig = 7;
                break;
            }
            if ((status[i] / 256) == 5)
            {
                errsig = 8;
                break;
            }
        }

        if(errsig == 4)
        {
            fprintf(stderr, "Error: cannot open output file\n");
            continue;
        }
        else if(errsig == 5)
        {
            fprintf(stderr, "Error: mislocated output redirection\n");
            continue;
        }
        else if(errsig == 6)
        {
            fprintf(stderr, "Error: command not found\n");
            
            char *nl = strchr(saved_cmdargs, '\n');
            if (nl)
            {
                *nl = '\0';
            }

            printf("+ completed '%s' ", saved_cmdargs);
            for (int i = 0; i < counter; i++)
            {
                printf("[%d]", (status[i] / 256));
            }
            printf("\n");

            continue;
        }
        else if(errsig == 7)
        {
            fprintf(stderr, "Error: cannot open input file\n");
            continue;
        }
        else if(errsig == 8)
        {
            fprintf(stderr, "Error: mislocated input redirection\n");
            continue;
        }

        /* Remove trailing newline from command line */
        char *nl = strchr(saved_cmdargs, '\n');
        if (nl)
        {
            *nl = '\0';
        }

        fprintf(stderr, "+ completed '%s' ", saved_cmdargs);
        for (int i = 0; i < counter; i++)
        {
            fprintf(stderr, "[%d]", (status[i] / 256));
        }
        fprintf(stderr, "\n");

            //printf("d = %d\n", (int)strchr(cmdargs, '|'));
    }

            // pid_t pid;

            // pid = fork();
            // if (pid == 0)
            // {
            //         if (filename[0] != NULL)
            //         {
            //                 int fd;
            //                 fd = open(filename[0], O_WRONLY | O_CREAT | O_TRUNC);
            //                 dup2(fd, STDOUT_FILENO);
            //                 close(fd);
            //         }
            //         execvp(args[0], (char *const*)args);
            //         perror("execvp");
            //         exit(1);
            // }
            // else if (pid > 0)
            // {
            //         int status;
            //         waitpid(pid, &status, 0);
            //         printf("+ completed '%s' [%d]\n", cmd, status);
            // }
            // else
            // {
            //         perror("fork");
            //         exit(1);
            // }

            // /* Print command line if stdin is not provided by terminal */
            

            // /* Remove trailing newline from command line */
            // nl = strchr(cmd, '\n');
            // if (nl)
            //         *nl = '\0';

            // /* Builtin command */
            // if (!strcmp(cmd, "exit")) {
            //         fprintf(stderr, "Bye...\n");
            //         break;
            // }

            // /* Regular command */
            // retval = system(cmd);
            // fprintf(stdout, "Return status value for '%s': %d\n",
            //         cmd, retval);

    return EXIT_SUCCESS;
}
