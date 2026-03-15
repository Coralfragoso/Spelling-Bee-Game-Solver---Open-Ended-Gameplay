/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Program 3:  Spelling Bee Game & Solver - Open-Ended Gameplay
    A generalized version of the NYT Games' Spelling Bee. To build the hive, a hive and required centered letter is either chosen randomly in random mode or through the user consisting of 
    hiveSize letter, that can range from MIN_HIVE_SIZE = 2 to MAX_HIVE_SIZE = 12. Valid words contain at least MIN_HIVE_SIZE = 4 letters and can reuse hive letters. At the end and during 
    of the game, scores are calculated and assigned 1 point each for word of MIN_WORD_LENGTH, 1 point per letter, or points for each letter and each letter in hive for pangrams.
    Once in playMode, the game consists of the user repeatedly entering words and the program checking for valid words to collect and keeping score of points. An open-ended gameplay version
    is implemented where a randomly selected starting letter each round is required for the word. When playMode is off or ends, the program ends by finding and displaying the full set of 
    valid words of the hive. Additional features like specifically identifying pangrams in displays is included. Other tools may used for user's looking for hints.
Course: CS 211, Fall 2025, UIC 
Author: Coral Fragoso Herrera
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

const int MIN_HIVE_SIZE = 2; // minimum length of hive
const int MAX_HIVE_SIZE = 12; // maximum length of hive
const int MIN_WORD_LENGTH = 4; // minimum word length

typedef struct WordList_struct {
    char** words; // dynamic array of C-strings
    int numWords; // number of C-string pointers
    int capacity; // number of C-strings stored at any given time
} WordList;

/*
 * This function mallocs and creates an empty word list 
 * Returns:
 *  empty word list
 */
WordList* createWordList() {
    WordList* newList = malloc(sizeof(WordList)); // allocates memory for a new WordList
    newList->capacity = 4; // sets default capacity to 4 C-strings
    newList->numWords = 0; // sets default numWords to 0
    newList->words = malloc(newList->capacity * sizeof(char*)); // allocates memory for 4 c-strings in dynamic words array

    return newList; // returns new WordList
}

/* 
 * This function dynamically allocates memory for a growing word list and appends new word
 * Parameters:
 *  thisWordList - word list to append new word 
 *  newWord - C-string to add
 */
void appendWord(WordList* thisWordList, char* newWord) {
    if ((*thisWordList).numWords != (*thisWordList).capacity) { // checks if there is space for a new word
        char* str_copy = (char*)malloc(sizeof(char) * (strlen(newWord) + 1)); // allocates one more space for the word string
        strcpy(str_copy, newWord); // copies the newWord to str_copy
        thisWordList->words[thisWordList->numWords] = str_copy; // appends new word
        thisWordList->numWords++; // increments numWords by one
    }
    else { 
        char** temp; // temporary dynamic array of C-strings
        temp = (char**)malloc(sizeof(char*)* thisWordList->capacity * 2); // allocates twice the memory of thisWordList to temp
        
        // assigns all words from thisWordList to temp
        for (int i = 0; i < thisWordList->capacity; i++) {
            temp[i] = thisWordList->words[i];
        }

        char* str_copy = (char*)malloc(sizeof(char) * (strlen(newWord) + 1)); // allocates space for the new word string
        strcpy(str_copy, newWord);               // copies newWord to str_copy
        temp[thisWordList->capacity] = str_copy; //appends new word

        free(thisWordList->words);  // frees old memory of word array
        thisWordList->words = temp; // word array now points to temp
        thisWordList->numWords++;   // increments numWords by one
        thisWordList->capacity*=2;  // doubles capacity

    }
}

/*
 * This functions builds a dictionary of at least minLength size
 * Parameters:
 *  filename - dictionary file
 *  dictionaryList - empty dictionary list
 *  minLength - minimum length of dictionary words
 * Returns:
 *  longest word length found or -1 if there is an error reading/finding words
 */
int buildDictionary(char* filename, WordList* dictionaryList, int minLength) {

    FILE* myFile = NULL; // creates FILE struct to allow reading & writing to files
    char currWord[200];  // stores word string from read-ins

    myFile = fopen(filename, "r"); // opens stream for reading to the file filename
    if (myFile == NULL) { // checks if file was successfully opened
        return -1;        // -1 indicates error
    } 

    int max = 0; // stores length of the longest possible word

    // keeps reading next word until end of file 
    while (fscanf(myFile, "%s", currWord) != EOF) { 
        if (strlen(currWord) >= minLength) {      // checks if word is at least size of minLength
            appendWord(dictionaryList, currWord); // appends word to dictionaryList
            if (strlen(currWord) > max) { // checks if length is greater than current max length
                max = strlen(currWord);   // resets max length
            }
        }
    }
    fclose(myFile); // closes the file
   
    return max; // returns longest word length
}

/*
 * This function frees the memory used by the WordList, both the words' char* arrays and the char** words array itself
 * Parameters: 
 *  list - word list
 */
void freeWordList(WordList* list) {
    // frees each individual C-string in words array
    for (int i = 0; i < list->numWords; i++) {
        free(list->words[i]);
    }
    free(list->words); // frees dynamic word array
    free(list);        // frees word list struct itself
}

