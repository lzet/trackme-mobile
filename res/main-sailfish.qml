import QtQuick 2.0
import Sailfish.Silica 1.0
import TMClient 1.0


ApplicationWindow {
    id: applicationWindow
    visible: true
    allowedOrientations: Orientation.All
    property string covtext: qsTr("TRACKME\nSTOPPED")
    property string covclr: "red"

    cover: Component {
        Cover{
            Rectangle {
                id: cover
                anchors.fill: parent
                color: "#80008050"

                Image {
                    width: parent.width
                    y: parent.height/2 - height/2

                    source: "backcover.png"
                }

                Label {
                    id: coverlabel
                    anchors.centerIn: parent
                    text: applicationWindow.covtext
                    color: applicationWindow.covclr
                    font.bold: true
                }
            }
        }
    }
    initialPage: Component {

        Page {
            id: page
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
                    if(rtxt.length < 1) return;
                    //console.log(rtxt);

                    if(rtxt.match(/^S:/)) {
                        var started = false;
                        if(rtxt[2] === '0') {
                            startbtn.state = "RELEASED";
                        } else {
                            startbtn.state = "PRESSED";
                            started = true;
                        }
                        if(rtxt[4] === '0') {
                            startoncebtn.state = "RELEASED";
                        } else {
                            startoncebtn.state = "PRESSED";
                            started = true;
                        }
                        if(started) {
                            applicationWindow.covtext = qsTr("TRACKME\nSTARTED");
                            applicationWindow.covclr = "green";
                        } else {
                            applicationWindow.covtext = qsTr("TRACKME\nSTOPPED");
                            applicationWindow.covclr = "red";
                        }
                    } else if(rtxt.match(/^I:/)) {
                        label.text = rtxt.substr(2);
                        label.color = "white";
                    } else if(rtxt.match(/^E:/)) {
                        label.text = rtxt.substr(2);
                        label.color = "red";
                    } else if(rtxt.match(/^W:/)) {
                        label.text = rtxt.substr(2);
                        label.color = "yellow";
                    }

                }
            }
            SilicaFlickable {
                anchors.fill: parent
                contentHeight: column.height

                Column {
                    spacing: Theme.paddingSmall
                    id: column
                    width: page.width

                    PageHeader {
                        id: header
                        title: qsTr("Track Me")
                    }

                    TextField {
                        id: pointname
                        text: "track"+Qt.formatDateTime(new Date(), "yyMMddhhmmss")
                        horizontalAlignment: Text.AlignHCenter
                        anchors.right: parent.right
                        anchors.left: parent.left
                        anchors.rightMargin: 20
                        anchors.leftMargin: 20
                    }

                    Button {
                        id: startbtn
                        state: "RELEASED"
                        anchors.right: parent.right
                        anchors.left: parent.left
                        anchors.rightMargin: 20
                        anchors.leftMargin: 20

                        states: [
                            State {
                                name: "PRESSED"
                                PropertyChanges {
                                    target: startbtn
                                    color: "red"
                                    text: qsTr("STOP")
                                    enabled: true
                                }
                            },
                            State {
                                name: "RELEASED"
                                PropertyChanges {
                                    target: startbtn
                                    color: "green"
                                    text: qsTr("START")
                                    enabled: true
                                }
                            },
                            State {
                                name: "PROCESSING"
                                PropertyChanges {
                                    target: startbtn
                                    color: "yellow"
                                    text: qsTr("WAIT...")
                                    enabled: false
                                }
                            }
                        ]

                        transitions: [
                            Transition {
                                from: "PRESSED"
                                to: "RELEASED"
                                ColorAnimation { target: startbtn; duration: 500}
                            },
                            Transition {
                                from: "RELEASED"
                                to: "PROCESSING"
                                ColorAnimation { target: startbtn; duration: 500}
                            },
                            Transition {
                                from: "PROCESSING"
                                to: "PRESSED"
                                ColorAnimation { target: startbtn; duration: 500}
                            }
                        ]

                        onClicked: {
                            column.updateparameters();
                            if(tmclient.command('start')) startbtn.state = "PROCESSING";
                        }
                    }

                    Button {
                        id: startoncebtn
                        state: "RELEASED"
                        anchors.right: parent.right
                        anchors.left: parent.left
                        anchors.rightMargin: 20
                        anchors.leftMargin: 20

                        states: [
                            State {
                                name: "PRESSED"
                                PropertyChanges {
                                    target: startoncebtn
                                    color: "red"
                                    text: qsTr("IN PROCESS")
                                    enabled: false
                                }
                            },
                            State {
                                name: "RELEASED"
                                PropertyChanges {
                                    target: startoncebtn
                                    color: "green"
                                    text: qsTr("START ONCE")
                                    enabled: true
                                }
                            },
                            State {
                                name: "PROCESSING"
                                PropertyChanges {
                                    target: startoncebtn
                                    color: "yellow"
                                    text: qsTr("WAIT...")
                                    enabled: false
                                }
                            }
                        ]

                        transitions: [
                            Transition {
                                from: "PRESSED"
                                to: "RELEASED"
                                ColorAnimation { target: startoncebtn; duration: 500}
                            },
                            Transition {
                                from: "RELEASED"
                                to: "PROCESSING"
                                ColorAnimation { target: startoncebtn; duration: 500}
                            },
                            Transition {
                                from: "PROCESSING"
                                to: "PRESSED"
                                ColorAnimation { target: startoncebtn; duration: 500}
                            }
                        ]

                        onClicked: {
                            column.updateparameters();
                            if(tmclient.command('startonce')) startoncebtn.state = "PROCESSING";
                        }
                    }

                    Label {
                        id: label1
                        text: qsTr("Login info:")
                        font.pixelSize: Theme.fontSizeSmall
                        anchors.right: parent.right
                        anchors.left: parent.left
                        anchors.rightMargin: 20
                        anchors.leftMargin: 20
                    }

                    TextField {
                        id: urltxt
                        text: ""
                        horizontalAlignment: Text.AlignHCenter
                        anchors.right: parent.right
                        anchors.left: parent.left
                        anchors.rightMargin: 20
                        anchors.leftMargin: 20
                    }

                    TextField {
                        id: logintxt
                        text: ""
                        horizontalAlignment: Text.AlignHCenter
                        anchors.right: parent.right
                        anchors.left: parent.left
                        anchors.rightMargin: 20
                        anchors.leftMargin: 20
                    }

                    TextField {
                        id: passwordtxt
                        echoMode: TextInput.Password
                        text: ""
                        horizontalAlignment: Text.AlignHCenter
                        anchors.right: parent.right
                        anchors.left: parent.left
                        anchors.rightMargin: 20
                        anchors.leftMargin: 20
                    }

                    Label {
                        id: label2
                        text: qsTr("Interval (sec):")
                        font.pixelSize: Theme.fontSizeSmall
                        anchors.right: parent.right
                        anchors.left: parent.left
                        anchors.rightMargin: 20
                        anchors.leftMargin: 20
                    }

                    TextField {
                        id: intervaltxt
                        text: "60"
                        validator: RegExpValidator {
                            regExp: /\d{1,4}/
                        }
                        horizontalAlignment: Text.AlignHCenter
                        anchors.right: parent.right
                        anchors.left: parent.left
                        anchors.rightMargin: 20
                        anchors.leftMargin: 20
                    }

                    Label {
                        id: label3
                        text: qsTr("Info:")
                        font.pixelSize: Theme.fontSizeSmall
                        anchors.right: parent.right
                        anchors.left: parent.left
                        anchors.rightMargin: 20
                        anchors.leftMargin: 20
                    }

                    Label {
                        id: label
                        text: "..."
                        font.pixelSize: Theme.fontSizeSmall/2
                        anchors.right: parent.right
                        anchors.left: parent.left
                        anchors.rightMargin: 20
                        anchors.leftMargin: 20
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

                    Component.onCompleted:{
                        column.getparameters();
                    }
                }
            }
        }
    }
}
