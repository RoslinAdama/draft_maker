#ifndef PARSER_HPP
#define PARSER_HPP

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace draft {

enum class Role : std::uint8_t
{
  G,
  ARR_D,
  ARR_G,
  DCG,
  DCD,
  MDEF,
  MC,
  MOFF,
  AIL_G,
  AIL_D,
  BU
};

constexpr std::optional<Role> stringToRole(std::string_view str)
{
  if (str == "G")
  {
    return Role::G;
  }
  if (str == "Arr D")
  {
    return Role::ARR_D;
  }
  if (str == "Arr G")
  {
    return Role::ARR_G;
  }
  if (str == "DCG")
  {
    return Role::DCG;
  }
  if (str == "DCD")
  {
    return Role::DCD;
  }
  if (str == "MDEF")
  {
    return Role::MDEF;
  }
  if (str == "MC")
  {
    return Role::MC;
  }
  if (str == "MOFF")
  {
    return Role::MOFF;
  }
  if (str == "Ail G")
  {
    return Role::AIL_G;
  }
  if (str == "Ail D")
  {
    return Role::AIL_D;
  }
  if (str == "BU")
  {
    return Role::BU;
  }
  return std::nullopt;
}

inline std::ostream &operator<<(std::ostream &oss, const Role &role)
{
  oss << "r: ";
  switch (role)
  {
    case Role::G:
      oss << "G";
      break;
    case Role::ARR_D:
      oss << "Arr D";
      break;
    case Role::ARR_G:
      oss << "Arr G";
      break;
    case Role::DCG:
      oss << "DCG";
      break;
    case Role::DCD:
      oss << "DCD";
      break;
    case Role::MDEF:
      oss << "MDEF";
      break;
    case Role::MC:
      oss << "MC";
      break;
    case Role::MOFF:
      oss << "MOFF";
      break;
    case Role::AIL_G:
      oss << "Ail G";
      break;
    case Role::AIL_D:
      oss << "Ail D";
      break;
    case Role::BU:
      oss << "BU";
      break;
    default:
      oss << "Unknown";
      break;
  }
  return oss;
}

struct Player
{
  constexpr static size_t MAGIC_HASH = 0x9e3779b9;
  std::string             m_strPlayerName;
  std::uint8_t            m_nNote;
  Role                    m_role;
  std::string             m_teamName;

  struct Hash
  {
    size_t operator()(const Player &p) const noexcept
    {
      std::size_t hash1 = std::hash<std::string>{}(p.m_strPlayerName);
      std::size_t hash2 = std::hash<std::uint8_t>{}(p.m_nNote);
      std::size_t hash3 =
          std::hash<std::uint8_t>{}(static_cast<std::uint8_t>(p.m_role));
      std::size_t hash4 = std::hash<std::string>{}(p.m_teamName);
      std::size_t seed  = hash1;
      seed ^= hash2 + MAGIC_HASH + (seed << 6) + (seed >> 2);
      seed ^= hash3 + MAGIC_HASH + (seed << 6) + (seed >> 2);
      seed ^= hash4 + MAGIC_HASH + (seed << 6) + (seed >> 2);
      return seed;
    }
  };

  struct Equal
  {
    bool operator()(const Player &playerA, const Player &playerB) const noexcept
    {
      return playerA.m_strPlayerName == playerB.m_strPlayerName &&
             playerA.m_nNote == playerB.m_nNote &&
             playerA.m_role == playerB.m_role &&
             playerA.m_teamName == playerB.m_teamName;
    }
  };
};

using Composition  = std::vector<Player>;
using Compositions = std::vector<Composition>;

struct CompoHash
{
  std::size_t operator()(const Composition &c) const noexcept
  {
    std::size_t  seed = 0;
    Player::Hash ph;
    for (auto const &p : c)
    {
      std::size_t h = ph(p);
      seed ^= h + Player::MAGIC_HASH + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};

// Égalité pour Compo
struct CompoEqual
{
  bool operator()(const Composition &a, const Composition &b) const noexcept
  {
    if (a.size() != b.size())
    {
      return false;
    }
    for (std::size_t i = 0; i < a.size(); ++i)
    {
      if (!Player::Equal{}(a[i], b[i]))
      {
        return false;
      }
    }
    return true;
  }
};

class Parser
{
public:
  using Row   = std::vector<std::string>;
  using Sheet = std::vector<Row>;

  Parser(std::filesystem::path nameFilePath,
         std::filesystem::path rateFilePath);

  ~Parser() = default;

  Parser(const Parser &)            = delete;
  Parser &operator=(const Parser &) = delete;
  Parser(Parser &&)                 = default;
  Parser &operator=(Parser &&)      = default;

  bool load();

  [[nodiscard]] std::optional<std::uint8_t>
  getPlayerRate(Role role, const std::string &playerName);

  [[nodiscard]] std::string getPlayerTeam(const std::string &playerName);

  [[nodiscard]] bool isValid() const
  {
    return m_bIsLoaded && sheetSize(m_nameSheet) == sheetSize(m_rateSheet);
  }

  [[nodiscard]] const Sheet &getNameSheet() const { return m_nameSheet; };
  [[nodiscard]] const Sheet &getRateSheet() const { return m_rateSheet; };

private:
  std::filesystem::path m_nameFilePath;
  std::filesystem::path m_rateFilePath;
  Sheet                 m_nameSheet;
  Sheet                 m_rateSheet;
  bool                  m_bIsLoaded;

  static bool   parseCsv(const std::filesystem::path &filePath, Sheet &sheet);
  static size_t sheetSize(const Sheet &sheet);
};

class Data
{
public:
  using PlayersByPosition = std::unordered_map<Role, std::vector<Player>>;

  Data()  = default;
  ~Data() = default;
  explicit Data(const Parser &parser);
  Data(const Data &)            = delete;
  Data &operator=(const Data &) = delete;
  Data(Data &&)                 = default;
  Data &operator=(Data &&)      = default;

  [[nodiscard]] const PlayersByPosition &getPlayers() const
  {
    return m_players;
  }

private:
  PlayersByPosition m_players;
};

} // namespace draft
#endif