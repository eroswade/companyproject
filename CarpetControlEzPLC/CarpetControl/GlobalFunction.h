#pragma once

#ifndef GLOBALFUNCTION_H
#define GLOBALFUNCTION_H


#include<fstream>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

extern void WriteDebugData(string data);
extern void DebugFileOpen();
extern void DebugFileClose();

#endif