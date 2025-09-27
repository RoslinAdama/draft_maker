#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "Draft.hpp"

namespace {

void printHeader()
{
  std::cout << "=== Draft Maker C++ ===\n";
  std::cout << "=======================================\n";
}

void printFileInfo(const std::string &filepath, bool fromCommandLine)
{
  if (fromCommandLine)
  {
    std::cout << "Using file from command line: " << filepath << '\n';
  }
  else
  {
    std::cout << "Using default file: " << filepath << '\n';
    std::cout
        << "Tip: You can specify a different file as command line argument\n";
  }
}

void printResults(const draft::Composition &bestComposition, double totalValue)
{
  std::cout << "\n🏆 BEST COMPOSITION FOUND:\n";
  std::cout << "=========================\n";

  for (const auto &player : bestComposition)
  {
    std::cout << player.m_role << " : " << player.m_strPlayerName << ", ";
  }

  std::cout << "\nTotal composition value: " << totalValue << '\n';
  std::cout << "\nDraft generation completed successfully! 🎉\n";
}

} // anonymous namespace

int main(int argc, char *argv[])
{
  printHeader();

  std::vector<std::string> args(argv, argv + argc);
  std::string              nameFilePath    = "data/exemple.csv";
  std::string              rateFilePath    = "data/examples.csv";
  bool                     fromCommandLine = false;

  // Check for command line arguments
  if (args.size() > 1)
  {
    nameFilePath    = args[1];
    rateFilePath    = args[2];
    fromCommandLine = true;
  }

  printFileInfo(nameFilePath, fromCommandLine);
  printFileInfo(rateFilePath, fromCommandLine);

  std::cout << '\n';

  // Initialize draft maker
  draft::Draft draft(nameFilePath, rateFilePath);

  if (!draft.initialize())
  {
    std::cerr << "Failed to initialize draft maker. Please check your input "
                 "file.\n";
    return 1;
  }

  std::cout << "\nStarting composition search...\n";
  std::cout << "This may take a few minutes depending on the number of "
               "compositions to generate.\n\n";

  // Find best composition
  auto bestComposition = draft.findBestComposition();

  if (bestComposition.empty())
  {
    std::cerr << "Error: Could not find any valid composition.\n";
    return 1;
  }

  // Calculate and display total value
  double totalValue = draft::Draft::calculateCompositionValue(bestComposition);
  printResults(bestComposition, totalValue);

  return 0;
}