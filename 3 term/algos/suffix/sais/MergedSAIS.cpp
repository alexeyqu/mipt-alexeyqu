#define _SAIS__
#ifndef _SAIS_LIB__
#define _SAIS_LIB__

#include <iostream>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <string>
#include <vector>

#define REPORT( str ) //std::cout << "R " << str << std::endl;

typedef uint32_t Index_t;

enum SuffixType
{
  L_TYPE,
  S_TYPE
};

class SuffixArrayInducedSorting
{
private:

  class Worker
  {
  private:
    Index_t charCounter_, alphabetSize_;

    const std::vector <Index_t> &str_;
    std::vector <Index_t> &suffArray_;

    std::vector <SuffixType> charTypes_;
    std::vector <Index_t> lmsSubstrings_;
    std::vector <bool> lmsFlag_;
    std::vector <std::vector <Index_t> > baskets_;
    std::vector <Index_t> charInducedCounters_; 
    std::vector <Index_t> inducedString_;
    std::vector <Index_t> inducedSuffixArray_;

    void classifyChars();
    int findLMSBorders();
    void exportLMSChars();
    bool equalLMS (Index_t lms1, Index_t lms2);
    void initBaskets();
    void clearBaskets();
    std::vector <Index_t> computeSuffixArray();
    void inducedSortLMS (const std::vector <Index_t> &lmsSubstrings);
    void inducedSortSuff (const std::vector <Index_t> &inducedSuffixArray);
    void inducedSort (const std::vector <Index_t> &sortedData, std::vector <Index_t> &head, std::vector <Index_t> &end);

  public:
    Worker (const std::vector <Index_t> &str, std::vector <Index_t> &suffArray, const Index_t alphabetSize);

    ~Worker() {};

    void dumpWorker();

    void suffixArrayInducedSortingAlgo();
  };

public:
  ~SuffixArrayInducedSorting() {};

  std::vector <Index_t> getSuffixArray (const std::string &str);
};

void getLcpArrayKasai (const std::string &str, const std::vector <Index_t> &suffixArray, std::vector <Index_t> &lcpArray);

#endif


SuffixArrayInducedSorting::Worker::Worker (const std::vector <Index_t> &str, std::vector <Index_t> &suffArray, const Index_t alphabetSize) : \
  str_ (str), suffArray_ (suffArray), alphabetSize_ (alphabetSize) {}

void SuffixArrayInducedSorting::Worker::classifyChars() //+
{
  REPORT ("classifyChars");

  charTypes_.resize (str_.size());
  charTypes_[str_.size() - 1] = S_TYPE;

  //std::cout << '\'';
  // for (Index_t i = 0; i < str_.size(); i++)
  // {
  //   //std::cout << str_[i];
  // }
  //std::cout << "\'\n";
  //std::cout << std::endl << str_.size() << std::endl;

  for (Index_t i = str_.size() - 2; ; i--)
  {
    if (str_[i] > str_[i + 1]) charTypes_[i] = L_TYPE;
    if (str_[i] < str_[i + 1]) charTypes_[i] = S_TYPE;
    if (str_[i] == str_[i + 1]) charTypes_[i] = charTypes_[i + 1];
    if (i == 0) break;
  }

  REPORT ("/classifyChars");
}

void SuffixArrayInducedSorting::Worker::initBaskets() //+
{
  REPORT ("initBaskets");
  baskets_.resize (alphabetSize_);

  std::vector <Index_t> basketSize (alphabetSize_, 0);
  Index_t stringSize = str_.size();

  for (Index_t i = 0; i < stringSize; i++)
  {
    basketSize[str_[i]]++;
  }

  for (Index_t i = 0; i < alphabetSize_; i++)
  {
    baskets_[i].resize (basketSize[i], UINT32_MAX);
  }

  REPORT ("/initBaskets");
}

void SuffixArrayInducedSorting::Worker::clearBaskets() //+
{
  REPORT ("clearBaskets");
  for (Index_t i = 0; i < baskets_.size(); i++)
  {
    for (Index_t j = 0; j < baskets_[i].size(); j++)
    {
      baskets_[i][j] = UINT32_MAX;
    }
  }

  REPORT ("/clearBaskets");
}

int SuffixArrayInducedSorting::Worker::findLMSBorders()
{
  REPORT ("findLMSBorders");
  Index_t stringSize = str_.size();

  lmsFlag_.resize (stringSize, false); // find suitable LMS chars

  if (stringSize == 1)
  {
    lmsFlag_[0] = true;
    lmsSubstrings_.push_back (0);
    return 1;
  }

  for (Index_t i = 1; i < stringSize; i++)
  {
    if (charTypes_[i - 1] == L_TYPE && 
      charTypes_[i] == S_TYPE)
    {
      lmsFlag_[i] = true;
      lmsSubstrings_.push_back (i);
    }
  }

  return lmsSubstrings_.size();

  REPORT ("/findLMSBorders");
}

