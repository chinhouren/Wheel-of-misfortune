#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "family.h"
#include "reading.h"

#define BUF_SIZE	256

/* Starting with words (returned by read_words), generate and return
   a new word list with only those words of length len. Also, fill
   words_remaining with the number of words in the new word list.

   Allocate exactly enough memory to store only those words of length len.

   Note: Do not make copies of the words.
*/
char **prune_word_list(char **words, int len, int *words_remaining) {
    int counter = 0;
    int word_count = 0;
    while (words[counter] != NULL){
      if (strlen(words[counter]) == len){
          word_count++;
      }
      counter++;
    }
    *words_remaining = word_count;
    char **pruned = malloc(sizeof(char *) * (word_count + 1) );
    if (pruned == NULL){
      perror("malloc");
      exit(1);
    }
    counter = 0;
    word_count = 0;
    while (words[counter] != NULL){
      if (strlen(words[counter]) == len){
          pruned[word_count] = words[counter];
          word_count++;
      }
      counter++;
    }
    pruned[word_count] = NULL;
    return pruned;
}


/* Free memory acquired by prune_word_list.
*/
void deallocate_pruned_word_list(char **word_list) {
    free(word_list);
}


/* Return the word_list of all length-L words, and store that length in len.
   - ask user for the length of words to use
   - use prune_word_list to get a list of words of the appropriate length
   - if there are no words of this length in word list, then ask user to
    provide a different word length until there is at least one word of that
    length in words.

   If the user enters a non-numeric string, then ask the user to provide
   a different word length as above.  (Tip: use fgets and strtol instead of
   scanf)
*/
/* The only printf statements you may use in this function are:
    printf("Length of words to use? ");
    printf("There are no words of that length.\n");
*/
char **get_word_list_of_length(char **words, int *len) {
    char *endptr;
    char num[BUF_SIZE + 1];
    int words_remaining = 0;
    char **word_list;
    printf("Length of words to use? ");
    while (words_remaining == 0 && fgets(num, BUF_SIZE, stdin) != NULL){
        if(num[strlen(num) - 1] == '\n') {
            num[strlen(num) - 1] = '\0'; /*Delete newline*/
        }
        *len = strtol(num, &endptr, 10);
        while ((*endptr != '\0' || endptr == num)){
            printf("There are no words of that length.\n");
            printf("Length of words to use? ");
            fgets(num, BUF_SIZE, stdin);
            if(num[strlen(num) - 1] == '\n') {
                num[strlen(num) - 1] = '\0'; /*Delete newline*/
            }
            *len = strtol(num, &endptr, 10);
        }
        word_list = prune_word_list(words, *len, &words_remaining);
        if (words_remaining == 0){
            printf("There are no words of that length.\n");
            printf("Length of words to use? ");
            free(word_list);
        }
    }
    return word_list;
}


/* Continue to ask the user for their next guess, until it
   does not appear in letters_guessed. Add the new guess to letters_guessed,
   and return the guess.
*/
char get_next_guess(char *letters_guessed) {
    char guess;
    int i;

    do {
        printf("Next guess? ");
        fflush(stdout);

        if (scanf(" %c", &guess) != 1) {
            perror("scanf");
            exit(1);
        }

        if (guess >= 'a' && guess <= 'z') {
            i = guess - 'a';
            if (letters_guessed[i])
                printf("You already chose that letter.\n");
            else {
                letters_guessed[i] = guess;
                break;
            }
        } else
            printf("Please insert a lowercase letter.\n");

    } while (1);

    return guess;
}


/*Play one game of Wheel of Misfortune */
void play_round(char **words) {
    Family *famlist = NULL, *biggest_fam;
    char input_buffer[BUF_SIZE];
    char **word_list = NULL;
    int len, i, found;
    int guesses = 0;
    int game_over = 0; /*1 = game is over*/
    char guess;
    char *current_word; /*Representation of current word; each blank is a - */
    char *sig; /*Signature of a family*/
    char letters_guessed[26] = {'\0'}; /*Guesses so far*/

    /*Get a valid word_list from length (one that has at least one word)*/
    word_list = get_word_list_of_length(words, &len);

    while (guesses < 1 || guesses > 26) {
        printf("How many guesses would you like?\n");
        printf("Choose a number between 1 and 26: ");
        fflush(stdout);

        if (fgets(input_buffer, BUF_SIZE, stdin) == NULL) {
            perror("fgets");
            exit(1);
        }

        guesses = strtol(input_buffer, NULL, 10);
        if (guesses < 1 || guesses > 26) {
            printf("You entered an invalid number!\n\n");
	}
    }

    /*Word starts off as all unknowns*/
    current_word = malloc(len + 1);
    if (current_word == NULL) {
        perror("malloc");
        exit(1);
    }
    memset(current_word, '-', len);
    current_word[len] = '\0';

    while (!game_over) {
        printf("Guesses remaining: %d\n", guesses);
        printf("Word: %s\n", current_word);
        guess = get_next_guess(letters_guessed);
        deallocate_families(famlist);
        famlist = generate_families(word_list, guess);
        biggest_fam = find_biggest_family(famlist);

        sig = get_family_signature(biggest_fam);

        /*Search signature for letters in current_word*/
        found = 0;
        i = 0;
        while (*sig) {
            if (*sig == guess) {
                found = 1;
                current_word[i] = guess;
            }
            sig++;
            i++;
        }
        if (found) {
            printf("Good guess!\n");
            if (!strchr(current_word, '-')) {
                printf("You win! The word was %s.\n", current_word);
                game_over = 1;
            }
        }
        else {
            printf("There is no %c in the word.\n", guess);
            guesses--;
            game_over = guesses <= 0;
        }
        deallocate_pruned_word_list(word_list);
        word_list = get_new_word_list(biggest_fam);
    }

    if (guesses == 0) {
        printf("You lose! The word was %s.\n",
                get_random_word_from_family(biggest_fam));
    }

    deallocate_pruned_word_list(word_list);
    free(current_word);
    deallocate_families(famlist);
}


/* Read words, initialize families, and play as long as
   the user answers 'y'. */
int main(void) {
    char again;
    char **words;

    words = read_words("dictionary.txt");
    init_family(1024);

    do {
        play_round(words);
        printf("Play another round (y/n)? ");
        if (scanf(" %c", &again) != 1) {
            perror("scanf");
            break;
        }
        // In case of valid input, "consume" the newline character.
        getchar();

    } while (again == 'y');

    deallocate_words(words);
    return 0;
}