/*
 * This function finds the index of a letter
 * Parameters:
 *  str -  C-string
 *  aLet - letter to look for
 * Returns:
 *  Index of letter in the word or -1 if not found
 */
int findLetter(char* str, char aLet) {
    char* ptr = strchr(str, aLet); // stores pointer to letter in the word
    int charInd;                   // stores the index of the letter

    if (ptr != NULL) {       // checks if letter exists in word
        charInd = ptr - str; // assigns index to charInd
        return charInd;      // returns index of letter
    }
  
    return -1; // letter not found
}

/*
 * This function builds a unique, alphabetically-ordered hive
 * Parameters:
 *  str - C-string
 *  hive - empty word string
 * Returns:
 *  Populated hive implicitly
 */
void buildHive(char* str, char* hive) {
    int smallestInd;     // stores index of lowest ASCII value of a letter
    int temp;            // stores ASCII value of current letter
    int uniqueCount = 0; // stores count of unique letters in word
    
    // selection sorting algorithm to order string alphabetically
    for (int i = 0; i < strlen(str) - 1; i++) {
        smallestInd = i; // finds index of smallest remaining letter
        for (int j = i + 1; j < strlen(str); j++) {
            if ((int)str[j] < (int)str[smallestInd]) {
                smallestInd = j;
            }
        }

        // swap str[i] and str[smallestInd]
        temp = str[i];
        str[i] = str[smallestInd];
        str[smallestInd] = temp;
    }

    // searches through word to append unique letters
    for (int i = 0; i < strlen(str); i++) {   
        bool unique = true; // letter is unique

        // checks if letter already appears in hive
        for (int j = 0; j < uniqueCount; j++) {
            if (hive[j] == str[i]) {
                unique = false; // letter is a duplicate
                break;          // check the next letter
            }
        }
        if (unique) { // appends letter to hive if it is unique (true)
            hive[uniqueCount] = str[i];
            uniqueCount++; // increments hive length
        }
    }
    
    hive[uniqueCount] = '\0'; // adds null-terminated character to end of hive
}

/*
 * This function counts the number of unique letters in the input string
 * Parameters:
 *  str - C-string
 * Returns:
 *  The number of unique letters in the input string
 */
int countUniqueLetters(char* str) {
    char uniqueLet[100]; // stores unique letters
    int numLet = 0;      // stores number of unique letters

    // looks for unique letters in a word
    for (int i = 0; i < strlen(str); i++) {
        bool unique = true; // letter is unique
        for (int j = 0; j < numLet; j++) {
            if (uniqueLet[j] == str[i]) { // checks for duplicates
                unique = false; // letter is not unique
                break;          // check next letter
            } 

        }
        if (unique == true) { // checks if letter is unique
            uniqueLet[numLet] = str[i]; // appends to unique array of letters
            numLet++;                   // increments number of unique letters
        }   
    }
    uniqueLet[numLet] = '\0'; // adds nul-terminated character at the end of unique array

    return strlen(uniqueLet); // returns count of unique letters in string
}

/*
 * This function creates a WordList and adds all fit words from dictionaryList to the new WordList 
 * Parameters:
 *  dictionaryList - dictionary word list
 *  hiveSize - size of hive
 * Returns:
 *  The new WordList, fitWords, with all fit words from dictionaryList
 */
WordList* findAllFitWords(WordList* dictionaryList, int hiveSize) {
    WordList* fitWords = createWordList(); // creates an empty word list

    // finds and appends fit words to new word list
    for (int i = 0; i < dictionaryList->numWords; i++) {
        int uniqueCount = countUniqueLetters(dictionaryList->words[i]); // stores unique letter count
        if (uniqueCount == hiveSize) { // checks if fit word has exactly hiveSize unique letters
            appendWord(fitWords, dictionaryList->words[i]); // appends fit word to fitWords
        }
    }
  
    return fitWords; // returns word list of all fit words
}

/*
 * This function determines validity of user's word
 * Parameters: 
 *  word - input word
 *  hive - alphabetically ordered string of unique letters in puzzle
 *  reqLet - required hive letter word must have
 * Returns:
 *  True if word uses required letter and only hive letters or false otherwise
 */
bool isValidWord(char* word, char* hive, char reqLet) {
    bool useReqLet = false; // stores true if word uses reqLet
    
    // searches letters for the required letter
    for (int i = 0; i < strlen(word); i++) {
        bool useHiveLet = false; // stores true if it is a hive letter
        // searches through hive
        for (int j = 0; j < strlen(hive); j++) { 
            if (word[i] == hive[j]) {    // checks if there is a matching letter in hive
                useHiveLet = true;       // letter is valid
                if (word[i] == reqLet) { // checks if the letter is the required letter
                    useReqLet = true;    // found required letter 
                }
                break; // proceeds to next letter
            }
        }
        if (!useHiveLet) { // checks if letter is invalid
            return false;  // word is invalid
        }
    }
    if (useReqLet) { // checks if required letter is used
        return true; // word is valid
    }

    return false; // defaults to false
}

