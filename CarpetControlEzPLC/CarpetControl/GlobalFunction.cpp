#include "GlobalFunction.h"

static fstream file;

void DebugFileOpen()
{
    string strFilePath;
    strFilePath=("debug.dat");
    file.open(strFilePath, ios::out);

}

void DebugFileClose()
{
    file.close();
}

void WriteDebugData(string data)
{
    file<<data.c_str() << endl;
}