#ifndef _DATE_H_
#define _DATE_H_

/* Date has a get_text() function which takes a string format as an argument.
 * The format defaults to "#M #d, #Y".
 * The following characters get replaced:
 * #Y - Four-digit year.
 * #y - Two-digit year.
 * #M - Name of month.
 * #m - Month number.
 * #d - Day number.
 * ## - Literal #.
 * All other characters are left verbatim.
 */

#include <string>
#include <istream>

std::string month_name(int month);
int days_in_month(int month);

class Date
{
public:
  Date();
  ~Date();

  Date(const Date& copy);
  Date(int _year, int _month, int _day);

  std::string save_data();
  bool load_data(std::istream& data);

  bool operator==(const Date& other) const;
  bool operator!=(const Date& other) const;

  Date& operator+=(const Date& rhs);
  Date& operator-=(const Date& rhs);

  int get_year();
  int get_month();
  int get_day();
  std::string get_month_name();
  std::string get_text(std::string format = "");

  void advance(int days = 1);
  void standardize(); // Ensure day < 30/31, month < 12, etc

private:
  int year;
  int month;
  int day;
};

inline Date operator+(Date lhs, const Date& rhs)
{
  lhs += rhs;
  return lhs;
}

inline Date operator-(Date lhs, const Date& rhs)
{
  lhs -= rhs;
  return lhs;
}

#endif