/*
 * This function determines if a word is a pangram
 * Parameters:
 *  str - input word
 *  hive - alphabetically ordered string of unique letters in puzzle
 * Returns:
 *  True if str is a pangram or false otherwise
 */
bool isPangram(char* str, char* hive) {
    int count = 0; // stores count of unique hive letters used
    if (countUniqueLetters(str) >= strlen(hive)) { // checks if word is at least hiveSize long
        // loops through hive letters
        for (int i = 0; i < strlen(hive); i++) { 
            // loops through input word letters
            for (int j = 0; j < strlen(str); j++) {
                if (str[j] == hive[i]) { // checks if letters match
                    count++; // increments count of unique hive letters used
                    break;   // proceeds to next letter
                }
            }
        }
    }
    if (count == strlen(hive)) { // checks if count is the same as hive length
        return true; // word is a pangram
    }
   
    return false; // word is not a pangram
}

/*
 * This function displays the hive
 * Parameters:
 *  hive - alphabetically ordered string of unique letters in puzzle
 *  reqLetInd - index of required letter
 */
void printHive(char* hive, int reqLetInd) {
    printf("  Hive: \"%s\"\n", hive);
    printf("         ");
    for (int i = 0; i < reqLetInd; i++) {
        printf(" ");
    }
    printf("^");
    for (int i = reqLetInd + 1; i < strlen(hive); i++) {
        printf(" ");
    }
    printf(" (all words must include \'%c\')\n\n", hive[reqLetInd]); // displays required letter
}

/*
 * This function displays words in the list and total score
 * Parameters:
 *  thisWordList - word list
 *  hive  
 */
void printList(WordList* thisWordList, char* hive) {
    printf("  Word List:\n");
    int totScore = 0; // stores total score
    int score = 0;    // stores score for each word

    // loops through each word in word list
    for (int i = 0; i < thisWordList->numWords; i++) {
        // checks if a word is a perfect pangram
        if (isPangram(thisWordList->words[i], hive) && countUniqueLetters(thisWordList->words[i]) == strlen(hive)) { 
            printf("  *** ");
            score = strlen(hive) * 2; // doubles score
            totScore += score;        // increments total score by word score
        }
        else if (isPangram(thisWordList->words[i], hive)) { // checks if a word is a pangram
            printf("   *  ");
            score = strlen(hive) + strlen(thisWordList->words[i]); // assigns score with points for all hive letters + string length
            totScore += score;                                     // increments total score by word score
        }    
        else if (strlen(thisWordList->words[i]) == MIN_WORD_LENGTH) {   // checks if word is minimum length
            score = 1;       // assigns score with 1 point
            totScore+=score; // increments total score by word score
            printf("      ");
        }
        else { // regular word scoring
            score = strlen(thisWordList->words[i]); // assigns score with a point for each letter
            totScore+=score;                        // increments total score by word score
            printf("      ");
        }
        if (i < 10){ // displays word and scores below 10 with a space
            printf("( %d) %s\n", score, thisWordList->words[i]);
        }
        else { // displays all word and score
            printf("(%d) %s\n", score, thisWordList->words[i]);
        }    
        
    }

    printf("  Total Score: %d\n", totScore); // displays total score
}

/*
 * This function goes through each word in the dictionary and determines if that is a solution to the puzzle to append
 * Parameters:
 *  dictionaryList - dictionary
 *  solvedList - empty list of possible word solutions
 *  hive - alphabetically ordered string of unique letters in puzzle
 *  reqLet - required hive letter in word
 */
void bruteForceSolve(WordList* dictionaryList, WordList* solvedList, char* hive, char reqLet) {
    // loops through each word in dictionary
    for (int i = 0; i < dictionaryList->numWords; i++) {
        // adds the word to solvedList if it is a valid word
        if (isValidWord(dictionaryList->words[i], hive, reqLet)) {
            appendWord(solvedList, dictionaryList->words[i]);
        }
    }
}

/*
 * This function checks if partWord is the root/prefix of fullWord
 * Parameters: 
 *  partWord - substring to analyze
 *  fullWord - entire word
 * Returns:
 *  True is partWord is a root/prefix of fullWord; false otherwise 
 */
bool isPrefix(char* partWord, char* fullWord) {
    char* result = strstr(fullWord, partWord); // stores point to first occurrence of partWord in fullWord
    if (strlen(partWord) > strlen(fullWord)) { // checks if partWord is not a substring at all
        return false; // partWord is not a prefix
    }
    if  (result - fullWord == 0) { // first occurrence is at the beginning of fullWord
        return true; // partWord is a prefix
    }

    return false; // defaults to not a prefix otherwise
}

/*
 * This function finds words that fully or partially match the given word
 * Parameters:
 *  thisWordList - word list to find words matching given word
 *  aWord - given word to match
 *  loInd - starting index of current search space
 *  hiInd - ending index of current search space
 * Returns:
 *  index of word if found in thisWordList or -1 if a root word to another word; -99 otherwise
 */
