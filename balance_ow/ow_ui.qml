/*
    Copyright 2022 Benjamin Vedder	benjamin@vedder.se

    This file is part of VESC Tool.

    VESC Tool is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VESC Tool is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3
import Vedder.vesc.utility 1.0

import Vedder.vesc.commands 1.0
import Vedder.vesc.configparams 1.0

import Vedder.vesc.logwriter 1.0

// This example shows how to read and write settings using the custom
// config. It is also possible to send and receive custom data using
// send_app_data and set_app_data_handler on the euc-side and Commands
// onCustomAppDataReceived and mCommands.sendCustomAppData in qml.

Item {
    id: mainItem
    anchors.fill: parent
    anchors.margins: 5

    property Commands mCommands: VescIf.commands()
    property ConfigParams mMcConf: VescIf.mcConfig()
    property ConfigParams mCustomConf: VescIf.customConfig(0)
    
    //property var parentTabBar: parent.tabBarItem
    property var enableDlaCaliDumping: 0
    
    Component.onCompleted: {
        //parentTabBar.visible = true
        //parentTabBar.enabled = true
    }

    Component.onDestruction: {
        
    }
    
    LogWriter {
        id: mLogWriter
    }

    Timer {
        running: true
        repeat: true
        interval: 100
        
        onTriggered: {
            var buffer = new ArrayBuffer(1)
            var dv = new DataView(buffer)
            var ind = 0
            dv.setUint8(ind, 0x01); ind += 1
            mCommands.sendCustomAppData(buffer)
        }
    }

    ColumnLayout {
        anchors.fill: parent

        TabBar {
            id: localTabBar
            // parent: parentTabBar
            anchors.fill: parent
            currentIndex: swipeView.currentIndex
            
            background: Rectangle {
                opacity: 1
                color: Utility.getAppHexColor("lightBackground")
            }
            
            property int buttonWidth: Math.max(120, localTabBar.width / (rep.model.length))

            Repeater {
                id: rep
                model: ["Info", "DLA"]
                
                TabButton {
                    text: modelData
                    width: localTabBar.buttonWidth
                }
            }
        }
        
        SwipeView {
            id: swipeView
            currentIndex: localTabBar.currentIndex
            anchors.fill: parent
            clip: true
            
            Page {
                ColumnLayout {
                    anchors.fill: parent

                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        
                        ColumnLayout {
                            Text {
                                id: header
                                color: Utility.getAppHexColor("lightText")
                                font.family: "DejaVu Sans Mono"
                                Layout.margins: 0
                                Layout.leftMargin: 0
                                Layout.fillWidth: true
                                text: "Balance OW App RT Data"
                                font.underline: true
                                font.weight: Font.Black
                            }
                            Text {
                                id: valText1
                                color: Utility.getAppHexColor("lightText")
                                font.family: "DejaVu Sans Mono"
                                Layout.margins: 0
                                Layout.leftMargin: 5
                                Layout.preferredWidth: parent.width/3
                                text: "App not connected"
                            }
                        }
                        
                    }
                }
            }
            
    //        Page {
    //            ColumnLayout {
    //                anchors.fill: parent
    //
    //                ScrollView {
    //                    Layout.fillWidth: true
    //                    Layout.fillHeight: true
    //                    clip: true
                        
    //                    ParamList {
    //                        id: params
    //                        anchors.fill: parent
    //                    }
    //                }
    //            }
    //        }
            
            Page {
                ColumnLayout {
                    anchors.fill: parent

                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        
                        ColumnLayout {
                            id: gaugeColumn
                            anchors.fill: parent
                            
                            ColumnLayout {
                                Layout.fillWidth: true

                                Text{
                                    text: "Dla calibration file name"
                                }

                                TextInput {
                                    id: csvFilePath
                                    text: "/storage/emulated/0/Documents/logs/"
                                }

                                TextInput {
                                    id: csvFileName
                                    text: "log_file.csv"
                                }

                                Button {
                                    id: toggleDlaCalibDump
                                    text: "Null"
                                    Layout.fillWidth: true
                                    
                                    onClicked: {
                                        if (enableDlaCaliDumping == 0) {
                                            enableDlaCaliDumping = 1
                                            mLogWriter.openLogFileFromPath(csvFileName.text, csvFilePath.text)
                                            mLogWriter.writeToLogFile("ERPM,Current,Acceleration,Brake\n")
                                        }
                                        else {
                                            enableDlaCaliDumping = 0
                                            // Close file when done to ensure that all data is written.
                                            mLogWriter.closeLogFile()
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    Connections {
        target: mCommands
        
        // This function will be called when VESC_IF->send_app_data is used. To
        // send data back mCommands.sendCustomAppData can be used. That data
        // will be received in the function registered with VESC_IF->set_app_data_handler
        onCustomAppDataReceived: {
            // Ints and floats can be extracted like this from the data
            var dv = new DataView(data, 0)
            var ind = 0
            var time_diff = dv.getFloat32(ind); ind += 4;
            var state = dv.getInt16(ind); ind += 2;
            var motor_current = dv.getFloat32(ind); ind += 4;
            var filtered_current = dv.getFloat32(ind); ind += 4;
            var erpm = dv.getFloat32(ind); ind += 4;
            var acceleration = dv.getFloat32(ind); ind += 4;
            var braking = dv.getInt16(ind); ind += 2;
            var pid_value = dv.getFloat32(ind); ind += 4;
            var true_pitch = dv.getFloat32(ind); ind += 4;
            var pitch = dv.getFloat32(ind); ind += 4;
            var roll = dv.getFloat32(ind); ind += 4;
            var switch_state = dv.getInt16(ind); ind += 2;
            var adc1 = dv.getFloat32(ind); ind += 4;
            var adc2 = dv.getFloat32(ind); ind += 4;

            var stateString
            if(state == 0){
                stateString = "STARTUP"
            }else if(state == 1){
                stateString = "RUNNING"
            }else if(state == 2){
                stateString = "RUNNING_TILTBACK_DUTY"
            }else if(state == 3){
                stateString = "RUNNING_TILTBACK_HIGH_VOLTAGE"
            }else if(state == 4){
                stateString = "RUNNING_TILTBACK_LOW_VOLTAGE"
            }else if(state == 5){
                stateString = "RUNNING_TILTBACK_TEMP"
            }else if(state == 6){
                stateString = "FAULT_ANGLE_PITCH"
            }else if(state == 7){
                stateString = "FAULT_ANGLE_ROLL"
            }else if(state == 8){
                stateString = "FAULT_SWITCH_HALF"
            }else if(state == 9){
                stateString = "FAULT_SWITCH_FULL"
            }else if(state == 10){
                stateString = "Was FAULT_DUTY" // Can be remove
            }else if(state == 11){
                stateString = "FAULT_STARTUP"
            }else if(state == 12){
                stateString = "FAULT_REVERSE"
            }else if(state == 13){
                stateString = "FAULT_QUICKSTOP"
            }else if(state == 14){
                stateString = "RUNNING_WHEELSLIP"
            }else{     
                stateString = "UNKNOWN"
            }
            

            var switchString
            if(switch_state == 0){
                switchString = "Off"
            }else if(switch_state == 1){
                switchString = "Half"
            }else{
                switchString = "On"
            }
            
            
            valText1.text =
                "time             : " + (1/time_diff).toFixed(0) + " hz\n" +
                "state            : " + stateString + "\n" +
                "current          : " + motor_current.toFixed(2) + " A\n" +
                "filtered current : " + filtered_current.toFixed(2) + " A\n" +
                "erpm             : " + (erpm / 1000).toFixed(3) + " / 1000 \n" +
                "acceleration     : " + acceleration.toFixed(2) + "\n" +
                "braking          : " + braking + "\n" + 
                "pid              : " + pid_value.toFixed(2) + " A\n" +
                "true pitch       : " + true_pitch.toFixed(2) + "°\n" +
                "pitch            : " + pitch.toFixed(2) + "°\n" +
                "roll             : " + roll.toFixed(2) + "°\n" +
                "switch           : " + switchString + "\n" +
                "adc1             : " + adc1.toFixed(2) + " V \n" +
                "adc2             : " + adc2.toFixed(2) + " V \n";

            if (enableDlaCaliDumping == 0) {
                toggleDlaCalibDump.text = "Enable DLA Calib Csv Dump"
            }
            else {
                toggleDlaCalibDump.text = "Disable DLA Calib Csv Dump"
                mLogWriter.writeToLogFile(erpm.toFixed(0) + "," + filtered_current.toFixed(2) + "," + 
                                          acceleration.toFixed(3) + "," + braking + "\n")
            }
        }
    }
}
