#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// Function prototypes at the top of the file for function decomposition

/**
 * Print instructions for using the program.
 */
void printMenu();
/**
 * Returns the 0-based index in the English alphabet where `c` appears,
 * or -1 if `c` is not an uppercase letter in the alphabet.
 *
 * For example:
 *  - `findIndexInAlphabet('A')` returns 0
 *  - `findIndexInAlphabet('D')` returns 3
 *  - `findIndexInAlphabet('+')` returns -1
 *  - `findIndexInAlphabet('a')` returns -1
 */
int findIndexInAlphabet(char c);
/**
 * Returns `c` rotated by `amount` many characters forward. If we run out
 * of letters in the alphabet, wrap around back to 'A'. This method
 * assumes that `c` is an uppercase letter.
 * For example:
 *  - `rot('A', 0)` returns 'A'
 *  - `rot('A', 1)` returns 'B'
 *  - `rot('Z', 1)` returns 'A'
 *  - `rot('A', 10)` returns 'K'
 *  - `rot('J', 25)` returns 'I'
 */
char rot(char c, int amount);
/**
 * Returns a new string in which every character has been rotated by `amount`
 * letters. Lowercase letters are converted to uppercase and rotated.
 * Non-alphabetic characters are left alone.
 * For example:
 *  `rot("A", 0)` returns "A"
 *  `rot("AA", 3)` returns "DD"
 *  `rot("HELLO", 0)` returns "HELLO"
 *  `rot("HELLO", 1)` returns "IFMMP"
 *  `rot("oh HEL-LO!", 1)` returns "PI IFM-MP!"
 */
string rot(string line, int amount);

/*
* Binary search 
* Scans the dictionary and returns the index of a word, if found
* Returns -1 if nothing is found
*/
int search(const vector<string>& dict, const string& word, int low, int high);

int main() {
    string command;

    cout << "Welcome to Ciphers!" << endl;
    cout << "-------------------" << endl;
    cout << endl;

    do {
        printMenu();
        cout << endl
             << "Enter a command (case does not matter): ";

        // Use getline for all user input to avoid needing to handle
        // input buffer issues relating to using both >> and getline
        getline(cin, command);
        cout << endl;

        // TODO_STUDENT: Execute non-exit commands
        if (command == "C" || command == "c") {
            cout << "Enter the text to encrypt: ";

            string phrase;
            getline(cin, phrase);

            cout << "\nEnter the number of characters to rotate by: ";
            string holdShiftVariable;
            getline(cin, holdShiftVariable);
            int shift = stoi(holdShiftVariable);

            string encrypted = rot(phrase, shift);

            cout << encrypted << endl;
        }
        else if (command == "D" || command == "d") {
            fstream dict;
            dict.open("dictionary.txt");

            if (!dict.is_open()) {
                cout << "**FAILED TO OPEN FILE" << endl;
                continue;
            }

            string encrypted;
            string copy;
            cout << "Enter the text to Caesar-cipher decrypt:" << endl;
            getline(cin, encrypted);
            copy = encrypted;

            //add a space to make my loop work ;)
            encrypted.push_back(' ');

            //convert this encrypted string into a vector of words
            vector<string> encrVec; 

            while (encrypted.size() != 0) {
                int spacePos = encrypted.find(' ');
                //parse by whitespace, add it to the vector
                if (encrypted.substr(0, spacePos) != "") {
                    encrVec.push_back(encrypted.substr(0, spacePos));
                }
                //remove the word parsed, including the whitespace
                encrypted.erase(encrypted.begin(), encrypted.begin() + spacePos + 1);
            }
            encrypted = copy;

            //capitalize and remove non-alphabetical characters from each word in the vector
            for (auto& word : encrVec) {
                for (int j = 0; j < word.length(); j++) {

                    word[j] = toupper(word[j]);

                    if (!isalpha(word[j])) {
                        word.erase(word.begin() + j);
                        j--;
                    }
                }
            }

            //put the dictionary in a vector so we can compare our vector to it 
            vector<string> dictionary;
            while (!dict.eof()) {
                string word; 
                getline(dict, word);
                dictionary.push_back(word);
            }

            //The meat of the decryption begins here
            //Rotate the words each possible way until we find a valid solution
            for (int i = 26; i > 0; i--) {
                //populate new vector to hold rotated strings
                vector<string> rotated;
                for (auto word : encrVec) {
                    rotated.push_back(rot(word, 26 - i));
                }

                //search for the word in the dictionary
                int numWordsFound = 0;
                vector<string> goodWords;
                for (auto word : rotated) {
                    
                    int foundDex = search(dictionary, word, 0, dictionary.size() - 1);

                    if (foundDex > 0) {
                        numWordsFound++;
                    }

                    goodWords.push_back(word);
                }
                //if the percent numwordsfound is greater than 50%
                if ((double)numWordsFound / (double)encrVec.size() > 1.0/2.0) {
                    string rawletters = "";
                    //add the good words to an unformatted string so we can replace chars in encrypted one by one
                    for (int j = 0; j < goodWords.size(); j++) {
                        rawletters += goodWords.at(j);
                    }
                    
                    //replace the characters one by one, ignoring non-alphabetic chars
                    int rawcounter = 0;
                    for (int j = 0; j < encrypted.length(); j++) {
                        if (isalpha(encrypted[j])) {
                            encrypted[j] = rawletters[rawcounter];
                            rawcounter++;
                        }
                    }
                    cout << encrypted << endl;
                }
            }
            
            dict.close();
        }
        else if (command == "V" || command == "v") {
            cout << "Enter text to encrypt:" << endl;

            string phrase;
            getline(cin, phrase);

            cout << "Enter the Vigenère key:" << endl;
            string vKey;
            getline(cin, vKey);

            string vigEncrypted;

            int keyIndex = 0;

            for (auto letter : phrase) {
                //find letter in key we will use to offset
                char keylet = vKey[keyIndex%vKey.length()];
                //if the keylet is not alphabetical, move forward until we have on that is
                while (!isalpha(keylet)) {
                    keyIndex++;
                    keylet = vKey[keyIndex%vKey.length()];
                }

                //determines how much we will offset the letter by
                int offset = findIndexInAlphabet(toupper(keylet));

                //append the letter to the encrypted string
                vigEncrypted += rot(letter, offset);

                //only increment the index if it's alphabetical (rot ignores nonalphabeticals and returns the char as is)
                if (isalpha(letter)) {
                    keyIndex++;
                }
                
            }

            cout << vigEncrypted << endl;
        }

        cout << endl;

    } while (!(command == "x" || command == "X"));

    return 0;
}

