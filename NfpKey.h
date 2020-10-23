#pragma once

class NfpKey
{
public:
	NfpKey(int a, int b, bool inside, int arotation, int brotation);
	NfpKey();
	int GetA();
	void SetA(int a);
	int GetB();
	void SetB(int b);
	bool isInside();
	void SetInside(bool inside);
	int GetArotation();
	void SetArotation(int arotation);
	int GetBrotation();
	void SetBrotation(int brotation);

public:
	int m_a;
	int m_b;
	bool m_inside;
	int m_arotation;
	int m_brotation;
};