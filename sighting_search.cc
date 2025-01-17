#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <chrono>
#include <algorithm>


class Sighting {
public:
    int speed;
    int brightness;

    Sighting(int speed, int brightness) : speed(speed), brightness(brightness) {} // Constructor

    // Overload < operator for sorting
    bool operator<(const Sighting &other) const {
        if (brightness != other.brightness) {
            return brightness < other.brightness;
        }
        return speed < other.speed;
    }



    friend std::ostream &operator<<(std::ostream &os, const Sighting &s) {
        os << "Sighting(speed: " << s.speed << ", brightness: " << s.brightness << ")";
        return os;
    }
};

// Time part 
class Timer {
private:
    std::chrono::high_resolution_clock::time_point startTime;

public:
    void Reset() {
        startTime = std::chrono::high_resolution_clock::now();
    }

    double ElapsedMicroseconds() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::micro>(endTime - startTime).count();
    }

    friend std::ostream &operator<<(std::ostream &os, const Timer &timer) {
        os << timer.ElapsedMicroseconds() << " microseconds";
        return os;
    }
};


// Generic function to read a file and parse lines using a provided parser
template <typename T, typename Parser>
std::vector<T> readFile(const std::string &filename, Parser parser) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: cannot open file " + filename +".dat");
    }

    std::vector<T> container;
    std::string line;
    while (std::getline(file, line)) {
        container.push_back(parser(line));
    }

    return container;
}
// Parser for sightings
Sighting parseSighting(const std::string &line) {
    std::istringstream iss(line);
    int speed, brightness;
    if (!(iss >> speed >> brightness)) {
        throw std::runtime_error("Invalid format in sighting file");
    }
    return Sighting(speed, brightness);
}

// Parser for signatures
int parseSignature(const std::string &line) {
    std::istringstream iss(line);
    int signature;
    if (!(iss >> signature)) {
        throw std::runtime_error("Invalid format in signature file");
    }
    return signature;
}

// Linear search implementation
bool linearSearch(const std::vector<Sighting> &sightings, int signature) {
    for (const auto &s : sightings) {
        if (static_cast<int>(s.brightness) == signature) {
            return true;
        }
    }
    return false;
}

// Binary search implementation
bool binarySearch(const std::vector<Sighting> &sightings, int signature) {
    int left = 0, right = sightings.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int midSignature = static_cast<int>(sightings[mid].brightness);
        if (midSignature == signature) {
            return true;
        } else if (midSignature < signature) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return false;
}


int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <sighting_file.dat> <signature_file.dat> <result_file.dat>" << std::endl;
        return 1;
    }

    const std::string sightingFile = argv[1];
    const std::string signatureFile = argv[2];
    const std::string resultFile = argv[3];

    try {
        // Validate input file paths
        std::ifstream testFile;

        testFile.open(sightingFile);
        if (!testFile.is_open()) {
            std::cerr << "Error: cannot open file " << sightingFile << std::endl;
            return 1;
        }
        testFile.close();

        testFile.open(signatureFile);
        if (!testFile.is_open()) {
            std::cerr << "Error: cannot open file " << signatureFile << std::endl;
            return 1;
        }
        testFile.close();

        // Read sightings
        std::vector<Sighting> sightings = readFile<Sighting>(sightingFile, parseSighting);
        
        // Read signatures
        std::vector<int> signatures = readFile<int>(signatureFile, parseSignature);

        // Choose search method
        char choice;
        Timer timer;
        do {
            std::cout << "Choice of search method ([l]inear, [b]inary)? ";
            std::cin >> choice;
            if (choice != 'l' && choice != 'b') {
                std::cerr << "Incorrect choice" << std::endl;
            }
        } while (choice != 'l' && choice != 'b');

        // Sort sightings for binary search
        if (choice == 'b') {
            std::sort(sightings.begin(), sightings.end());
        }

        // Start search
        int count = 0;
        timer.Reset();
        for (const auto &signature : signatures) {
            bool found = (choice == 'l') ? linearSearch(sightings, signature) : binarySearch(sightings, signature);
            if (found) {
                count++;
            }
        }

        // Print CPU time before outputting results
        std::cout << "CPU time: " << timer.ElapsedMicroseconds() << " microseconds" << std::endl;

        // Output results to file
        std::ofstream outFile(resultFile);
        if (!outFile.is_open()) {
            std::cerr << "Error: cannot open file " << resultFile << std::endl;
            return 1;
        }
        outFile << count;
        outFile.close();

        // Re-open and validate result file
        std::ifstream validateFile(resultFile);
        if (!validateFile.is_open()) {
            std::cerr << "Error: cannot open file" << std::endl;
            return 1;
        }
        validateFile.close();

        // Print matches found after writing to file
        std::cout << "Matches found: " << count << std::endl;

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}

