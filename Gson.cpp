#include "Gson.h"


Gson::Gson()
{

}

Gson::~Gson()
{

}

string Gson::toJson(NfpKey key)
{
    str = "A:" + to_string(key.a) + "Arotation:" + to_string(key.aRotation) + "B:" + to_string(key.b) + "Brotation:" + to_string(key.bRotation) + "B:";
    if (key.inside)
	{
        str = str + "true";
	}
	else
	{
        str = str + "false";
	}
    return str;
}
