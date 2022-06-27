#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <fcntl.h>


void main(){
  char* cmd[3]={"clear","touch","strconcat"};
  while(1){
    printf("Shell> ");
    char *str;
    int p=0, ri=0, ro=0;
    char ch = 'a';//any value
    int len = 0,size = 0,spaces = 0;
    str = realloc(NULL, sizeof(*str)*size);
    while(ch != '\n') {
      scanf("%c", &ch);
      if(ch == '|')
        p=1;
      if(ch == '<')
        ri=1;
      if(ch == '>')
        ro=1;
      if(ch == ' ')
        spaces++;
        str[len++]=ch;
        if(len==size){
          str = realloc(str, sizeof(*str)*(size+=1));
        }
    }
    str[len++]='\0';
    //printf("%d  |  %s\n", spaces+1,str);

    char *strD[spaces+1];
    char * token=strtok(str," ");

    int u=0;
    while(token != NULL)
    {
      strD[u] = (char*) realloc(NULL, sizeof(*token));
      if(u==spaces){
      	int i=0;
      	while(token[i+1] != '\0') {
		strD[u][i]=token[i];
     		i++;
	}
      }
      else{
      	strcpy(strD[u],token);
      }
      token=strtok(NULL," ");
      u++;
    }
    strD[u]="\0";

    //printf("\n\n%s %s\n\n",strD[0],strD[1]);
    int i1,i2;
    for(i1=0;i1<spaces+1;i1++){
      if(strcmp(strD[i1],"|")==0 || strcmp(strD[i1],"<")==0 || strcmp(strD[i1],">")==0){
        i2=spaces+1-i1;
        i1++;
        //printf("%i , %i",i1,i2);
        break;
      }
    }
    char* s1[i1];
    char* s2[i2];
    if(p==1 || ri==1 || ro==1){
	    for(u=0;u<i1-1;u++){
	      s1[u] = (char*) realloc(NULL, sizeof(*strD[u]));
	      strcpy(s1[u],strD[u]);
	      //printf("\n\"%s\"",s1[u]);
	    }
	    s1[u++] = NULL;
	    int v;
	    for(v=0;u<i1+i2-1;u++){
	      s2[v] = (char*) realloc(NULL, sizeof(*strD[u]));
	      //strcpy(s2[v],strD[u]);
	      int i=0;
	      if(u==(i1+i2-1)){
	      	while(strD[u][i+1] != '\0') {
			s2[v][i]=strD[u][i];
	     		i++;
		}
	      }
	      else{
	      	strcpy(s2[v],strD[u]);
	      }
	      //printf("\n\"%s\"",s2[v]);
	      v++;
	    }
    }
    
    //s2[v-1]="-l";
    
	/*for(u=0;u<i1;u++){
      printf("\n\"%s\"",s1[u]);
    }
    printf("\n");
    s1[u++] = "\0";
    for(v=0;u<i1+i2;u++){
      printf("\n\"%s\"",s2[v]);
      v++;
    }*/

  //printf("\n\n%s",strD[0]);
  int fd[2];
  if(strcmp(cmd[0],strD[0])==0){
       system(cmd[0]);
  }
  else if(strcmp(cmd[1],strD[0])==0){
      fopen(strD[1],"w+");
  }
  else if(strcmp(cmd[2],strD[0])==0){
      char* s = (char*) realloc(NULL, sizeof(*strD[1])+sizeof(*strD[2]));
      strcpy(s,strD[1]);
      strcat(s,strD[2]);
      printf("%s",s);
  }
  else if(p==1){
      if (pipe(fd) == -1)
        perror("Failed to create the pipe");
      if (fork() == 0) {
        dup2(fd[1], 1);
        close(fd[0]);
        execvp(s1[0],s1);
      }
      else if (fork() == 0) {
        dup2(fd[0], 0);
        close(fd[1]);
        execvp(s2[0],s2);
      }
      else{
        close(fd[0]);
        close(fd[1]);
        wait(0);
        wait(0);
      }
    }
    else if(ri==1 || ro==1){
    	if(fork()==0){
    		//printf("ERORRRRRRRR\n");
	    	int f = open(s2[0], O_RDWR|O_CREAT|O_APPEND, 0600);
	    	int save_out;
		int save_in;
	    	if(ri){
			save_in = dup(fileno(stdin));
	    		if (-1 == dup2(f, fileno(stdin))) { perror("cannot redirect stdout"); return 255; }	
	    		execvp(s1[0],s1);
	    	}
	    	if(ro){
	    		save_out = dup(fileno(stdout));
	    		if (-1 == dup2(f, fileno(stdout))) { perror("cannot redirect stdout"); return 255; }
	    		execvp(s1[0],s1);
	    	}
    	}
    	else
    		wait(0);
    }
    else {
	    if(fork()==0){
	    	execvp(strD[0],strD);
	    	printf("%s_%s\n",strD[0],strD[1]);
	    }
	    else
	    	{
	    	wait(0);
	    }//main();
    }
  }
}