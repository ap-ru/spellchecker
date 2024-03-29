#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024
#define MAX_WORD_LENGTH 100

int anyErrors = 0;

// 1st argument - dictionary file
//      - process the entire file and put all words into an array, then sort
// 2nd+ arguments - test files/directories
//      - iterate through all files within and find all .txt files
//      - read the file and generate sequence of position-annotated words
// For each word in the sequence, check whether it is in the dictionary

char **resize_array(char **arr, size_t *capacity) {
    size_t new_capacity = *capacity * 2;
    char **new_arr = realloc(arr, new_capacity * sizeof(char *));
    if (new_arr == NULL) {
        // Reallocation failed, return the original array
        return arr;
    }
    *capacity = new_capacity;
    return new_arr;
}

char* to_lower(const char *str) {
    size_t len = strlen(str);
    char *lower_str = malloc(len + 1);
    if (lower_str == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < len; i++) {
        lower_str[i] = tolower(str[i]);
    }
    lower_str[len] = '\0';

    return lower_str;
}

char* to_upper(const char *str) {
    size_t len = strlen(str);
    char *upper_str = malloc(len + 1);
    if (upper_str == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < len; i++) {
        upper_str[i] = toupper(str[i]);
    }
    upper_str[len] = '\0';

    return upper_str;
}

