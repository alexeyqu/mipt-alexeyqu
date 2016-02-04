#include "sais.h"

std::vector <long long> getNumOfNonEmptySubstrings (int lenSubstring, std::string str)
{
  size_t problemSize = str.size();

  str += str;

  std::vector <long long> ans;

  ans.resize (problemSize);

  for (size_t i = 0; i < problemSize; i++)
  {
    SuffixArrayInducedSorting sais;

    std::string substring = str.substr (i, lenSubstring);

    std::vector <Index_t> suffixArray = sais.getSuffixArray (substring);

    substring += '^'; // ^ < any of [a..z]

    std::vector <Index_t> lcpArray;
    getLcpArrayKasai (substring, suffixArray, lcpArray);

    for (size_t j = 0; j < lenSubstring; j++)
    {
      ans[i] += lenSubstring - suffixArray[j + 1] - lcpArray[j];
    }
  }

  return ans;
}

int main()
{
  int key = 0;
  std::string str;

  std::cin >> key >> str;

  std::vector<long long> ans = getNumOfNonEmptySubstrings (key, str);

  for (size_t i = 0; i < ans.size(); i++)
  {
    std::cout << ans[i] << ' ';
  }

  return 0;
}