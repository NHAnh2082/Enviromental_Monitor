import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia

Item
{
    // Property nhận dữ liệu thực
    property string weekday: ""     // ví dụ "Thứ Ba"
    property string date: ""        // "02/07/2024"
    property string time: ""        // "12:16:21"
    property string ampm: "AM"
    property real temp: 31
    property int humi: 14
    property real co2: 1000
    property int batteryLevel: 15 // nếu muốn update pin thực tế
    property string tempColor: "#111"

    // Cảnh báo logic
    property bool warnTemp: serialManager.isOpen && temp >= 30
    property bool warnHumi: serialManager.isOpen && humi <= 15
    property bool warnCO2: serialManager.isOpen && co2 >= 1000

    // Hiệu ứng nhấp nháy (opacity động)
    property real blink: 1.0

    property bool darkMode: false

    // Lắng nghe signal từ SerialPortManager
    Connections
    {
        target: serialManager

        function onReceivedValues(temperature, humidity, co2Value, timeString, dayStr, dateStr)
        {
            temp = temperature
            humi = Math.round(humidity)
            co2  = co2Value

            // time
            time = timeString

            // AM/PM
            var t = timeString.split(":")
            var h = t.length > 0 ? parseInt(t[0]) : 0
            ampm = h >= 12 ? "PM" : "AM"

            // date
            date = dateStr;

            // week dayStr
            var thuVN = ["", "Chủ nhật", "Thứ Hai", "Thứ Ba", "Thứ Tư", "Thứ Năm", "Thứ Sáu", "Thứ Bảy"];
            var dIdx = Number(dayStr);  // dayStr là "1", "2", ...

            if (isNaN(dIdx) || dIdx < 1 || dIdx > 7)
                dIdx = 1; // Mặc định Chủ nhật nếu sai

            weekday = thuVN[dIdx];

            console.log("== UART receive: time=", time, " day=", weekday, " date=", date)

            // Bạn có thể lấy giá trị pin từ signal khác hoặc code giả lập.
        }
    }

    // Blink Timer cho hiệu ứng nhấp nháy cảnh báo
    Timer
    {
        interval: 400
        repeat: true
        running: warnTemp || warnHumi || warnCO2
        onTriggered: blink = (blink === 1.0 ? 0.3 : 1.0)
    }

    // Âm thanh cảnh báo (dùng beep hệ thống, hoặc thay file .wav nếu có)
    SoundEffect
    {
        id: beepEffect
        source: "" // Để rỗng sẽ beep hệ thống, muốn dùng file thì: "qrc:/sounds/alert.wav"
        volume: 1.0
    }
    Timer
    {
        interval: 1500
        running: warnTemp || warnHumi || warnCO2
        repeat: true
        onTriggered: beepEffect.play()
    }

    // Popup cảnh báo
    Popup
    {
        id: warningPopup
        modal: true
        focus: true
        visible: warnTemp || warnHumi || warnCO2
        anchors.centerIn: Overlay.overlay
        width: 260
        height: 120
        background: Rectangle { color: "#fff3f3"; radius: 14; border.color: "#FF5252"; border.width: 3 }

        contentItem: ColumnLayout
        {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            Text
            {
                text: "⚠️  CẢNH BÁO!"
                font.pixelSize: 23
                color: "#FF2222"
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter
            }

            Text
            {
                text: (warnTemp ? "Nhiệt độ quá cao!\n" : "") +
                      (warnHumi ? "Độ ẩm quá thấp!\n" : "") +
                      (warnCO2 ? "CO₂ vượt ngưỡng!\n" : "")

                font.pixelSize: 14
                color: "#111"
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter
            }

            Button
            {
                text: "Đã hiểu"
                Layout.alignment: Qt.AlignHCenter
                onClicked: function()
                {
                    warningPopup.close()
                }
            }
        }
    }

    // Main card giao diện
    Rectangle
    {
        anchors.centerIn: parent
        width: parent.width * 0.98
        height: parent.height * 0.9
        color: warnTemp || warnHumi || warnCO2 ? "#FFF1F1" : "#ffffff"
        radius: 14
        border.color: "#cccccc"
        border.width: 2

        // Battery Indicator (top-right)
        RowLayout
        {
            anchors.right: parent.right
            anchors.rightMargin: 8
            anchors.top: parent.top
            anchors.topMargin: 8
            spacing: 4

            Rectangle
            {
                id: batteryFrame
                width: 26
                height: 12
                radius: 2
                border.color: "#555"
                color: "#ffffff"

                Rectangle
                {
                    id: batteryFill
                    width: (batteryFrame.width - 2) * (batteryLevel / 100)
                    height: batteryFrame.height - 2
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 1
                    color: batteryLevel > 20 ? "#4CAF50" : "#FF5252"
                    radius: 2

                    // Animate width
                    Behavior on width
                    {
                        NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
                    }

                    // Animate color for low battery
                    SequentialAnimation on color {
                        running: batteryLevel <= 20
                        loops: Animation.Infinite
                        ColorAnimation { to: "#FF5252"; duration: 400 }
                        ColorAnimation { to: "#ffffff"; duration: 400 }
                    }
                }
            }
            Text
            {
                text: batteryLevel + "%"
                font.pixelSize: 12
                color: "#333"
            }
        }

        // Main Content
        ColumnLayout
        {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 10

            // Row 1: Clock
            RowLayout
            {
                Layout.fillWidth: true
                spacing: 10

                // Clock icon + AM/PM
                ColumnLayout
                {
                    RowLayout
                    {
                        Layout.fillWidth: true
                        spacing: 10

                        // Clock icon bên trái
                        Canvas
                        {
                            width: 36
                            height: 36
                            Layout.alignment: Qt.AlignLeft
                            onPaint:
                            {
                                var ctx = getContext("2d");
                                ctx.clearRect(0,0,width,height);
                                ctx.strokeStyle = "#333";
                                ctx.lineWidth = 2.5;
                                ctx.beginPath();
                                ctx.arc(width/2,height/2,19,0,2*Math.PI);
                                ctx.stroke();

                                // Kim giờ/phút động
                                var t = time.split(":");
                                var h = t.length > 0 ? parseInt(t[0]) : 0;
                                var m = t.length > 1 ? parseInt(t[1]) : 0;
                                var hourAngle = ((h % 12) + m/60) * 30 * Math.PI / 180;
                                var minAngle = m * 6 * Math.PI / 180;

                                // Kim giờ
                                ctx.save();
                                ctx.translate(width/2, height/2);
                                ctx.rotate(hourAngle - Math.PI/2);
                                ctx.strokeStyle = "#222";
                                ctx.lineWidth = 3;
                                ctx.beginPath();
                                ctx.moveTo(0, 0);
                                ctx.lineTo(0, -12);
                                ctx.stroke();
                                ctx.restore();

                                // Kim phút
                                ctx.save();
                                ctx.translate(width/2, height/2);
                                ctx.rotate(minAngle - Math.PI/2);
                                ctx.strokeStyle = "#03A9F4";
                                ctx.lineWidth = 2;
                                ctx.beginPath();
                                ctx.moveTo(0, 0);
                                ctx.lineTo(0, -18);
                                ctx.stroke();
                                ctx.restore();
                            }
                        }

                        // Phần chữ căn giữa
                        ColumnLayout
                        {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignHCenter
                            spacing: 2

                            Text
                            {
                                text: weekday + (date ? ", " + date : "")
                                font.pixelSize: 18
                                color: "#444"
                                horizontalAlignment: Text.AlignHCenter
                                Layout.alignment: Qt.AlignHCenter
                            }

                            RowLayout
                            {
                                Layout.alignment: Qt.AlignHCenter
                                spacing: 6

                                Text
                                {
                                    text: time
                                    font.pixelSize: 32
                                    font.bold: true
                                    color: "#111"
                                }

                                Text
                                {
                                    text: ampm
                                    font.pixelSize: 16
                                    color: "#555"
                                    verticalAlignment: Text.AlignBottom
                                    leftPadding: 4
                                }
                            }
                        }
                    }
                }
            }

            Rectangle
            {
                height: 1.5
                color: "#dddddd"
                Layout.fillWidth: true
            }

            // Row 2: Temp, Humidity & CO2
            RowLayout
            {
                Layout.fillWidth: true
                spacing: 36

                // Nhiệt độ
                Rectangle
                {
                    color: warnTemp ? "#FF5252" : "transparent"
                    radius: 7
                    Layout.preferredWidth: 120
                    Layout.preferredHeight: 56

                    RowLayout
                    {
                        spacing: 5

                        Text
                        {
                            text: "🌡️"
                            font.pixelSize: 22
                        }

                        Text
                        {
                            text: temp.toFixed(1)
                            font.pixelSize: 23
                            font.bold: true
                            // color: "#111"
                            color: warnTemp ? "#fff" : "#c40"
                            opacity: warnTemp ? blink : 1.0
                        }

                        Text
                        {
                            text: "°C"
                            font.pixelSize: 18
                            color: "#555"
                            bottomPadding: 4
                        }

                        // Cảnh báo nhiệt độ
                        Text
                        {
                            text: warnTemp ? "!" : ""
                            color: "#fff"
                            font.pixelSize: 18
                            font.bold: true
                            opacity: warnTemp ? blink : 0

                            Rectangle
                            {
                                anchors.fill: parent
                                color: "transparent"
                            }
                        }
                    }
                }

                // Humidity
                Rectangle
                {
                    color: warnHumi ? "#2196F3" : "transparent"
                    radius: 7
                    Layout.preferredWidth: 120
                    Layout.preferredHeight: 56

                    RowLayout
                    {
                        spacing: 5

                        Text
                        {
                            text: "💧"
                            font.pixelSize: 22
                        }

                        Text
                        {
                            text: humi
                            font.pixelSize: 23
                            font.bold: true
                            // color: "#111"
                            color: warnHumi ? "#fff" : "#06A"
                            opacity: warnHumi ? blink : 1.0
                        }

                        Text
                        {
                            text: "%"
                            font.pixelSize: 18
                            color: "#555"
                            bottomPadding: 4
                        }

                        // Cảnh báo độ ẩm
                        Text
                        {
                            text: warnHumi ? "!" : ""
                            color: "#fff"
                            font.pixelSize: 18
                            font.bold: true
                            opacity: warnHumi ? blink : 0
                            Rectangle
                            {
                                anchors.fill: parent
                                color: "transparent"
                            }
                        }
                    }
                }

                // CO2
                Rectangle
                {
                    color: warnCO2 ? "#FF5252" : "transparent"
                    radius: 7
                    Layout.preferredWidth: 120
                    Layout.preferredHeight: 56

                    ColumnLayout
                    {
                        anchors.centerIn: parent
                        spacing: 0

                        CO2_icon
                        {
                            width: 36
                            height: 26
                        }

                        RowLayout
                        {
                            Layout.alignment: Qt.AlignHCenter
                            spacing: 3

                            Text
                            {
                                text: co2.toFixed(1)
                                font.pixelSize: 23
                                font.bold: true
                                color: warnCO2 ? "#fff" : "#277f27"
                                opacity: warnCO2 ? blink : 1.0
                                horizontalAlignment: Text.AlignHCenter
                            }

                            Text
                            {
                                text: "ppm"
                                font.pixelSize: 15
                                color: "#555"
                                bottomPadding: 2
                                verticalAlignment: Text.AlignBottom
                            }
                        }
                    }
                }
            }

            // Nút setup
            Rectangle
            {
                Layout.alignment: Qt.AlignCenter
                width: 130
                height: 34
                radius: 16
                color: "#f0f0f0"
                border.color: "#bbbbbb"

                Text
                {
                    anchors.centerIn: parent
                    text: qsTr("Setup thời gian")
                    color: "#666"
                    font.pixelSize: 10
                }

                MouseArea
                {
                    anchors.fill: parent

                    onClicked:
                    {
                        console.log("Setup thời gian được nhấn");
                    }
                }
            }
        }
    }
}
