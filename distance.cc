//  Copyright 2013 Google Inc. All Rights Reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include <stdio.h>
#include <string.h>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
using namespace std;

const long long max_size = 2000;         // max length of strings
const long long N = 40;                  // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries

int split(const string& str_val, char delim, vector<string>& results)
{
    const char *p1, *p2;

    p1 = p2 = str_val.c_str();
    results.clear();

    while(*p1 != '\0'){
        while(*p2 != '\0' && *p2 != delim){
            ++p2;
        }
        results.push_back(string(p1, p2));
        p1 = (*p2 == '\0' ? p2 : ++p2);
    }

    return results.size();
}


int main(int argc, char **argv) {
  char st1[max_size];
  char *bestw[N];
  char file_name[max_size], st[100][max_size];
  float dist, len, bestd[N], vec[max_size];
  long long words, size, a, b, c, d, cn, bi[100];
  char ch;
  float *M;
  char *vocab;
  if (argc < 2) {
    printf("Usage: ./distance <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n");
    return 0;
  }
  strcpy(file_name, argv[1]);
  
    
  FILE *fp = NULL;
  char *line_read = NULL;
  size_t line_read_len = 0;
  ssize_t read_size = -1;
  if (!(fp = fopen(file_name, "r"))){
     printf("Input file not found\n");
     return -1;
  }   
  fscanf(fp, "%lld", &words);
  fscanf(fp, "%lld", &size);
  cout<< words <<"\t"<< size;
  vocab = (char *)malloc((long long)words * max_w * sizeof(char));
  for (a = 0; a < N; a++) bestw[a] = (char *)malloc(max_size * sizeof(char));
  M = (float *)malloc((long long)words * (long long)size * sizeof(float));
  if (M == NULL) {
    printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words * size * sizeof(float), words, size);
    return -1;
  }
  
  b=0;
  while ((read_size = getline(&line_read, &line_read_len, fp)) != -1)
  {
        vector<string> vec_str;
        string line = line_read;
        if(split(line, '\t', vec_str) != size+2){
            cout<< vec_str.size()<<"size error"<<b<< endl;
            continue;
        }
        //vocab[b * max_w] = vec_str[0].c_str();
        for (int i = 0; i < vec_str[1].length(); ++i)
            vocab[b * max_w+i] = vec_str[1][i]; 
        for (a = 0; a < size; a++) M[a + b * size] = atof(vec_str[2+a].c_str());
        len = 0;
        for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
        len = sqrt(len);
        for (a = 0; a < size; a++) M[a + b * size] /= len;

        b++;
  }
  cout<<"word count:" << words << " read count:"<<b<<endl;
  cout<<"max_w:" << max_w<<endl;
  for(int j=1932019;j<1932029;j++){
    cout<<endl<<"voacb:"<<j << endl;
    for(int i=0;i<max_w;i++)
         cout<<vocab[j*max_w+i]<<" ";
    }

  if (line_read) {
        free(line_read);
        line_read = NULL;
    }

    if (fp) {
        fclose(fp);
    }
  
  
  while (1) {
    for (a = 0; a < N; a++) bestd[a] = 0;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
    printf("Enter word or sentence (EXIT to break): ");
    a = 0;
    while (1) {
      st1[a] = fgetc(stdin);
      if ((st1[a] == '\n') || (a >= max_size - 1)) {
        st1[a] = 0;
        break;
      }
      a++;
    }
    cout<<"char count:"<<a<<" word:"<<st1<<endl;
    if (!strcmp(st1, "EXIT")) break;
    cn = 0;
    b = 0;
    c = 0;
    while (1) {
      st[cn][b] = st1[c];
      b++;
      c++;
      st[cn][b] = 0;
      if (st1[c] == 0) break;
      if (st1[c] == ' ') {
        cn++;
        b = 0;
        c++;
      }
    }
    cn++;
    cout<<"cn count:"<<cn<<" word:"<<st[0]<<endl;
    for (a = 0; a < cn; a++) {
      for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[a])) break;
      if (b == words) b = -1;
      bi[a] = b;
      printf("\nWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
      if (b == -1) {
        printf("Out of dictionary word!\n");
        break;
      }
    }
    if (b == -1) continue;
    printf("\n                                              Word       Cosine distance\n------------------------------------------------------------------------\n");
    for (a = 0; a < size; a++) vec[a] = 0;
    for (b = 0; b < cn; b++) {
      if (bi[b] == -1) continue;
      for (a = 0; a < size; a++) vec[a] += M[a + bi[b] * size];
    }
    len = 0;
    for (a = 0; a < size; a++) len += vec[a] * vec[a];
    len = sqrt(len);
    for (a = 0; a < size; a++) vec[a] /= len;
    for (a = 0; a < N; a++) bestd[a] = -1;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
    for (c = 0; c < words; c++) {
      a = 0;
      for (b = 0; b < cn; b++) if (bi[b] == c) a = 1;
      if (a == 1) continue;
      dist = 0;
      for (a = 0; a < size; a++) dist += vec[a] * M[a + c * size];
      for (a = 0; a < N; a++) {
        if (dist > bestd[a]) {
          for (d = N - 1; d > a; d--) {
            bestd[d] = bestd[d - 1];
            strcpy(bestw[d], bestw[d - 1]);
          }
          bestd[a] = dist;
          strcpy(bestw[a], &vocab[c * max_w]);
          break;
        }
      }
    }
    for (a = 0; a < N; a++) printf("%50s\t\t%f\n", bestw[a], bestd[a]);
  }
  return 0;
}