char* capitalize_first_letter(const char *str) {
    if (str == NULL || *str == '\0') {
        return NULL; // If string is empty or NULL, return NULL
    }
    size_t len = strlen(str);
    char *result = malloc(len + 1);
    strcpy(result, str); // Create a copy of the input string
    if (result == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    result[0] = toupper(result[0]); // Capitalize the first character of the copy

    return result;
}

int binary_search(const char *word, char **arr, size_t size) {
    int low = 0;
    int high = size - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        int cmp = strcmp(arr[mid], word);
        if (cmp == 0) {
            //printf("we found the index of '%s': %d\n", word, mid);
            return mid;
        } else if (cmp < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return -1;
}

int isInteger(const char *str) {
    if (*str == '\0') return 0; // Empty string is not an integer
    
    // Check for optional leading sign
    if (*str == '+' || *str == '-') str++;
    
    // Check each character for digits
    while (*str != '\0') {
        if (!isdigit(*str)) return 0; // Non-digit character found
        str++;
    }
    return 1; // All characters are digits
}

int permuteCapitalization(const char *tmp, char **words, size_t word_count) {
    int n = strlen(tmp);
    char *word = malloc((n+1) * sizeof(char));
    strcpy(word, tmp);

    int totalPermutations = 1 << n; // Total number of permutations is 2^n

    // Loop to generate all permutations
    for (int i = 0; i < totalPermutations; i++) {
        for (int j = 0; j < n; j++) {
            // Check if jth bit is set in i
            if (i & (1 << j)) {
                // If bit is set, convert character to uppercase
                word[j] = toupper(word[j]);
            } else {
                // If bit is not set, keep character as it is
                word[j] = tolower(word[j]);
            }
        }
        // Call the function to check if the word is in the array
        int result = binary_search(word, words, word_count);
        if (result > -1) {
            // Word found, exit with value 1
            free(word);
            return 1;
        }
    }
    // Word not found, exit with value 0
    free(word);
    return 0;
}

int is_word_valid (const char *word, char **words, size_t word_count) {
    int len = strlen(word);
    char *temp = malloc((len + 1)*sizeof(char));
    strcpy(temp, word);
    char *remember = temp;

    int contains_hyphen = 0;

    if (ispunct(temp[0])) {
        // Ignore quotation marks and brackets at the start of the word
        while (len > 0 && (temp[0] == '\'' || temp[0] == '\"' || temp[0] == '(' || temp[0] == '[' || temp[0] == '{')) {
            temp++; // Move pointer to the next character
            len--;
        }
    }

    // Check if the last character is punctuation
    if (len > 0 && ispunct(temp[len - 1])) {
        // Ignore punctuation marks occurring at the end of the word
        while (len > 0 && ispunct(temp[len - 1])) {
            temp[len - 1] = '\0'; // Remove the punctuation mark
            len--;
        }
    }

    for (int i = 0; temp[i] != '\0'; i++) {
        if (temp[i] == '-'){
            contains_hyphen++;
        }
    }

    if (contains_hyphen != 0) {
        int le = strlen(temp);
        char *token = malloc(le + 1);
        char *placeholder = token;
        strcpy(token, temp);
        token = strtok(temp, "-");
        while (token != NULL) {
            if (!is_word_valid(token, words, word_count)) {
                free(placeholder);
                free(temp);
                return 0;
            } else {
                token = strtok(NULL, "-");
            }
        }
        free(placeholder);
    }

    if (isInteger(temp)) {
        free(remember);
        return 1;
    }

    if (binary_search(temp, words, word_count) > -1){ // if word in dic
        free(remember);
        return 1;
    }

    char *t = capitalize_first_letter(temp);
    char *l = to_lower(temp);

    if (strcmp(temp, t) == 0) { // if only first letter caps
        if (binary_search(l, words, word_count) > -1) { // if lowercase in dic, valid
            free(l);
            free(t);
            free(remember);
            return 1;
        }
    }
    free(l);
    free(t);
    
    char *u = capitalize_first_letter(temp);
    if (strcmp(temp, u) == 0) { // if word is all upper
        if (permuteCapitalization(temp, words, word_count)) { // check all options
            free(u);
            free(remember);
            return 1;
        }
    }
    free(u);
    free(remember);
    
    return 0;
}

// if word is lowercase, only need to check if it is there
// if first letter of word is cap, then check all lower
// if word is all caps, need to check every permutation (cap) of that word

void process_dictionary_file (const char *dictionary_file, char ***words, size_t *word_count, size_t *capacity){
    int fd = open(dictionary_file, O_RDONLY);

    if (fd == -1) {
        perror("error opening file");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE+1];
    ssize_t bytes_read;
    char *last_non_null_token = NULL;
    char *prev = NULL;
    char *cat = malloc((MAX_WORD_LENGTH * 3) * sizeof(char));
    //char *tmp = NULL;

    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        buffer[bytes_read] = '\0';
        char *token = strtok(buffer, "\n");
        if (prev != NULL && prev[0] != token[0]) {
            strcpy(cat, prev);
            strcat(cat, token);
            free(prev);
            token = cat;
        } else {
            free(prev);
        }

        last_non_null_token = NULL;

        while (token != NULL) {
            //printf("%s\n", token);
            last_non_null_token = token;
            if (*word_count >= *capacity) {
                *words = resize_array(*words, capacity);
            }
            size_t len = strlen(token);
            char *result = malloc(len + 1);
            strcpy(result, token);
            (*words)[*word_count] = result;
            (*word_count)++;
            token = strtok(NULL, "\n");
        }
        char *tmp = malloc((strlen(last_non_null_token)+1) * sizeof(char));
        prev = tmp;
        strcpy(prev, last_non_null_token);
    }
    /* if (bytes_read < 0 && buffer[0] != '\0') {
        char *token = strtok(buffer, "\n");
        while (token != NULL) {
            if (*word_count >= *capacity) {
                *words = resize_array(*words, capacity);
            }
            size_t len = strlen(token);
            char *r = malloc(len + 1);
            strcpy(r, token);
            (*words)[*word_count] = r;
            (*word_count)++;
            token = strtok(NULL, "\n");
        }
    } */
    free(prev);
    free(cat);
    
    close(fd);
}

void check_test_file (char *test_file, char **dictionary, int dictionary_size) {
    int fd = open(test_file, O_RDONLY);
    if (fd == -1) {
        perror("Error opening test file");
        exit(EXIT_FAILURE);
    }
    char buffer[BUFFER_SIZE + 1];
    char word[MAX_WORD_LENGTH];
    int line_number = 1;
    int column_number = 1;
    ssize_t bytes_read;
    memset(word, 0, sizeof(word));

    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        buffer[bytes_read] = '\0';
        for (int i = 0; i < bytes_read; i++) {
            char c = buffer[i];
            if (c == ' ' || c == '\t' || c == '\n') { //marks end of word
                if (strlen(word) > 0) {
                    //printf("%s\n", word);
                    if(!(is_word_valid(word, dictionary, dictionary_size))){
                        printf("%s (%d,%ld): %s\n", test_file, line_number, column_number - strlen(word), word);
                        anyErrors = 1;
                    }
                    memset(word, 0, sizeof(word));
                }
                if (c == '\n') {
                    line_number++; // Move to next line
                    column_number = 1; // Reset column number
                } else {
                    column_number++; // Move to next column
                }
            } else {
                if (strlen(word) < MAX_WORD_LENGTH - 1) {
                    word[strlen(word)] = c; // Append character to word
                    column_number++;
                }
            }
        }
    }
    close(fd);
}

