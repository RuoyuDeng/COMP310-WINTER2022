#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Book
{
    /* data */
    char *title;
    char *author;
    char *subject;
    int book_id;
};

struct Book *createBook(int pNum, char *pTitle, char *pAuthor, char *pSubject){
    // allocate enough space for the whole struct Book
    struct Book *b = malloc(sizeof(struct Book));
    // allocate enough space for the fileds of struct Book (since each is a string, we add 1 addition space to store \0 at the end)
    b->title = malloc(strlen(pTitle)+1);
    b->author = malloc(strlen(pAuthor)+1);
    b->subject = malloc(strlen(pSubject)+1);

    // after having enough space, we can assign values to these fields
    strcpy(b->title,pTitle);
    strcpy(b->author,pAuthor);
    strcpy(b->subject,pSubject);
    b->book_id = pNum;
    return b;
}

void printBook(struct Book* b){
    printf("Book id is: %d\n",b->book_id);
    printf("Book author is: %s\n",b->author);
    printf("Book title is: %s\n",b->title);
    printf("Book subject is: %s\n",b->subject);
}
int main(int argc, char* argv[]){

    struct Book *b1 = createBook(10086,"Ocean Cat","Dick Face","Horror");
    printBook(b1);

}
