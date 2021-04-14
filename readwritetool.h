#ifndef READWRITETOOL_H
#define READWRITETOOL_H
#include <iostream>
#include <fstream>
#include <QCoreApplication>
template <typename T>
static void write3DVectorToTxt(T vec ,std::string name)
{
    std::ofstream ofs(name);
    for(auto p:vec)
    {
        ofs << p.vertex[0].x() << ' ' <<p.vertex[0].y()<< ' '<<p.vertex[0].x()<<std::endl;
        ofs << p.vertex[1].x() << ' ' <<p.vertex[1].y()<< ' '<<p.vertex[1].z()<<std::endl;
        ofs << p.vertex[2].x() << ' ' <<p.vertex[2].y()<< ' '<<p.vertex[2].z()<<std::endl;
    }
    ofs.close();
};
template <typename T>

static void writeSegmentsTotxt(T vec ,std::string subname)
{
    std::string name=(QCoreApplication::applicationDirPath()+"/Borderfinder/").toStdString();
    name = name + subname + "segs.txt";
    std::ofstream out(name);
    out<<vec.GetSegments()->size();
    for(auto &p:*(vec.GetSegments()))
    {
        out << ' ' << '[' << p.getX() << ',' << p.getY() << ']';
    }
    out<<std::endl;
    out.close();
}
class readWriteTool
{
public:
    readWriteTool();
};

#endif // READWRITETOOL_H
