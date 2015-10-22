
/*
 * CS-252
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%token	<string_val> WORD

%token 	NOTOKEN GREAT LESS NEWLINE GREATAMPERSAND AMPERSAND GREATGREAT GREATGREATAMPERSAND PIPE 
%union	{
		char   *string_val;
	}

%{
//#define yylex yylex
#include <stdio.h>
#include "command.h"
#include <string.h>
void yyerror(const char * s);
int yylex();

%}

%%

goal:	
	commands
	;

commands: 
	command
	| commands command 
	;

command: simple_command
        ;

simple_command:	
	command_and_args pipe_list iomodifier_opt amp NEWLINE {
		//printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();
	}
	| NEWLINE 
	| error NEWLINE { yyerrok; }
	;

command_and_args:
	command_word arg_list {
		Command::_currentCommand.
			insertSimpleCommand( Command::_currentSimpleCommand );
	}
	;

arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
	WORD {
              // printf("   Yacc: insert argument \"%s\"\n", $1);

	       Command::_currentSimpleCommand->wildcard( $1 );
	}
	;

command_word:
	WORD {
              // printf("   Yacc: insert command \"%s\"\n", $1);
	       
	       Command::_currentSimpleCommand = new SimpleCommand();
	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;

iomodifier_opt:
	GREAT WORD iomodifier_opt {
		if(Command::_currentCommand._outFile){
			printf("Ambiguous output redirect\n");
			exit(0);
		}else{
	//	printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		}
	}
	|  
	LESS WORD iomodifier_opt {
		if(Command::_currentCommand._inputFile){
		   printf("Ambiguous output redirect\n");
		   exit(0);
		   }else{
	//	printf("   Yacc: getting input \"%s\"\n", $2);
		Command::_currentCommand._inputFile = $2;
		}
	}
	|
	GREATAMPERSAND WORD iomodifier_opt {
	/*	if(Command::_currentCommand._outFile){
		   printf("Ambiguous output redirect\n");
		   exit(0);
		   }else{*/
		 
		// printf("   Yacc: insert both Stout and sterr output \"%s\"\n", $2);
		 Command::_currentCommand._outFile = $2;
		 char*k = strdup($2);
		 Command::_currentCommand._errFile = k;
		//}
	}
	|
	GREATGREAT WORD iomodifier_opt {
	
		//printf("   Yacc: appends output  \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand.append  =  1;

	

	}
	|
	GREATGREATAMPERSAND WORD iomodifier_opt {
		/*if(Command::_currentCommand._outFile){
		   printf("Ambiguous output redirect\n");
		   exit(0);
		   }else{*/
	
	//	printf("   Yacc: appends both Stout and sterr output \"%s\"\n", $2);
	        Command::_currentCommand._outFile = $2;
		char*g = strdup($2);
                Command::_currentCommand._errFile = g;
				Command::_currentCommand.append = 1;

	}
	|
	;

amp:
	AMPERSAND {
		// printf("   Yacc: background exists \n");
		 Command::_currentCommand._background = 1;
	}
	|
	;


/*env:
	ENV {
		Command::_currentCommand._env = 1;
	}
	|
	;
*/
pipe_list:
	pipe_list PIPE command_and_args {
//		printf("   Pipe line performing\n");
	}
        |
	command_and_args
	|
	;

%%




void
yyerror(const char * s)
{
	fprintf(stderr,"%s", s);
}

#if 0
main()
{
	yyparse();
}
#endif
