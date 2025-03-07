/*
Detailed Description:
This program processes astronomical sightings by reading speed and brightness data from a file,
calculates a signature for each sighting using the formula:
    signature = ceil((speed * brightness) / 10)
It then reads a list of signature values from another file and allows the user to choose a search method 
(linear or binary) to check if each signature from the file exists among the calculated signatures.
If binary search is selected, the program sorts the sightings based on their computed signature.
The program counts the number of matching signatures and writes the result to an output file,
while also printing CPU time measurements for various operations.
*/

#include <iostream>      // Include I/O stream library for console I/O.
#include <fstream>       // Include file stream library for file I/O.
#include <vector>        // Include vector container to store dynamic arrays.
#include <string>        // Include string library for using std::string.
#include <sstream>       // Include string stream library for parsing strings.
#include <stdexcept>     // Include standard exception support for error handling.
#include <chrono>        // Include chrono library for high-resolution clock (timing).
#include <algorithm>     // Include algorithm library for std::sort.
#include <cmath>         // Include math library for std::ceil.

// Class representing a single sighting record with speed and brightness.
class Sighting {
public:
    int speed;       // Holds the speed value of the sighting.
    int brightness;  // Holds the brightness value of the sighting.

    // Constructor: Initializes a Sighting object with given speed and brightness.
    Sighting(int speed, int brightness)
        : speed(speed),          // Initialize member 'speed' with parameter 'speed'.
          brightness(brightness) // Initialize member 'brightness' with parameter 'brightness'.
    {}

    // Overload the less-than operator to allow sorting.
    // This operator compares sightings first by brightness, then by speed if brightness is equal.
    bool operator<(const Sighting &other) const {
        if (brightness != other.brightness) {        // If brightness values differ,
            return brightness < other.brightness;      // compare based on brightness.
        }
        return speed < other.speed;                    // If brightness is the same, compare based on speed.
    }
};

// Timer class for measuring elapsed CPU time in microseconds.
class Timer {
private:
    // Stores the time point when the timer was last reset.
    std::chrono::high_resolution_clock::time_point startTime;

public:
    // Resets the timer by capturing the current high-resolution time.
    void Reset() {
        startTime = std::chrono::high_resolution_clock::now(); // Record current time as startTime.
    }

    // Returns the elapsed time in microseconds since the timer was last reset.
    double ElapsedMicroseconds() const {
        auto endTime = std::chrono::high_resolution_clock::now(); // Capture the current time as endTime.
        // Calculate the difference in time and convert it to microseconds.
        return std::chrono::duration<double, std::micro>(endTime - startTime).count();
    }

    // Overload the stream insertion operator to allow easy printing of the timer.
    friend std::ostream &operator<<(std::ostream &os, const Timer &timer) {
        // Print the elapsed CPU time in microseconds.
        os << "CPU time: " << timer.ElapsedMicroseconds() << " microseconds";
        return os; // Return the output stream.
    }
};

// Template function to read a file line-by-line and parse each line into an object of type T.
// The 'Parser' function is used to convert a string (line) into an object of type T.
template <typename T, typename Parser>
std::vector<T> readFile(const std::string &filename, Parser parser) {
    std::ifstream file(filename);            // Open the file with the given filename.
    if (!file.is_open()) {                   // Check if the file opened successfully.
        // Throw an exception if the file cannot be opened.
        throw std::runtime_error("Error: cannot open file " + filename);
    }

    std::vector<T> container;                // Create a vector to hold the parsed objects.
    std::string line;                        // Temporary string to hold each line from the file.
    // Read the file line by line.
    while (std::getline(file, line)) {
        container.push_back(parser(line));   // Parse the line into an object of type T and add it to the vector.
    }

    return container;                        // Return the vector containing all parsed objects.
}

// Function to calculate signature values from a vector of (speed, brightness) pairs.
// Each signature is calculated as the ceiling of (speed * brightness) divided by 10.
std::vector<int> calcSig(const std::vector<std::pair<int, int>> &data) {
    std::vector<int> signature;              // Vector to store calculated signatures.
    // Iterate through each pair in the input data.
    for (const auto &pairs : data) {
        // Calculate the signature using the formula: ceil((speed * brightness) / 10.0)
        int sig = static_cast<int>(std::ceil(static_cast<double>(pairs.first * pairs.second) / 10.0));
        signature.push_back(sig);          // Append the calculated signature to the vector.
    }
    return signature;                      // Return the vector containing all signature values.
}

// Function to parse a single line of text into a Sighting object.
// Expects the line to contain two integers: first for speed and second for brightness.
Sighting parseSighting(const std::string &line) {
    std::istringstream iss(line);           // Create an input string stream from the line.
    int speed, brightness;                  // Declare variables for speed and brightness.
    // Attempt to extract two integers from the stream.
    if (!(iss >> speed >> brightness)) {
        // If extraction fails, throw an exception indicating an invalid file format.
        throw std::runtime_error("Invalid format in sighting file");
    }
    return Sighting(speed, brightness);     // Return a new Sighting object initialized with the extracted values.
}

// Function to parse a single line of text into an integer signature.
// Expects the line to contain a single integer.
int parseSignature(const std::string &line) {
    std::istringstream iss(line);           // Create an input string stream from the line.
    int signature;                          // Declare a variable to store the signature.
    // Attempt to extract an integer from the stream.
    if (!(iss >> signature)) {
        // If extraction fails, throw an exception indicating an invalid file format.
        throw std::runtime_error("Invalid format in signature file");
    }
    return signature;                       // Return the extracted signature.
}

