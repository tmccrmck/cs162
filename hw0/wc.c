#include <stdio.h>
#include <ctype.h>

void wc(FILE *ofile, FILE *infile, char *inname) {
    int lines = 0; 
    int words = 0; 
    int bytes = 0;
    char c;
    int nul_byte_seen = 0;
    char lastline = ' ';
    while(c = fgetc(infile), c != EOF){
	bytes += 1;
	if(c == '\n'){
	    lines += 1;
	}
	if(isspace(c) && !isspace(lastline)){
	    words += 1;
	}
        else if(c == '\0' && !isspace(lastline)){
	    words -= 1;
	}

	lastline = c;
    }
    if(strcmp(inname, "input") != 0)	
        fprintf(ofile, " %d %d %d %s\n", lines, words, bytes, inname);
    else
	fprintf(ofile, " %d %d %d \n", lines, words, bytes);	
	
}

int main (int argc, char *argv[]) {
    FILE * input = fopen(argv[1], "r");

    if(argc == 3){
	FILE * output = fopen(argv[2], "w");
	wc(output, input, argv[1]);
	fclose(output);
	fclose(input);
    } else if(argc == 2){
	wc(stdout, input, argv[1]);
	fclose(input);
    } else if(argc == 1){
	wc(stdout, stdin, "input");
    }
    return 0;
}
