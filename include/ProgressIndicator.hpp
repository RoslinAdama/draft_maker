#pragma once

#include <chrono>
#include <string>

namespace draft {

/**
 * @brief Simple progress indicator for long-running operations
 *
 * A lightweight alternative to Python's tqdm for showing progress
 */
class ProgressIndicator
{
public:
  explicit ProgressIndicator(size_t      total,
                             std::string description = "Processing");
  ~ProgressIndicator();
  ProgressIndicator(const ProgressIndicator &)       = delete;
  ProgressIndicator &operator=(ProgressIndicator &)  = delete;
  ProgressIndicator(ProgressIndicator &&)            = delete;
  ProgressIndicator &operator=(ProgressIndicator &&) = delete;

  /**
   * @brief Update progress counter
   * @param increment Number of items processed (default: 1)
   */
  void update(size_t increment = 1);

  /**
   * @brief Set current progress directly
   * @param current Current progress value
   */
  void setProgress(size_t current);

  /**
   * @brief Mark as completed
   */
  void finish();

private:
  size_t                                m_stTotal;
  size_t                                m_stCurrent;
  std::string                           m_strDescription;
  std::chrono::steady_clock::time_point m_startTime;
  bool                                  m_bFinished;

  void                    display();
  static std::string      formatTime(double seconds);
  static constexpr size_t DISPLAY_WIDTH = 50;
};

} // namespace draft