#include "Parser.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <utility>

namespace draft {
Parser::Parser(std::filesystem::path nameFilePath,
               std::filesystem::path rateFilePath)
    : m_nameFilePath(std::move(nameFilePath)),
      m_rateFilePath(std::move(rateFilePath)), m_bIsLoaded(false)
{
}

bool Parser::load()
{
  if (!std::filesystem::exists(m_nameFilePath) ||
      !std::filesystem::exists(m_rateFilePath))
  {
    std::cerr << "File does not exist: " << m_nameFilePath
              << " or: " << m_rateFilePath << '\n';
    return false;
  }

  if (m_nameFilePath.extension().string() != ".csv" ||
      m_rateFilePath.extension().string() != ".csv")
  {
    std::cerr << "Not CSV = unsupported " << m_nameFilePath
              << " or: " << m_rateFilePath << '\n';
    return false;
  }

  m_bIsLoaded = parseCsv(m_nameFilePath, m_nameSheet) &&
                parseCsv(m_rateFilePath, m_rateSheet);

  return m_bIsLoaded;
}

bool Parser::parseCsv(const std::filesystem::path &filePath, Sheet &sheet)
{
  sheet = {};
  std::ifstream file(filePath);
  std::string   content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  file.close();

  std::istringstream stream(content);
  std::string        line;

  while (std::getline(stream, line))
  {
    if (line.empty())
    {
      continue;
    }

    Row                row;
    std::istringstream lineStream(line);
    std::string        cell;
    while (std::getline(lineStream, cell, ','))
    {
      if (!cell.empty() && cell.front() == '"' && cell.back() == '"' &&
          cell.length() > 1)
      {
        cell = cell.substr(1, cell.length() - 2);
      }
      row.emplace_back(cell);
    }
    if (!row.empty())
    {
      sheet.emplace_back(std::move(row));
    }
  }

  return !sheet.empty();
}

size_t Parser::sheetSize(const Sheet &sheet)
{
  return sheet.empty() ? 0 : sheet[0].size() * sheet.size();
}

Data::Data(const Parser &parser)
{
  if (!parser.isValid())
  {
    return;
  }
  const auto &nameSheet = parser.getNameSheet();
  const auto &rateSheet = parser.getRateSheet();
  for (size_t col = 1; col < nameSheet[0].size(); ++col)
  {
    std::optional<Role> role = stringToRole(nameSheet[0][col]);
    if (!role)
    {
      continue;
    }
    std::vector<Player> playersInPosition;
    for (size_t row = 1; row < nameSheet.size(); ++row)
    {
      if (col < nameSheet[row].size() && col < rateSheet[row].size() &&
          !nameSheet[row][col].empty() && !rateSheet[row][col].empty())
      {
        Player player = {.m_strPlayerName = nameSheet[row][col],
                         .m_nNote         = static_cast<std::uint8_t>(
                             std::stoi(rateSheet[row][col])),
                         .m_role     = *role,
                         .m_teamName = nameSheet[row][0]};
        playersInPosition.emplace_back(std::move(player));
      }
    }

    if (!playersInPosition.empty())
    {
      m_players.emplace(*role, std::move(playersInPosition));
    }
  }
}

} // namespace draft