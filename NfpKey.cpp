#include "NfpKey.h"


NfpKey::NfpKey(int a, int b, bool inside, int aRotation, int bRotation)
{
    this->a = a;
    this->b = b;
    this->inside = inside;
    this->aRotation = aRotation;
    this->bRotation = bRotation;
}

NfpKey::NfpKey()
{
}

int NfpKey::getA()
{
    return a;
}

void NfpKey::setA(int a)
{
    this->a = a;
}

int NfpKey::getB()
{
    return b;
}

void NfpKey::setB(int b)
{
    this->b = b;
}

bool NfpKey::isInside()
{
    return inside;
}

void NfpKey::setInside(bool inside)
{
    this->inside = inside;
}

int NfpKey::getARotation()
{
    return aRotation;
}

void NfpKey::setARotation(int aRotation)
{
    this->aRotation = aRotation;
}

int NfpKey::getBRotation()
{
    return bRotation;
}

void NfpKey::setBRotation(int bRotation)
{
    this->bRotation = bRotation;
}

