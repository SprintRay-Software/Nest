#include "NfpPair.h"

NfpPair::NfpPair()
{
}
NfpPair::NfpPair(NestPath a, NestPath b, NfpKey key)
{
	m_a = a;
	m_b = b;
	m_key = key;
}

NestPath NfpPair::GetA()
{
	return m_a;
}

void NfpPair::SetA(NestPath a)
{
	m_a = a;
}

NestPath NfpPair::GetB()
{
	return m_b;
}

void NfpPair::SetB(NestPath b)
{
	m_b = b;
}

NfpKey NfpPair::GetKey()
{
	return m_key;
}

void NfpPair::SetKey(NfpKey key)
{
	m_key = key;
}

