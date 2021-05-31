%{
	int keywords = 0 ;
	int identifier = 0 ;
	int operator = 0 ;
	int separator = 0 ;
	int constants = 0 ;
	int identation = 0;
%}

%%
([ ])|\t|\r|\n|\a {identation++;}
while|if|else|int|char|for|final|return|float|void|printf {keywords++;fprintf(yyout, "<KY, %s>\n", yytext);}
[a-zA-Z_][a-zA-Z0-9_]* {identifier++;fprintf(yyout, "<ID, %s>\n", yytext);}
"<="|"=="|"="|"++"|"-"|"*"|"+"|"!"|"/"|"|"|"<"|">"|">="|"--"|"!="|"+="|"−="|"*="|"/=" {operator++;fprintf(yyout, "<OP, %s>\n", yytext);}
[(){}|,;] {separator++;fprintf(yyout, "<SP, %s>\n", yytext);}
([0-9]*"."[0-9]+)|([0-9]+)|(\"(.)*\")|(\'.\') {constants++;fprintf(yyout, "<CT, %s>\n", yytext);}
%%

int yywrap(void){ return 1; }

int main(){

	extern FILE *yyin, *yyout;
	yyin = fopen("Input.txt", "r");
	yyout = fopen("Output.txt", "w");

	yylex();

	fprintf(yyout, "Total number of tokens in the above program are %d\n", keywords+identifier+operator+constants+separator);

	return 0;
}
