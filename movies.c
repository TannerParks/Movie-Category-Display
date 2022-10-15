#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct movie {
    char *title;
    int year;
    char *language[5];  // Max of 5 languages
    int num_langs;
    double rating;
    struct movie *next;
};

struct movie *add_to_list(struct movie *list, char *title, int year, char *language, double rating) {
    struct movie *node;
    node = malloc(sizeof(struct movie));

    char token_list[110][110]; // 110 = max # languages * max char length + semicolons + bracket
    char *context = NULL;   // used so strtok_r saves its state
    char *token = strtok_r(language+1, ";", &context);  // language+1 gets rid of the first bracket
    char *brack_tok;
    int num_langs = 0;

    while (token != NULL){  // Separates languages from the semicolons
       strcpy(token_list[num_langs], token); // Copy to token list
       num_langs++;
       token = strtok_r(NULL, ";", &context);  
    }

    for (int i=0; i < num_langs; i++) { // Gets rid of the closing bracket and saves to the language array
        brack_tok = strtok_r(token_list[i], "]", &context);
        strcpy(token_list[i], brack_tok);
        node->language[i] = strdup(token_list[i]);
    }

    node->title = strdup(title);
    node->year = year;
    node->num_langs = num_langs;
    node->rating = rating;
    node->next = list;

    return node;
  
};

struct movie *get_data(char *line) {
    char *movie;
    int year;
    char *language;
    double rating;
    char *rating_temp;

    char token_list[110][110]; // 110 = max # languages * max char length + semicolons + bracket
    char *context = NULL; // used so strtok_r saves its state
    char *token = strtok_r(line, ",", &context);
    int counter = 0;

    static struct movie *head = NULL;  // List starts as empty

    while (token != NULL) { // Splits each line into categories (title, year, rating, languages)
        strcpy(token_list[counter], token);
        counter++;
        token = strtok_r(NULL, ",", &context);
    };

    movie = token_list[0];
    year = atoi(token_list[1]); 
    language = token_list[2];
    rating = strtod(token_list[3], &rating_temp);

    head = add_to_list(head, movie, year, language, rating);    // New movies become the head of the list

    return head;
};

int main(int argc, char *argv[]) {
    FILE *movie_data;
    char str[500];
    struct movie *list = NULL;  // List starts as empty
    movie_data = fopen(argv[1], "r");
    int compare = 0; 

    int num_movies = 0;

    // Menu and input variables
    int input;
    char input2; // This is so the program doesn't loop infinitely when a character is entered instead of an  integer
    int year;
    char *lang;
    lang = malloc(21);
    int years[122] = {0}; // number of years
    int years_cpy[122] = {0};   // So the list can reset
    int mov_count = 0;
    struct movie *temp = NULL;
    struct movie *highest = NULL;
    struct movie *current = NULL;

    if (argc < 2) {
        printf("You must provide the name of the file to process\n");
        exit(0);
    }

    while (1){

        if (fgets(str, 500, movie_data) != NULL) {
            compare = strcmp(str, "Title,Year,Languages,Rating Value"); 
            if (compare == 1) { // skips the header line
                continue;
            }

            num_movies++;
            list = get_data(str);
        };

        if (feof(movie_data)) { // breaks at end of the file
            break;
        }

    }
    fclose(movie_data);

    printf("Processed file %s %s %d %s", argv[1], "and parsed data for", num_movies, "movies");
    
    temp = list;    // Used to traverse through and compare elements of the linked list
    current = list;
    highest = list;

    // Menu
    while (1) {
        printf("\n1. Show movies released in the specified year\n2. Show highest rated movie for each year\n3. Show the title and year of release of all movies in a specific language\n4. Exit from the program\n\n");
        scanf("%d", &input);
        scanf("%c", &input2);   // Bandaid used to prevent problems with non integer inputs
        
        if (input == 1) {
            printf("Enter a year to see all movies released in that year: \t");
            scanf("%d", &year);

            if (year >= 1900 && year <=2021) {
                while (temp != NULL) {
                    if (temp->year == year){
                        printf("%s\n", temp->title);
                        mov_count++;    // Counts how many movies released in this year
                    }
                    temp = temp->next;
                }
                if (mov_count == 0) {
                    printf("No movies found for the year %d\n", year);
                }
                temp = list;    // Resets the temp list since the previous while loop makes it the end of the linked list
                mov_count = 0;  // Resets movie count so it can be ran more than once
            }
            else {
                printf("Invalid input\n");
            }

            input = 0; // reset so a mistyped number doesn't run the same option twice
        }

        else if (input == 2) {
            printf("Highest rated\n");
            while (current != NULL) {
                temp = list;
                while (temp != NULL) {
                    if (temp->year == highest->year && temp->rating > highest->rating) {
                        highest = temp;
                    }
                    temp = temp->next;
                }
                if (years[2021-highest->year] == 0) {
                    printf("%d %.1f %s\n", highest->year, highest->rating, highest->title);
                    years[2021-highest->year] = 1;
                }
                current = current->next;
                highest = current;
            }
            // Reset all the values in case this option is chosen again
            temp = list;
            current = list;
            highest = list;
            memcpy(years, years_cpy, sizeof(years));
            input = 0;
        }

        else if (input == 3) {
            printf("Enter a language to see all movies in that language\t");
            scanf("%s", lang);
            while (temp != NULL) {
                for (int i = 0; i < 5; i++) {
                    // Since the for loop is to 5 the below code stops it from looping unnecessarily and causing problems
                    if (i == temp->num_langs) {
                        break;
                    }
                    compare = strcmp(temp->language[i], lang);
                    if (compare == 0) {
                        // Output movie if it has the inputted language
                        printf("%d %s\n", temp->year, temp->title);
                        mov_count++;
                        break;
                    }
                }
                temp = temp->next;
            }
            if (mov_count == 0) {
                printf("\nNo movies found in the language %s\n", lang);
            }            
            temp = list;
            mov_count = 0;
            input = 0;
        }

        else if (input == 4) {
            free(lang);
            exit(0);    // Bet you can't guess what this line does
        }

        else {
            printf("Invalid input\n");
        }
    };

    return 0;
};
