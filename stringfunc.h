#ifndef _STRINGFUNC_H_
#define _STRINGFUNC_H_

#include "color.h"
#include <string>
#include <vector>
#include <istream>

#define STD_DELIM "</>"

std::vector<std::string> break_into_lines(std::string text, int linesize);

std::string load_to_delim(std::istream &datastream, std::string delim);
std::string load_to_character(std::istream &datastream, char ch,
                              bool _trim = false);
std::string load_to_character(std::istream &datastream, std::string chars,
                              bool _trim = false);

std::string slurp_file(const std::string &filename);

std::string trim                (const std::string &orig);
std::string all_caps            (const std::string &orig);
std::string no_caps             (const std::string &orig);
// capitalize("here are-the words") => "Here are-the words"
std::string capitalize          (const std::string &orig);
// capitalze_all_words("here are-the words") => "Here Are-The Words"
std::string capitalize_all_words(const std::string &orig);
std::string remove_color_tags   (const std::string &orig);

int tagless_length              (const std::string &orig);

// Convert an int to a string
std::string itos(int num);
// Digits in an int - in this file because it's mainly used for alignment
// For negative numbers, it's digits_in(0 - num) + 1 (for the - character)
int digits_in(int num);
// move_decimal() moves the decimal place <moves> places to the left, and
// returns the number as a string.  For instance, move_decimal(108, 2) would
// return "1.08".
std::string move_decimal(int num, int moves);

// Finds the breakpoints that value falls between, and returns a color tag
// from the colors provided.
std::string color_gradient(int value, std::vector<int> breakpoints,
                           std::vector<nc_color> colors);

// Returns a list of letters that may follow ch; if vowel_before is true, then
// there should be a vowel before ch.
std::string letter_after(long ch, bool vowel_before = false);

bool is_letter(char ch);
bool is_vowel (char ch);

#endif
