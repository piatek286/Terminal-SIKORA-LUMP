import QtQuick 2.0

Item {
    id: przycisk
    Rectangle {
        id: zerowanie
        width: 200
        height: 51
        color: "#2876d7"
        radius: 26
        border.color: "#081b5c"
        border.width: 6
    }
    states: [
        State {
            name: "State1"

            PropertyChanges {
                target: zerowanie
                color: "#6091d2"
                opacity: 1
                clip: false
                rotation: 0
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                anchors.bottomMargin: -1
                anchors.topMargin: 0
            }
        }
    ]

}
