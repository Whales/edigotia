#ifndef _FILES_H_
#define _FILES_H_

#include <vector>
#include <string>
#include <istream>

bool directory_exists(std::string name);
bool create_directory(std::string name);
bool file_exists     (std::string name);
std::vector<std::string> files_in(std::string dir, std::string suffix = "");
std::vector<std::string> directories_in(std::string dir);

std::string slurp_file(const std::string &filename);

void chomp(std::istream& data);
void clear_line(std::istream& data);

#endif
