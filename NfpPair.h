#pragma once
#include "NestPath.h"
#include "NfpKey.h"

class NfpPair
{
public:
	NfpPair();
	NfpPair(NestPath a, NestPath b, NfpKey key);
	NestPath GetA();
	void SetA(NestPath a);
	NestPath GetB();
	void SetB(NestPath b);
	NfpKey GetKey();
	void SetKey(NfpKey key);

public:
	NestPath m_a;
	NestPath m_b;
	NfpKey m_key;
};