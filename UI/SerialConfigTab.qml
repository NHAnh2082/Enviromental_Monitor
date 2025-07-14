import QtQuick
import QtQuick.Controls

Item
{
    property bool darkMode: false

    // Biến QML để lưu dữ liệu nhận được
    property string receivedData: "No Data Received"

    Column
    {
        spacing: 10
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 16

        ComboBox
        {
            id: portCombo
            width: 180
            model: serialManager.availablePorts
            onActivated: serialManager.refreshPorts()
        }

        ComboBox
        {
            id: baudCombo
            width: 180
            model: [9600, 19200, 38400, 57600, 115200]
            currentIndex: 4
        }

        ComboBox
        {
            id: dataBitsCombo
            width: 180
            model: [7, 8]
            currentIndex: 0
        }

        ComboBox
        {
            id: parityCombo
            width: 180
            model: ["None", "Even", "Odd"]
            currentIndex: 0
        }

        // ICON STATUS
        Rectangle
        {
            width: 18
            height: 18
            radius: 9
            color: serialManager.isOpen ? "#4CAF50" : (serialManager.lastError ? "#FF5252" : "#BDBDBD")
            border.color: "#666"

            Text
            {
                anchors.centerIn: parent
                text: serialManager.isOpen ? "\u2714" : (serialManager.lastError ? "\u26A0" : "\u26AB")
                color: "#fff"
                font.pixelSize: 13
            }

            ToolTip.text: serialManager.isOpen ? "Đã kết nối" : (serialManager.lastError ? "Lỗi: " + serialManager.lastError : "Chưa kết nối")
        }

        Row
        {
            spacing: 8

            Button
            {
                // load thông tin hiển thị dựa vào việc kết nối hay chưa
                text: serialManager.isOpen ? "Disconnect" : "Connect"

                onClicked:
                {
                    // nếu đang là "Connect" thì nhấn vào sẽ thành "Disconnect" và ngược lại
                    if (serialManager.isOpen)
                    {
                        serialManager.closePort()
                    }
                    else
                    {
                        // mapping để lấy thông tin kết nối
                        serialManager.openPort(
                            portCombo.currentText,
                            parseInt(baudCombo.currentText),
                            parseInt(dataBitsCombo.currentText) === 8 ? 8 : 7,
                            parityCombo.currentIndex
                        )
                    }
                }
            }

            // nút nhấn cập nhật lại danh sách cổng COM
            Button
            {
                text: "Refresh"
                onClicked: serialManager.refreshPorts()
            }
        }

        // Dòng trạng thái kết nối
        Text
        {
            color: serialManager.isOpen ? "green" : "red"
            text: serialManager.isOpen
                ? "Connect Successfully"
                : (serialManager.lastError.length > 0 ? serialManager.lastError : "Not connected")
        }

        // Dữ liệu nhận được, hiện phía dưới dòng trạng thái để quan sát
        Text
        {
            id: receivedText
            text: receivedData
            color: "#222266"
            font.pointSize: 11
            wrapMode: Text.WordWrap
        }
    }

    // Lắng nghe signal nhận dữ liệu gốc từ SerialPortManager
    Connections
    {
        target: serialManager

        function onReceivedRawData(data)
        {
            receivedData = data.length > 0 ? data : "No Data Received"
            console.log(receivedData);
        }

        // Khi ngắt kết nối thì reset dữ liệu
        function onIsOpenChanged()
        {
            if (!serialManager.isOpen)
            {
                receivedData = "No Data Received"
            }
        }
    }
}
