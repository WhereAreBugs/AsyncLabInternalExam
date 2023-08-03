//
// Created by 神奇bug在哪里 on 8/2/23.
//

#ifndef SIMPLELABMANAGER_SEATDATA_H
#define SIMPLELABMANAGER_SEATDATA_H

#include <mutex>
#include "databaseCore.h"

class seatData : public databaseCore {
public:
    enum seatStatus{
        empty = 0,
        fixed = 1,
        flexible = 2
    };
    struct seat{
        int id;
        enum seatStatus status;
        std::string username;
        time_t  rubbishTime = 0;
    };
private:
    std::vector<seat> data;
    std::mutex globalVectorMutex;
protected:
    std::string filename = "seatData.dat";

public:
    void setSeatStatus(int id, enum seatStatus status);
    void setSeatUsername(int id, std::string username);
    void addSeat(int id, enum seatStatus status);
    void removeSeat(int id);
    void write2File() override;
    void readFromFile() override;
    enum seatStatus getSeatStatus(int id);
    enum seatStatus getSeatStatus(const std::string &username);
    time_t getRubbishTime(std::string name);
    bool ResetSeatStatus(const std::string& name);
    bool bindSeat(const std::string& name, int id);
    bool unbindSeat(const std::string& name);
};


#endif //SIMPLELABMANAGER_SEATDATA_H
