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