int findWord(WordList* thisWordList, char* aWord, int loInd, int hiInd) {
    if (hiInd < loInd) { // Base case 2: aWord not found in words[]
        // checks if word is a prefix and in bounds
        if (loInd < thisWordList->numWords && isPrefix(aWord, thisWordList->words[loInd])) { 
            return -1; // words match this root (partial match)
        }
        else {
            return -99; // no more words matching this root (no match)
        }
    }

    int mdInd = (hiInd + loInd) / 2;

    if (strcmp(aWord, thisWordList->words[mdInd]) == 0) { // Base case 1: found tryWord at midInd
        return mdInd;
    }

    if (strcmp(aWord, thisWordList->words[mdInd]) > 0) { // Recursive case: search upper half
        return findWord(thisWordList, aWord, mdInd + 1, hiInd);
    }

    // Recursive case: search lower half
    return findWord(thisWordList, aWord, loInd, mdInd - 1);
}

/*
 * This function build all an possible solution word list
 * Parameters:
 *  dictionaryList - dictionary
 *  solvedList - all possible solutions
 *  tryWord - input word
 *  hive - alphabetically ordered string of unique letters in puzzle
 *  reqLet - required letter for word
 * Returns:
 *  implicitly returns solvedList with solutions
 */
void findAllMatches(WordList* dictionaryList, WordList* solvedList, char* tryWord, char* hive, char reqLet) {
    int curLen = strlen(tryWord); // stores length of tryWord

    // Base case: if tryWord is empty, all possibilities are finished
    if (curLen == 0) {
        return;
    }

    // binary search to find word in dictionary or if prefix
    // stores index, -1 for prefix, or -99 for no match
    int index = findWord(dictionaryList, tryWord, 0, dictionaryList->numWords - 1);  
    
    // match found in dictionary
    if (index >= 0) {
        // checks if dictionary word is valid
        if (isValidWord(tryWord, hive, reqLet) && curLen >= MIN_WORD_LENGTH) {      
            appendWord(solvedList, tryWord); // adds valid word to solvedList       
        }
      
        char* tempWord = (char*)malloc((curLen + 2) * sizeof(char)); // allocates memory for null char and 2 more char
        strcpy(tempWord, tryWord);   // copies tryWord to tempWord
        tempWord[curLen] = hive[0];  // appends first hive letter
        tempWord[curLen + 1] = '\0'; // adds null character

        // recursively searches for words beginning with new prefix
        findAllMatches(dictionaryList, solvedList, tempWord, hive, reqLet);

        free(tempWord); // frees allocated memory of tempWord
        
        return;
    }
    if (index == -1) { // checks if not a dictionary word but prefix
        char* tempWord = (char*)malloc((curLen + 2) * sizeof(char)); // allocates memory for 2 more char and a null char
        strcpy(tempWord, tryWord);   // copies the prefix tryWord to tempWord
        tempWord[curLen] = hive[0];  // appends first hive letter
        tempWord[curLen + 1] = '\0'; // adds null character

        // recursively searches for words beginning with longer prefix
        findAllMatches(dictionaryList, solvedList, tempWord, hive, reqLet);
        
        free(tempWord); // frees allocated memory of tempWord
        return;
    }
    
    // otherwise, not a word or prefix

    char* newTryWord = (char*)malloc((curLen + 1) * sizeof(char)); // one more for null char
    strcpy(newTryWord, tryWord); // copies prefix tryWord to newTryWord

    while (strlen(newTryWord) > 0) {
        int newLen = strlen(newTryWord);
        char lastChar = newTryWord[newLen - 1]; // gets last character
        int lastCharInd = findLetter(hive, lastChar); // finds index of last character

        if (lastCharInd == -1) { // checks if character in hive
            break;
        }
        if (lastCharInd < (int)strlen(hive) - 1) { // checks if proceeding to next character is possible
            newTryWord[newLen - 1] = hive[lastCharInd + 1]; // get next hive letter

            // recursively searchers for words with newTryWord
            findAllMatches(dictionaryList, solvedList, newTryWord, hive, reqLet); 
            free(newTryWord); // frees allocated memory of newTryWord
            return;
        }
        newTryWord[newLen - 1] = '\0'; // adds null character
    }
    free(newTryWord); // frees allocated memory of newTryWord
}

/* 
 * This function sets program parameters using command-line arguments
 * Parameters: 
 *  argc - number of arguments in command-line
 *  argv - array storing each argument
 *  pRandMode - randMode pointer
 *  pNumLets - pointer for number of letters for spelling bee game
 *  dictFile - dictionary file
 *  pPlayMode -  playMode pointer
 *  pBruteForceMode -  bruteForceMode pointer
 *  pSeedSelection - starting point for random number generator 
 * Returns:
 *  True or false if setting is valid
 */
