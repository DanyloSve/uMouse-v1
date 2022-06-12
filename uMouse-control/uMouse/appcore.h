#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <QDebug>

#include <QQmlEngine>
#include <QJSEngine>

#include <QString>

#include <stdint.h>
#include "bluetooth.h"


class AppCore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int buttonId      MEMBER buttonId     WRITE uiPressedControllButtons)
    Q_PROPERTY(int speed         MEMBER speed        WRITE uiChangedSpeed)
    Q_PROPERTY(int turningSpeed  MEMBER turningSpeed WRITE uiChangedTurningSpeed)
    Q_PROPERTY(bool turningMode       MEMBER  turningMode    WRITE uiChangedTurningMode)
    Q_PROPERTY(bool zeroingAngle       MEMBER  zeroingAngle    WRITE uiPressedZeroingAngle)
    Q_PROPERTY(bool zeroingDistance    MEMBER  zeroingDistance WRITE uiPressedZeroingDistance)


    Q_PROPERTY(int     angle      MEMBER  angle      NOTIFY uiUpdateAngle)
    Q_PROPERTY(double  distance   MEMBER  distance   NOTIFY uiUpdateDistance)
    Q_PROPERTY(double  motorSpeed MEMBER  motorSpeed NOTIFY uiUpdateMotorSpeed)
    Q_PROPERTY(double  battery    MEMBER  battery    NOTIFY uiUpdateBattery)


    Q_PROPERTY(bool sensorLS  MEMBER  sensorLS     NOTIFY uiUpdateSensorLS)
    Q_PROPERTY(bool sensorLF  MEMBER  sensorLF     NOTIFY uiUpdateSensorLF)
    Q_PROPERTY(bool sensorRF  MEMBER  sensorRF     NOTIFY uiUpdateSensorRF)
    Q_PROPERTY(bool sensorRS  MEMBER  sensorRS     NOTIFY uiUpdateSensorRS)

public:
    static QObject *qmlInstance(QQmlEngine *engine,  QJSEngine *scriptEngine);
    static AppCore *instance();

    int  getCntr() const;
    void setCntr(const int number);

public slots:
    void cntrInc(const int number);
    void uiChangedSpeed           (const int value);
    void uiChangedTurningSpeed    (const int value);
    void uiPressedControllButtons (const int buttonId);
    void uiPressedZeroingAngle    (const bool value);
    void uiPressedZeroingDistance (const bool value);
    void uiChangedTurningMode     (const bool value);

    void updateUi                 (char *data);

signals:
    void cntrChanged();

    void uiUpdateAngle();
    void uiUpdateDistance();
    void uiUpdateMotorSpeed();
    void uiUpdateBattery();

    void uiUpdateSensorLS();
    void uiUpdateSensorLF();
    void uiUpdateSensorRF();
    void uiUpdateSensorRS();

private:
    explicit AppCore(QObject *parent = nullptr);
    static AppCore *m_pThis;

    ~AppCore();

    void transmitData();

    Bluetooth *HC05;
    char      *buffTx;
    int        cntr;

    int speed;
    int turningSpeed;
    int buttonId;

    int    angle;
    double distance;
    double prevDistance;
    double motorSpeed;
    double battery;

    bool sensorLS;
    bool sensorLF;
    bool sensorRF;
    bool sensorRS;

    bool zeroingAngle;
    bool zeroingDistance;
    bool turningMode;

    enum Turning
    {
      TURNING_LINEAR,
      TURNING_AXIAL
    };
};

#endif // APPCORE_H
