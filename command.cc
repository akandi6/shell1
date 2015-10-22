
/*
 * CS252: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <libgen.h>
#include <regex.h>
#include <dirent.h>
#include "command.h"

int nEntries = 0;
int maxEntries = 20;
char **array  = (char**)malloc(maxEntries*sizeof(char*));
extern char **environ;

SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}
	
	int i = 2;
	
	int countm = 0;
	
	char* temp = argument;
	char* k1  = (char*)malloc(100);	

	//printf("argument og: %s\n",argument );	

	for (int i = 0; i < strlen(argument); i++)
	{
		if(argument[i] == '\\'){
			int hh;
			for ( hh = i+1; hh < strlen(argument) ; hh++)
			{
				if(argument[hh] == '\\'){	
					hh = hh+1;
					k1[countm] = argument[hh]; 
					//printf("fff: %c\n",k1[countm]);
					countm++;
				}else{
					k1[countm] = argument[hh]; 
					countm++;
				}
			}
			i = hh;
		}else
		if(argument[0] == '~'  && argument[1] == '\0' ){
			char* user =  getenv("USER");
			struct passwd*pass = getpwnam(user);
			char*dir = (char*)malloc(100);
			dir = pass->pw_dir;
			for (int jj = 0; jj < strlen(dir); jj++)
			{
				k1[countm] = dir[jj]; 
				countm++;
			}
			k1[countm] = '/';
			countm++;		
		}else if(argument[0] == '~'  && argument[1] != '\0' ){
			int k = 1;
			int k2 = 0;
			char*user1  = (char*)malloc(100);
			while(argument[k] != '/' && argument[k] != '\0'){
				user1[k2] = argument[k];
				k++;
				k2++;
			}
			user1[k2] == '\0';

			struct passwd*pass1 = getpwnam(user1);
			char*dir2 = (char*)malloc(100);
			dir2 = pass1->pw_dir;
			
			//if(argument[k+1] != '\0'){
			for (int ff = 0; ff < strlen(dir2); ff++)
			{
				k1[countm] = dir2[ff]; 
				countm++;
			}
			k1[countm] = '/';
			countm++;
			i = k;
			//}
		}
		else if(argument[i] == '$' && argument[i+1] == '{'){
			char* k  = (char*)malloc(100);
			int f = i+2;
			int j = 0;
			while(argument[f] != '}'){
				k[j] = argument[f];
				j++;
				f++;
			}	
			k[j] = '\0';
			//printf("k    %s\n",k );
			char* tt = (char*)malloc(100);
			tt = getenv(k);
			for (int ii = 0; ii < strlen(tt); ii++)
			{
				k1[countm] = tt[ii]; 
				countm++;
			}
		//	printf(" k1  %s\n",k1 );
			i = f;
		//	free(k);
		}else{
			k1[countm] = argument[i];
			countm++;
		}
	}

	k1[countm] = '\0';
		/*if(argument[0] == '$'){
			while(temp[i] != '}'){
				k[j] = temp[i];
				i++;
				j++;
			}
			k[j] = '\0';
			_arguments[ _numberOfArguments ] = getenv(k);
		}else{
			_arguments[ _numberOfArguments ] = argument;
		}*/
	
	//printf("modified %s\n",k1 );		



	

	// Add NULL argument at the end
	_arguments[ _numberOfArguments ] = k1;
	
	_arguments[ _numberOfArguments + 1] = NULL;
	
	_numberOfArguments++;
	
}
int cmp(const void *a , const void *b){
	const char*aa = *(const char**)a;
	const char*bb = *(const char**)b;

	return strcmp(aa,bb);
}

