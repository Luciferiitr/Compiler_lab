%{
	#include <stdlib.h>
	#include <stdio.h>
%}

%%
"."|"["|"]"|"("|")"|"{"|"}"|","|";"|"*"|"#"|"=" {fprintf(yyout, "special character ");}
auto|double|struct|break|long|switch|case|enum|register|typedef|extern|union|const|short|unsigned|continue|signed|do|static|while|if|else|int|char|for|return|float|void {fprintf(yyout, "Keyword ");}
[a-zA-Z_][a-zA-Z0-9_]* {if(yyleng <= 31){fprintf(yyout, "identifier ");}}
"<="|"=="|"="|"++"|"-"|"*"|"+"|"!"|"/"|"|"|"<"|">"|">="|"--"|"!="|"+="|"−="|"*="|"/=" {fprintf(yyout, "operator ");}
([0-9]*"."[0-9]+)|([0-9]+)|(\"(.)*\")|(\'.\') {fprintf(yyout, "constant ");}
%%

int yywrap(void){ return 1; }

int main(){

	extern FILE *yyin, *yyout;
	yyin = fopen("Input.txt", "r");
	yyout = fopen("Output.txt", "w");

	yylex();

	return 0;
}