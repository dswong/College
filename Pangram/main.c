
/*
   pangram.c
   Determines if an input sentence is a pangram
   (a.k.a. "holoalphabetic sentence") or not.
   Derek Wong, 5/22/12
*/

#include <stdio.h>
#include <ctype.h>

/* prototypes for functions you must implement below main */
void getSentence(char *sentence);
int isPangram(char *sentence, int missing[26]);

void showNegativeResults(int[]); /* already implemented below main */

int main(void) {
    char sentence[1024] = {'\0'};
    int missing[26] = {0};

    printf("Enter sentence\n(ending with a period like this one).\n\n");
    getSentence(sentence);
    
    printf("\nSentence: \"%s.\"", sentence);    
    
    if ( isPangram(sentence, missing) )
        printf("\n\nThe sentence IS a pangram!\n\n");
    else
        showNegativeResults(missing);
        
    return 0;
}

void getSentence(char *sentence) {
    int i=0;
    while ((sentence[i]=getchar())!= '.') /*while not a period, keep reading*/
    {
        i++;
    }
    sentence[i]='\0';  /*add a null character to the end*/  
}

int isPangram(char *sentence, int missing[26]) {
    int i=0,j,k,pan=1;
    for (j=0;j<26;j++) /*unless letter is found, they are all missing*/
    {
        missing[j]=1;
    }
    do
    {
        sentence[i]=tolower(sentence[i]); /*lowercase letters to simplify*/
        if (sentence[i]>='a'&sentence[i]<='z') /*checks to see its a letter*/
        {
            missing[sentence[i]-'a']=0; /*changes to 0 in missing array*/
        }
        i++;
    } while(sentence[i]!='\0'); /*keep checking the string until it ends*/
    for (k=0;k<26;k++) /*check to see if there are letters missing*/
    {
        if (missing[k]==1)
        {
            pan=0; /*if a letter is missing, break and go to return*/
            break;
        }
    }  
    return pan; /*default true, unless check loop sets to false*/
}

void showNegativeResults(int missing[26]) {
    int c;
    printf("\n\nThe sentence is NOT a pangram.\n");
    printf("Missing letters:");
    for(c = 0; c < 26; c++)
        if (missing[c])
            printf(" %c", ('a' + c));
    printf("\n\n");
}