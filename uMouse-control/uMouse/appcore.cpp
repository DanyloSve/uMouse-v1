#include "appcore.h"

AppCore *AppCore::m_pThis = nullptr;

AppCore::AppCore(QObject *parent) : QObject(parent)
{
    //const QString hc05addr    {"E8:EB:11:0A:57:C3"};
    const QString hc05addr    {"00:18:E5:04:9E:F4"};
    const QString serviceUuid {"00001101-0000-1000-8000-00805F9B34FB"};

    const int buffTxSize {3};
    const int buffRxSize {6};

    HC05 = new Bluetooth(hc05addr, serviceUuid, buffTxSize, buffRxSize);
    buffTx = new char[buffTxSize];

    speed         = 0;
    turningSpeed  = 0;
    buttonId      = 0;

    angle           = 0;
    distance        = 0;
    motorSpeed      = 0;
    battery         = 0;

    sensorLS = 0;
    sensorLF = 0;
    sensorRF = 0;
    sensorRS = 0;

    zeroingAngle    = 0;
    zeroingDistance = 0;
    turningMode = 0;

    connect(HC05, SIGNAL(newDataReceived(char*)), this, SLOT(updateUi(char*)));
}

AppCore::~AppCore()
{
    delete HC05;
}

QObject *AppCore::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return AppCore::instance();
}

AppCore *AppCore::instance()
{
    if (!m_pThis) // avoid creation of new instances
    {
        m_pThis = new AppCore;
    }

    return m_pThis;
}

void AppCore::setCntr(const int number)
{
    this->cntr = number;
    emit cntrChanged();
}

void AppCore::cntrInc(const int number)
{
    this->cntr++;
    emit cntrChanged();
}

void AppCore::uiChangedSpeed(const int value)
{
    speed = value;
    transmitData();
}

void AppCore::uiChangedTurningSpeed(const int value)
{
    turningSpeed = value;
    transmitData();
}

void AppCore::uiPressedControllButtons(const int buttonId)
{
    this->buttonId = buttonId;
    transmitData();
}

void AppCore::uiPressedZeroingAngle(const bool value)
{
    zeroingAngle = 1;
    transmitData();
}

void AppCore::uiPressedZeroingDistance(const bool value)
{
    zeroingDistance = 1;
    transmitData();
}

void AppCore::uiChangedTurningMode(const bool value)
{
    turningMode = value;
}

void AppCore::updateUi(char *data)
{
    sensorRS = (data[0] >> 7) & 0x01;
    sensorRF = (data[0] >> 6) & 0x01;
    sensorLF = (data[0] >> 5) & 0x01;
    sensorLS = (data[0] >> 4) & 0x01;

    battery          =  ((double)((uint16_t)(( (uint16_t)( (uint8_t)data[0] & 0x0F ) << 8 ) | ( (uint8_t)data[1] ) )))* ((3.3 / 4095.0))* 3.0;

    angle            =  (double)((int16_t)((uint16_t)(((uint16_t) ( (uint8_t)data[2] ) << 8 ) | ( (uint8_t)data[3] ))));
    //angularVelocity  =  ( (uint16_t)( (uint8_t)data[4] ) << 8 ) | ( (uint8_t)data[5] );
    distance         =  (double)((int16_t)(( (uint16_t)( (uint8_t)data[4] ) << 8 ) | ( (uint8_t)data[5] ))) / (6.0 * 51.45) * 0.032*3.141592;

    motorSpeed = (distance - prevDistance) / 0.04;
    prevDistance = distance;

    emit uiUpdateAngle();
    emit uiUpdateDistance();
    emit uiUpdateMotorSpeed();
    emit uiUpdateBattery();

    emit uiUpdateSensorLS();
    emit uiUpdateSensorLF();
    emit uiUpdateSensorRF();
    emit uiUpdateSensorRS();

}

void AppCore::transmitData()
{

    switch(buttonId)
    {
    case 0:
    {
        buffTx[0] = speed * 1.1333; // normalize speed to run astraight line
        buffTx[1] = speed * 0.96;   // normalize speed to run astraight line
    }
        break;

    case 1:
    {
        if (turningMode == TURNING_LINEAR)
        {
            buffTx[0] = (speed + turningSpeed) % 100;
            buffTx[1] = (speed - turningSpeed) % 100;

            if (buffTx[0] == buffTx[1])
            {
                buffTx[0]++;
            }
        }
        else
        {
            buffTx[0] = (speed +  turningSpeed) % 100;
            buffTx[1] = -buffTx[0];

        }

    }
        break;

    case 2:
    {
        buffTx[0] = -speed * 0.9; // normalize speed to run astraight line
        buffTx[1] = -speed;
    }
        break;
    case 3:
    {
        if (turningMode == TURNING_LINEAR)
        {
            buffTx[0] = (speed -  turningSpeed) % 100;
            buffTx[1] = (speed +  turningSpeed) % 100;

            if (buffTx[0] == buffTx[1])
            {
                buffTx[0]++;
            }
        }
        else
        {
            buffTx[1] = (speed + turningSpeed) % 100;
            buffTx[0] = -buffTx[1];
        }
    }
        break;

    case -1:
    default:{
        buffTx[0] = 0;
        buffTx[1] = 0;
    }
    }

    buffTx[2] = (zeroingAngle ? 0x01 : 0x00) | (zeroingDistance ? 0x02 : 0x00);

    zeroingAngle = 0;
    zeroingDistance = 0;

    HC05->transmitData(buffTx, HC05->getBuffTxSize());
}

int AppCore::getCntr() const
{
    return cntr;
}



