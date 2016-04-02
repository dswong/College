
#include <stdio.h>
#include <math.h>
void flush(void) 
{
    while (getchar() != '\n')
        ;
}
int main(void) 
{
    char onemore;
    void flush(void);
    do
    {
        int spaces=0,stars=1,inputcount=0,size=0; 
        int i,j,k,l,m,n; /*assorted variables for for-loops inside main*/
        
        do 
        {
            printf("enter ODD size > 0: ");
            inputcount = scanf("%d",&size);
            if (inputcount==0) /*check to see if words were input*/
            {
                printf("must enter integer\n");
                flush(); /* flush to avoid errors if number input after*/
            }
            else
            if  (size<=0||size%2==0)
            {
                printf("%d not odd and > 0.\n",size);
            }   

        } while (size<=0||inputcount==0||size%2==0);
        
        spaces = size/2;/*decide the starting number of spaces in first line*/
        for (i=0;i<(size/2)+1;i++) /*prints only the top half of diamond*/
        {
            for(j=0;j<spaces;j++)
            {
                putchar(' ');
            }
            spaces--; /*subtract one space for the line after*/
            for(k=0;k<stars;k++)
            {
                putchar('*');
            }
            stars = stars+2; /*adds 2 stars to keep odd numbers*/
            printf("\n");     
        }
        
        spaces = spaces+2; /*add spaces to reset count*/
        stars = stars-4; /*due to extra adding, 4 must be subtracted*/
        
        for (l=0;l<(size/2);l++)/*bottom half,spaces increase,stars decrease*/
        {
            for(m=0;m<spaces;m++)
            {
                putchar(' ');
            }
            spaces++;
            for(n=0;n<stars;n++)
            {
                putchar('*');
            }
            stars = stars-2;
            printf("\n");
        }
        printf("more? "); 
        flush(); /*getchar might not work*/
        onemore = getchar();
        flush(); /*flush to prevent errors if a valid word is input*/
    } while(onemore=='y'||onemore=='Y');
    return 0;
}

