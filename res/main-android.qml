import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.3
import TMClient 1.0


ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 640
    height: 480
    Material.theme: Material.Dark
    color: "#000000"

    title: qsTr("Track ME")

    TMClient {
        id: tmclient
    }

    Timer {
        id: updatetimer
        interval: 5000
        running: true
        repeat: true
        onTriggered: {
            var rtxt = tmclient.update();
            //console.log(rtxt);
            if(rtxt.startsWith("S:")) {
                if(rtxt[2] === '0') {
                    startbtn.state = "RELEASED";
                } else {
                    startbtn.state = "PRESSED";
                }
                if(rtxt[4] === '0') {
                    startoncebtn.state = "RELEASED";
                } else {
                    startoncebtn.state = "PRESSED";
                }
            } else if(rtxt.startsWith("I:")) {
                label.text = rtxt.substr(2);
                label.color = "white";
            } else if(rtxt.startsWith("E:")) {
                label.text = rtxt.substr(2);
                label.color = "red";
            } else if(rtxt.startsWith("W:")) {
                label.text = rtxt.substr(2);
                label.color = "yellow";
            }

        }
    }

    Flickable {
        boundsBehavior: Flickable.StopAtBounds
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        contentHeight: parent.width > parent.height ? parent.width : parent.height

        ColumnLayout {
            id: columnLayout
            x: 20
            y: 10
            anchors.rightMargin: 20
            anchors.leftMargin: 20
            anchors.bottomMargin: 10
            anchors.topMargin: 10
            anchors.fill: parent

            Label {
                id: label
                text: "..."
                anchors.right: parent.right
                anchors.left: label3.right
                anchors.bottom: parent.bottom
                anchors.top: label3.bottom
                anchors.topMargin: 0
                font.pointSize: label3.font.pointSize*0.7
            }

            Label {
                id: label3
                text: qsTr("Info:")
                anchors.topMargin: 10
                anchors.top: intervaltxt.bottom
                anchors.left: parent.left
            }

            Label {
                id: label1
                text: qsTr("Login info:")
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.top: startoncebtn.bottom
                anchors.topMargin: 10
            }

            Label {
                id: label2
                text: qsTr("Interval (sec):")
                anchors.left: parent.left
                anchors.top: passwordtxt.bottom
                anchors.topMargin: 10
            }

            TextField {
                id: intervaltxt
                text: "60"
                inputMask: "900"
                horizontalAlignment: Text.AlignHCenter
                anchors.top: label2.bottom
                anchors.right: parent.right
                anchors.left: parent.left
            }

            TextField {
                id: urltxt
                text: ""
                horizontalAlignment: Text.AlignHCenter
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.top: label1.bottom
                anchors.topMargin: 0
            }

            TextField {
                id: pointname
                text: "track"+Qt.formatDateTime(new Date(), "yyMMddhhmmss")
                anchors.top: parent.top
                anchors.topMargin: 0
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0
                horizontalAlignment: Text.AlignHCenter
            }

            TextField {
                id: logintxt
                text: ""
                horizontalAlignment: Text.AlignHCenter
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.top: urltxt.bottom
                anchors.topMargin: 0
            }

            TextField {
                id: passwordtxt
                echoMode: TextInput.Password
                text: ""
                anchors.top: logintxt.bottom
                anchors.topMargin: 0
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0
                horizontalAlignment: Text.AlignHCenter
            }

            Button {
                id: startbtn
                anchors.top: pointname.bottom
                anchors.topMargin: 0
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0
                state: "RELEASED"
                contentItem: Text {
                    id: startbtntext
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                states: [
                    State {
                        name: "PRESSED"
                        PropertyChanges {
                            target: startbtntext
                            color: "red"
                            text: qsTr("STOP")
                        }
                        PropertyChanges {
                            target: startbtn
                            enabled: true
                        }
                    },
                    State {
                        name: "RELEASED"
                        PropertyChanges {
                            target: startbtntext
                            color: "green"
                            text: qsTr("START")
                        }
                        PropertyChanges {
                            target: startbtn
                            enabled: true
                        }
                    },
                    State {
                        name: "PROCESSING"
                        PropertyChanges {
                            target: startbtntext
                            color: "yellow"
                            text: qsTr("WAIT...")
                        }
                        PropertyChanges {
                            target: startbtn
                            enabled: false
                        }
                    }
                ]

                transitions: [
                    Transition {
                        from: "PRESSED"
                        to: "RELEASED"
                        ColorAnimation { target: startbtntext; duration: 500}
                    },
                    Transition {
                        from: "RELEASED"
                        to: "PROCESSING"
                        ColorAnimation { target: startbtntext; duration: 500}
                    },
                    Transition {
                        from: "PROCESSING"
                        to: "PRESSED"
                        ColorAnimation { target: startbtntext; duration: 500}
                    }
                ]

                onClicked: {
                    updatetimer.running = false;
                    applicationWindow.updateparameters();
                    if(tmclient.command('start')) startbtn.state = "PROCESSING";
                    updatetimer.running = true;
                }
            }

            Button {
                id: startoncebtn
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.top: startbtn.bottom
                anchors.topMargin: 0
                state: "RELEASED"
                contentItem: Text {
                    id: startoncebtntext
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                states: [
                    State {
                        name: "PRESSED"
                        PropertyChanges {
                            target: startoncebtntext
                            color: "red"
                            text: qsTr("IN PROCESS")
                        }
                        PropertyChanges {
                            target: startoncebtn
                            enabled: false
                        }
                    },
                    State {
                        name: "RELEASED"
                        PropertyChanges {
                            target: startoncebtntext
                            color: "green"
                            text: qsTr("START ONCE")
                        }
                        PropertyChanges {
                            target: startoncebtn
                            enabled: true
                        }
                    },
                    State {
                        name: "PROCESSING"
                        PropertyChanges {
                            target: startoncebtntext
                            color: "yellow"
                            text: qsTr("WAIT...")
                        }
                        PropertyChanges {
                            target: startoncebtn
                            enabled: false
                        }
                    }
                ]

                transitions: [
                    Transition {
                        from: "PRESSED"
                        to: "RELEASED"
                        ColorAnimation { target: startoncebtntext; duration: 500}
                    },
                    Transition {
                        from: "RELEASED"
                        to: "PROCESSING"
                        ColorAnimation { target: startoncebtntext; duration: 500}
                    },
                    Transition {
                        from: "PROCESSING"
                        to: "PRESSED"
                        ColorAnimation { target: startoncebtntext; duration: 500}
                    }
                ]

                onClicked: {
                    updatetimer.running = false;
                    applicationWindow.updateparameters();
                    if(tmclient.command('startonce')) startoncebtn.state = "PROCESSING";
                    updatetimer.running = true;
                }
            }
        }
    }

    function updateparameters() {
        tmclient.login = logintxt.text
        tmclient.password = passwordtxt.text
        tmclient.url = urltxt.text
        tmclient.track = pointname.text
        tmclient.timeout = Number(intervaltxt.text)
    }

    function getparameters() {
        logintxt.text = tmclient.login
        passwordtxt.text = tmclient.password
        urltxt.text = tmclient.url
        //pointname.text = tmclient.track
        intervaltxt.text = tmclient.timeout
    }

    Component.onCompleted: getparameters()
}
