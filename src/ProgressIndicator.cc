#include "ProgressIndicator.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace draft {

ProgressIndicator::ProgressIndicator(size_t total, std::string description)
    : m_stTotal(total), m_stCurrent(0),
      m_strDescription(std::move(description)),
      m_startTime(std::chrono::steady_clock::now()), m_bFinished(false)
{
  display();
}

ProgressIndicator::~ProgressIndicator()
{
  if (!m_bFinished)
  {
    finish();
  }
}

void ProgressIndicator::update(size_t increment)
{
  m_stCurrent += increment;
  m_stCurrent = std::min(m_stCurrent, m_stTotal);
  display();
}

void ProgressIndicator::setProgress(size_t current)
{
  m_stCurrent = (current > m_stTotal) ? m_stTotal : current;
  display();
}

void ProgressIndicator::finish()
{
  m_stCurrent = m_stTotal;
  m_bFinished = true;
  display();
  std::cout << '\n';
}

void ProgressIndicator::display()
{
  if (m_stTotal == 0)
  {
    return;
  }

  auto now = std::chrono::steady_clock::now();
  auto elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime)
          .count() /
      1000.0;

  double percentage = (static_cast<double>(m_stCurrent) / m_stTotal) * 100.0;
  auto filled = static_cast<size_t>((m_stCurrent * DISPLAY_WIDTH) / m_stTotal);

  // Clear line and move cursor to beginning
  std::cout << "\r\033[K";

  // Display progress bar
  std::cout << m_strDescription << ": " << std::fixed << std::setprecision(1)
            << percentage << "%|";

  for (size_t i = 0; i < DISPLAY_WIDTH; ++i)
  {
    if (i < filled)
      std::cout << "█";
    else
      std::cout << "░";
  }

  std::cout << "| " << m_stCurrent << "/" << m_stTotal;

  // Show time information
  if (elapsed > 0)
  {
    std::cout << " [" << formatTime(elapsed);

    if (m_stCurrent > 0 && m_stCurrent < m_stTotal)
    {
      double rate = m_stCurrent / elapsed; // incréments par seconde
      double eta  = (m_stTotal - m_stCurrent) / rate;

      std::cout << "<" << formatTime(eta) << " | " << std::setprecision(2)
                << rate << " it/s";
    }

    std::cout << "]";
  }

  std::cout.flush();
}

std::string ProgressIndicator::formatTime(double seconds)
{
  std::ostringstream oss;

  if (seconds < 60)
  {
    oss << std::fixed << std::setprecision(1) << seconds << "s";
  }
  else if (seconds < 3600)
  {
    int minutes = static_cast<int>(seconds / 60);
    int secs    = static_cast<int>(seconds) % 60;
    oss << minutes << ":" << std::setfill('0') << std::setw(2) << secs;
  }
  else
  {
    int hours   = static_cast<int>(seconds / 3600);
    int minutes = static_cast<int>(seconds / 60) % 60;
    oss << hours << ":" << std::setfill('0') << std::setw(2) << minutes
        << ":00";
  }

  return oss.str();
}

} // namespace draft