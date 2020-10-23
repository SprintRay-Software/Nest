#pragma once
#include "string"
#include "NfpKey.h"

using namespace std;


class Gson
{
public:
	Gson();
	~Gson();
	string ToJson(NfpKey key);

public:
	string m_str;
};