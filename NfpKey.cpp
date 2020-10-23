#include "NfpKey.h"


NfpKey::NfpKey(int a, int b, bool inside, int arotation, int brotation)
{
	m_a = a;
	m_b = b;
	m_inside = inside;
	m_arotation = arotation;
	m_brotation = brotation;
}

NfpKey::NfpKey()
{
}

int NfpKey::GetA()
{
	return m_a;
}

void NfpKey::SetA(int a)
{
	m_a = a;
}

int NfpKey::GetB()
{
	return m_b;
}

void NfpKey::SetB(int b)
{
	m_b = b;
}

bool NfpKey::isInside()
{
	return m_inside;
}

void NfpKey::SetInside(bool inside)
{
	m_inside = inside;
}

int NfpKey::GetArotation()
{
	return m_arotation;
}

void NfpKey::SetArotation(int arotation)
{
	m_arotation = arotation;
}

int NfpKey::GetBrotation()
{
	return m_brotation;
}

void NfpKey::SetBrotation(int brotation)
{
	m_brotation = brotation;
}

