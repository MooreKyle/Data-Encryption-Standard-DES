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

// Expansion function E expands the 32-bit half block to 48 bits according to the E-table.
bitset<48> expansionFunction(const bitset<32>& rightHalf) {
    bitset<48> expandedBlock;
    int E_TABLE[48] = {
        32, 1, 2, 3, 4, 5,
        4, 5, 6, 7, 8, 9,
        8, 9, 10, 11, 12, 13,
        12, 13, 14, 15, 16, 17,
        16, 17, 18, 19, 20, 21,
        20, 21, 22, 23, 24, 25,
        24, 25, 26, 27, 28, 29,
        28, 29, 30, 31, 32, 1
    };

    for(int i = 0; i < 48; ++i) {
        expandedBlock[i] = rightHalf[E_TABLE[i] - 1];
    }

    return expandedBlock;
}

// Substitution boxes (S-boxes) defined as per DES standard
    // Array indicates the S-boxes used in the DES algorithm, each of which takes a 6-bit input and produces a 4-bit output
    // The dimensions of the array mean 8 S-Boxes. The 6-bit input's first and last bits determine which of the '4' rows in the S-Box to use. '16' represents the 16-columns, chosen based on the middle 4 bits of the input.
        // The S-boxes are used to introduce non-linearity into the encryption process, which is a crucial part of the DES algorithm's security