// Function to perform a linear search on the vector of sightings.
// It calculates the signature for each sighting and checks if it matches the target signature.
bool linearSearch(const std::vector<Sighting> &sightings, int signature) {
    // Iterate over each Sighting object in the vector.
    for (const auto &s : sightings) {
        // Compute the signature for the current sighting using the formula.
        int calculatedSignature = static_cast<int>(std::ceil(static_cast<double>(s.speed * s.brightness) / 10.0));
        // Check if the computed signature equals the target signature.
        if (calculatedSignature == signature) {
            return true; // Return true if a matching signature is found.
        }
    }
    return false; // Return false if no match is found.
}

// Function to perform a binary search on a sorted vector of sightings.
// Assumes that the sightings vector is already sorted based on the computed signature.
bool binarySearch(const std::vector<Sighting> &sightings, int signature) {
    int left = 0;                          // Set the left boundary of the search to the start of the vector.
    int right = sightings.size() - 1;        // Set the right boundary to the last index in the vector.
    // Continue searching while the left index does not exceed the right index.
    while (left <= right) {
        // Calculate the middle index to avoid potential overflow.
        int mid = left + (right - left) / 2;
        // Compute the signature for the sighting at the middle index.
        int calculatedSignature = static_cast<int>(std::ceil(static_cast<double>(sightings[mid].speed * sightings[mid].brightness) / 10.0));
        // Check if the computed signature matches the target signature.
        if (calculatedSignature == signature) {
            return true;  // Match found; return true.
        } else if (calculatedSignature < signature) {
            left = mid + 1;  // If the computed signature is less than the target, search in the right half.
        } else {
            right = mid - 1; // If the computed signature is greater than the target, search in the left half.
        }
    }
    return false;  // Return false if the target signature is not found.
}

// Main function: Program entry point.
int main(int argc, char *argv[]) {
    // Validate that exactly three command-line arguments (besides the program name) are provided.
    if (argc != 4) {
        // Output the proper usage if the argument count is incorrect.
        std::cerr << "Usage: " << argv[0] << " <sighting_file.dat> <signature_file.dat> <result_file.dat>" << std::endl;
        return 1; // Exit with error code 1.
    }

    // Extract file names from the command-line arguments.
    const std::string sightingFile = argv[1];   // File containing sighting data (speed and brightness).
    const std::string signatureFile = argv[2];    // File containing pre-calculated signature values.
    const std::string resultFile = argv[3];       // File where the matching count result will be written.

    try {
        // Read the sightings data from the file and parse it into a vector of Sighting objects.
        std::vector<Sighting> sightings = readFile<Sighting>(sightingFile, parseSighting);

        // Convert each Sighting object into a pair of (speed, brightness) for signature calculation.
        std::vector<std::pair<int, int>> sightingPairs;
        for (const auto &sighting : sightings) {
            sightingPairs.emplace_back(sighting.speed, sighting.brightness); // Add the pair to the vector.
        }
        // Calculate the signatures for the sightings using the provided formula.
        std::vector<int> calculatedSignatures = calcSig(sightingPairs);

        // Read the signature values from the signature file into a vector of integers.
        std::vector<int> signatures = readFile<int>(signatureFile, parseSignature);

        char choice;   // Variable to store the user's choice for the search method.
        Timer timer;   // Create a Timer object to measure CPU time.
        // Loop to prompt the user until a valid search method ('l' for linear or 'b' for binary) is entered.
        do {
            std::cout << "Choice of search method ([l]inear, [b]inary)? "; // Prompt user.
            std::cin >> choice;   // Read user input.
            if (choice != 'l' && choice != 'b') { // If input is invalid,
                std::cerr << "Incorrect choice" << std::endl; // Output error message.
            }
            std::cout << "CPU: " << timer << std::endl; // Display the current CPU time.
        } while (choice != 'l' && choice != 'b'); // Continue until a valid choice is made.

        // If the user selects binary search, sort the sightings vector by computed signature.
        if (choice == 'b') {
            timer.Reset(); // Reset the timer before sorting.
            // Sort the sightings using a lambda function that computes and compares the signature values.
            std::sort(sightings.begin(), sightings.end(), [](const Sighting &a, const Sighting &b) {
                int sigA = static_cast<int>(std::ceil(static_cast<double>(a.speed * a.brightness) / 10.0)); // Compute signature for 'a'.
                int sigB = static_cast<int>(std::ceil(static_cast<double>(b.speed * b.brightness) / 10.0)); // Compute signature for 'b'.
                return sigA < sigB; // Return true if 'a' should come before 'b'.
            });
            // Output the time taken to perform the sorting.
            std::cout << "Sorting CPU time: " << timer << std::endl;
        }

        int count = 0;   // Counter to track the number of matching signatures.
        timer.Reset();   // Reset the timer before beginning the search process.
        // Iterate through each signature value read from the signature file.
        for (const auto &signature : signatures) {
            // Depending on the user's choice, perform a linear or binary search.
            bool found = (choice == 'l') ? linearSearch(sightings, signature) : binarySearch(sightings, signature);
            if (found) { // If a match is found,
                count++; // Increment the match counter.
            }
        }
        // Display the CPU time taken for the search process.
        std::cout << timer << std::endl;

        // Open the result file to write the count of matching signatures.
        std::ofstream outFile(resultFile);
        if (!outFile.is_open()) {  // Check if the file opened successfully.
            std::cerr << "Error: cannot open file " << resultFile << std::endl;
            return 1; // Exit with error code if the file cannot be opened.
        }
        outFile << count;  // Write the number of matches to the file.
        outFile.close();   // Close the output file.

        // Print the number of matches found to the console.
        std::cout << "Matches found: " << count << std::endl;
    }
    catch (const std::exception &e) {
        // If any exception is thrown during processing, print the error message.
        std::cerr << e.what() << std::endl;
        return 1; // Exit with error code 1.
    }

    return 0; // Return 0 to indicate successful program termination.
}