void SuffixArrayInducedSorting::Worker::exportLMSChars()
{
  REPORT ("exportLMSChars");
  charInducedCounters_.resize (str_.size(), 0);

  charCounter_ = 0;
  bool start = true;
  Index_t prevLMS = 0;

  for (Index_t i = 0; i < baskets_.size(); i++)
  {
    for (Index_t j = 0; j < baskets_[i].size(); j++)
    {
      if (lmsFlag_[baskets_[i][j]])
      {
        if (start)
          charInducedCounters_[baskets_[i][j]] = 0;
        else
        {
          if (!equalLMS (prevLMS, baskets_[i][j])) charCounter_++;

          charInducedCounters_[baskets_[i][j]] = charCounter_;
        } 

        prevLMS = baskets_[i][j];
        start = false;
      }
    }
  }
  REPORT ("/exportLMSChars");
}

std::vector <Index_t> SuffixArrayInducedSorting::Worker::computeSuffixArray()
{
  REPORT ("computeSuffixArray");
  std::vector <Index_t> inducedSuffixArray;
  inducedSuffixArray.resize (inducedString_.size(), 0);

  for (Index_t i = 0; i < inducedString_.size(); i++)
  {
    inducedSuffixArray[inducedString_[i]] = i;
  }

  return inducedSuffixArray;
  REPORT ("/computeSuffixArray");
}

void SuffixArrayInducedSorting::Worker::inducedSortLMS (const std::vector <Index_t> &lmsSubstrings) //+
{
  REPORT ("inducedSortLMS");
  std::vector <Index_t> head (baskets_.size(), 0);
  std::vector <Index_t> end (baskets_.size(), 0);

//Stage 1

  for (Index_t i = 0; i < lmsSubstrings.size(); i++)
  {
    Index_t curBasket = str_[lmsSubstrings[i]];
    baskets_[curBasket][baskets_[curBasket].size() - 1 - head[curBasket]] = lmsSubstrings[i];
    head[curBasket]++;
  }

  inducedSort (lmsSubstrings, head, end);

  REPORT ("/inducedSortLMS");
}

void SuffixArrayInducedSorting::Worker::inducedSortSuff (const std::vector <Index_t> &inducedSuffixArray)
{
  REPORT ("inducedSortSuff");
  std::vector <Index_t> head (baskets_.size(), 0);
  std::vector <Index_t> end (baskets_.size(), 0);

//Stage 1
  Index_t curBasket = 0;

  for (Index_t i = inducedSuffixArray.size() - 1; ; i--)
  {
    curBasket = str_[lmsSubstrings_[inducedSuffixArray[i]]];
    //std::cout << curBasket << std::endl;
    baskets_[curBasket][baskets_[curBasket].size() - 1 - head[curBasket]] = lmsSubstrings_[inducedSuffixArray[i]];
    head[curBasket]++;

    if (i == 0) break;
  }

  inducedSort (inducedSuffixArray, head, end);

  REPORT ("/inducedSortSuff");
}

void SuffixArrayInducedSorting::Worker::inducedSort (const std::vector <Index_t> &sortedData, std::vector <Index_t> &head, std::vector <Index_t> &end) //+
{
  REPORT ("inducedSort");
//common finish of stage 1
  for (Index_t i = 0; i < baskets_.size(); i++)
  {
    if (baskets_[i].size() != 0)
      end[i] = baskets_[i].size() - 1;
    else
      end[i] = 0;

    head[i] = 0;
  }

//Stage 2 L_TYPE
  for (Index_t i = 0; i < baskets_.size(); i++)
  {
    for (Index_t j = 0; j < baskets_[i].size(); j++)
    {
      if (baskets_[i][j] == 0 || 
        baskets_[i][j] == UINT32_MAX)
        continue;

      Index_t curPosition = baskets_[i][j] - 1;
      Index_t curChar = str_[curPosition];

      if (charTypes_[curPosition] == L_TYPE)
      {
        baskets_[curChar][head[curChar]] = curPosition;
        head[curChar]++;
      }
    }
  }

//Stage 3 S_TYPE
  Index_t i = baskets_.size();
  for (; i != 0; )
  {
    i--;

    Index_t j = baskets_[i].size();
    for (; j != 0; )
    {
      j--;
      if (baskets_[i].size() == 0 ||
        baskets_[i][j] == UINT32_MAX || 
        baskets_[i][j] == 0)
        continue;

      Index_t curPosition = baskets_[i][j] - 1;
      Index_t curChar = str_[curPosition];

  // std::cout << "Hello!" << i << ' ' << j << std::endl;
  // std::cout << curChar << ' ' <<  end[curChar] << std::endl;

      if (charTypes_[curPosition] == S_TYPE)
      {
        baskets_[curChar][end[curChar]] = curPosition;
        end[curChar]--;
      }
    }
  }

  //std::cout << "After" << std::endl;

  REPORT ("/inducedSort");
}

