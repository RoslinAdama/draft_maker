#include "Draft.hpp"
#include <cstddef>
#include <iostream>
#include <random>
#include <string>
#include <unordered_set>
#include "Parser.hpp"
#include "ProgressIndicator.hpp"

namespace draft {

Draft::Draft(const std::filesystem::path &nameFilePath,
             const std::filesystem::path &rateFilePath)
    : m_parser(nameFilePath, rateFilePath), m_rng(std::random_device{}())
{
}

bool Draft::initialize()
{
  if (!m_parser.load())
  {
    std::cerr << "Error failed to load from file" << '\n';
    return false;
  }

  m_players = Data{m_parser};

  const auto &players = m_players.getPlayers();
  if (players.empty())
  {
    std::cerr << "Error: No players found in the data" << '\n';
    return false;
  }
  std::cout << "Loaded " << players.size() << "positions with players:" << '\n';
  for (const auto &[position, playerList] : players)
  {
    std::cout << "  " << position << playerList.size() << " players" << '\n';
  }

  return true;
}

Composition Draft::findBestComposition()
{
  if (!isInitialized())
  {
    std::cerr << "Drafter not init" << '\n';
  }
  std::cout << "Generating " << DEFAULT_COMPOSITIONS_COUNT
            << " random compositions..." << '\n';

  auto randomCompositions = generateRandomCompositions();

  if (randomCompositions.empty())
  {
    std::cerr << "Failed to generate compositions" << '\n';
  }

  auto validCompositions = filterValidCompositions(randomCompositions);

  if (validCompositions.empty())
  {
    std::cerr << "No valid compositions after filter" << '\n';
  }

  Composition bestComposition;
  double      bestValue = -1.0;

  ProgressIndicator progress(validCompositions.size(), "Evaluating");

  for (const auto &comp : validCompositions)
  {
    double value = calculateCompositionValue(comp);
    if (value > bestValue)
    {
      bestValue       = value;
      bestComposition = comp;
    }
    progress.update();
  }

  progress.finish();
  std::cout << "Best comp values" << bestValue << '\n';
  return bestComposition;
}

Compositions Draft::generateRandomCompositions()
{
  const auto &players = m_players.getPlayers();

  if (players.empty())
  {
    return {};
  }

  std::unordered_set<Composition, CompoHash, CompoEqual> uniqueCompositions;

  ProgressIndicator progress(DEFAULT_COMPOSITIONS_COUNT, "Generating");

  for (size_t iter = 0; iter < DEFAULT_COMPOSITIONS_COUNT; ++iter)
  {
    Composition comp;
    for (const auto &[position, playerList] : players)
    {
      if (playerList.empty())
      {
        continue;
      }
      std::uniform_int_distribution<size_t> dist(0, playerList.size() - 1);
      size_t                                randomIndex = dist(m_rng);
      const Player &selectedPlayer = playerList[randomIndex];
      comp.emplace_back(selectedPlayer);
    }

    if (!uniqueCompositions.contains(comp))
    {
      uniqueCompositions.insert(comp);
    }

    progress.update();
  }
  progress.finish();
  std::cout << "generated" << uniqueCompositions.size() << " unique compos"
            << '\n';
  Compositions compos;
  compos.reserve(uniqueCompositions.size());
  compos.insert(
      compos.end(), uniqueCompositions.begin(), uniqueCompositions.end());
  return compos;
}

Compositions Draft::filterValidCompositions(const Compositions &compositions)
{
  Compositions validCompositions;
  validCompositions.reserve(compositions.size());
  ProgressIndicator progress(compositions.size(), "Filtering");

  for (const auto &compo : compositions)
  {
    if (isCompositionValid(compo))
    {
      validCompositions.emplace_back(compo);
    }
    progress.update();
  }
  progress.finish();
  return validCompositions;
}

double Draft::calculateCompositionValue(const Composition &composition)
{
  double totalValue = 0.0;

  for (const auto &player : composition)
  {
    totalValue += player.m_nNote;
  }
  return totalValue;
}

bool Draft::isCompositionValid(const Composition &composition)
{
  std::unordered_set<std::string> sTeamName;
  for (const auto &player : composition)
  {
    const auto &teamName = player.m_teamName;
    if (sTeamName.contains(teamName))
    {
      return false;
    }
    sTeamName.emplace(teamName);
  }
  return true;
}

} // namespace draft