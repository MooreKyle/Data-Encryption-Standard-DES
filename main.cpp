// Kyle Moore
// March 4, 2024
// CIS 4634-003 - Applied Cryptography & Security
// Data Encryption Standard DES

// Libraries
#include <iostream>   // include iostream library to use cout (output) and cin (input) functions
#include <string>     // include string library to use string data type

using namespace std;     // using namespace std to avoid writing std:: before every cout, cin, string, etc.

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
    string ciphertext = encrypt(plaintext, key);     //
    cout << "Ciphertext: " << ciphertext << endl;
    
    // Decrypt the ciphertext
    string decryptedText = decrypt(ciphertext, key);     // attempt to recover the original plaintext & show result of decryption process
    cout << "Decrypted text: " << decryptedText << endl;

    return 0;     // Successful termination of the program
}