bool SuffixArrayInducedSorting::Worker::equalLMS (Index_t lms1, Index_t lms2) //+
{
  REPORT ("equalLMS");
  bool lType = false;

  while (str_[lms1] == str_[lms2] &&
    charTypes_[lms1] == charTypes_[lms2])
  {
    if (charTypes_[lms1] == L_TYPE)
      lType = true;

    if (lType && charTypes_[lms1] == S_TYPE)
      return true;

    //std::cout << lms1 << ' ' << lms2 << std::endl;

    lms1++;
    lms2++;

    if (lms1 >= str_.size() && lms2 >= str_.size()) abort();
  }
  REPORT ("/equalLMS");

  return false;
}

void SuffixArrayInducedSorting::Worker::suffixArrayInducedSortingAlgo() //+
{
  REPORT ("Algo started");

  classifyChars();

  // dumpWorker();

  int numOfLMSSubstrings = findLMSBorders();

  initBaskets();

  inducedString_.resize (numOfLMSSubstrings);

  inducedSortLMS (lmsSubstrings_);

  exportLMSChars();

  for (Index_t i = 0; i < lmsSubstrings_.size(); i++)
  {
    inducedString_[i] = charInducedCounters_[lmsSubstrings_[i]];
  }

  if (charCounter_ + 1 == lmsSubstrings_.size())
    inducedSuffixArray_ = computeSuffixArray();
  else
  {
    Worker nextLevel = Worker (inducedString_, inducedSuffixArray_, charCounter_ + 1);
    nextLevel.suffixArrayInducedSortingAlgo();
  }

  clearBaskets();
    //std::cout << "Check\n" << std::endl;

  inducedSortSuff (inducedSuffixArray_);
    //std::cout << "Check\n" << std::endl;

  //std::cout << "A\n\n" << std::endl;

  suffArray_.resize (str_.size());
  Index_t index = 0;

  for (Index_t i = 0; i < baskets_.size(); i++)
  {
    for (Index_t j = 0; j < baskets_[i].size(); j++)
    {
      suffArray_[index] = baskets_[i][j];
      index++;
    }
  }
  //std::cout << "AD\n\n" << std::endl;
}

std::vector <Index_t> SuffixArrayInducedSorting::getSuffixArray (const std::string &str) //+
{
  Index_t stringSize = str.size();

  char minChar = CHAR_MAX;
  char maxChar = CHAR_MIN;

  for (Index_t i = 0; i < stringSize; i++)
  {
    if (str[i] < minChar) minChar = str[i];
    if (str[i] > maxChar) maxChar = str[i];
  }

  std::vector <Index_t> normalizedString (stringSize + 1, 0);
  for (Index_t i = 0; i < stringSize; i++)
  {
    normalizedString[i] = str[i] - minChar + 1;
    //std::cout << normalizedString[i];
  }

  //std::cout << "\nSize = " << normalizedString.size() << std::endl;
  //std::cout << "alphabetSize_ = " << maxChar - minChar + 2 << std::endl;

  normalizedString[stringSize] = 0;

  std::vector <Index_t> suffArray;

  Worker sais = Worker (normalizedString, suffArray, maxChar - minChar + 2);

  REPORT ("constructed")

  sais.suffixArrayInducedSortingAlgo();

  return suffArray;
}

void getLcpArrayKasai (std::string &str, std::vector <Index_t> &suffixArray, std::vector <Index_t> &lcpArray) 
{
  Index_t stringSize = str.size();
  Index_t k = 0;

  lcpArray.resize (stringSize);
  std::vector <Index_t> rank (stringSize);

  for (Index_t i = 0; i < stringSize; i++) rank[suffixArray[i]] = i;

  for (Index_t i = 0; i < stringSize; i++, k ? k-- : 0 )
  {
    if (rank[i] == stringSize - 1)
    {
      // lcpArray[str.size() - 1] = UINT32_MAX;
      k = 0;
      continue;
    }

    Index_t j = suffixArray[rank[i] + 1];

    while (i + k < stringSize &&
      j + k < stringSize &&
      str[i + k] == str[j + k]) k++;

    lcpArray[rank[i]] = k;
  }
}

void SuffixArrayInducedSorting::Worker::dumpWorker()
{
  Index_t i = 0;

  for (i = 0; i < charTypes_.size(); i++)
  {
    //std::cout << str_[i] << ' ' << charTypes_[i] << std::endl;
  }
}


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