bool setSettings(int argc, char* argv[], bool* pRandMode, int* pNumLets, char dictFile[100], bool* pPlayMode, bool* pBruteForceMode, bool* pSeedSelection) {
    // sets default mode for settings
    *pRandMode = false;
    *pNumLets = 0;
    strcpy(dictFile, "dictionary.txt");
    *pPlayMode = false;
    *pBruteForceMode = true;
    *pSeedSelection = false;
    srand((int)time(0));
    
    // loops through command-line arguments
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-r") == 0) { // checks for random mode
            ++i; // gets next index
            if (argc == i) {  // insufficient command-line arguments
                return false; // invalid command-line arguments
            }
            *pRandMode = true; // sets randMode to true 
            *pNumLets = atoi(argv[i]); // sets number of letters for game
            if (*pNumLets < MIN_HIVE_SIZE || *pNumLets > MAX_HIVE_SIZE) { // checks if numLet is in bounds
                return false; // out of bounds (invalid argument)
            }
        }
        else if (strcmp(argv[i], "-d") == 0) { // checks for dictFile
            ++i; // gets next index
            if (argc == i) {  // insufficient command-line arguments
                return false; // invalid command-line arguments
            }
            strcpy(dictFile, argv[i]);
            FILE* filePtr = fopen(dictFile, "r");
            if (filePtr == NULL) {
                return false;
            }
            fclose(filePtr);
        }
        else if (strcmp(argv[i], "-s") == 0) { // checks for seedSelection
            ++i; // gets next index
            if (argc == i) {  // insufficient command-line arguments
                return false; // invalid command-line arguments
            }
            *pSeedSelection = true;
            int seed = atoi(argv[i]);
            srand(seed);
        }
        else if (strcmp(argv[i], "-p") == 0) { // checks for play mode
            *pPlayMode = true;
        }
        else if (strcmp(argv[i], "-o") == 0) { // checks for optimized approach
            *pBruteForceMode = false;
        }
        else {
            return false;
        }
    }

    return true;
}

/*
 * This function displays the mode's activity
 * Parameters:
 *  mode - game mode activity (true or false)
 */
void printONorOFF(bool mode) {
    if (mode) {
        printf("ON\n");
    }
    else {
        printf("OFF\n");
    }
}

/*
 * This function displays if a mode is on
 * Parameters:
 *  mode - game mode activity (true or false)
 */
void printYESorNO(bool mode) {
    if (mode) {
        printf("YES\n"); // game mode is on
    }
    else {
        printf("NO\n"); // game mode is off
    }
}