const int S_BOX1[8][4][16] = {
    {
        // S1
        {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
        {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
        {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
        {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
    }
};

const int S_BOX2[8][4][16] = {
    {
        // S2
        {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
        {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
        {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
        {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
    }
};

const int S_BOX3[8][4][16] = {
    {
        // S3
        {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
        {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
        {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
        {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
    }
};

const int S_BOX4[8][4][16] = {
    {
        // S4
        {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
        {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
        {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
        {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
    }
};

const int S_BOX5[8][4][16] = {
    {
        // S5
        {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
        {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
        {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
        {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
    }
};

const int S_BOX6[8][4][16] = {
    {
        // S6
        {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
        {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
        {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
        {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
    }
};

const int S_BOX7[8][4][16] = {
    {
        // S7
        {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
        {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
        {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
        {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
    }
};

const int S_BOX8[8][4][16] = {
    {
        // S8
        {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
        {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
        {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
        {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
    }
};

// Substitution function that applies the S-boxes to a 48-bit input, producing a 32-bit output
bitset<32> substitutionFunction(const bitset<48>& rightHalfXORedWithKey) {
    bitset<32> substituted;
    const int (*S_BOXES[8])[4][16] = {S_BOX1, S_BOX2, S_BOX3, S_BOX4, S_BOX5, S_BOX6, S_BOX7, S_BOX8};

    for(int i = 0; i < 8; ++i) {
        int section = (rightHalfXORedWithKey >> (42 - 6 * i)).to_ulong() & 0x3F; // Extract 6 bits
        int row = (section & 0x20) >> 4 | (section & 0x01); // First and last bits for row
        int col = (section >> 1) & 0x0F; // Middle 4 bits for column
        int sBoxValue = (*S_BOXES[i])[row][col];
        
        // Place the 4-bit S-box value in the output
        for(int j = 0; j < 4; ++j) {
            substituted[31 - i * 4 - j] = (sBoxValue >> (3 - j)) & 1;
        }
    }

    return substituted;
}

// DES Permutation (P) Table - used after the S-box substitution
int P_TABLE[32] = {
    16, 7, 20, 21,
    29, 12, 28, 17,
    1, 15, 23, 26,
    5, 18, 31, 10,
    2, 8, 24, 14,
    32, 27, 3, 9,
    19, 13, 30, 6,
    22, 11, 4, 25,
};

// Permutation function that applies a permutation to the 32-bit output of the substitution step
    // Rearranges the bits after substitution to directly create diffusion in the data
        // The permutation function is a crucial part of the DES algorithm, as it ensures that the data is thoroughly mixed before the next round of encryption begins
bitset<32> permutationFunction(const bitset<32>& substitutedHalf) {
    bitset<32> permuted;
    for(int i = 0; i < 32; ++i) {
        permuted[31 - i] = substitutedHalf[P_TABLE[i] - 1];
    }
    return permuted;
}

// Function to combine the left and right halves after the final round, before final permutation
bitset<64> combineLeftAndRight(const bitset<32>& left, const bitset<32>& right) {
    bitset<64> combined;

    // Combine the left half
    for (int i = 0; i < 32; ++i) {
        combined[63 - i] = left[31 - i];
    }

    // Combine the right half
    for (int i = 0; i < 32; ++i) {
        combined[31 - i] = right[31 - i];
    }

    return combined;
}

// DES Final Permutation (FP) (or IP^-1) Table - the inverse of the Initial Permutation
int FP[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25
};

// Final permutation function that applies the final permutation to the data after 16 rounds
    // Takes the 64-bit combinedData from combining the left and right halves, and applies the Final Permutation (FP) (or IP^-1) to reorder the bits for the final ciphertext output
        // The final permutation is the inverse of the initial permutation, ensuring that the structure of the output block is rearranged according to the DES standard
bitset<64> finalPermutationFunction(const bitset<64>& combinedData) {
    bitset<64> finalPermuted;
    for (int i = 0; i < 64; ++i) {
        finalPermuted[63 - i] = combinedData[FP[i] - 1];
    }
    return finalPermuted;
}

// Utility function to convert bitset<64> to hex string for output
string bitsetToHex(const bitset<64>& bs) {
    stringstream ss;
    ss << hex << uppercase << bs.to_ullong();
    return ss.str();
}

/* encrypt function implements the entire DES encryption algorithm workflow
       1. Takes plaintext and key as input
       2. Converts the plaintext and key from strings to bitset<64> (binary format)
       3. Generates the 16 round keys required for DES encryption
           3.1 Apply PC-1 to reduce the 64-bit secret key to a 56-bit key
           3.2 Split the 56-bit key into two halves
           3.3 Perform a series of left shifts (rotations) on the two halves, specific to each round.
           3.4 Apply PC-2 to each combination of the two left-shifted left and right halves to produce a 48-bit round key
           3.5 Repeat the process to generate 16 total round keys */
string encrypt(const string& plaintext, const string& key) {
    //Convert the key to bitset<64>
        // Necessary to change key from string format to binary format (bitset), as DES operates on binary data
    bitset<64> keyBits = stringToBitset64(key);

    // Generate the 16 round keys
        // Prepare the 16 round keys that will be used, one for each of the 16 rounds of DES encryption
            // Will be used to apply PC-1, rotation, and PC-2
    vector<bitset<48>> roundKeys = generateRoundKeys(keyBits); // Generate round keys
    
    // Convert plaintext to bitset and apply initial permutation
        // Important for the diffusion process, setting the stage for the encryption rounds
            // Ensures the data is mixed throughly before encryption begins
    bitset<64> plaintextBits = stringToBitset64(plaintext); // Convert plaintext to bitset
    bitset<64> permutedPlaintext = applyInitialPermutation(plaintextBits); // Apply IP to plaintext bitset

    // The heart/core of DES encryption process
        // Iterates through 16 rounds of processing. Each round includes expanding the right half of the data, XORing with a round-specific key, applying substitution boxes to introduce non-linearity, and a final permutation to mix the data thoroughly.
        // The swapping of the left and right halves at each round's end ensures that the encryption process is both secure and complex.
    bitset<32> left((permutedPlaintext >> 32).to_ulong()), right((permutedPlaintext & bitset<64>(0xFFFFFFFF)).to_ulong());
    bitset<32> expandedRight, xorWithKey, substituted, permuted;

    for (int round = 0; round < 16; ++round) {
        // Expand the right half from 32 to 48 bits to prepare for XOR with the round key
        expandedRight = expansionFunction(right);
        // XORs the expanded right half with the current round key to mix in the key's data
        xorWithKey = expandedRight ^ roundKeys[round];
        // Apply the S-boxes to the result of the XOR operation, reducing it from 48 to 32 bits while introducing non-linearity (confusion)
        substituted = substitutionFunction(xorWithKey);
        // Permutates the substituted bits to further scramble the data (diffusion)
        permuted = permutationFunction(substituted);

        // Saves the current right half for swapping: This step prepares for the next round by temporarily storing the current right half
            // In the DES encryption process, swapping the left and right halves at the end of each round ensures that the encryption is thoroughly mixed, contributing to the security and complexity of the final ciphertext. 
            // *****This swapping mechanism is fundamental to the Feistel structure of DES, which ensures that each round's operations can't be easily reversed without the correct key.*****
                // XOR the permuted bits with the left half to mix in the data from the right half
        bitset<32> temp = right;
        // Right half results from XOR of left half with the permuted bits, which blends the halves together
            // Operation mixes the two halves together, using the results of the substitution and permutation steps, which introduced confusion and diffusion respectively, into the data
        right = left ^ permuted;
        // Updates the left half to the previous round's stored right half (completes the swap)
            // Crucial for the Feistel structure: allows each round to build on the complexity introduced in previous rounds, enhancing security of the encryption process
        left = temp; // Swap for next round
    }

    // Apply final permutation to the combined block to produce the ciphertext
        // Combine left and right halves to prepare for final permutation (after all rounds)
    bitset<64> combined = combineLeftAndRight(right, left);     // After the final (16th) round, an additional swap is performed before the final permutation
    // Final permutation is applied to the combined left and right halves of the 16th round to produce the 64-bit ciphertext block
        // This step is the inverse of the initial permutation performed at the beginning of the encryption process, ensuring that the structure of the output block is rearranged according to the DES standard.
            // The result of this permutation is the ciphertext, which is now ready to be transmitted or stored securely.
    bitset<64> finalPermutation = finalPermutationFunction(combined);

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
