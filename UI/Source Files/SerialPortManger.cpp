#include "SerialPortManger.h"

SerialPortManager::SerialPortManager(QObject *parent)
    : QObject(parent)
{
    // Kết nối signal readyRead (QSerialPort) với slot handleReadyRead() (khi có dữ liệu nhận về)
    connect(&m_serialPort, &QSerialPort::readyRead, this, &SerialPortManager::handleReadyRead);

    // tạo đối tượng (lấy danh sách cổng ban đầu)
    refreshPorts();
}

QStringList SerialPortManager::availablePorts() const
{
    return m_availablePorts;
}

bool SerialPortManager::isOpen() const
{
    return m_serialPort.isOpen();
}

QString SerialPortManager::lastError() const
{
    return m_lastError;
}

void SerialPortManager::refreshPorts() {
    QStringList ports;

    // Lấy danh sách cổng COM hiện có (qua QSerialPortInfo)
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts())
    {
        ports << info.portName();
    }

    // Nếu danh sách thay đổi thì cập nhật và phát signal
    if (ports != m_availablePorts)
    {
        m_availablePorts = ports;
        emit availablePortsChanged();
    }
}

void SerialPortManager::openPort(const QString &portName, int baudRate, int dataBits, int parity)
{
    // Đóng cổng cũ nếu đang mở
    if (m_serialPort.isOpen())
        m_serialPort.close();

    // setup thông số
    m_serialPort.setPortName(portName);
    m_serialPort.setBaudRate(baudRate);
    m_serialPort.setDataBits(static_cast<QSerialPort::DataBits>(dataBits));
    m_serialPort.setParity(static_cast<QSerialPort::Parity>(parity));

    if (m_serialPort.open(QIODevice::ReadWrite))
    {
        m_lastError.clear();
        emit isOpenChanged();
        emit lastErrorChanged();
    }
    else
    {
        m_lastError = m_serialPort.errorString(); // Lưu thông báo lỗi
        emit lastErrorChanged();
        emit isOpenChanged();
    }
}

// Đóng port, cập nhật trạng thái
void SerialPortManager::closePort()
{
    if (m_serialPort.isOpen())
    {
        m_serialPort.close();
        emit isOpenChanged();
    }
}

// Khi có dữ liệu đến (readyRead), đọc toàn bộ data, phát signal lên QML
void SerialPortManager::handleReadyRead()
{
    // QByteArray data = m_serialPort.readAll();
    // emit receivedRawData(QString::fromUtf8(data));
    // parseData(data);

    QByteArray data = m_serialPort.readAll();
    m_receiveBuffer.append(data);

    // Giả sử mỗi message luôn kết thúc bằng "_END"
    while (1)
    {
        int endIdx = m_receiveBuffer.indexOf("_END");

        if (endIdx == -1)
            break; // chưa đủ message

        // Lấy trọn message (kể cả "_END"), cắt ra khỏi buffer
        QByteArray oneMsg = m_receiveBuffer.left(endIdx + 4); // "_END" = 4 bytes
        m_receiveBuffer.remove(0, endIdx + 4);

        // Chuyển sang QString và emit lên QML
        emit receivedRawData(QString::fromUtf8(oneMsg));
        parseData(oneMsg);
    }
}

// Chỉ định định dạng để xử lý dữ liệu nhận được
void SerialPortManager::parseData(const QByteArray& data)
{
    QString strData = QString::fromUtf8(data);

    // Sử dụng regex phù hợp với format mới
    QRegularExpression rx("DATA_T=(\\d+\\.?\\d*),H=(\\d+\\.?\\d*),C=(\\d+\\.?\\d*),DAY=(\\d+),DATE=(\\d+/\\d+/\\d+),TIME=(\\d+/\\d+/\\d+)_END");
    QRegularExpressionMatch match = rx.match(strData);

    if (match.hasMatch())
    {
        double temperature = match.captured(1).toDouble();
        double humidity = match.captured(2).toDouble();
        double co2 = match.captured(3).toDouble();
        QString day = match.captured(4);
        QString date = match.captured(5);
        QString time = match.captured(6).replace("/", ":");

        emit receivedValues(temperature, humidity, co2, time, day, date);
    }
}
