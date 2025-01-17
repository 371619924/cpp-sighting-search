#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <chrono>
#include <algorithm>
#include <cmath>

// Class representing a single sighting
class Sighting
{
public:
    int speed;       // Speed of the sighting
    int brightness;  // Brightness of the sighting

    // Constructor to initialize speed and brightness
    Sighting(int speed, int brightness) : speed(speed), brightness(brightness) {}

    // Overload < operator to enable sorting based on brightness, then speed
    bool operator<(const Sighting &other) const
    {
        if (brightness != other.brightness)
        {
            return brightness < other.brightness;
        }
        return speed < other.speed;
    }
};

// Timer class for measuring elapsed time
class Timer
{
private:
    std::chrono::high_resolution_clock::time_point startTime; // Start time

public:
    // Resets the timer
    void Reset()
    {
        startTime = std::chrono::high_resolution_clock::now();
    }

    // Calculates elapsed time in microseconds
    double ElapsedMicroseconds() const
    {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::micro>(endTime - startTime).count();
    }

    // Overload << operator to output elapsed time
    friend std::ostream &operator<<(std::ostream &os, const Timer &timer)
    {
        os << "CPU time: " << timer.ElapsedMicroseconds() << " microseconds";
        return os;
    }
};

// Function template to read and parse a file
// T: Type of object to be parsed
// Parser: Function to parse each line into a T object
template <typename T, typename Parser>
std::vector<T> readFile(const std::string &filename, Parser parser)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Error: cannot open file " + filename);
    }

    std::vector<T> container; // Container to store parsed objects
    std::string line;
    while (std::getline(file, line))
    {
        container.push_back(parser(line));
    }

    return container;
}

// Function to calculate signatures from speed and brightness pairs
std::vector<int> calcSig(const std::vector<std::pair<int, int>> &data)
{
    std::vector<int> signature;
    for (const auto &pairs : data)
    {
        // Calculate signature using the given formula
        int sig = static_cast<int>(std::ceil(static_cast<double>(pairs.first * pairs.second) / 10.0));
        signature.push_back(sig);
    }
    return signature;
}

// Function to parse a line into a Sighting object
Sighting parseSighting(const std::string &line)
{
    std::istringstream iss(line);
    int speed, brightness;
    if (!(iss >> speed >> brightness))
    {
        throw std::runtime_error("Invalid format in sighting file");
    }
    return Sighting(speed, brightness);
}

// Function to parse a line into an integer signature
int parseSignature(const std::string &line)
{
    std::istringstream iss(line);
    int signature;
    if (!(iss >> signature))
    {
        throw std::runtime_error("Invalid format in signature file");
    }
    return signature;
}

// Linear search function to check if a signature exists in the sightings
bool linearSearch(const std::vector<Sighting> &sightings, int signature)
{
    for (const auto &s : sightings)
    {
        // Calculate the signature for each sighting
        int calculatedSignature = static_cast<int>(std::ceil(static_cast<double>(s.speed * s.brightness) / 10.0));
        if (calculatedSignature == signature)
        {
            return true; // Found match
        }
    }
    return false; // No match found
}

// Binary search function to check if a signature exists in sorted sightings
bool binarySearch(const std::vector<Sighting> &sightings, int signature) {
    int left = 0, right = sightings.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        // Calculate the signature for the midpoint
        int calculatedSignature = static_cast<int>(std::ceil(static_cast<double>(sightings[mid].speed * sightings[mid].brightness) / 10.0));
        if (calculatedSignature == signature) {
            return true; // Found match
        } else if (calculatedSignature < signature) {
            left = mid + 1; // Search right half
        } else {
            right = mid - 1; // Search left half
        }
    }
    return false; // No match found
}

int main(int argc, char *argv[])
{
    // Check for proper command-line arguments
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <sighting_file.dat> <signature_file.dat> <result_file.dat>" << std::endl;
        return 1;
    }

    // Extract file names from arguments
    const std::string sightingFile = argv[1];
    const std::string signatureFile = argv[2];
    const std::string resultFile = argv[3];

    try
    {
        // Read sightings from file
        std::vector<Sighting> sightings = readFile<Sighting>(sightingFile, parseSighting);

        // Convert sightings into pairs of speed and brightness
        std::vector<std::pair<int, int>> sightingPairs;
        for (const auto &sighting : sightings)
        {
            sightingPairs.emplace_back(sighting.speed, sighting.brightness);
        }
        // Calculate signatures for the sightings
        std::vector<int> calculatedSignatures = calcSig(sightingPairs);

        // Read signatures from file
        std::vector<int> signatures = readFile<int>(signatureFile, parseSignature);

        // Ask the user to choose a search method
        char choice;
        Timer timer;
        do
        {
            std::cout << "Choice of search method ([l]inear, [b]inary)? ";
            std::cin >> choice;
            if (choice != 'l' && choice != 'b')
            {
                std::cerr << "Incorrect choice" << std::endl;
            }
            std::cout << "CPU: " << timer << std::endl;
        } while (choice != 'l' && choice != 'b');

        // If binary search is chosen, sort the sightings
        if (choice == 'b')
        {
            timer.Reset(); 
            std::sort(sightings.begin(), sightings.end(), [](const Sighting &a, const Sighting &b)
                      {
        int sigA = static_cast<int>(std::ceil(static_cast<double>(a.speed * a.brightness) / 10.0));
        int sigB = static_cast<int>(std::ceil(static_cast<double>(b.speed * b.brightness) / 10.0));
        return sigA < sigB; });
            std::cout << "Sorting CPU time: " << timer << std::endl;
        }

        // Perform the chosen search method and count matches
        int count = 0;
        timer.Reset(); 
        for (const auto &signature : signatures)
        {
            bool found = (choice == 'l') ? linearSearch(sightings, signature) : binarySearch(sightings, signature);
            if (found)
            {
                count++;
            }
        }
        std::cout << timer << std::endl; // Output elapsed time

        // Write the result to the output file
        std::ofstream outFile(resultFile);
        if (!outFile.is_open())
        {
            std::cerr << "Error: cannot open file " << resultFile << std::endl;
            return 1;
        }
        outFile << count;
        outFile.close();

        std::cout << "Matches found: " << count << std::endl;
    }
    catch (const std::exception &e)
    {
        // Handle any exceptions that occur
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
