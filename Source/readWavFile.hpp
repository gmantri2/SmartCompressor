#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>


using namespace std;

vector<float> readWav(const char* fileName);
void vectToWav(vector<float> input, const char* fileName, const char* newFileName);
vector<pair<float,float> > convertToPairs(vector<float> result);
vector<float> convertBack(vector<pair<float,float> > compressed);
