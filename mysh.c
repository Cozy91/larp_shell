#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
#include<fcntl.h>
#include<limits.h>
int handle_redirect(char *args[]);
int handle_cd(char* args[]);
int pipes(char* args[]);

int main(int argc,char *argv[]){
char buf[1024];
FILE* input = stdin;

if(argc > 1){
  input=fopen(argv[1],"r");
  
}

  while(true){
  if(input == stdin){
   printf("larp > "); 
  }
  
  if(fgets(buf,sizeof(buf),input) == NULL){ // for shell scripting
    if(input != stdin){
        fclose(input);
        input=stdin; //if no script
        continue;
    }
  }
 
  char* nl=strchr(buf,'\n');
  if(nl)*nl='\0';

  char* args[20];
  int nargs=0;
  
  args[0]=strtok(buf," ");
  while(args[nargs] != NULL){
    args[++nargs] = strtok(NULL," "); //tokenisation
  }

 if(args[0] == NULL)continue;
 if(strcmp(args[0],"exit") == 0) exit(0);
   pid_t pid=fork(); 

   if(pid > 0){ //parent process
     wait(NULL);
   }

   else{ //child process
   if(handle_redirect(args));
   if(handle_cd(args)) continue;
   execvp(args[0],args);
      
   }
  }

}

int handle_redirect(char* args[]){
  char *path;
  for(int i=0;args[i]!=NULL;i++){
    if(strcmp(args[i], ">") == 0){
      
        path=args[i+1];
      
     int fd=open(path,O_WRONLY|O_CREAT|O_TRUNC,0644); //0644-read+write permissions
     dup2(fd,STDOUT_FILENO);//to make fd point to the same thing as STDOUT_FILENO(stdout)
     close(fd);
     args[i]=NULL; // for >
     return 1;
      }
  }
  return 0;
}

int handle_cd(char *args[]){
   // char* path;
   if(strcmp(args[0],"cd") == 0){
    
    if(chdir(args[1]) == 0) return 1;
   }
  return 0;
}

int pipes(char* args[]){
  int j;
  char buff[PIPE_MAX];
  
  for(int i=0;args[i]!=NULL;i++){
    if(args[i]=='|'){
      j=i;
      break;
    }
  
    int pipe(int filedes[2]);
    if(pipe(filedes) == -1)fprintf(stderr, "error creating pipe\n");

    switch(fork()){
      case 0: //to check if read end of file is closed while writing the contents of pipe to child process
        if(filedes[0] == -1)fprintf(stderr, "pipe not working\n");
      
        while(read(filedes[0],buff,))

        break;
      case default: //for checking the read end of pipe for parent
        if(filedes[2]==-1)fprintf(stderr, "pipe not working\n");
    }
  }
}
