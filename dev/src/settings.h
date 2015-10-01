#ifndef SETTINGS_H
#define SETTINGS_H

#include "libconfig.h++"

using namespace std;
using namespace libconfig;

class Settings
{
    Config *cfg;
    bool load(const char *FileName);
public:
    Settings();
    const Setting& get(const char *Name);
    Config* Cfg();
};

Settings* getSettings();

#endif // SETTINGS_H
