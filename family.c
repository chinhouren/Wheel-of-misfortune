#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "family.h"

/* Number of word pointers allocated for a new family.
   This is also the number of word pointers added to a family
   using realloc, when the family is full.
*/
static int family_increment = 0;


/* Set family_increment to size, and initialize random number generator.
   The random number generator is used to select a random word from a family.
   This function should be called exactly once, on startup.
*/
void init_family(int size) {
    family_increment = size;
    srand(time(0));
}


/* Given a pointer to the head of a linked list of Family nodes,
   print each family's signature and words.

   Do not modify this function. It will be used for marking.
*/
void print_families(Family* fam_list) {
    int i;
    Family *fam = fam_list;

    while (fam) {
        printf("***Family signature: %s Num words: %d\n",
               fam->signature, fam->num_words);
        for(i = 0; i < fam->num_words; i++) {
            printf("     %s\n", fam->word_ptrs[i]);
        }
        printf("\n");
        fam = fam->next;
    }
}


/* Return a pointer to a new family whose signature is
   a copy of str. Initialize word_ptrs to point to
   family_increment+1 pointers, numwords to 0,
   maxwords to family_increment, and next to NULL.
*/
Family *new_family(char *str) {
    Family *fam = malloc(sizeof(Family));
    if (fam == NULL){
      perror("malloc");
      exit(1);
    }
    fam -> signature = malloc(sizeof(char) * (strlen(str) + 1));
    if ((fam -> signature) == NULL){
      perror("malloc");
      exit(1);
    }
    strcpy(fam -> signature, str);
    fam -> word_ptrs = malloc(sizeof(char *) * (family_increment + 1));
    if ((fam -> word_ptrs) == NULL){
      perror("malloc");
      exit(1);
    }
    fam -> num_words = 0;
    fam -> max_words = family_increment;
    fam -> next = NULL;
    return fam;
}


/* Add word to the next free slot fam->word_ptrs.
   If fam->word_ptrs is full, first use realloc to allocate family_increment
   more pointers and then add the new pointer.
*/
void add_word_to_family(Family *fam, char *word) {
    if (fam -> num_words == fam -> max_words){
        (fam -> max_words) = (fam -> max_words) + family_increment;
        fam -> word_ptrs = realloc(fam -> word_ptrs, (sizeof(char *) * (fam -> max_words) + 1));
    }
    (fam -> word_ptrs)[fam -> num_words] = word;
    (fam -> num_words)++;
    return;
}


/* Return a pointer to the family whose signature is sig;
   if there is no such family, return NULL.
   fam_list is a pointer to the head of a list of Family nodes.
*/
Family *find_family(Family *fam_list, char *sig) {
    Family *fam = fam_list;
    while (fam != NULL && strcmp(fam -> signature, sig) != 0){
        fam = fam -> next;
    }
    return fam;
}


/* Return a pointer to the family in the list with the most words;
   if the list is empty, return NULL. If multiple families have the most words,
   return a pointer to any of them.
   fam_list is a pointer to the head of a list of Family nodes.
*/
Family *find_biggest_family(Family *fam_list) {
    Family *largest = fam_list;
    Family *fam = fam_list;
    while (fam != NULL){
        if (fam -> num_words > largest -> num_words){
            largest = fam;
        }
        fam = fam -> next;
    }
    return largest;
}


/* Deallocate all memory rooted in the List pointed to by fam_list. */
void deallocate_families(Family *fam_list) {
    Family *current = fam_list;
    Family *prev;
    while (current != NULL){
        free(current -> word_ptrs);
        free(current -> signature);
        prev = current;
        current = current -> next;
        free(prev);
    }
    return;
}


/* Generate and return a linked list of all families using words pointed to
   by word_list, using letter to partition the words.

   Implementation tips: To decide the family in which each word belongs, you
   will need to generate the signature of each word. Create only the families
   that have at least one word from the current word_list.
*/
Family *generate_families(char **word_list, char letter) {
    Family *famhead = NULL;
    Family *current = NULL;
    Family *last = NULL;
    int counter = 0;
    int found = 0;
    if( word_list == NULL){
      perror("Empty word_list");
      exit(1);
    }
    int len = strlen(word_list[0]);
    char sig[len + 1];
    while (word_list[counter] != NULL){
        strcpy(sig, word_list[counter]); /*Generates signature for the word*/
        for (int i = 0; i < len; i++){
          if (sig[i] != letter){
            sig[i] = '-';
          }
        }
        /*Assigns head of family if none exists, also assigns last member*/
        if (famhead == NULL){
          famhead = new_family(sig);
          last = famhead;
          add_word_to_family(famhead, word_list[counter]);
        }
        /*Searches for same signature to add to that family*/
        else{
          current = famhead;
          found = 0;
          while (current != NULL){
            if (strcmp(sig, get_family_signature(current)) == 0){
                add_word_to_family(current, word_list[counter]);
                found++;
            }
            current = current -> next;
          }
          /*If !found => new signature, creates new and adds it to the last*/
          if (!found){
              last -> next = new_family(sig);
              last = last -> next;
              add_word_to_family(last, word_list[counter]);

          }
        }
        counter++;
    }
    return famhead;
}


/* Return the signature of the family pointed to by fam. */
char *get_family_signature(Family *fam) {
    return (fam -> signature);
}


/* Return a pointer to word pointers, each of which
   points to a word in fam. These pointers should not be the same
   as those used by fam->word_ptrs (i.e. they should be independently malloc'd),
   because fam->word_ptrs can move during a realloc.
   As with fam->word_ptrs, the final pointer should be NULL.
*/
char **get_new_word_list(Family *fam) {
    char **new_word_ptrs = malloc(sizeof(char *) * (fam -> num_words + 1));
    if (new_word_ptrs == NULL){
      perror("malloc");
      exit(1);
    }
    for (int i = 0; i < (fam -> num_words); i++){
      new_word_ptrs[i] = (fam -> word_ptrs)[i];
    }
    new_word_ptrs[fam -> num_words] = NULL;
    return new_word_ptrs;
}


/* Return a pointer to a random word from fam.
   Use rand (man 3 rand) to generate random integers.
*/
char *get_random_word_from_family(Family *fam) {
    int i = rand() % (fam -> num_words);
    return (fam -> word_ptrs)[i];
}
