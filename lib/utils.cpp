#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
const int N = 10000;
void makeNext(const char* str, int* next) {
    int k = -1, j = 0, len = strlen(str);
    next[j] = k;
    while (j < len) {
        if (k == -1 || str[j] == str[k]) {
            next[++j] = ++k;
        } else {
            k = next[k];
        }
    }
}
int findSubStr(const char* str, const char* subStr) {
    int next[N] = {}, i = 0, j = 0, lenStr = strlen(str), lenSubStr = strlen(subStr);
    makeNext(subStr, next);
    while (i < lenStr && j < lenSubStr) {
        if (j == -1 || str[i] == subStr[j]) {
            j++;
            i++;
        } else {
            j = next[j];
        }
    }
    if (j == lenSubStr) return i - j;
    return -1;
}

int subStrCount(const char* str, const char* subStr) {
  int result = 0, i= 0, j = 0, strLen = strlen(str), subStrLen = strlen(subStr);
  int next[N] = {};
  makeNext(subStr, next);
  while(i < strLen) {
    if (j == -1 || str[i] == subStr[j]) {
      i++;
      j++;
    } else {
      j = next[j];
    }
    if (j == subStrLen) {
      result ++;
      j = next[j];
    }
  }
  return result;
}


