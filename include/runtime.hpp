#ifndef MAREX_RUNTIME_HPP
#define MAREX_RUNTIME_HPP

#include <string>
#include <vector>

bool execute_source_once(const std::string &source, const std::vector<std::string> &args);
bool execute_script_file(const std::string &file_path, const std::vector<std::string> &args);

#endif //MAREX_RUNTIME_HPP
