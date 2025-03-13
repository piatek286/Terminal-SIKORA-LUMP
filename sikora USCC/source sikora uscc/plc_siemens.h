#ifndef PLC_SIEMENS_H
#define PLC_SIEMENS_H

#include "snap7.h"
#include <QThread>
#include <QDebug>
#include <QString>
#include <QMutex>

class PLC_Siemens : public QThread
{


    typedef float *pfloat;

public:
    enum dataType {eBit, eByte, eInt, eDInt, eWord, eDowrd, eReal};
    PLC_Siemens(QString ipAddress);
    PLC_Siemens(QString ipAddress, uint DB_No, uint DB_Length);
    //
    bool connect();
    bool disConnect();
    bool ReadCycle(uint dbNum, uint offset, uint size, unsigned char *buffer);
    bool WriteCycle(uint dbNum, uint offset, uint size, unsigned char *buffer);
    //
    //QMutex mutex;

    // Helper functions
    uint getUInt16(byte* Buffer, uint Pos);

    int getInt16(byte* Buffer, uint Pos);

    uint getUInt32(byte* Buffer, uint Pos);

    int getInt32(byte* Buffer, uint Pos);

    double getReal(byte* Buffer, uint Pos);
    //
    QString ipAddress;
    int DB_No=2;
    int DB_Length=5;

    TS7Client *MyS7Client;
    byte DB_Buffer[200];

protected:
    //void run();
private:
    int old_value;
};

#endif // PLC_SIEMENS_H
