#include "Gson.h"


Gson::Gson()
{

}

Gson::~Gson()
{

}

string Gson::ToJson(NfpKey key)
{
	m_str = "A:" + to_string(key.m_a) + "Arotation:" + to_string(key.m_arotation) + "B:" + to_string(key.m_b) + "Brotation:" + to_string(key.m_brotation) + "B:";
	if (key.m_inside)
	{
		m_str = m_str + "true";
	}
	else
	{
		m_str = m_str + "false";
	}
	return m_str;
}