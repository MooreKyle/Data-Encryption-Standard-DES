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

// Global declaration for left shift schedule
int leftShifts[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

// DES Initial Permutation (IP) Table
    // Used at beginning of the DES encryption process to permute the original plaintext block
        // Initial permutation's process helps faciliate the diffusion process by preparing the data through mixing it before the rounds of encryption start
            // 1. The 64-bit plaintext block is permuted according to the fixed order defined by the table
            // 2. The permuted block is then divided into two halves, a left half (L0) and a right half (R0), each 32 bits long
            // 3. The two halves are then processed through 16 rounds of (en/de)cryption
            // 4. The final output of the 16th round is then permuted using the Inverse Initial Permutation (IP^-1) Table to produce the 64-bit ciphertext block
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
int PC1[56] = {
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
int PC2[48] = {
    14, 17, 11, 24, 1, 5,
    3, 28, 15, 6, 21, 10,
    23, 19, 12, 4, 26, 8,
    16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32 
};

// Function to convert a string to a bitset<64>
    // Necessary because the DES algorithm operates on 64-bit blocks of data (binary data)
bitset<64> stringToBitset64(const string& str) {
    bitset<64> bits;
    for(size_t i = 0; i < str.size(); ++i) {
        for(size_t j = 0; j < 8 && i * 8 + j < 64; ++j) {
            bits[i * 8 + j] = (str[i] >> j) & 1;
        }
    }
    return bits;
}

// Function to apply the Initial Permutation (IP) to a bitset<64>
    // Reorders the 64-bit block according to the DES standard's IP table, setting up the data for subsequent encryption rounds
bitset<64> applyInitialPermutation(const bitset<64>& input) {
    bitset<64> output;
    for(int i = 0; i < 64; ++i) {
        output[63-i] = input[IP[i]-1];
    }
    return output;
}

// Function that generates and returns the 16 round keys required for DES encryption.
vector<bitset<48>> generateRoundKeys(const bitset<64>& key) {
    vector<bitset<48>> roundKeys(16);     // Vector to hold the 16 generated round keys
    bitset<56> key56;   // Bitset to hold the 56-bit key  
    // Apply PC-1 to get a 56-bit key from the original 64-bit key
    for(int i = 0; i < 56; ++i) {
        key56[55-i] = key[64-PC1[i]];
    }

    // Split the 56-bit key into two halves
    bitset<28> left = bitset<28>((key56.to_ullong() >> 28) & 0x0FFFFFFF);
    bitset<28> right = bitset<28>(key56.to_ullong() & 0x0FFFFFFF);

    // Generate the 16 round keys
    for(int round = 0; round < 16; ++round) {
        // Left shifting the halves
        left = bitset<28>((left.to_ulong() << leftShifts[round]) | (left.to_ulong() >> (28 - leftShifts[round])));
        right = bitset<28>((right.to_ulong() << leftShifts[round]) | (right.to_ulong() >> (28 - leftShifts[round])));

        // Combine the halves and apply PC-2
        bitset<56> combined = (bitset<56>(left.to_ulong()) << 28) | bitset<56>(right.to_ulong());
        for(int i = 0; i < 48; ++i) {
            roundKeys[round][47-i] = combined[56-PC2[i]];
        }
    }

    return roundKeys;
}

// Placeholder for the encrypt function
string encrypt(const string& plaintext, const string& key) {
    //Convert the key to bitset<64>
        // Necessary to change key from string format to binary format (bitset), as DES operates on binary data
    bitset<64> keyBits = stringToBitset64(key);

    // Generate the 16 round keys
        // Prepare the 16 round keys that will be used, one for each of the 16 rounds of DES encryption
            // Will be used to apply PC-1, rotation, and PC-2
    vector<bitset<48>> roundKeys = generateRoundKeys(keyBits); // Generate round keys
    
    bitset<64> plaintextBits = stringToBitset64(plaintext); // Convert plaintext to bitset
    bitset<64> permutedPlaintext = applyInitialPermutation(plaintextBits); // Apply IP to plaintext bitset

    // Placeholder for DES rounds process

    // Placeholder to apply final permutation to encryptedData
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

    // Convert key string from user to bitset<64>
    bitset<64> keyBits = stringToBitset64(key);

    // Convert plaintext string from user to bitset<64>
    bitset<64> plaintextBits = stringToBitset64(plaintext);

    // Apply Initial Permutation to plaintext bitset
    bitset<64> permutedPlaintext = applyInitialPermutation(plaintextBits);

    // Encrypt the plaintext
    string ciphertext = encrypt(plaintext, key);     // attempt to encrypt the plaintext & show result of encryption process
    cout << "Ciphertext: " << ciphertext << endl;

    // ------------------------------------------------------------------------------------------------------------------------------
    
    // Decrypt the ciphertext
    string decryptedText = decrypt(ciphertext, key);     // attempt to recover the original plaintext & show result of decryption process
    cout << "Decrypted text: " << decryptedText << endl;

    return 0;     // Successful termination of the program
}
