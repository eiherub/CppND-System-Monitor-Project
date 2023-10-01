#include "format.h"

#include <string>
using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function

string Format::ElapsedTime(long seconds) {
  int const secondsinDay = 86400;
  int const secondsinHours = 3600;
  int const secondsinMinutes = 60;

  int elapseDays = (seconds / secondsinDay);
  int elapseHours = (seconds - (elapseDays * secondsinDay)) / secondsinHours;
  int elapseMinute =
      (seconds - (elapseDays * secondsinDay) - (elapseHours * secondsinHours)) /
      secondsinMinutes;
  int elapseSeconds = static_cast<int>(seconds - (elapseDays * secondsinDay) -
                                       (elapseHours * secondsinHours) -
                                       (elapseMinute * secondsinMinutes));

  // return (to_string(elapseDays) + " Days " + to_string(elapseHours) +":" +
  // to_string(elapseMinute) +":" + to_string(elapseSeconds));
  return to_string(elapseHours) + ":" + to_string(elapseMinute) + ":" +
         to_string(elapseSeconds);
}