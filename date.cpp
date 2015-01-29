#include "date.h"
#include <sstream>

Date::Date()
{
  year = 400;
  month = 1;
  day = 1;
}

Date::~Date()
{
}

Date::Date(const Date& copy)
{
  year  = copy.year;
  month = copy.month;
  day   = copy.day;
  standardize();
}

Date::Date(int _year, int _month, int _day)
{
  year  = _year;
  month = _month;
  day   = _day;
  standardize();
}

std::string Date::save_data()
{
  std::stringstream ret;

  ret << year << " " << month << " " << day;

  return ret.str();
}

bool Date::load_data(std::istream& data)
{
  data >> year >> month >> day;
  return true;
}

bool Date::operator==(const Date& other) const
{
  Date left = *this;
  left.standardize();
  Date right = other;
  right.standardize();
  return (year == other.year && month == other.month && day == other.day);
}

bool Date::operator!=(const Date& other) const
{
  return !(*this == other);
}

Date& Date::operator+=(const Date& rhs)
{
  year  += rhs.year;
  month += rhs.month;
  day   += rhs.day;

  standardize();
  return *this;
}

Date& Date::operator-=(const Date& rhs)
{
  year  -= rhs.year;
  month -= rhs.month;
  day   -= rhs.day;

  standardize();
  return *this;
}

int Date::get_year()
{
  standardize();
  return year;
}

int Date::get_month()
{
  standardize();
  return month;
}

int Date::get_day()
{
  standardize();
  return day;
}

std::string Date::get_month_name()
{
  standardize();
  return month_name(month);
}

std::string Date::get_text(std::string format)
{
  standardize();
  if (format == "") {
    format = "#M #d, #Y";
  }

  std::stringstream ret;
  for (int i = 0; i < format.size(); i++) {
    bool used_tag = false;
    if (format[i] == '#' && i + 1 < format.size()) {
      used_tag = true;
      switch (format[i + 1]) {
        case 'Y':
          ret << year;
          break;
        case 'y':
          ret << (year % 100);  // Two-digit
          break;
        case 'M':
          ret << get_month_name();
          break;
        case 'm':
          ret << month;
          break;
        case 'd':
          ret << day;
          break;
        default:
          used_tag = false;
          break;
      }
    }
    if (used_tag) {
      i++;
    } else {
      ret << format[i];
    }
  }

  return ret.str();
}

void Date::advance(int days)
{
// days defaults to 1.
  day += days;
  standardize();
}

void Date::standardize()
{
// First, make sure we have a real month (we'll need it for days_in_month())
  while (month < 1) {
    month += 12;
    year--;
  }
  while (month > 12) {
    month -= 12;
    year++;
  }
  while (day > days_in_month(month)) {
    day -= days_in_month(month);
    month++;
    if (month >= 13) {
      month -= 12;
      year++;
    }
  }
}

std::string month_name(int month)
{
  switch (month) {
    case  1:  return "January";
    case  2:  return "February";
    case  3:  return "March";
    case  4:  return "April";
    case  5:  return "May";
    case  6:  return "June";
    case  7:  return "July";
    case  8:  return "August";
    case  9:  return "September";
    case 10:  return "October";
    case 11:  return "November";
    case 12:  return "December";
    default:  return "SMARCH";
  }
  return "ESCAPED SWITCH";
}

int days_in_month(int month)
{
  switch (month) {
    case  1:  return 31;
    case  2:  return 28;  // No leap years, I'm lazy!
    case  3:  return 31;
    case  4:  return 30;
    case  5:  return 31;
    case  6:  return 30;
    case  7:  return 31;
    case  8:  return 31;
    case  9:  return 30;
    case 10:  return 31;
    case 11:  return 30;
    case 12:  return 31;
    default:  return 30;
  }
  return 30;
}
