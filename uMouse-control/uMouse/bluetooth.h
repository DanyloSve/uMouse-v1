#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QTimer>

//#include <QFile>
//#include <QTextStream>
//#include <QDir>

#include <qbluetoothsocket.h>
#include <QAbstractSocket>

class Bluetooth : public QWidget
{
    Q_OBJECT
public:
    explicit Bluetooth(const QString bluetoothAddr, const QString serviceUuid,
                       const int buffTxSize = 1, const int buffRxSize = 1,
                       QWidget *parent = nullptr);
    ~Bluetooth();

    char *getReceivedData();
    void transmitData(char *data, const int size);

    int getBuffTxSize();
    int getBuffRxSize();

private:
    QString      bluetoothAddr;
    QString      serviceUuid;

    QTimer       *connectTimer;

    QBluetoothSocket *socket;

    int buffTxSize;
    int buffRxSize;

    char *buffTx;
    char *buffRx;

    bool buffTxIsBusy;

    void receiveData();
    void clearReceiveBuffer();

signals:
    void newDataReceived(char* buffRx);

private slots:
    void readyRead();
    void connected();
    void disconnected();

};

#endif // BLUETOOTH_H
