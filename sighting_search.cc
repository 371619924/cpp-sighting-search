#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

class Sighting {
public:
    int speed;
    int brightness;

    Sighting(int speed, int brightness) : speed(speed), brightness(brightness) {} // Constructor

    // Overload < operator for sorting
    bool operator<(const Sighting &other) const {
        int thisSignature = static_cast<int>(brightness);
        int otherSignature = static_cast<int>(other.brightness);
        return thisSignature == otherSignature ? speed < other.speed : thisSignature < otherSignature;
    }


    friend std::ostream &operator<<(std::ostream &os, const Sighting &s) {
        os << "Sighting(speed: " << s.speed << ", brightness: " << s.brightness << ")";
        return os;
    }
};

// Generic function to read a file and parse lines using a provided parser
template <typename T, typename Parser>
std::vector<T> readFile(const std::string &filename, Parser parser) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Fail");
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

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <sighting_file> <signature_file>" << std::endl;
        return 1;
    }

    const std::string sightingFile = argv[1];
    const std::string signatureFile = argv[2];

    try {
        // Read sightings
        std::vector<Sighting> sightings = readFile<Sighting>(sightingFile, parseSighting);
        
        // Read signatures
        std::vector<int> signatures = readFile<int>(signatureFile, parseSignature);

        // Debugging output
        std::cout << "Sightings:" << std::endl;
        for (const auto &sighting : sightings) {
            std::cout << sighting << std::endl;
        }

        std::cout << "\nSignatures:" << std::endl;
        for (const auto &signature : signatures) {
            std::cout << signature << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
