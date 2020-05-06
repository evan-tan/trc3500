#pragma once

#include <map>
#include <string>

namespace BarCodeReader {

std::string parity_check(std::string some_str) {
  int count_ones;
  for (int i = 0; i < some_str.size() - 1; i++) {
    if (some_str.substr(i, 1) == "1") {
      count_ones++;
    }
  }

  if (count_ones % 2) {
    return "E";
  } else {
    return "O";
  }
}

class LeftDigitMap {
  std::map<std::string, std::string> ld_map;
  LeftDigitMap(std::string parity) {
    if (parity == "even") {
      ld_map = {{"0100111", "0"}, {"0110011", "1"}, {"0011011", "2"},
                {"0100001", "3"}, {"0011101", "4"}, {"0111001", "5"},
                {"0000101", "6"}, {"0010001", "7"}, {"0001001", "8"},
                {"0010111", "9"}};
    } else if (parity == "odd") {
      ld_map = {{"0001101", "0"}, {"0011001", "1"}, {"0010011", "2"},
                {"0111101", "3"}, {"0100011", "4"}, {"0110001", "5"},
                {"0101111", "6"}, {"0111011", "7"}, {"0110111", "8"},
                {"0001011", "9"}};
    }
  }
};

class RightDigitMap {
  std::map<std::string, std::string> rd_map;
  RightDigitMap() {
    rd_map = {{"1110010", "0"}, {"1100110", "1"}, {"1101100", "2"},
              {"1000010", "3"}, {"1011100", "4"}, {"1001110", "5"},
              {"1010000", "6"}, {"1000100", "7"}, {"1001000", "8"},
              {"1110100", "9"}};
  }
};

class ParityMap {
  std::map<std::string, std::string> p_map;
  ParityMap() {
    p_map["OOOOOO"] = "0";
    p_map["OOEOEE"] = "1";
    p_map["OOEEOE"] = "2";
    p_map["OOEEEO"] = "3";
    p_map["OEOOEE"] = "4";
    p_map["OEEOOE"] = "5";
    p_map["OEEEOO"] = "6";
    p_map["OEOEOE"] = "7";
    p_map["OEOEEO"] = "8";
    p_map["OEEOEO"] = "9";
  }
};
} // namespace BarCodeReader