int main(int argc, char* argv[]) {

    printf("\n----- Welcome to the CS 211 Spelling Bee Game & Solver! -----\n\n");

    // default program settings
    bool randMode = false;
    int hiveSize = 0;
    char dict[100] = "dictionary.txt";
    bool playMode = false;
    bool bruteForce = true;
    bool seedSelection = false;
    char hive[MAX_HIVE_SIZE + 1] = {}; // array with max hive size elements
    hive[0] = '\0';
    int reqLetInd = -1;
    char reqLet = '\0';

    // program terminates if user does not specify appropriate settings
    if (!setSettings(argc, argv, &randMode, &hiveSize, dict, &playMode, &bruteForce, &seedSelection)) {
        printf("Invalid command-line argument(s).\nTerminating program...\n");
        return 1;
    }
    else { // displays program settings
        printf("Program Settings:\n");
        printf("  random mode = ");
        printONorOFF(randMode);
        printf("  play mode = ");
        printONorOFF(playMode);
        printf("  brute force solution = ");
        printONorOFF(bruteForce);
        printf("  dictionary file = %s\n", dict);
        printf("  hive set = ");
        printYESorNO(randMode);
        printf("\n\n");
    }

    // builds empty dictionary
    printf("Building array of words from dictionary... \n");
    WordList* dictionaryList = createWordList(); 

    // maxWordLength is decided by the returned longest word length from the dictionary file from all at least MIN_WORD_LENGTH letter words
    int maxWordLength = buildDictionary(dict, dictionaryList, MIN_WORD_LENGTH);
    if (maxWordLength == -1) { // there is an invalid file or there are no words of at least MIN_WORD_LENGTH
        printf("  ERROR in building word array.\n");
        printf("  File not found or incorrect number of valid words.\n");
        printf("Terminating program...\n");
        return -1; // program ends
    }
    printf("   Word array built!\n\n");


    printf("Analyzing dictionary...\n");

    if (dictionaryList->numWords < 0) { // dictionaryList not created/Populated
        printf("  Dictionary %s not found...\n", dict);
        printf("Terminating program...\n");
        return -1; // program ends
    }

    // end program if file has zero words of minimum desired length or longer
    if (dictionaryList->numWords == 0) {
        printf("  Dictionary %s contains insufficient words of length %d or more...\n", dict, MIN_WORD_LENGTH);
        printf("Terminating program...\n");
        return -1;
    }
    else { // displays all words from dictionary of minimum desired length or longer
        printf("  Dictionary %s contains \n  %d words of length %d or more;\n", dict, dictionaryList->numWords, MIN_WORD_LENGTH);
    }



    // A hive is randomly constructed to have a pangram and the required letter is also randomly chosen
    if (randMode) {
        printf("==== SET HIVE: RANDOM MODE ====\n");
        // find all words in word list that use only hiveSize unique letters and returns list
        WordList* fitWords = findAllFitWords(dictionaryList, hiveSize);
        int numFitWords = fitWords->numWords; // stores number or words in list
        // pick one at random
        int pickOne = rand() % numFitWords;
        char* chosenFitWord = fitWords->words[pickOne];

        // and alaphabetize the unique letters to make the letter hive
        buildHive(chosenFitWord, hive); // a hive is built from the chosen word in the dictionary
        freeWordList(fitWords); // memory used by fitWords is freed

        reqLetInd = rand() % hiveSize; // required letter index is chosen from hive
        reqLet = hive[reqLetInd]; // required letter is set

    }
    else { // randMode is off
        printf("==== SET HIVE: USER MODE ====\n");

        bool valid = false; // stores boolean value for word validity
        
        // continues until hive string is valid
        while (valid == false) {
            printf("  Enter a single string of lower-case,\n  unique letters for the letter hive... ");
            scanf(" %s", hive);

            // checks if hive length is outside bounds from min length to max length
            if (strlen(hive) > MAX_HIVE_SIZE || strlen(hive) < MIN_HIVE_SIZE) {
                printf("  HIVE ERROR: \"%s\" has invalid length;\n  valid hive size is between %d and %d, inclusive\n\n",hive, MIN_HIVE_SIZE, MAX_HIVE_SIZE);
                continue; // invalid, get next string

            }
            bool validLet = true; // stores boolean value for string validity

            // compares all letters in hive 
            for (int i = 0; i < strlen(hive); i++) {
                // checks if hive letter is uppercase or not a alphabet character
                if (isupper(hive[i]) || !(isalpha(hive[i]))) {
                    validLet = false; // string is invalid
                    printf("  HIVE ERROR: \"%s\" contains invalid letters;\n  valid characters are lower-case alpha only\n\n",hive);
                    break; // stops looking
                }
            }
            if (validLet == false) {
                continue; // invalid, get next string
            }

            bool duplicateLet = false; // stores boolean value for letter duplicates

            // compares all hive letters to each other
            for (int i = 0; i < strlen(hive) - 1; i++) { 
                if (duplicateLet == true) { // breaks if there is duplicate letters
                    break;
                }
                for (int j = i + 1; j < strlen(hive); j++) { // searches all other letters after
                    if (hive[j] == hive[i]) { // checks if there is another same letter
                        duplicateLet = true;  // letter is a duplicate
                        printf("  HIVE ERROR: \"%s\" contains duplicate letters\n\n",hive);
                        break; // stops searching
                    }
                }
            }
            if (duplicateLet == true) {
                continue; // invalid, get next string
            }
            valid = true; // else, string is valid
        }

        // properly builds hive alphabetically ordered
        buildHive(hive, hive);      

        hiveSize = strlen(hive); // gets hive length

        // required letter is not set
        reqLetInd = -1;
        reqLet = '\0';

        bool validReqLetter = false; // stores boolean value for letter validity

        // continues until letter is valid
        while (!validReqLetter) {
            printf("  Enter the letter from \"%s\"\n  that is required for all words: ", hive);
            scanf(" %c", &reqLet);
            reqLet = tolower(reqLet); // changes letter to lowercase

            bool found = false; // stores whether letter belongs to hive or not

            // compares letter to hive letters
            for (int i = 0; i < strlen(hive); i++) {
                if ((int)hive[i] == (int)reqLet) { // checks if letter exists in hive
                    found = true; // belongs to hive
                    break;        // stops searching
                }
            }
            if (!found) { // checks if letter is not found in hive
                printf("  HIVE ERROR: \"%s\" does not contain the character \'%c\'\n\n",hive,reqLet);
                continue; // invalid, gets next letter
            }
            validReqLetter = true; // letter is valid
        }
            
    }
    char* ptr; // stores a pointer to letter in hive
    ptr = strchr(hive, reqLet); // sets pointer to required letter
    reqLetInd = (int)(ptr - hive); // gets the index of the reqLet

    // hive is printed with the required letter
    printHive(hive, reqLetInd);

    if (playMode) { // playMode is on
        printf("==== PLAY MODE ====\n");
    //---------------------------------------------------------------------
    //              BEGINNING OF OPEN-ENDED GAMEPLAY SECTION
    //---------------------------------------------------------------------
    
        // allocates memory for user input
        char* userWord = (char*)malloc((maxWordLength + 1) * sizeof(char));
        strcpy(userWord, "default"); // copies default to userWord

        // creates list of valid words the user has found
        WordList* userWordList = createWordList();

        int roundNum = 1;                                          // stores current round number
        char currStartLetter;                                      // stores current required starting letter
        int UsedLettersCount = 0;                                  // stores count of used letters
        bool lettersUsed[MAX_HIVE_SIZE];                           // stores array of letters from hive used (boolean)
        WordList* solvedList = createWordList();                   // creates empty solution list
        bruteForceSolve(dictionaryList, solvedList, hive, reqLet); // populates solution list with all valid words

        bool hasWord[MAX_HIVE_SIZE];         // tracks if each hive letter has a solution for starting letter
        int validStartLetInd[MAX_HIVE_SIZE]; // stores index of valid starting letters
        int numValidLet = 0;                 // stores number of valid starting letters

        // check if each hive letter is the beginning of a solution
        for (int i = 0; i < hiveSize; i++) {
            hasWord[i] = false;
            for (int j = 0; j < solvedList->numWords; j++) {
                // checks if hive letter is the first letter or solution
                if (solvedList->words[j][0] == hive[i]) { 
                    hasWord[i] = true; // valid starting letter
                    validStartLetInd[numValidLet] = i; // assigns valid index
                    numValidLet++; // increases count of valid starting letters
                    break; // proceeds to next hive letter
                }
            }
        }

        // initializes lettersUsed
        for (int i = 0; i < MAX_HIVE_SIZE; i++) {
            lettersUsed[i] = false;
        }

    
        int startLetterInd = rand() % numValidLet; // randomly selects index for starting letter
        currStartLetter = hive[validStartLetInd[startLetterInd]]; // assigns current starting letter
        lettersUsed[startLetterInd] = true;        // indicates letter is used in array
        UsedLettersCount++;                        // increments count of letters used

        printf("............................................\n");
        printf("    REQUIRED STARTING LETTER MODE\n");
        printf("............................................\n");
        printf(" Available starting letters: ");
        for (int i = 0; i < numValidLet; i++) {
            printf("'%c' ", hive[validStartLetInd[i]]);
        }
        printf("\n");
        printHive(hive, reqLetInd);

        // displays round
        printf("    Round %d: Words must start with '%c'\n", roundNum, currStartLetter);

        // gets user input for a word
        printf("  Enter a word (enter DONE to quit): ");
        scanf("%s", userWord);
        printf("\n");

        // keeps getting words from user until user is done playing
        while (strcmp(userWord, "DONE") != 0) {  
            // checks if word is not at leat MIN_WORD_LENGTH
            if (strlen(userWord) < MIN_WORD_LENGTH) {
                printf("Invalid word. Word is too short.\n");
                printf("  Enter a word (enter DONE to quit): ");
                // prompts user for new word 
                scanf("%s", userWord);
                if (strcmp(userWord, "DONE") == 0) {
                    break;
                }
                printf("\n");
                continue;
            }
            // checks if word does not start with required letter
            else if (userWord[0] != currStartLetter) {
                printf("Invalid starting letter. Word must start with '%c'\n", currStartLetter);
                printf("  Enter a word (enter DONE to quit): ");
                // prompts user for new word
                scanf("%s", userWord);
                if (strcmp(userWord, "DONE") == 0) {
                    break;
                }
                printf("\n");
                continue;
            }
            // checks if word did not use required letter
            else if (findLetter(userWord, reqLet) == -1) {
                printf("Invalid word. Missing required letter '%c'\n", reqLet);
                printf("  Enter a word (enter DONE to quit): ");
                // prompts user for new word
                scanf("%s", userWord);
                if (strcmp(userWord, "DONE") == 0) {
                    break;
                }
                printf("\n");
                continue;
            }
            // checks if word is not valid
            else if (!isValidWord(userWord, hive, reqLet)) {
                printf("Invalid word. Uses non-hive letters.\n");
                printf("  Enter a word (enter DONE to quit): ");
                // prompts user for new word
                scanf("%s", userWord);
                if (strcmp(userWord, "DONE") == 0) {
                    break;
                }
                printf("\n");
                continue;
            }
            else {
                // appends word if valid
                if (findWord(solvedList, userWord, 0, solvedList->numWords -1) >= 0 && findWord(userWordList, userWord, 0, userWordList->numWords -1) < 0) {
                    appendWord(userWordList, userWord);
                    roundNum++; // increases number for next round
                } 
                else { // word has already been used before or DNE
                    printf("Invalid word. Not found or duplicate\n");
                    printf("  Enter a word (enter DONE to quit): ");
                    // prompts user for new word
                    scanf("%s", userWord);
                    if (strcmp(userWord, "DONE") == 0) {
                        break;
                    }
                    printf("\n");
                    continue;
                }
            }

            char oldStartLetter = currStartLetter; // stores previous starting letter to avoid next round

            // checks if all hive letters haven't been used as the starting letter
            if (UsedLettersCount < numValidLet) {
                int unusedLettersInd[MAX_HIVE_SIZE]; // stores index of unused letters
                int unusedCount = 0;                 // stores count of unused letters

                // looks for unused letters in hive
                for (int i = 0; i < numValidLet; i++) {
                    int index = validStartLetInd[i];
                    if(!lettersUsed[index]) { // checks if letter is unused
                        unusedLettersInd[unusedCount] = index; // index is added
                        unusedCount++;                     // increments count of unused letters

                    }
                }
                // picks random unused letter
                if (unusedCount > 0) {
                    int newStartLetterInd = unusedLettersInd[rand() % unusedCount]; // stores index of next starting letter
                    currStartLetter = hive[newStartLetterInd];                      // assigns new startng letter to currStartLetter
                    lettersUsed[newStartLetterInd] = true;                          // indicates letter is used in array
                    UsedLettersCount++;                                             // increments count of used letters
                }
                

            }
            else {
                // starting letters have all been used at least once but make sure letter is different from previous
                do {
                    startLetterInd = rand() % numValidLet;
                    currStartLetter = hive[validStartLetInd[startLetterInd]];
                } while (currStartLetter == oldStartLetter && numValidLet > 1 );

                // resets tracking to reuse letters
                for (int i = 0; i < hiveSize; i++) {
                    lettersUsed[i] = false;
                }
                int newInd = findLetter(hive, currStartLetter);
                lettersUsed[newInd] = true;
                UsedLettersCount = 1;

                printf("Starting new cycle.... Words starting with all hive letters have been found!\n");
            }
            
           
            //prints the list and the hive, and gets the next input
            printf("\n");
            printList(userWordList, hive);
            printf("............................................\n");
            printHive(hive, reqLetInd);

            printf("    Round %d: Words must start with '%c'\n", roundNum, currStartLetter);
            printf("  Enter a word (enter DONE to quit): ");
            scanf("%s", userWord);
            printf("\n");


        }

        // frees all memory of the solution list, user list, and user word
        freeWordList(solvedList);
        freeWordList(userWordList);
        free(userWord);

    //---------------------------------------------------------------------    
    //                 END OF OPEN-ENDED GAMEPLAY SECTION
    //---------------------------------------------------------------------
    }
    

    printf("==== SPELLING BEE SOLVER ====\n");

    printf("  Valid words from hive \"%s\":\n", hive);
    printf("                         ");
    for (int i = 0; i < reqLetInd; i++) {
        printf(" ");
    }
    printf("^\n");

    // creates the solution word list
    WordList* solvedList = createWordList();

    if (bruteForce) { // find all words that work... (1) brute force
        bruteForceSolve(dictionaryList, solvedList, hive, reqLet);
    }
    else { // finds all words that work recursively

        char* tryWord = (char*)malloc(sizeof(char) * (maxWordLength + 1)); // allocates memory for word + null char

        tryWord[0] = hive[0]; // appends first letter of hive
        tryWord[1] = '\0';    // adds null terminating character
        findAllMatches(dictionaryList, solvedList, tryWord, hive, reqLet); // returns list of all fit words
        free(tryWord);        // frees all memory used by tryWord

    }

    
    int longestSolvedWordLen = 0; // gets the longest possible solved word length

    // compares each word in solvedList
    for (int i = 0; i < solvedList->numWords; i++) {
        // checks if word is longer than previous
        if (strlen(solvedList->words[i]) > longestSolvedWordLen) {
            longestSolvedWordLen = strlen(solvedList->words[i]); // sets new max length for word
        }
    }

    // Helpful variables
    int numValidWords = solvedList->numWords;
    int numPangrams = 0;
    int numPerfectPangrams = 0;
    int totScore = 0;
    int score = 0;
    bool isBingo = true;

    int matchingBingo = 0; // stores number of bingos (at least one for each letter)

    // compares each letter in hive to letter in solvedList word
    for (int i = 0; i < strlen(hive); i++) {
        for (int j = 0; j < solvedList->numWords; j++) {
            // checks if words starts with hive letter
            if (hive[i] == solvedList->words[j][0]) { 
                matchingBingo++; // increments count of bingos
                break;
            }

        }
    }

    // checks if there is not a word beginning with each hive letter
    if (matchingBingo != strlen(hive)) {
        isBingo = false; // there is no bingo
    }
    
    // displays results
    for (int i = 0; i < solvedList->numWords; i++) {
        // checks if a word is a perfect pangram
        if (isPangram(solvedList->words[i], hive) && strlen(solvedList->words[i]) == strlen(hive)) {
            printf("  *** "); 
            score = strlen(hive) * 2; // doubles score
            totScore+=score;          // increments total score by word score
            numPerfectPangrams++;     // increments number of perfect pangrams
            numPangrams++;            // increments number of pangrams         
        }
        // checks if a word is a pangram
        else if (isPangram(solvedList->words[i], hive)) {
            printf("   *  ");
            score = strlen(hive) + strlen(solvedList->words[i]); // sets hiveSize points and point for each letter
            totScore+=score; // increments total score by word score
            numPangrams++;   // increments number of pangrams
        }
        // checks if word is minimum length
        if (strlen(solvedList->words[i]) == MIN_WORD_LENGTH) {
            score = 1;       // assigns only 1 poiny
            totScore+=score; // incrments total score by word score
            printf("      ");
        }
        // checks for regular words
        else if (isPangram(solvedList->words[i], hive) == false) {
            score = strlen(solvedList->words[i]); // assigns word length points
            totScore+=score;                      // increments total score by word score
            printf("      ");
        }
        if (score < 10){ // displays results for scores under 10
            printf("( %d) %s\n", score, solvedList->words[i]);
        }
        else { // displays all other results
            printf("(%d) %s\n", score, solvedList->words[i]);
        }
        
        
    }
    
    // Additional results are printed here:
    printf("\n");
    printf("  Total counts for hive \"%s\":\n", hive);
    printf("                         ");
    for (int i = 0; i < reqLetInd; i++) {
        printf(" ");
    }
    printf("^\n");
    printf("    Number of Valid Words: %d\n", numValidWords);
    printf("    Number of ( * ) Pangrams: %d\n", numPangrams);
    printf("    Number of (***) Perfect Pangrams: %d\n", numPerfectPangrams);
    printf("    Bingo: ");
    printYESorNO(isBingo);
    printf("    Total Score Possible: %d\n", totScore);

    freeWordList(dictionaryList); // frees all dictionary memory
    freeWordList(solvedList); // frees all solvedList memory
    printf("\n\n");
    return 0;
}