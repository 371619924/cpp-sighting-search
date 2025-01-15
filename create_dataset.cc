#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>

int main(int argc, char *argv[]) {
  if (argc < 4) {
    std::cerr << "Usage: " << argv[0]
        << " <num_sights> <num_signatures> <suffix> [seed]"
        << std::endl;
    exit(1);
  }

  int nsights = std::stoi(argv[1]);
  int nsigs = std::stoi(argv[2]);
  if (nsights <= 0 || nsigs <= 0) {
    std::cerr << "Error: wrong arguments" << std::endl;
    exit(1);
  }

  std::ofstream sights, sigs;
  sights.open(std::string("sightings_") + argv[3] + ".dat",
             std::ofstream::trunc);
  sigs.open(std::string("signatures_") + argv[3] + ".dat",
            std::ofstream::trunc);
  if (!sights.good() || !sigs.good()) {
    std::cerr << "Error: cannot open output file(s)" << std::endl;
    exit(1);
  }

  /* random generator */
  std::random_device rd;
  std::mt19937 mt(rd());

  if (argc == 5) {
    int seed = std::stoi(argv[4]);
    if (seed < 0) {
      std::cerr << "Error: wrong seed" << std::endl;
      exit(1);
    }
    mt.seed(seed);
  }

  /* random number distribution for sights */
  std::normal_distribution<float> speed_dist(25, 20);
  std::normal_distribution<float> bright_dist(0, 20);
  for (int i = 0; i < nsights; i++) {
    int s = std::min(nsights, std::max(1, static_cast<int>(speed_dist(mt))));
    int b = std::min(30, std::max(-30, static_cast<int>(bright_dist(mt))));
    sights << s << " " << b << "\n";
  }

  /* random number distribution for signatures */
  std::normal_distribution<float> sig_dist(0, 25 * 20 / 10.0);
  for (int i = 0; i < nsigs; i++) {
    int s = static_cast<int>(sig_dist(mt));
    sigs << s << "\n";
  }

  sights.close();
  sigs.close();

  return 0;
}
