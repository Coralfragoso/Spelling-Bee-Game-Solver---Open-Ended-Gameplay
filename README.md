# Spelling-Bee-Game-Solver---Open-Ended-Gameplay
Spelling Bee game and solver with hive letters (2-12), required letter, and word validation. Features interactive gameplay, brute-force and optimized solvers using recursive binary search. Implements WordList dynamic array with capacity doubling. Includes pangram detection, scoring, and performance analysis via gprof. 

Features

    Flexible hive building: User-input or random hive generation (guaranteed pangram)
    
    Command-line arguments:
  
      -r <size>: Random mode with specified hive size
      
      -d <file>: Specify dictionary file (default: dictionary.txt)
      
      -p: Play mode ON (interactive gameplay)
      
      -o: Use optimized solver (binary search) instead of brute force

    WordList dynamic array: Doubles capacity when full, stores C-string pointers
    
    Gameplay mechanics:

      Words must use only hive letters and include required letter
      
      Minimum word length: 4 characters
      
      Scoring: 1 point per letter, pangrams get hiveSize bonus points
      
      Perfect pangrams: use each hive letter exactly once
      
    Two solver approaches:
      
      Brute force: Linear scan through dictionary
      
      Optimized: Recursive binary search with prefix checking
      
    Performance analysis: gprof timing comparison between solvers

Implementation

    appendWord(): Dynamic array growth with capacity doubling
    
    countUniqueLetters(): Helper for pangram detection
    
    isValidWord(): Validates words against hive and dictionary
    
    findAllMatches(): Core optimized solver algorithm
    
    freeWordList(): Comprehensive memory cleanup

Sample Output Features

    Word list display with pangram markers (* and ***)
    
    Total score calculation
    
    Bingo detection (words starting with each hive letter)
    

Requirements

    No realloc() - manual reallocation only
    
    WordList struct with dynamic array of C-string pointers
    
    Proper memory management (no leaks)
    
    Extended makefile with custom targets
