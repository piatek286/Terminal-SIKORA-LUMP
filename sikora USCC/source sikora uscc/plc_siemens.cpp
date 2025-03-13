#include "plc_siemens.h"

PLC_Siemens::PLC_Siemens(QString ipAddress)
{
    int i = 0;
    bool zero = false;
    do{

        if(ipAddress[i] == '.'){
            zero = true;//sprawdzaj czy zero
            this->ipAddress += ipAddress[i];
        }
        else if(zero && ipAddress[i] != '0'){
            zero = false;
            this->ipAddress += ipAddress[i];
        }
        else if (!zero) {
            this->ipAddress += ipAddress[i];
        }

         i++;
    }while(i<15);

    MyS7Client = new TS7Client();
    qDebug()<< "Adress1"<<ipAddress;
    qDebug()<< "Adress2"<<this->ipAddress.toStdString().c_str();
    //connect();
}

PLC_Siemens::PLC_Siemens(QString ipAddress, uint DB_No, uint DB_Length)
{

    int i = 0;
    bool zero = false;
    do{

        if(ipAddress[i] == '.'){
            zero = true;//sprawdzaj czy zero
            this->ipAddress += ipAddress[i];
        }
        else if(zero && ipAddress[i] != '0'){
            zero = false;
            this->ipAddress += ipAddress[i];
        }
        else if (!zero) {
            this->ipAddress += ipAddress[i];
        }

         i++;
    }while(i<15);

    MyS7Client = new TS7Client();
    this->ipAddress = ipAddress;
    this->DB_No= DB_No;
    this->DB_Length = DB_Length;
    //connect();
}

bool PLC_Siemens::connect()
{
    if ( !MyS7Client->Connected() )
    {
        //qDebug()<< "no connect";
        //qDebug()<< QString::number(MyS7Client->Connected());
        if(MyS7Client->ConnectTo(ipAddress.toStdString().c_str(), 0, 0))return false;
        return true;
    }
    else {
        //qDebug()<< "connect";
        //qDebug()<< QString::number(MyS7Client->Connected());
        return MyS7Client->Connected();
    }
}


bool PLC_Siemens::disConnect()
{
    if ( MyS7Client->Connected() ){
        if(MyS7Client->Disconnect())return false;
        return true;
    }
    return true;
}

bool PLC_Siemens::WriteCycle(uint dbNum, uint offset, uint size, unsigned char *buffer)
{
    if(connect()){
        if (MyS7Client->DBWrite(dbNum, offset, size, buffer))
        {
            MyS7Client->Disconnect();
            return false;
        }
        //qDebug()<< "write";
        return true;
    }
    return false;
}

bool PLC_Siemens::ReadCycle(uint dbNum, uint offset, uint size, unsigned char *buffer)
{
    if(connect()){
        if (MyS7Client->DBRead(dbNum, offset, size, buffer))
        {
            //qDebug()<< "Error read from DB:" << "2 "<<QString::number(res);
            MyS7Client->Disconnect();
            return false;
        }
        //qDebug()<< "read";
        return true;
    }
    return false;
}

uint PLC_Siemens::getUInt16(byte *Buffer, uint Pos)
{
    return ((Buffer[Pos] << 8) | Buffer[Pos + 1]);
}

int PLC_Siemens::getInt16(byte *Buffer, uint Pos)
{
    return ((Buffer[Pos] << 8) | Buffer[Pos + 1]);
}

uint PLC_Siemens::getUInt32(byte* Buffer, uint Pos)
{
    uint Result;
    Result = Buffer[Pos]; Result <<= 8;
    Result |= Buffer[Pos + 1]; Result <<= 8;
    Result |= Buffer[Pos + 2]; Result <<= 8;
    Result |= Buffer[Pos + 3];
    return Result;
}

int PLC_Siemens::getInt32(byte *Buffer, uint Pos)
{
    int Result;
    Result = Buffer[Pos]; Result <<= 8;
    Result += Buffer[Pos + 1]; Result <<= 8;
    Result += Buffer[Pos + 2]; Result <<= 8;
    Result += Buffer[Pos + 3];
    return Result;
}

double PLC_Siemens::getReal(byte *Buffer, uint Pos)
{
    uint dw = getUInt32(Buffer,Pos);
    return *(pfloat(&dw));
}

/*
int PLC_Siemens::WriteVal(dataType typ, int dbNum, int offset, int val)
{
    QMutexLocker locker(&mutex);
    if (MyS7Client->Connected())
    {
        if (typ == eByte )
        {
            int res = MyS7Client->DBWrite(dbNum, offset,1,&val);
            qDebug()<< "Write to DB" << dbNum <<".DBB"<<offset << ", res= " <<QString::number(res);
            return res;
        }
        else
        {
            return false;
        }
    }
    else
    {
        //connect();
        return false;
    }
}
*/
/*
void PLC_Siemens::run()
{
    while(true)
    {

        if (MyS7Client->Connected())
        {
            QMutexLocker locker(&mutex);
            ReadCycle();
        }
        else {
            connect();
        }
        this->sleep(ulong(0.1*10e-6));// 100000us=100ms
        //qDebug() << "przerwanie";
    }
}
*/
