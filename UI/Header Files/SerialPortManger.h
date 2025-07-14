#ifndef SERIALPORTMANGER_H
#define SERIALPORTMANGER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QRegularExpression>

class SerialPortManager : public QObject
{
        Q_OBJECT

        // danh sách tên các cổng COM
        Q_PROPERTY(QStringList availablePorts READ availablePorts NOTIFY availablePortsChanged)

        // trạng thái kết nối
        Q_PROPERTY(bool isOpen READ isOpen NOTIFY isOpenChanged)

        // trạng thái lỗi
        Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)

    public:
        // constructor - truyền parent để quản lý memory
        explicit SerialPortManager(QObject *parent = nullptr);

        // cập nhật lại danh sách cổng
        Q_INVOKABLE void refreshPorts();

        // mở cổng serial theo cấu hình
        Q_INVOKABLE void openPort(const QString &portName, int baudRate, int dataBits, int parity);

        // ngắt kết nối
        Q_INVOKABLE void closePort();

        QStringList availablePorts() const;
        bool isOpen() const;
        QString lastError() const;

    signals:    // khai báo các signals sẽ được phát

        // Khi danh sách cổng thay đổi
        void availablePortsChanged();

        // Khi trạng thái kết nối thay đổi
        void isOpenChanged();
        void lastErrorChanged();

        // Khi nhận được dữ liệu
        void receivedValues(double temperature, double humidity, double co2, QString time, QString day, QString date);
        void receivedRawData(const QString &data);

    public slots:   // slot xử lý khi serial có data nhận về
        void handleReadyRead();

    private:
        QSerialPort m_serialPort;       // instance QSerialPort
        QStringList m_availablePorts;   // danh sách các cổng COM
        QString m_lastError;
        QByteArray m_receiveBuffer;
        void parseData(const QByteArray& data);
};


#endif // SERIALPORTMANGER_H
