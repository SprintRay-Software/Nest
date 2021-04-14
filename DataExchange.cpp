#include "DataExchange.h"


DataExchange::DataExchange(Segments p1, Segments p2, double rx, double ry, double angle, double largearc, int sweep, bool flag)
{
    this->p1 = p1;
    this->p2 = p2;
    this->rx = rx;
    this->ry = ry;
    this->angle = angle;
    this->largearc = largearc;
    this->sweep = sweep;
    this->flag = flag;
}

DataExchange::DataExchange(Segments center, double rx, double ry, double theta, double extent, double angle, bool flag)
{
    this->center = center;
    this->rx = rx;
    this->ry = ry;
    this->theta = theta;
    this->extent = extent;
    this->angle = angle;
    this->flag = flag;
}

string DataExchange::toString()
{
	string s = "";
    if (flag) {
        s += " p1 = " + p1.toString() + " p2 = " + p2.toString() + "\n rx = " + to_string(rx) + " ry = " + to_string(ry) + " angle = " + to_string(angle) + " largearc = " + to_string(largearc) + " sweep = " + to_string(sweep);
	}
	else {
        s += " center = " + center.toString() + "\n rx = " + to_string(rx) + " ry = " + to_string(ry) + " theta = " + to_string(theta) + " extent = " + to_string(extent) + " angle = " + to_string(angle);
	}
	return s;
}