void expand(char*p,char*suff){
	if(suff[0] == 0){
		//Command::_currentSimpleCommand->insertArgument( strdup(p));
		if(nEntries == maxEntries){
				maxEntries *=2;
				array = (char**)realloc(array,maxEntries*sizeof(char*));
		}

			array[nEntries] = strdup(p);
		//	printf("entry :%s %d \n",array[nEntries], nEntries);
			nEntries++;
		
		return;
	}

	char*s = strchr(suff,'/');
	char comp[1024];
	if(s != NULL){
		strncpy(comp,suff,s-suff);
		suff = s+1;
	}else{
		strcpy(comp,suff);
		suff = suff + strlen(suff); 
	}
	char newp[1024];
	if(strchr(comp,'*') == NULL  && strchr(comp,'?') == NULL){
		if(comp[0] != '\0'){
			sprintf(newp,"%s/%s",p,comp);
			expand(newp,suff);
		}else{
			expand(strdup(""),suff);
		}	
		return;
	}

	char*reg = (char*)malloc(2*strlen(suff)+10);
	char*a = comp;
	char*r = reg;
	*r = '^';
	r++;
	while(*a){
		if(*a == '*' ){
			*r = '.';
			r++;
			*r = '*';
			r++;
		}else if(*a == '?'){
			*r = '.';
			r++;
		}else if (*a == '.'){
			*r = '\\';
			r++;
			*r = '.';
			r++;
		}else{
			*r = *a;
			r++;
		}	
		a++;
	}
	*r = '$';
	r++;
	*r = 0;
	regex_t re;
	int expbuf = regcomp(&re,reg,REG_EXTENDED|REG_NOSUB);
	if(expbuf != 0){
		perror("compile");
		return;
	}
	DIR*d ;

	if(p == NULL){
		d = opendir(".");
	}else if(strchr(p,'/') == NULL){
		d = opendir("/");
	}
	else{
		d = opendir(p);
	}

	
	if(d == NULL){
		return;
	}

	struct dirent*ent;
	while((ent = readdir(d))!= NULL){
		regmatch_t match;
		if(regexec(&re,ent->d_name,1,&match,0) == 0 ){
			//Command::_currentSimpleCommand->insertArgument( strdup(ent->d_name));
			if(ent->d_name[0] == '.'){
				if(comp[0] == '.'){
					if(p != NULL){
						if(strlen(p) == 1 && p[0] == '/'){		
				 	  		sprintf(newp,"/%s",ent->d_name);
				 	  	}else
				 	  	sprintf(newp,"%s/%s",p,ent->d_name);

					}else{
						sprintf(newp,"%s",ent->d_name);
					}
					expand(newp,suff);
				}
			}else{
				if(p != NULL){		
				    sprintf(newp,"%s/%s",p,ent->d_name);
				}else{
					sprintf(newp,"%s",ent->d_name);
				}
				expand(newp,suff);
			}
		}
	}
	closedir(d);
}

void 
SimpleCommand::wildcard(char* argument){
	if(strchr(argument,'*') == NULL && strchr(argument,'?') == NULL){
		insertArgument(strdup(argument));
	}else{

	/*char*reg = (char*)malloc(2*strlen(argument)+10);
	char*a = argument;
	char*r = reg;
	*r = '^';
	r++;
	while(*a){
		if(*a == '*' ){
			*r = '.';
			r++;
			*r = '*';
			r++;
		}else if(*a == '?'){
			*r = '.';
			r++;
		}else if (*a == '.'){
			*r = '\\';
			r++;
			*r = '.';
			r++;
		}else{
			*r = *a;
			r++;
		}	
		a++;
	}
	*r = '$';
	r++;
	*r = 0;
	regex_t re;
	int expbuf = regcomp(&re,reg,REG_EXTENDED|REG_NOSUB);
	if(expbuf != 0){
		perror("compile");
		return;
	}

	DIR*dir = opendir(".");
	if(dir == NULL){
		perror("opendir");
		return;
	}
	struct dirent*ent;
	int maxEntries = 20;
	int nEntries = 0;
	char **array  = (char**)malloc(maxEntries*sizeof(char*));

	while((ent = readdir(dir))!= NULL){
		regmatch_t match;
		if(regexec(&re,ent->d_name,1,&match,0) == 0 ) {
			if(nEntries == maxEntries){
				maxEntries *=2;
				array = (char**)realloc(array,maxEntries*sizeof(char*));
			}
			if(ent->d_name[0] == '.'){
				if(argument[0] == '.'){
					//if(strchr(ent->d_name,'*') == NULL && strchr(ent->d_name,'?') == NULL){
						
						array[nEntries] = strdup(ent->d_name);
						nEntries++;
				//	}
				}
			}else{
				//if(strchr(ent->d_name,'*') == NULL && strchr(ent->d_name,'?') == NULL){
						
						array[nEntries] = strdup(ent->d_name);
						nEntries++;
				//}
			}
		}
	}
	closedir(dir);	
	qsort(array,nEntries,sizeof(char*),cmp );
	for (int i = 0; i < nEntries; i++)
	{
		insertArgument(array[i]);
	}
	free(array);*/
	
	
	//int nEntries = 0;
	
	expand(NULL,strdup(argument));

	qsort(array,nEntries,sizeof(char*),cmp );
	//printf("array:  %d %d\n",nEntries,maxEntries);
	for (int i = 0; i < nEntries; i++)
	{
		insertArgument(array[i]);
		//printf("array: %s %d %d\n",array[i],nEntries,maxEntries);
	}
	free(array);
	nEntries = 0;
	maxEntries = 20;
	array = (char**)malloc(maxEntries*sizeof(char*));
	}	
}


Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

	if ( _outFile ) {
		free( _outFile );
	}

	if ( _inputFile ) {
		free( _inputFile );
	}

	if ( _errFile ) {
		free( _errFile );
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
}

void
Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
		}
		printf("\n");
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO");
	printf( "\n\n" );
//	printf("\n");
}

void
Command::execute()
{
	// Don't do anything if there are no simple commands
	if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}

	// Print contents of Command data structure
