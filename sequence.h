#pragma once

#include <string>
#include <vector>

const std::vector<size_t> SIZES = {10000, 100000, 1000000, 10000000};
const int MIN_VALUE = 1;
const int MAX_VALUE = 1000000;

std::string fileName(size_t size);
std::vector<int> readFromFile(const std::string& name);

// Creates a file for every size from SIZES that doesn't exist yet.
// Existing files are not overwritten, so the /O2 and /Od runs use the same data.
void generateFiles();
