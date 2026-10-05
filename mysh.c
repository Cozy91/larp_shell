#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>

int handle_redirect(char *args[]);
int handle_cd(char* args[]);

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
    args[++nargs] = strtok(NULL," ");
  }

 if(args[0] == NULL)continue;
 if(strcmp(args[0],"exit") == 0) exit(0);
   pid_t pid=fork(); 

   if(pid > 0){ //parent process
     wait(NULL);
   }

   else{ //child process
   if(handle_redirect(args)) exit(0);
   if(handle_cd(args)) continue;
   execvp(args[0],args);
      
   }
  }

}

int handle_redirect(char* args[]){
  for(int i=0;args[i]!=NULL;i++){
    if(strcmp(args[i], ">") == 0){
      FILE *fp=fopen(args[i+1],"w"); 
      for(int j=0;j<i;j++){
        fwrite(args[j],sizeof(char),strlen(args[j]),fp);
      }
          fclose(fp);
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