//	print();

	// Add execution here
	// For every simple command fork a new process
	// Setup i/o redirection
	// and call exec
	
	
	int tmpin = dup(0);
	int tmpout = dup(1);
	int tmperr = dup(2);
	int fdin;


	if(_inputFile){
		fdin = open(_inputFile,O_RDONLY,0660);
	}else{
		fdin = dup(tmpin);
	}



	int ret;
	int fdout;
	int fderr;
	for(int i = 0; i < _numberOfSimpleCommands; i++){
		dup2(fdin,0);
		close(fdin);
		if(i == _numberOfSimpleCommands-1){
			if(_outFile){
				if(append == 1){
					fdout = open(_outFile,O_WRONLY|O_APPEND,0660);
				}else{
					fdout = open(_outFile,O_CREAT|O_WRONLY|O_TRUNC,0660);
				}
			}else{
				fdout = dup(tmpout);
			}
			
			if (_errFile)
			{
				fderr = open(_errFile,O_WRONLY|O_APPEND,0660);
			}else{
				fderr = dup(tmperr);
			}
		}else{
			int fdpipe[2];
			pipe(fdpipe);
			fderr = fdpipe[2];
			fdout = fdpipe[1];
			fdin = fdpipe[0];
		}
		dup2(fderr,2);
		dup2(fdout,1);
		close(fdout);
		close(fderr);
		
		/*int k,f,c,l;
		char*t = (char*)malloc(100);
		for ( k = 0; k <_simpleCommands[i]->_numberOfArguments; k++)
		{
			for(c = 0; c < strlen(_simpleCommands[i]->_arguments[k])-1;c++){
				if(_simpleCommands[i]->_arguments[k][0] == '$'){
					char*b = (char*)malloc(100);
					int g = 2;
					int j = 0;
					while(_simpleCommands[i]->_arguments[k][g] != '}'){
						b[j] =  _simpleCommands[i]->_arguments[k][g];
						g++;
						j++;
					}
					b[j] = '\0';	
					//printf("%s\n",b );

					char*temp  = strdup(getenv(b));
					_simpleCommands[i]->_arguments[k] = strdup(temp);		
				}
				
			/*	if(_simpleCommands[i]->_arguments[k][c] == '/'){
						int gg = 0;
						for(l = 1 ; l < strlen(_simpleCommands[i]->_arguments[k])-1 ; l++ ){
						   
							t[gg] = _simpleCommands[i]->_arguments[k][l]; 					
							gg++;
						}	
						t[gg] = '\0';
					_simpleCommands[i]->insertArgument(t);	
				}
			}
		}*/

		
		if(strcmp(_simpleCommands[i]->_arguments[0],"cd") == 0){
			int bb;
			if(_simpleCommands[i]->_arguments[1] == NULL){
				bb = chdir(getenv("HOME"));
			}else{
				bb = chdir(_simpleCommands[i]->_arguments[1]);
			}

			if(bb != 0){
				perror("No such file or directory\n");
			}
		}

		else if(strcmp(_simpleCommands[i]->_arguments[0],"setenv") == 0){
				char*a = strdup(_simpleCommands[i]->_arguments[1]);
				char*b = strdup(_simpleCommands[i]->_arguments[2]);
				setenv(a,b,1);	
		}else if(strcmp(_simpleCommands[i]->_arguments[0],"unsetenv") == 0){
				unsetenv(_simpleCommands[i]->_arguments[1]);
		}
		//else if(strcmp(_simpleCommands[i]->_arguments[0],"printenv") ==0){
					/*int i = 0;
					while(environ[i] != '\0'){
					printf("%s\n",environ[i++]);
					}*/
					//execvp(_simpleCommands[i]->_arguments[0],_simpleCommands[i]->_arguments);
					//perror("execvp");
				
		else{	

		ret = fork();
		if(ret == 0){
		/*	if(_env){
				int i = 0;
				while(environ[i] != '\0'){
					printf("%s\n",environ[i++]);
				}
			}*/
			//if(strcmp(_simpleCommands[i]->_arguments[0],"printenv") !=0 && strcmp(_simpleCommands[i]->_arguments[0],"setenv") != 0 && strcmp(_simpleCommands[i]->_arguments[0],"unsetenv") != 0 ){
				
				
				execvp(_simpleCommands[i]->_arguments[0],_simpleCommands[i]->_arguments);
				perror("execvp");
				_exit(1);
				//}	
			//}
		}

		else if(ret < 0){
			perror("fork");
			return;
		}
		}
	}

	dup2(tmpin,0);
	dup2(tmpout,1);
	dup2(tmperr,2);
	close(tmpin);
	close(tmpout);
	close(tmperr);
	
	if(!_background){
		waitpid(ret,NULL,0);
	}


	// Clear to prepare for next command
	clear();
	
	// Print new prompt

	prompt();
	
}

// Shell implementation

void handel(int pro){
	while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {}
}

void disp(int pro){
	printf("ctrl c\n");
}


void
Command::prompt()
{	
	if(isatty(0)){
	printf("myshell>");
	fflush(stdout);
	}
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

int yyparse(void);

main()
{
	
	signal(SIGINT, disp);
	signal(SIGCHLD,handel);
	Command::_currentCommand.prompt();
	yyparse();
}

