#ifndef IDATABASE_H
#define IDATABASE_H


class IDatabase {
public:
    virtual void  LogEntry(int,const char *)=0;
};

#endif // IDATABASE_H
