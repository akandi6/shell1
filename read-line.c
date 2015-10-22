/*
 * CS354: Operating Systems. 
 * Purdue University
 * Example that shows how to read one line with simple editing
 * using raw terminal.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#define MAX_BUFFER_LINE 2048

// Buffer where line is stored
int line_length;;
char line_buffer[MAX_BUFFER_LINE];

// Simple history array
// This history does not change. 
// Yours have to be updated.

int ccc = 0;


int history_index = 0;
char * history [100];
int history_length = sizeof(history)/sizeof(char *);

void read_line_print_usage()
{
  char * usage  =  "\n"
    " ctrl-?       Print usage\n"
    " Backspace    Deletes last character\n"
    " up arrow     See last command in the history\n";
  write(1, usage, strlen(usage));
}

/* 
 * Input a line with some basic editing.
 */
char * read_line() {
struct termios orig_attr;
tcgetattr(0,&orig_attr);
  // Set terminal in raw mode
  tty_raw_mode();

  line_length = 0;
  int counter = 0;
  int dd = 0;
  // Read one line until enter is typed
  while (1) {

    // Read one character in raw mode.
    char ch;
    read(0, &ch, 1);
    

    if (ch>=32 && ch != 127 ) {
      // It is a printable character. 
      // Do echo
      if(line_length != counter ){
      char temp;
      int i;
     
      for ( i = line_length-1; i >= counter; i--)
      {     
            temp = line_buffer[i];
            line_buffer[i] = line_buffer[i-1];
            line_buffer[i+1] = temp;
      }
      
      line_buffer[counter] = ch;
       

      for ( i = counter; i > 0; i--)
      {
          ch = 8;
          write(1,&ch,1); 
      }

      for ( i = 0; i < line_length; i++)
      {
        ch = ' ';
        write(1,&ch,1);
      }

      for ( i = line_length; i > 0; i--)
      {
          ch = 8;
          write(1,&ch,1); 
      }

      for ( i = 0; i < line_length+1; i++)
      {
          ch = line_buffer[i];
          write(1,&ch,1); 
      }

      line_length++;

      for ( i = line_length-counter-1; i > 0 ; i--)
      {
          ch = 8;
          write(1,&ch,1); 
      }
      

    //  write(1,&ch,1);
      counter++;

      // If max number of character reached return.
      if (line_length==MAX_BUFFER_LINE-2) break; 
      // add char to buffer.
     // line_buffer[line_length]=ch;
      //line_length++;
    }else {


      write(1,&ch,1);
      line_buffer[line_length] = ch;
      line_length++;
      counter++;
    }
    }
    else if (ch == 10) {
      line_buffer[line_length] = '\0';
      history[ccc] = strdup(line_buffer);
      ccc++;
      //history_index++;
  //    printf("\ndddd1 %d\n" ,history_index);
      // <Enter> was typed. Return line
     // ch = '\n';
      // Print newline
      history_index = ccc ;
      write(1,&ch,1);

      break;
    }
    else if (ch == 31) {
      // ctrl-?
      read_line_print_usage();
      line_buffer[0]=0;
      break;
    }else if(ch == 4 ){
      //if(counter == line_length){
       
        if(line_length!= 0 && counter < line_length ){
        int i;
        char temp; 
        
        for ( i = counter; i < line_length; i++)
        {
            temp = line_buffer[i+1];
            line_buffer[i+1] = line_buffer[i+2];
            line_buffer[i] = temp; 
        }

        for ( i = counter; i > 0; i--)
        {
          ch = 8;
          write(1,&ch,1); 
        }
        
        for ( i = 0; i < line_length; i++)
        {
          ch = ' ';
          write(1,&ch,1);
        }

        for ( i = line_length; i > 0; i--)
        {
          ch = 8;
          write(1,&ch,1); 
        }

        for ( i = 0; i < line_length-1; i++)
        {
          ch = line_buffer[i];
          write(1,&ch,1); 
        }

        for ( i = line_length-counter-1; i > 0 ; i--)
        {
          ch = 8;
          write(1,&ch,1); 
        }

        line_length--;    
      }
    }
    else if (ch == 8 || ch == 127) {
      // <backspace> was typed. Remove previous character read.

      // Go back one character
     /* ch = 8;
      write(1,&ch,1);
      counter--;
      // Write a space to erase the last character read
      ch = ' ';
      write(1,&ch,1);

      // Go back one character
      ch = 8;
      write(1,&ch,1);
      counter--;
      // Remove one character from buffer
      line_length--;*/
       if(line_length!= 0 && counter > 0 ){
        int i;
        char temp; 
        
        for ( i = counter; i < line_length; i++)
        {
            temp = line_buffer[i];
            line_buffer[i] = line_buffer[i+1];
            line_buffer[i-1] = temp; 
        }

        for ( i = counter; i > 0; i--)
        {
          ch = 8;
          write(1,&ch,1); 
        }
        
        for ( i = 0; i < line_length; i++)
        {
          ch = ' ';
          write(1,&ch,1);
        }

        for ( i = line_length; i > 0; i--)
        {
          ch = 8;
          write(1,&ch,1); 
        }

        for ( i = 0; i < line_length-1; i++)
        {
          ch = line_buffer[i];
          write(1,&ch,1); 
        }

        for ( i = line_length-counter-1; i >= 0; i--)
        {
          ch = 8;
          write(1,&ch,1); 
        }
        counter--;
        line_length--;    
      }
    }
    else if(ch == 1){
      int i;
        for ( i = counter ; i > 0; i--)
        {
          ch = 8;
          write(1,&ch,1); 
          counter--;
        }
    }else if(ch == 5){
      int i;
        for ( i = counter; i < line_length; i++)
        {
          ch = line_buffer[i];   
          write(1,&ch,1);
          counter++;
        }
    }
    else if (ch==27) {
      // Escape sequence. Read two chars more
      //
      // HINT: Use the program "keyboard-example" to
      // see the ascii code for the different chars typed.
      //
      char ch1; 
      char ch2;
      //char ch3;
      read(0, &ch1, 1);
      read(0, &ch2, 1);
     
  if (ch1 == 91 && ch2 == 65 && history_index > 0 ) {
	// Up arrow. Print next line in history.
  history_index--;
	// Erase old line
	// Print backspaces
 // printf("sss%d\n",history_index );
  line_length = strlen(line_buffer);
 // history_index--;
	int i = 0;
	for (i = 0; i < counter; i++) {
	  ch = 8;
	  write(1,&ch,1);
   // counter--;
	}

	// Print spaces on top
	for (i =0; i < line_length; i++) {
	  ch = ' ';
	  write(1,&ch,1);
	}

	// Print backspaces
	for (i = 0; i < line_length; i++) {
	  ch = 8;
	  write(1,&ch,1);
    //counter--;
	}	

  int j = 0;
	// Copy line from history
//  printf(" line %s his%s \n",line_buffer,history[history_index] );
	strcpy(line_buffer, history[history_index]);
	line_length = strlen(history[history_index]);
  //history_index=(history_index+1)%history_length;
	//echo line
  counter = 0;
  while(j < line_length){
	  write(1, &line_buffer[j],1);
    j++;
  }
  counter  = line_length;
  }else if(ch1 == 91 && ch2 == 66 && history_index+1 < ccc){
    history_index++;
  // Erase old line
  // Print backspaces
 // printf("sss%d\n",history_index );
  line_length = strlen(line_buffer);
 // history_index--;
  int i = 0;
  for (i = 0; i < counter; i++) {
    ch = 8;
    write(1,&ch,1);
   // counter--;
  }

  // Print spaces on top
  for (i =0; i < line_length; i++) {
    ch = ' ';
    write(1,&ch,1);
  }

  // Print backspaces
  for (i = 0; i < line_length; i++) {
    ch = 8;
    write(1,&ch,1);
    //counter--;
  } 

  int j = 0;
  // Copy line from history
//  printf(" line %s his%s \n",line_buffer,history[history_index] );
  strcpy(line_buffer, history[history_index]);
  line_length = strlen(history[history_index]);
  //history_index=(history_index+1)%history_length;
  //echo line
  counter = 0;
  while(j < line_length){
    write(1, &line_buffer[j],1);
    j++;
  }
  counter  = line_length;
  }else if(ch1 == 91 && ch2 == 66 && history_index+1 == ccc){
    int i = 0;
    for (i = 0; i < counter; i++) {
      ch = 8;
      write(1,&ch,1);
   // counter--;
    }
   // printf("aaa\n" );
    for (i =0; i < line_length; i++) {
      ch = ' ';
      write(1,&ch,1);
    }

    for (i = 0; i < line_length; i++) {
      ch = 8;
      write(1,&ch,1);
     //counter--;
    } 
    counter = 0;
  
  }
      else if(ch1 == 91 && ch2 == 68 && counter != 0){
        ch = 8;
       write(1,&ch,1);
       counter--;
      }else if(ch1 == 91 && ch2 == 67 && counter != line_length){
       ch = line_buffer[counter];
       write(1,&ch,1);
       counter++;
      }else if(ch1 == 79 && ch2 == 72 ){
        int i;
        for ( i = counter ; i > 0; i--)
        {
          ch = 8;
          write(1,&ch,1); 
          counter--;
        }
      }else if(ch1 == 79 && ch2 == 70 ){
        int i;
        for ( i = counter; i < line_length; i++)
        {
          ch = line_buffer[i];   
          write(1,&ch,1);
          counter++;
        }
      }
      else if(ch1 == 91 && ch2 == 51){
         char ch3;
         read(0,&ch3,1);

        if(line_length!= 0 && counter < line_length  && ch3 == 126){
        int i;
        char temp; 
        
        for ( i = counter; i < line_length; i++)
        {
            temp = line_buffer[i+1];
            line_buffer[i+1] = line_buffer[i+2];
            line_buffer[i] = temp; 
        }

        for ( i = counter; i > 0; i--)
        {
          ch = 8;
          write(1,&ch,1); 
        }
        
        for ( i = 0; i < line_length; i++)
        {
          ch = ' ';
          write(1,&ch,1);
        }

        for ( i = line_length; i > 0; i--)
        {
          ch = 8;
          write(1,&ch,1); 
        }

        for ( i = 0; i < line_length-1; i++)
        {
          ch = line_buffer[i];
          write(1,&ch,1); 
        }

        for ( i = line_length-counter-1; i > 0 ; i--)
        {
          ch = 8;
          write(1,&ch,1); 
        }

        line_length--;    
      }
      }
    }
    
  }

  // Add eol and null char at the end of string
  line_buffer[line_length]=10;
  line_length++;
  line_buffer[line_length]=0;
  tcsetattr(0,TCSANOW,&orig_attr);

  return line_buffer;
}

