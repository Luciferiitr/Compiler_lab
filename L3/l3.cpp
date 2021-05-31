#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<bits/stdc++.h>

using namespace std;

void FIRST(char[], char);
void addToResultSet(char[], char);
char *removeSpaces(char *str);

int num_of_prod;
char productions[20][20];
char non_terminals[20];
unordered_map<char, int> nullable;


int main()
{
    int i;
    char choice;
    char c;
    size_t buf_size = 20;
    char result[20];
    printf("\nEnter number of production rules: ");
    scanf("%d", &num_of_prod);
    getchar();
    printf("\n");

    char parsed_prod[20];
    char *b = parsed_prod;
    char delim[] = "|";

    int total_prod = 0;

    while(total_prod < num_of_prod){
        getline(&b, &buf_size, stdin);
        removeSpaces(b);

        char *ptr = strtok(parsed_prod, delim);

        char curr_non_terminal = ptr[0];
        productions[total_prod][0] = curr_non_terminal;
        productions[total_prod][1] = '=';
        int pt = 2;
        while(ptr[pt+1] != '\0'){
            productions[total_prod][pt] = ptr[pt+1];
            pt++;
        }
        total_prod++;

        ptr = strtok(NULL, delim);
        while(ptr != NULL){
            productions[total_prod][0] = curr_non_terminal;
            productions[total_prod][1] = '=';
            pt = 0;
            while(ptr[pt] != '\0'){
                productions[total_prod][pt+2] = ptr[pt];
                pt++;
            }

            ptr = strtok(NULL, delim);
            total_prod++;
        }
    }

    int sz = 0;
    for(i=0; i<num_of_prod; i++){
        int present = 0;
        for(int j=0; j<sz; j++){
            if(non_terminals[j] == productions[i][0]){
                present = 1;
                break;    
            }
        }
        if(!present){
            non_terminals[sz] = productions[i][0];
            sz++;
        }
    }

    int done = 0;
    int last_size = 0;
    for(i=0; i<num_of_prod; i++){
        if(productions[i][2] == '@'){
            nullable[productions[i][0]] = 1;
            last_size++;
        }
    }
    
    while(!done){
        int current_size = last_size;
        for(i = 0; i<num_of_prod; i++){
            if(nullable[productions[i][0]] != 1){
                int ite = 2;
                bool all_null = true;
                while(productions[i][ite] != '\0' && all_null){
                    if(!nullable[productions[i][ite]]) all_null = false;
                    ite++;
                }
                if(all_null){
                    nullable[productions[i][0]] = 1;
                    current_size++;    
                }
            }
        }
        if(current_size == last_size) done = true;
        else last_size = current_size;
    }

    printf("\n");
    for(i=0; i<sz; i++){
        FIRST(result, non_terminals[i]);
        if(nullable[non_terminals[i]]){
            int set_sz = 0;
            while(result[set_sz] != '\0') set_sz++;
            result[set_sz] = '@';
        }
        printf("FIRST(%c) = [", non_terminals[i]);
        for(int j=0; result[j] != '\0'; j++){
            printf("%c", result[j]);
            if(result[j+1] != '\0') printf(", ");
        }
        printf("]\n");
    }
    printf("\n");

}



/*
 *Function FIRST:
 *Compute the elements in FIRST(c) and write them
 *in Result Array.
 */
void FIRST(char* Result, char c)
{
    int i, j, k;
    char subResult[20];
    int foundEpsilon;
    subResult[0] = '\0';
    Result[0] = '\0';

    //If X is terminal, FIRST(X) = {X}.
    if(!(isupper(c))){
        addToResultSet(Result, c);
            return ;
    }

    //If X is non terminal
    //Read each production
    for(i=0; i<num_of_prod; i++)
    {
        //Find production with X as LHS
        if(productions[i][0] == c)
        {
            //If X → ε is a production, then don't bother.
            if(productions[i][2]=='@') continue;
            //If X is a non-terminal, and X → Y1 Y2 … Yk
            //is a production, then add a to FIRST(X)
            //if for some i, a is in FIRST(Yi),
            //and ε is in all of FIRST(Y1), …, FIRST(Yi-1).
            else{
                j=2;
                while(productions[i][j] != '\0'){
                    foundEpsilon = 0;
                    FIRST(subResult, productions[i][j]);
                    for(k=0; subResult[k]!='\0'; k++)
                        addToResultSet(Result,subResult[k]);
                    if(nullable[productions[i][j]]) foundEpsilon = 1;
                    //No ε found, no need to check next element
                    if(!foundEpsilon)
                        break;
                    j++;
                }
            }
        }
    }
    return ;
}

/* addToResultSet adds the computed
 *element to result set.
 *This code avoids multiple inclusion of elements
  */
void addToResultSet(char Result[],char val)
{
    int k;
    for(k=0; Result[k]!='\0'; k++)
        if(Result[k]==val)
            return;
    Result[k]=val;
    Result[k+1]='\0';
}

// remove all possible whitespace in the production
char *removeSpaces(char *str){
	int i = 0, j = 0;
	while(str[i]){
		if (str[i] != ' ') str[j++] = str[i];
		i++;
	}
	str[j] = '\0';
	return str;
}
