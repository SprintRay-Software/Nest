#include "NfpPair.h"

NfpPair::NfpPair()
{
}
NfpPair::NfpPair(NestPath a, NestPath b, NfpKey key)
{
    this->a = a;
    this->b = b;
    this->key = key;
}

NestPath NfpPair::getA()
{
    return a;
}

void NfpPair::setA(NestPath a)
{
    this->a = a;
}

NestPath NfpPair::getB()
{
    return b;
}

void NfpPair::setB(NestPath b)
{
    this->b = b;
}

NfpKey NfpPair::getKey()
{
    return key;
}

void NfpPair::setKey(NfpKey key)
{
    this->key = key;
}

