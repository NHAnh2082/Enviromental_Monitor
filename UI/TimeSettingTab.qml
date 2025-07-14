import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item
{
    id: root
    property bool darkMode: false

    // Properties cho các trường nhập liệu (có thể binding ngược nếu muốn)
    property int hour: 12
    property int minute: 0
    property int second: 0
    property int day: 1
    property int month: 1
    property string year: "2025"

    Rectangle
    {
        anchors.centerIn: parent
        width: 370
        height: 230
        radius: 18
        color: darkMode ? "#21222a" : "#f7f7f7"
        border.color: darkMode ? "#333" : "#bbb"
        border.width: 2

        ColumnLayout
        {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 18

            Text
            {
                text: "Cài đặt thời gian"
                font.pixelSize: 22
                font.bold: true
                color: darkMode ? "#fff" : "#1e293b"
                Layout.alignment: Qt.AlignHCenter
            }

            RowLayout
            {
                spacing: 10
                Layout.alignment: Qt.AlignHCenter

                // Giờ, phút, giây
                SpinBox
                {
                    id: sbHour
                    from: 0
                    to: 23
                    value: root.hour
                    width: 52
                    onValueChanged: root.hour = value
                }

                Text
                {
                    text: ":"
                    font.pixelSize: 19
                    color: darkMode ? "#eee" : "#222"
                }

                SpinBox
                {
                    id: sbMin
                    from: 0
                    to: 59
                    value: root.minute
                    width: 52
                    onValueChanged: root.minute = value
                }

                Text
                {
                    text: ":"
                    font.pixelSize: 19
                    color: darkMode ? "#eee" : "#222"
                }

                SpinBox
                {
                    id: sbSec
                    from: 0
                    to: 59
                    value: root.second
                    width: 52
                    onValueChanged: root.second = value
                }

                Text
                {
                    text: "Giờ:phút:giây"
                    font.pixelSize: 12
                    color: "#888"
                    leftPadding: 8
                }
            }

            RowLayout
            {
                spacing: 10
                Layout.alignment: Qt.AlignHCenter

                ComboBox
                {
                    id: cbDay
                    width: 56
                    model: Array(31).fill(0).map((_,i)=>(i+1).toString())
                    currentIndex: root.day - 1
                    onCurrentTextChanged: root.day = parseInt(currentText)
                }

                Text { text: "/"; font.pixelSize: 16 }

                ComboBox
                {
                    id: cbMonth
                    width: 56
                    model: Array(12).fill(0).map((_,i)=>(i+1).toString())
                    currentIndex: root.month - 1
                    onCurrentTextChanged: root.month = parseInt(currentText)
                }

                Text { text: "/"; font.pixelSize: 16 }

                ComboBox
                {
                    id: cbYear
                    width: 68
                    model: ["2024", "2025", "2026", "2027"]
                    currentIndex: cbYear.model.indexOf(root.year)
                    onCurrentTextChanged: root.year = currentText
                }

                Text { text: "Ngày/tháng/năm"; font.pixelSize: 12; color: "#888"; leftPadding: 8 }
            }

            Button
            {
                text: "Cập nhật thời gian"
                Layout.alignment: Qt.AlignHCenter
                background: Rectangle { color: darkMode ? "#1976D2" : "#3b82f6"; radius: 9 }

                contentItem: Text
                {
                    text: parent.text
                    color: "#fff"
                    font.pixelSize: 15
                    anchors.centerIn: parent
                }
                onClicked:
                {
                    // Gửi bản tin duy nhất xuống STM32
                    serialManager.sendDateTime(
                        root.hour, root.minute, root.second,
                        root.day, root.month, root.year
                    )
                    // Optional: Có thể hiện thông báo popup nhỏ là "Đã gửi yêu cầu cập nhật thời gian!"
                    Qt.callLater(function() {
                        root.showToast("Đã gửi yêu cầu cập nhật thời gian!");
                    })
                }
            }
        }

        // Toast thông báo nhỏ, tự động tắt
        Rectangle
        {
            id: toast
            visible: false
            width: 210; height: 36
            radius: 11
            color: "green"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            z: 10

            Text
            {
                anchors.centerIn: parent
                text: toast.text
                color: "#fff"
                font.pixelSize: 14
            }
        }

        function showToast(msg)
        {
            toast.text = msg
            toast.visible = true
            toast.opacity = 1.0
            toastTimer.restart()
        }

        Timer
        {
            id: toastTimer
            interval: 1800

            onTriggered:
            {
                toast.visible = false
            }
        }
    }
}
