#include "esko.h"

std::vector <long long> getNumOfNonEmptySubstrings (int lenSubstring, std::string str)
{
  int problemSize = str.size();

  for (size_t i = 0; i < problemSize; i++)
  {
    str[i] -= 'a';
  }

  str += str;

  std::vector <long long> ans;

  ans.resize (problemSize);

  for (size_t i = 0; i < problemSize; i++)
  {
    std::string substring = str.substr (i, lenSubstring);

    SuffixTree tree = buildTree (substring);

    ans[i] = tree.calcSum();
  }

  return ans;
}

int main() // this part was designed to solve acm.timus.ru #1706 problem
{
  int key = 0;
  std::string str;

  std::cin >> key >> str;

  std::vector <long long> ans = getNumOfNonEmptySubstrings (key, str);

  for (size_t i = 0; i < ans.size(); i++)
  {
    std::cout << ans[i] << ' ';
  }

  return 0;
}