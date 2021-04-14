#pragma once

class NfpKey
{
public:
    NfpKey(int a, int b, bool inside, int aRotation, int bRotation);
	NfpKey();
    int getA();
    void setA(int a);
    int getB();
    void setB(int b);
	bool isInside();
    void setInside(bool inside);
    int getARotation();
    void setARotation(int aRotation);
    int getBRotation();
    void setBRotation(int bRotation);

public:
    int a;
    int b;
    bool inside;
    int aRotation;
    int bRotation;
};
