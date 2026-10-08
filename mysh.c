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
   if(pipes(args)) continue;
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
     dup2(fd,STDOUT_FILENO);//making STDOUT_FILENO point to the same place where fd points to
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
  int j,filedes[2],k;
  bool found=false;
   for(int i=0;args[i]!=NULL;i++){
    if(strcmp(args[i],"|")==0){
      k=i+1;
      j=i-1;
      found=true;
      break;
    }
  }
   if(!found){return 0;}
   char *newargs[20];
   for(int i=0;i<=j;i++){
         newargs[i]=args[i];
   }
  newargs[j+1]=NULL; //for splitting the given command into two parts " " | " "
 char *ch2args[20];
 int i;
 for(i=0;args[k]!=NULL;i++){
   ch2args[i] = args[k];
   k++;
 }
 ch2args[i]=NULL;
    //int pipe(int filedes[2]);
    if(pipe(filedes) == -1){fprintf(stderr, "error creating pipe\n"); return 0;}
     
    pid_t child1=fork(); //first child who will write into the pipe
    
    if(child1 == 0){
       close(filedes[0]); // don't need the read end while writing to pipe
      
       dup2(filedes[1],STDOUT_FILENO); //just an alias for STDOUT_FILENO, filedes[1] will have same behavior as STDOUT_FILENO or the standard output points to the pipe's write end  
       close(filedes[1]);
       execvp(newargs[0],newargs);

    }
     pid_t child2=fork(); // for reading from the pipe
     
     if(child2 == 0){
       close(filedes[1]); // don't need the write end 
       
       dup2(filedes[0],STDIN_FILENO); // the read end will behave same as stdin or the standard input points to the pipe's read end or make this process's standard input come from the pipe
       close(filedes[0]);
       execvp(ch2args[0],ch2args);

     }
     close(filedes[0]);
     close(filedes[1]);
     int status;
     waitpid(-1,&status,0);
     waitpid(-1,&status,0);
     return 1;
   }
