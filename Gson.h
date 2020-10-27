#pragma once
#include "string"
#include "NfpKey.h"

using namespace std;


class Gson
{
public:
	Gson();
	~Gson();
    string toJson(NfpKey key);

public:
    string str;
};
