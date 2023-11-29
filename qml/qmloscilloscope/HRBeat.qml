import QtQuick 2.0
import QtQuick.Layouts 1.15

Item {
    property int bpm
    RowLayout {
        anchors.fill: parent
        Image {
            id: heart
            source: "qrc:/image/heart.png"
            Layout.preferredWidth: 40
            Layout.preferredHeight: 40
            Layout.alignment: Qt.AlignCenter
            transformOrigin: Item.Center

            SequentialAnimation {
                id: heartBeatAnimation
                loops: Animation.Infinite

                NumberAnimation {
                    id: systole
                    target: heart
                    property: "scale"
                    to: 0.5
                }

                NumberAnimation {
                    id: diastole
                    target: heart
                    property: "scale"
                    to: 1.0
                }
            }
        }
        Text {
            id: bpmText
            font.pixelSize: 16
        }
    }

    function resetAnimation() {
        if (bpm === 0)
            heartBeatAnimation.stop()
        else {
            // avoid divided by 0 when bpm is 0
            systole.duration = 1000 * 60 / bpm / 3
            diastole.duration = 1000 * 60 * 2 / bpm / 3
            if (heartBeatAnimation.stopped())
                heartBeatAnimation.start()

            // component have just initialized and bpm updated for the first time
            if (!heartBeatAnimation.stopped() && !heartBeatAnimation.started()) {
                heartBeatAnimation.start()
            }
        }
        bpmText.text = String(bpm) + " BPM"
    }

    onBpmChanged: {
        resetAnimation()
    }
}
