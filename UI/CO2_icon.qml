import QtQuick

Item
{
    id: co2Icon
    width: 36; height: 26

    // Mây CO₂
    Canvas {
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);

            // Vẽ mây
            ctx.beginPath();
            ctx.arc(13, 18, 11, Math.PI*0.9, Math.PI*1.5, false);
            ctx.arc(20, 19, 12, Math.PI*1.2, Math.PI*2.0, false);
            ctx.arc(28, 15, 9, Math.PI*1.3, Math.PI*2, false);
            ctx.closePath();
            ctx.fillStyle = "#89d4f7";
            ctx.fill();

            // Các chấm nhỏ phía trên
            ctx.fillStyle = "#89d4f7";
            for (var i = 0; i < 10; ++i) {
                var px = 8 + (i%4)*7;
                var py = 3 + Math.floor(i/4)*4 + Math.random()*1.2;
                ctx.beginPath();
                ctx.arc(px, py, 1.3 + Math.random()*0.5, 0, 2*Math.PI);
                ctx.fill();
            }
        }
    }
    // Chữ CO₂
    Text {
        text: "CO"
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 10
        font.bold: true
        color: "#234"
    }
    Text {
        text: "2"
        anchors.left: parent.left
        anchors.leftMargin: 24
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 8
        font.bold: true
        color: "#234"
        verticalAlignment: Text.AlignBottom
    }
}
