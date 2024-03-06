// Kyle Moore
// March 4, 2024
// CIS 4634-003 - Applied Cryptography & Security
// Data Encryption Standard DES

// Libraries
#include <iostream>   // include iostream library to use cout (output) and cin (input) functions
#include <string>     // include string library to use string data type
#include <bitset>     // include bitset library to use bitset data type
#include <vector>     // include vector library to use vector data type

using namespace std;     // using namespace std to avoid writing std:: before every cout, cin, string, etc.

// DES Initial Permutation (IP) Table
    // Used at beginning of the DES encryption process to permute the original plaintext block
int IP[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

// DES Permuted Choice 1 (PC-1) Table
    // Used in key generation process. It selects 56 bits (following the fixed order defined by the table) from the original 64 bit-key, dropping 8 bits (usually parity bits).
        // The parity bits are used in the original key. They are NOT used for (en/de)cryption, but are used to check for errors in the key when keys are transmitted or stored
int IP[64] = {
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36, 
    63, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29,
    21, 13, 5, 28, 20, 12, 4
};

// DES Permuted Choice 2 (PC-2) Table
    // Used in key generation process. It selects 48 bits from the 56-bit key (post-PC-1 processing), following the fixed order defined by the table.
        // These 48 bits form the round key for each of the 16 (en/de)cryption rounds (each round yields a 48-bit subkey)
int IP[64] = {
    14, 17, 11, 24, 1, 5,
    3, 28, 15, 6, 21, 10,
    23, 19, 12, 4, 26, 8,
    16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32 
};

// Placeholder for the encrypt function
string encrypt(const string& plaintext, const string& key) {
    // TODO: Implement the encryption algorithm here
    return "encrypted_text";
}

// Placeholder for the decrypt function
string decrypt(const string& ciphertext, const string& key) {
    // TODO: Implement the decryption algorithm here
    return "decrypted_text";
}





// main function
int main() {
    string plaintext, key;     // declarations to store plaintext & key
    
    // Get plaintext from the user
    cout << "Enter the plaintext: ";
    getline(cin, plaintext);     //NOTE: getline function is used as it ensures the entire line, including spaces (unlike cin, which stops reading at the first whitespace), is read into 'plaintext' and 'key' variables.
    
    // Get the secret key from the user
    cout << "Enter the secret key: ";
    getline(cin, key);
    
    // Encrypt the plaintext
    string ciphertext = encrypt(plaintext, key);     // attempt to encrypt the plaintext & show result of encryption process
    cout << "Ciphertext: " << ciphertext << endl;
    
    // Decrypt the ciphertext
    string decryptedText = decrypt(ciphertext, key);     // attempt to recover the original plaintext & show result of decryption process
    cout << "Decrypted text: " << decryptedText << endl;

    return 0;     // Successful termination of the program
}
