
/*
 *
 * CS-252
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <string.h>
#include "y.tab.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


extern "C" char * read_line();

int mygetc(FILE * f) {
  static char *p;
  char ch;

  if (!isatty(0)) {
    // stdin is not a tty. Call real getc
    return getc(f);
  }

  // stdin is a tty. Call our read_line.

  if (p==NULL || *p == 0) {
    char * s = read_line();
    p = s;
  }

  ch = *p;
  p++;

  return ch;
}

#undef getc
#define getc(f) mygetc(f)

static  void yyunput (int c,char *buf_ptr  );

void myunputc(int c) {
    unput(c);
}

%}

%%

\n 	{
		return NEWLINE;
	}

[ \t] 	{
		/* Discard spaces and tabs */
     	}	

">" 	{
		return GREAT;
	}

"<"	{
		return LESS;
	}

">&"	{
		return GREATAMPERSAND;
	}

"&"	{
		return AMPERSAND;
	}

">>"	{
		return GREATGREAT;
	}

">>&"   {
		return GREATGREATAMPERSAND;
	}

"|"	{
		return PIPE;
	
	}


\`[^\n\`]*\` {

	int defin = dup(0);
	int defout = dup(1);

	int fdpipe[2];
	int fdpipe1[2];
	int out;
	int i ,j = 0; 

	char* temp = (char*)malloc(4000);
	for(i = 1; i < strlen(yytext)-1; i++ ){
		temp[j] = yytext[i];
		j++;
	}
	
	temp[j] = '\0';
	

	
	//printf("aaa %s\n",temp);
	

	if(pipe(fdpipe) < 0){
		perror("pipe");
		exit(0);
	}
	if(pipe(fdpipe1) < 0){
		perror("pipe");
		exit(0);
	}

	dup2(fdpipe[0],0);
	close(fdpipe[0]);

	dup2(fdpipe1[1],1);
	close(fdpipe1[1]);

	int ret = fork();
	if(ret == 0){
		
		dup2(fdpipe1[0],0);
		close(fdpipe1[0]);

		dup2(fdpipe[1],1);
		close(fdpipe[1]);
		
		
		//char*arrays[2];
		//arrays[0] = strdup("../shell");
		//arrays[1] = NULL;

		execlp("/proc/self/exe","/proc/self/exe",NULL);

		perror("execvp");
		exit(0);
	}else if(ret < 0){
		perror("fork");
		exit(0);
	}

	strcat(temp,"\n");
	write(1,temp,strlen(temp));
	dup2(defout,1);
	close(defout);	
	//	printf("temp :%s\n", temp);

	close(fdpipe[0]);
	close(fdpipe[1]);
	close(fdpipe1[0]);
	close(fdpipe1[1]);
	char a;
	int hh = 0;
	char*a1 = (char*)malloc(4000);  
	dup2(defout,1);
	close(defout);
	while(read(0,&a,1)!= 0){
		if(a  == '\n'){
			a1[hh] = ' ';
		}else{
			a1[hh] = a;
		}
		hh++;
	}

	a1[hh] = '\0';
	//read(0,&a1,strlen(a1));
	//printf("ddd :%s\n",a1);
	dup2(defin,0);
	close(defin);
	
	
	waitpid(ret,0,0);
 	//yyunput(strlen(a1),a1);
 	int ff;
 	for(ff = strlen(a1); ff >= 0 ; ff-- ){
 		myunputc(a1[ff]); 
 	}
	
}

 \"[^\n\"]*\" {
	int i ,j = 0; 
	char* temp = (char*)malloc(100);
	for(i = 1; i < strlen(yytext)-1; i++ ){
		temp[j] = yytext[i];
		j++;
	}
	temp[j] = '\0';
	yytext = temp;
	//printf("pbj: %s",temp);
	yylval.string_val = strdup(temp);
	return WORD;
}

[^ >|\t\n][^ >|\t\n]*  {
		/* Assume that file names have only alpha chars */
		if(!strcmp(yytext,"exit")){
			printf("Bye!\n");
			exit(0);
		}else{
			yylval.string_val = strdup(yytext);
		return WORD;
		}
}



