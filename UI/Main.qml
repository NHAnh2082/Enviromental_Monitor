import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow
{
    id: window
    width: 480
    height: 330
    visible: true
    title: "STM32 UART Monitor"

    // Light / Dark mode
    property bool darkMode: false

    // Chuyển mode ở góc phải trên
    Button
    {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 10
        anchors.topMargin: 8
        text: darkMode ? "☀" : "🌙"

        onClicked: function()
        {
            darkMode = !darkMode
        }

        z: 100
    }

    TabBar
    {
        id: bar
        width: parent.width
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        TabButton { text: "Serial Config" }
        TabButton { text: "Monitor" }
        TabButton { text: "Time Setting" }
    }

    StackLayout
    {
        id: stack
        anchors.top: bar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        currentIndex: bar.currentIndex

        SerialConfigTab
        {
            darkMode: window.darkMode
        }

        MonitorTab
        {
            darkMode: window.darkMode
        }

        TimeSettingTab
        {
            darkMode: window.darkMode
        }
    }
}