void search_test_dir (const char *test_dir, char **dic, int dic_size) {
    char **d = dic;
    int ds = dic_size;
    
    //opens test directory
    DIR *dir = opendir(test_dir);

    if (!dir){
        perror("Error: cannot open dictionary file\n");
        return;
    }

    struct dirent *entry;
    char file_path[PATH_MAX];

    while((entry = readdir(dir)) != NULL) {
        //skips . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        //creates file_path which is the og path + /entryname
        snprintf(file_path, sizeof(file_path), "%s/%s", test_dir, entry->d_name);
        
        //need to check if entry is a file or directory
        struct stat sbuf;
        int r = stat(file_path, &sbuf);
        if (r < 0) {
            perror("test file information could not be retrieved");
            continue;
        }
        //if file
        if (S_ISREG(sbuf.st_mode)) {
            if (entry->d_name[0] == '.'){
                continue;
            }
            if (strlen(file_path) >= 5) { // if does not start with . and ends with .txt, spell check
                if (strcmp(file_path + strlen(file_path) - 4, ".txt") == 0){
                    //printf("check_test_file called for file named %s\n", file_path);
                    check_test_file(file_path, d, ds);
                }
            }
        } else if (S_ISDIR(sbuf.st_mode)) { // if directory, recursively search
            search_test_dir(file_path, d, ds);
        } else {
            continue;
        }
    }

    closedir(dir);
}

int compare(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

int main(int argc, char *argv[]){
    if (argc < 3){
        printf("Error: dictionary and/or test file not provided\n");
        return EXIT_FAILURE;
    }
    size_t word_count = 0;
    size_t capacity = 1;
    char **words = malloc(capacity * sizeof(char *));
    char *dict_file = argv[1];
    process_dictionary_file(dict_file, &words, &word_count, &capacity);
    qsort(words, word_count, sizeof(char *), compare);

    

    //int index = binary_search("reference", words, word_count);
    //printf("%d\n%ld\n", index, word_count);

    for (int i = 2; i < argc; i++) {
        //search_test_dir(argv[2], words, word_count);
        struct stat sbuf;
        int r = stat(argv[i], &sbuf);
        if (r < 0) {
            perror("file information could not be retrieved");
        }
        if (S_ISREG(sbuf.st_mode)) {
            check_test_file(argv[i], words, word_count);
        } else if (S_ISDIR(sbuf.st_mode)) {
            search_test_dir(argv[i], words, word_count);
        }
    }

    for (size_t i = 0; i < word_count; i++) {
        free(words[i]);
    }

    free(words);

    if (anyErrors) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// given test file/directory
//      - search through it if directory to find all test files
// call check_test_file for each test file
//      - will iterate through test file
//          - iterate line number and column number as we go
//          - for each word, binary search that word (and similar alternatives) in the dictionary

// make sure to free words at the end