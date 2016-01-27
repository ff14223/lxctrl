#ifndef DBOBJECT_H
#define DBOBJECT_H


class DbObject
{
    long m_objectid;
    bool m_isnew;
public:
    bool getIsNew(){return m_isnew;}
    void setIsNew(bool value){m_isnew=value;}
    long getObjectId(){return m_objectid;}
    DbObject(long id) { m_objectid = id;}
};

#endif // DBOBJECT_H
