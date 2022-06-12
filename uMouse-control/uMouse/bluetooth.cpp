#include "bluetooth.h"

Bluetooth::Bluetooth(const QString bluetoothAddr, const QString serviceUuid,
                     const int buffTxSize, const int buffRxSize,
                     QWidget *parent) : QWidget{parent}
{
    this->bluetoothAddr = bluetoothAddr;
    this->serviceUuid = serviceUuid;

    this->buffTxSize = buffTxSize;
    this->buffRxSize = buffRxSize;

    buffTx = new char[buffTxSize];
    buffRx = new char[buffRxSize];

    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    connect(socket, &QBluetoothSocket::readyRead, this, &Bluetooth::readyRead);
    socket->connectToService(QBluetoothAddress(bluetoothAddr), QBluetoothUuid(serviceUuid), QIODevice::ReadWrite);

    connectTimer = new QTimer(this);


    connect(socket, &QBluetoothSocket::readyRead,    this, &Bluetooth::readyRead);
    connect(socket, &QBluetoothSocket::disconnected, this, &Bluetooth::disconnected);
    connect(socket, &QBluetoothSocket::connected,    this, &Bluetooth::connected);

}

Bluetooth::~Bluetooth()
{
    delete[] buffTx;
    delete[] buffRx;
}

char *Bluetooth::getReceivedData()
{
    return buffRx;
}

void Bluetooth::transmitData(char *data, const int size)
{
    buffTxIsBusy = 1;

    for (int i{0}; i < buffTxSize; ++i)
    {
        buffTx[i] = data[i];

        if (i <= size)
        {
            buffTx[i] = data[i];
        }
        else
        {
            buffTx[i] = 0;
        }
    }
    socket->write(buffTx, buffTxSize);
}

int Bluetooth::getBuffTxSize()
{
    return buffTxSize;
}

int Bluetooth::getBuffRxSize()
{
    return buffRxSize;
}



void Bluetooth::receiveData()
{
    socket->read(buffRx, buffRxSize);
    emit newDataReceived(buffRx);
}

void Bluetooth::clearReceiveBuffer()
{
    char ch;
    int bytesAvailabele = socket->bytesAvailable();

    for (int i{bytesAvailabele}; i >  buffRxSize; --i)
    {
        socket->read(&ch, 1);
    }
}

void Bluetooth::readyRead()
{
    static qint64 cntr = 0;

    if (socket->bytesAvailable() > buffRxSize)
    {
        clearReceiveBuffer();
        cntr = 0;
    }

    qint64 bytesAvailable = socket->bytesAvailable();
    qDebug() << "cntr" << cntr << bytesAvailable;


    qDebug() << "cntr" << cntr << bytesAvailable ;

    socket->read(&buffRx[cntr], bytesAvailable);
    (cntr > buffRxSize)?(cntr = bytesAvailable):(cntr += bytesAvailable);


    qDebug() << (int)buffRx[0]<< (int)buffRx[1]<< (int)buffRx[2]
            << (int)buffRx[3]<<(int) buffRx[4]<< (int)buffRx[5]
            << (int)buffRx[6]<< (int)buffRx[7] << "\n";


    if (cntr == buffRxSize)
    {
        cntr = 0;

        emit newDataReceived(buffRx);


         for (int i{0}; i < buffRxSize; i++)
         {
             buffRx[0] = '0';
         }
    }
}



void Bluetooth::connected()
{
    connectTimer->stop();
}

void Bluetooth::disconnected()
{
    connectTimer->start(50);
}