void printMenu() {
    cout << "Ciphers Menu" << endl;
    cout << "------------" << endl;
    // Caesar Ciphers
    cout << "C - Encrypt with Caesar Cipher" << endl;
    cout << "D - Decrypt Caesar Cipher" << endl;
    // Vigenere
    cout << "V - Encrypt with Vigenère" << endl;
    // Utility
    cout << "X - Exit Program" << endl;
}

int findIndexInAlphabet(char c) {

    int index = ALPHABET.find(c); 

    //return -1 if the char is not in ALPHABET
    if (index == string::npos) {
        return -1;
    }

    //otherwise return the index
    return index; 
}

char rot(char c, int amount) {
    //uppercase required
    c = toupper(c);

    //we want the amount to be in between 0-25
    //functionally the rotation is no different if we alter it by 26
    while (amount > 26) {
        amount -= 26;
    }
   
   if (amount < 0 || !isalpha(c)) {
        return c;
   }

    //A int rep is 65
    //Z int rep is 90
    c += amount; //this will offset the character
    
    //if we're above 90, subtract by 26 to retrieve the correct char
    if (c > 90) {
        c -= 26;
    }
    //if we're below 65 (if the offset is negative) add by 26 to retrieve the correct char
    else if (c < 65) {
        c += 26;
    }

    return c;
}

string rot(string line, int amount) {
    
    for (int i = 0; i < line.length(); i++) { //loop through string

        //only change the character if its alphabetical 
        if (isalpha(line[i])) {
            line[i] = rot(toupper(line[i]), amount);
        }
    }

    return line;
}

int search(const vector<string>& dict, const string& word, int low, int high) {

    //reusing this code from a CS211 project. I think it looks nice and it works wonders :)

    int middle = (high + low) / 2;

    int difference = word.compare(dict.at(middle));

    if (!difference) {
        return middle;
    }
    else if (high == low || low > high) {
        return -1;
    }
    else if (difference < 0) {
        return search(dict, word, low, middle - 1);
    }
    else if (difference > 0) {
        return search(dict, word, middle + 1, high);
    }
}
