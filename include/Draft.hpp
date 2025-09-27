#ifndef DRAFT_HPP
#define DRAFT_HPP

#include <random>
#include "Parser.hpp"

namespace draft {
class Draft
{
public:
  static constexpr size_t DEFAULT_COMPOSITIONS_COUNT = 500000;

  Draft(const std::filesystem::path &nameFilePath,
        const std::filesystem::path &rateFilePath);
  ~Draft() = default;

  Draft(const Draft &)            = delete;
  Draft &operator=(const Draft &) = delete;
  Draft(Draft &&)                 = default;
  Draft &operator=(Draft &&)      = default;

  bool initialize();

  Composition findBestComposition();

  Compositions generateRandomCompositions();

  static double calculateCompositionValue(const Composition &composition);

  [[nodiscard]] bool isInitialized() const { return m_parser.isValid(); }

private:
  Parser       m_parser;
  Data         m_players;
  std::mt19937 m_rng;

  static bool isCompositionValid(const Composition &composition);

  static Compositions filterValidCompositions(const Compositions &compositions);
};
} // namespace draft

#endif