//
// Created by 神奇bug在哪里 on 7/15/23.
//

#ifndef SIMPLELABMANAGER_DATABASECORE_H
#define SIMPLELABMANAGER_DATABASECORE_H
#include <string>
#include <utility>


class databaseCore {
private:
    std::string filename;
public:
    void setFilename(std::string newFilename){filename = std::move(newFilename);}
    std::string getFilename(){return filename;}
    virtual void write2File() = 0;
    virtual void readFromFile() = 0;
};


#endif //SIMPLELABMANAGER_DATABASECORE_H
