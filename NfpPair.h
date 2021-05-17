#pragma once
#include "NestPath.h"
#include "NfpKey.h"

class NfpPair
{
public:
	NfpPair();
	NfpPair(NestPath a, NestPath b, NfpKey key);
    NestPath getA();
    void setA(NestPath a);
    NestPath getB();
    void setB(NestPath b);
    NfpKey getKey();
    void setKey(NfpKey key);

public:
    NestPath a;
    NestPath b;
    NfpKey key;
};
