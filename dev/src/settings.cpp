#include <iostream>
#include "settings.h"
#include "exception"
#include "string"
#include <libconfig.h++>

using namespace  std;
using namespace libconfig;

static Settings *s = NULL;

Settings* getSettings()
{
    if( s == NULL )
        s = new Settings();
    return s;
}


class SettingNotFoundException : public exception
{
    string s1;
    virtual const char* what() const throw()
    {
        // s1.append("Einstellungen konnten nicht geladen werden");
        return s1.c_str();
    }
public:
    void setReason(string reason){s1=reason;}
    virtual ~SettingNotFoundException() throw() { }
} SettingNotFound;


Config* Settings::Cfg()
{
    return cfg;
}

Settings::Settings()
{
    cfg = new Config();
    if( load("/etc/lxctrl.cfg") == false )
        if( load("/home/georg/dev/lxctrl/dev/lxctrl.cfg") == false)
        {
            SettingNotFound.setReason("Konfigurationsdatei nicht gefunden");
            throw SettingNotFound;
        }
}


const Setting& Settings::get(const char *Name)
{
    const Setting& root = cfg->getRoot();
    return root[Name];
}

bool Settings::load(const char *FileName)
{
    // Read the file. If there is an error, report it and exit.
    try
    {
        cfg->readFile(FileName);
        return true;
    }
    catch(const FileIOException &fioex)
    {
        std::cerr << "I/O error while reading file. (" << FileName <<")"<< std::endl;
    }
    catch(const ParseException &pex)
    {
        std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine() << " - " << pex.getError() << std::endl;
    }



    return false;
}
