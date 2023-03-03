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
import Qt.labs.settings 1.0

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
    property var dialogParent: ApplicationWindow.overlay
    property var enableDataDumping: 0
    property var dumpingText: 0
    property var dumpingCount: 0
    property var tuneInfoText: ""
    property var paramsArr:  [["double", "pitch_th"], ["double", "pitch_th_b"], ["double", "pitch_th_c"], ["double", "pitch_thi"], 
                              ["double", "pitch_thi_b"], ["double", "pitch_thi_c"], ["double", "gyro_th"], ["double", "gyro_th_b"],
                              ["double", "gyro_th_c"], ["double", "current_out_filter"], ["double", "current_out_filter_b"],
                              ["double", "current_out_filter_c"], ["bool", "tune_b_only_for_brakes"], ["bool", "tune_c_only_for_brakes"],
                              ["double", "brake_max_amp_change"], ["double", "brake_max_amp_change_b"], ["double", "brake_max_amp_change_c"],
                              ["double", "pitch_thi_limit"], ["double", "pitch_thi_limit_b"], ["double", "pitch_thi_limit_c"],
                              ["bool", "reset_pitch_thi_on_entering_b"], ["bool", "reset_pitch_thi_on_entering_c"],
                              ["double", "pitch_thi_decay_on_wheelslip"], ["double", "pitch_thi_decay_on_wheelslip_b"],
                              ["double", "pitch_thi_decay_on_wheelslip_c"], ["double", "tunea_transition_speed"], ["double", "tuneb_transition_speed"],
                              ["double", "tunec_transition_speed"], ["enum", "tunes_mixing_b"], ["enum", "tunes_mixing_c"], ["double", "asym_min_accel_b"],
                              ["double", "asym_max_accel_b"], ["int", "asym_min_erpm_b"], ["int", "asym_max_erpm_b"], ["double", "asym_min_accel_c"],
                              ["double", "asym_max_accel_c"], ["int", "asym_min_erpm_c"], ["int", "asym_max_erpm_c"], ["double", "mahony_kp"],
                              ["double", "fault_pitch"], ["double", "fault_roll"], ["int", "fault_delay_pitch"], ["int", "fault_delay_roll"],
                              ["double", "tiltback_duty_angle"], ["double", "tiltback_duty_speed"], ["double", "tiltback_duty"],
                              ["double", "tiltback_hv_angle"], ["double", "tiltback_hv_speed"], ["double", "tiltback_lv_angle"],
                              ["double", "tiltback_lv_speed"], ["double", "tiltback_return_speed"], ["double", "tiltback_constant"],
                              ["int", "tiltback_constant_erpm"], ["double", "tiltback_variable"], ["double", "tiltback_variable_max"], 
                              ["int", "tiltback_variable_start_erpm"], ["double", "noseangling_speed"], ["double", "startup_pitch_tolerance"], 
                              ["double", "startup_roll_tolerance"], ["double", "torquetilt_start_current"], ["double", "torquetilt_start_current_b"],
                              ["double", "torquetilt_angle_limit"], ["double", "torquetilt_on_speed"], ["double", "torquetilt_off_speed"],
                              ["double", "torquetilt_strength"], ["double", "torquetilt_strength_regen"], ["double", "torquetilt_filter"],
                              ["enum", "turntilt_mixing_mode"], ["double", "roll_turntilt_weight"], ["double", "roll_turntilt_strength"],
                              ["double", "roll_turntilt_angle_limit"], ["double", "roll_turntilt_start_angle"], ["int", "roll_turntilt_start_erpm"],
                              ["double", "roll_turntilt_speed"], ["int", "roll_turntilt_erpm_boost"], ["int", "roll_turntilt_erpm_boost_end"],
                              ["double", "yaw_turntilt_weight"], ["double", "yaw_turntilt_strength"], ["double", "yaw_turntilt_angle_limit"], 
                              ["double", "yaw_turntilt_start_angle"], ["int", "yaw_turntilt_start_erpm"], ["double", "yaw_turntilt_speed"],
                              ["int", "yaw_turntilt_erpm_boost"], ["int", "yaw_turntilt_erpm_boost_end"], ["int", "yaw_turntilt_aggregate"],
                              ["bool", "enable_traction_control"], ["double", "traction_control_mul_by"], ["double", "booster_min_pitch"],
                              ["double", "booster_max_pitch"], ["double", "booster_pitch_scale"], ["double", "booster_base"],
                              ["double", "booster_exponent"], ["double", "booster_out_scale"], ["double", "booster_limit"],
                              ["double", "booster_min_pitch_b"], ["double", "booster_max_pitch_b"], ["double", "booster_pitch_scale_b"],
                              ["double", "booster_base_b"], ["double", "booster_exponent_b"], ["double", "booster_out_scale_b"],
                              ["double", "booster_limit_b"], ["double", "booster_min_pitch_c"], ["double", "booster_max_pitch_c"],
                              ["double", "booster_pitch_scale_c"], ["double", "booster_base_c"], ["double", "booster_exponent_c"],
                              ["double", "booster_out_scale_c"], ["double", "booster_limit_c"]]
    property var extraParams: [["int", "hertz"], ["double", "fault_adc1"], ["double", "fault_adc2"], ["int", "fault_delay_switch_half"],
                               ["int", "fault_delay_switch_full"], ["int", "fault_adc_half_erpm"], ["bool", "fault_is_single_switch"],
                               ["int", "fault_adc_to_copy"], ["double", "tiltback_hv"], ["double", "tiltback_lv"], ["double", "startup_speed"],
                               ["double", "brake_current"], ["double", "temp_tiltback_start_offset"], ["double", "temp_tiltback_speed"],
                               ["double", "temp_tiltback_angle"], ["bool", "enable_reverse_stop"], ["bool", "enable_quickstop"],
                               ["int", "quickstop_erpm"], ["double", "quickstop_angle"], ["int", "startup_click_current"]]
    
    Settings {
        id: settingStorage
    }
    
    Component.onCompleted: {
        displaySavedTunes()
    }

    Component.onDestruction: {
        
    }
    
    LogWriter {
        id: mLogWriter
    }
    
    LogWriter {
        id: mArchiveCsvWriter
    }

    // Timer 1, 10hz for ble comms
    Timer {
        running: true
        repeat: true
        interval: 100
        
        onTriggered: {
            var buffer = new ArrayBuffer(2)
            var dv = new DataView(buffer)
            var ind = 0
            dv.setUint8(ind, 101); ind += 1; // Balance OW Package
            dv.setUint8(ind, 0x01); ind += 1
            mCommands.sendCustomAppData(buffer)

            // Process Controls
            if(reverseButton.pressed){
                var buffer = new ArrayBuffer(6)
                var dv = new DataView(buffer)
                var ind = 0
                dv.setUint8(ind, 101); ind += 1; // Balance OW Package
                dv.setUint8(ind, 7); ind += 1; // Command ID: RC Move
                dv.setUint8(ind, 0); ind += 1; // Direction
                dv.setUint8(ind, movementStrengthSlider.value); ind += 1; // Current
                dv.setUint8(ind, 1); ind += 1; // Time
                dv.setUint8(ind, movementStrengthSlider.value + 1); ind += 1; // Sum = time + current
                mCommands.sendCustomAppData(buffer)
            }
            if(forwardButton.pressed){
                var buffer = new ArrayBuffer(6)
                var dv = new DataView(buffer)
                var ind = 0
                dv.setUint8(ind, 101); ind += 1; // Balance OW Package
                dv.setUint8(ind, 7); ind += 1; // Command ID: RC Move
                dv.setUint8(ind, 1); ind += 1; // Direction
                dv.setUint8(ind, movementStrengthSlider.value); ind += 1; // Current
                dv.setUint8(ind, 1); ind += 1; // Time
                dv.setUint8(ind, movementStrengthSlider.value + 1); ind += 1; // Sum = time + current
                mCommands.sendCustomAppData(buffer)
            }
        }
    }

    Dialog {
        id: tuneInfoPopup
        title: "Tune Info"
        standardButtons: Dialog.Ok
        modal: true
        focus: true
        width: parent.width - 20
        closePolicy: Popup.CloseOnEscape
        x: 10
        y: 10 + parent.height / 2 - height / 2
        parent: ApplicationWindow.overlay
        
        Overlay.modal: Rectangle {
            color: "#AA000000"
        }
        
        Text {
            color: Utility.getAppHexColor("lightText")
            verticalAlignment: Text.AlignVCenter
            anchors.fill: parent
            wrapMode: Text.WordWrap
            text: tuneInfoText
        }
    }

    ColumnLayout {
        id: root
        anchors.fill: parent

        TabBar {
            id: tabBar
            currentIndex: 0
            Layout.fillWidth: true
            clip: true
            enabled: true

            background: Rectangle {
                opacity: 1
                color: {color = Utility.getAppHexColor("lightBackground")}
            }
            property int buttons: 3
            property int buttonWidth: 120

            Repeater {
                model: ["Info", "Tunes", "Control", "Dev"]
                TabButton {
                    text: modelData
                    onClicked:{
                        stackLayout.currentIndex = index
                    }
                }
            }
        }

        StackLayout {
            id: stackLayout
            Layout.fillWidth: true
            Layout.fillHeight: true


            ColumnLayout {// RT Data Page
                id: rtDataColumn

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                        
                    ColumnLayout {
                        Text {
                            id: header1
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
                            id: rt_data_text
                            color: Utility.getAppHexColor("lightText")
                            font.family: "DejaVu Sans Mono"
                            Layout.margins: 0
                            Layout.leftMargin: 5
                            Layout.preferredWidth: parent.width/3
                            text: "App not connected"
                        }

                        Text {
                            id: header2
                            color: Utility.getAppHexColor("lightText")
                            font.family: "DejaVu Sans Mono"
                            Layout.margins: 0
                            Layout.leftMargin: 0
                            Layout.fillWidth: true
                            text: "Setpoints"
                            font.underline: true
                            font.weight: Font.Black
                        }
                        Text {
                            id: setpoints_text
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
            
            ColumnLayout { // Tunes Page
                id: tunesColumn

                RowLayout {
                    Layout.fillWidth: true
                    Button {
                        id: quicksave1Button
                        text: "Quicksave 1"
                        Layout.fillWidth: false
                        onClicked: {
                            quickload1Button.enabled = true
                            settingStorage.setValue("ow_qs1_populated", "1")
                            quickSaveTune("ow_qs1")
                            VescIf.emitStatusMessage("Quicksave 1 complete!", true)
                        }
                    }
                    Button {
                        id: quickload1Button
                        text: "Quickload 1"
                        Layout.fillWidth: true
                        enabled: settingStorage.value("ow_qs1_populated") == "1"
                        onClicked: {
                            quickLoadTune("ow_qs1")
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Button {
                        id: quicksave2Button
                        text: "Quicksave 2"
                        Layout.fillWidth: false
                        onClicked: {
                            quickload2Button.enabled = true
                            settingStorage.setValue("ow_qs2_populated", "1")
                            quickSaveTune("ow_qs2")
                            VescIf.emitStatusMessage("Quicksave 2 complete!", true)
                        }
                    }
                    Button {
                        id: quickload2Button
                        text: "Quickload 2"
                        Layout.fillWidth: true
                        enabled: settingStorage.value("ow_qs2_populated") == "1"
                        onClicked: {
                            quickLoadTune("ow_qs2")
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Button {
                        id: quicksave3Button
                        text: "Quicksave 3"
                        Layout.fillWidth: false
                        onClicked: {
                            quickload3Button.enabled = true
                            settingStorage.setValue("ow_qs3_populated", "1")
                            quickSaveTune("ow_qs3")
                            VescIf.emitStatusMessage("Quicksave 3 complete!", true)
                        }
                    }
                    Button {
                        id: quickload3Button
                        text: "Quickload 3"
                        Layout.fillWidth: true
                        enabled: settingStorage.value("ow_qs3_populated") == "1"
                        onClicked: {
                            quickLoadTune("ow_qs3")
                        }
                    }
                }

                TextInput {
                    id: cloudCsvFileName
                    text: "CloudCsvFile_.csv"
                }

                Button {
                    id: exportBtn
                    text: "Export CSV to archive"
                    Layout.fillWidth: true
                    onClicked: {
                        // REMOVE ME FOR PUBLIC RELEASE
                        mArchiveCsvWriter.openLogFileFromPath(cloudCsvFileName.text, "/storage/emulated/0/Documents/logs/")
                        mArchiveCsvWriter.writeToLogFile("_name,")
                        for(var i in paramsArr) {
                            mArchiveCsvWriter.writeToLogFile(paramsArr[i][0] + "_" + paramsArr[i][1])
                            if (i < paramsArr.length - 1) {
                                mArchiveCsvWriter.writeToLogFile(",")
                            }
                        }
                        mArchiveCsvWriter.writeToLogFile("\n")
                        mArchiveCsvWriter.writeToLogFile("Name,")
                        for(var i in paramsArr) {
                            if (paramsArr[i][0] == "double") {
                                mArchiveCsvWriter.writeToLogFile(mCustomConf.getParamDouble(paramsArr[i][1]))
                            }
                            else if (paramsArr[i][0] == "int") {
                                mArchiveCsvWriter.writeToLogFile(mCustomConf.getParamInt(paramsArr[i][1]))
                            }
                            else if (paramsArr[i][0] == "bool") {
                                mArchiveCsvWriter.writeToLogFile(mCustomConf.getParamBool(paramsArr[i][1])?1:0)
                            }
                            else if (paramsArr[i][0] == "enum") {
                                mArchiveCsvWriter.writeToLogFile(mCustomConf.getParamEnum(paramsArr[i][1]))
                            }
                            if (i < paramsArr.length - 1) {
                                mArchiveCsvWriter.writeToLogFile(",")
                            }
                        }
                        mArchiveCsvWriter.closeLogFile()
                        VescIf.emitStatusMessage("Save Csv Complete!", true)
                        // VescIf.emitStatusMessage("This feature is disabled!", true)
                    }
                }

                Button {
                    id: downloadTunesButton
                    text: "Refresh Tune Archive"
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: {
                        downloadTunesButton.text = "Downloading Tunes..."
                        downloadedTunesModel.clear()
                        var http = new XMLHttpRequest()
                        var url = "http://eitan3.pythonanywhere.com/get_tunes?version=1"
                        http.open("GET", url, true);
                        http.onreadystatechange = function() {
                            if (http.readyState == XMLHttpRequest.DONE) {
                                downloadTunesButton.text = "Refresh Tune Archive"
                                if (http.status == 200) {
                                    settingStorage.setValue("tunes_csv", http.responseText)
                                    displaySavedTunes()
                                    VescIf.emitStatusMessage("Tune Download Success", true)
                                } else {
                                    VescIf.emitStatusMessage("Tune Download Failed: " + http.status, false)
                                }
                            }
                        }
                        http.send();
                    }
                }

                ListView {
                    id: downloadedTunesList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 5

                    model: ListModel {
                        id: downloadedTunesModel
                    }

                    delegate: RowLayout {
                        width: parent.width
                        Layout.fillWidth: true
                        Button {
                            text: "?"
                            Layout.fillWidth: false
                            onClicked: {
                                tuneInfoText = tune._info
                                tuneInfoPopup.open()
                            }
                        }
                        Button {
                            text: tune._name
                            Layout.fillWidth: true
                            onClicked: {
                                applyDownloadedTune(tune)
                            }
                        }
                        Button {
                            text: "?"
                            Layout.fillWidth: false
                            onClicked: {
                                tuneInfoText = tune._info
                                tuneInfoPopup.open()
                            }
                        }
                    }
                }

            }

            ColumnLayout { // Controls Page
                id: controlsColumn
                Layout.fillWidth: true

                // Movement controls
                Text {
                    id: movementControlsHeader
                    color: Utility.getAppHexColor("lightText")
                    font.family: "DejaVu Sans Mono"
                    Layout.margins: 0
                    Layout.leftMargin: 0
                    Layout.fillWidth: true
                    text: "Movement Controls"
                    font.underline: true
                    font.weight: Font.Black
                    font.pointSize: 14
                }
                RowLayout {
                    id: movementStrength
                    Layout.fillWidth: true

                    Text {
                        id: movementStrengthLabel
                        color: Utility.getAppHexColor("lightText")
                        font.family: "DejaVu Sans Mono"
                        text: "Strength:"
                    }
                    Slider {
                        id: movementStrengthSlider
                        from: 20
                        value: 40
                        to: 80
                        stepSize: 1
                    }
                }
                RowLayout {
                    id: movementControls
                    Layout.fillWidth: true
                    Button {
                        id: reverseButton
                        text: "Reverse"
                        Layout.fillWidth: true
                    }
                    Button {
                        id: forwardButton
                        text: "Forward"
                        Layout.fillWidth: true
                    }
                }
                
                // Tilt controls
                Text {
                    id: tiltControlsHeader
                    color: Utility.getAppHexColor("lightText")
                    font.family: "DejaVu Sans Mono"
                    Layout.margins: 0
                    Layout.leftMargin: 0
                    Layout.fillWidth: true
                    text: "Tilt Controls"
                    font.underline: true
                    font.weight: Font.Black
                    font.pointSize: 14
                }
                 CheckBox {
                    id: tiltEnabled
                    checked: false
                    text: qsTr("Enabled (Overrides Remote)")
                    onClicked: {
                        if(tiltEnabled.checked && mCustomConf.getParamEnum("inputtilt_remote_type", 0) != 1){
                            mCustomConf.updateParamEnum("inputtilt_remote_type", 1)
                            mCommands.customConfigSet(0, mCustomConf)
                        }
                    }
                }
                Slider {
                    id: tiltSlider
                    from: -1
                    value: 0
                    to: 1
                    Layout.fillWidth: true
                }
            }

            ColumnLayout { // Dev Page
                Layout.fillWidth: true

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
                                text: "Dumping file name"
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
                                id: toggleDataDump
                                text: "Null"
                                Layout.fillWidth: true
                                
                                onClicked: {
                                    // REMOVE ME FOR PUBLIC RELEASE
                                    if (enableDataDumping == 0) {
                                        enableDataDumping = 1
                                        mLogWriter.openLogFileFromPath(csvFileName.text, csvFilePath.text)
                                        var header = "erpm,braking,tuneB_weight,tuneC_weight,"
                                        header += "tuneA_current,tuneB_current,tuneC_current,"
                                        header += "current_request,tuneA_booster,tuneB_booster,tuneC_booster\n"
                                        mLogWriter.writeToLogFile(header)
                                    }
                                    else {
                                        enableDataDumping = 0
                                        // Close file when done to ensure that all data is written.
                                        mLogWriter.closeLogFile()
                                    }
                                    // VescIf.emitStatusMessage("This feature is disabled!", true)
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
            var magicnr = dv.getUint8(ind); ind += 1;
            if (magicnr != 101) {
                return;
            }

            var running = dv.getInt16(ind); ind += 2;
            var time_diff = dv.getFloat32(ind); ind += 4;
            var state = dv.getInt16(ind); ind += 2;
            var motor_current = dv.getFloat32(ind); ind += 4;
            var filtered_current = dv.getFloat32(ind); ind += 4;
            var erpm = dv.getFloat32(ind); ind += 4;
            var acceleration = dv.getFloat32(ind); ind += 4;
            var braking = dv.getInt16(ind); ind += 2;
            var current_request = dv.getFloat32(ind); ind += 4;
            var true_pitch = dv.getFloat32(ind); ind += 4;
            var pitch = dv.getFloat32(ind); ind += 4;
            var roll = dv.getFloat32(ind); ind += 4;
            var switch_state = dv.getInt16(ind); ind += 2;
            var adc1 = dv.getFloat32(ind); ind += 4;
            var adc2 = dv.getFloat32(ind); ind += 4;

            var start_setpoint = dv.getFloat32(ind); ind += 4;
            var noseangling_setpoint = dv.getFloat32(ind); ind += 4;
            var torque_setpoint = dv.getFloat32(ind); ind += 4;
            var yaw_setpoint = dv.getFloat32(ind); ind += 4;
            var roll_setpoint = dv.getFloat32(ind); ind += 4;
            var turntilt_setpoint = dv.getFloat32(ind); ind += 4;
            var tuneA_current = dv.getFloat32(ind); ind += 4;
            var tuneB_current = dv.getFloat32(ind); ind += 4;
            var tuneC_current = dv.getFloat32(ind); ind += 4;
            var tuneB_weight = dv.getFloat32(ind); ind += 4;
            var tuneC_weight = dv.getFloat32(ind); ind += 4;
            var tuneA_booster_current = dv.getFloat32(ind); ind += 4;
            var tuneB_booster_current = dv.getFloat32(ind); ind += 4;
            var tuneC_booster_current = dv.getFloat32(ind); ind += 4;

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
            
            rt_data_text.text =
                "Time               : " + (1/time_diff).toFixed(0) + " hz\n" +
                "State              : " + stateString + "\n" +
                "Tune A Current     : " + tuneA_current.toFixed(2) + " A\n" +
                "Tune B Current     : " + tuneB_current.toFixed(2) + " A\n" +
                "Tune C Current     : " + tuneC_current.toFixed(2) + " A\n" +
                "Tune (B) Weight    : " + tuneB_weight.toFixed(2) + "\n" +
                "Tune (C) Weight    : " + tuneC_weight.toFixed(2) + "\n" +
                "Current (Request)  : " + current_request.toFixed(2) + " A\n" +
                "Current (Motor)    : " + motor_current.toFixed(2) + " A\n" +
                "Current (Filtered) : " + filtered_current.toFixed(2) + " A\n" +
                "Tune A Booster     : " + tuneA_booster_current.toFixed(2) + " A\n" + 
                "Tune B Booster     : " + tuneB_booster_current.toFixed(2) + " A\n" + 
                "Tune C Booster     : " + tuneC_booster_current.toFixed(2) + " A\n" + 
                "ERPM               : " + (erpm / 1000).toFixed(3) + " / 1000 \n" +
                "Acceleration       : " + acceleration.toFixed(2) + "\n" +
                "Braking            : " + braking + "\n" + 
                "True Pitch         : " + true_pitch.toFixed(2) + "°\n" +
                "Pitch              : " + pitch.toFixed(2) + "°\n" +
                "Roll               : " + roll.toFixed(2) + "°\n" +
                "Switch             : " + switchString + "\n" +
                "ADC1 / ADC2        : " + adc1.toFixed(2) + "V / " + adc2.toFixed(2) + "V \n";
            
            setpoints_text.text =
                "Start Setpoint    : " + start_setpoint.toFixed(2) + "\n" +
                "Noseangling SP    : " + noseangling_setpoint.toFixed(2) + "\n" +
                "Torquetilt SP     : " + torque_setpoint.toFixed(2) + "\n" +
                "Yaw Turntilt SP   : " + yaw_setpoint.toFixed(2) + "\n" +
                "Roll Turntilt SP  : " + roll_setpoint.toFixed(2) + "\n" +
                "Total Turntilt SP : " + turntilt_setpoint.toFixed(4) + "\n";

            if (enableDataDumping == 0) {
                toggleDataDump.text = "Enable Csv Dump"
            }
            else {
                toggleDataDump.text = "Disable Csv Dump"
                if (running == 1) {
                    dumpingCount += 1
                    dumpingText += erpm + "," + braking + "," + tuneB_weight.toFixed(3) + "," + tuneC_weight.toFixed(3) + "," + 
                                   tuneA_current.toFixed(3) + "," + tuneB_current.toFixed(3) + "," + tuneC_current.toFixed(3) + "," + 
                                   current_request.toFixed(3) + "," + tuneA_booster_current.toFixed(3) + "," + 
                                   tuneB_booster_current.toFixed(3) + "," + tuneC_booster_current.toFixed(3) + "\n"
                    if (dumpingCount == 100){
                        // REMOVE ME FOR PUBLIC RELEASE
                        mLogWriter.writeToLogFile(dumpingText)
                        dumpingText = ""
                        dumpingCount = 0
                    }
                }
            }
        }
    }

    function quickSaveTuneOneArr(saveName, arr) {
        for(var i in arr) {
            if (arr[i][0] == "double") {
                settingStorage.setValue(saveName + "_" + arr[i][1], mCustomConf.getParamDouble(arr[i][1]))
            }
            else if (arr[i][0] == "int") {
                settingStorage.setValue(saveName + "_" + arr[i][1], mCustomConf.getParamInt(arr[i][1]))
            }
            else if (arr[i][0] == "bool") {
                settingStorage.setValue(saveName + "_" + arr[i][1], mCustomConf.getParamBool(arr[i][1])?1:0)
            }
            else if (arr[i][0] == "enum") {
                settingStorage.setValue(saveName + "_" + arr[i][1], mCustomConf.getParamEnum(arr[i][1]))
            }
        }
    }

    function quickSaveTune(saveName){
        quickSaveTuneOneArr(saveName, paramsArr)
        quickSaveTuneOneArr(saveName, extraParams)
    }

    function quickLoadTuneOneArr(saveName, arr){
        for(var i in arr) {
            if (arr[i][0] == "double") {
                mCustomConf.updateParamDouble(arr[i][1], settingStorage.value(saveName + "_" + arr[i][1], mCustomConf.getParamDouble(arr[i][1])))
            }
            else if (arr[i][0] == "int") {
                mCustomConf.updateParamInt(arr[i][1], settingStorage.value(saveName + "_" + arr[i][1], mCustomConf.getParamInt(arr[i][1])))
            }
            else if (arr[i][0] == "bool") {
                mCustomConf.updateParamBool(arr[i][1], parseInt(settingStorage.value(saveName + "_" + arr[i][1], mCustomConf.getParamBool(arr[i][1])?1:0)))
            }
            else if (arr[i][0] == "enum") {
                mCustomConf.updateParamEnum(arr[i][1], settingStorage.value(saveName + "_" + arr[i][1], mCustomConf.getParamEnum(arr[i][1])))
            }
        }
        mCommands.customConfigSet(0, mCustomConf)
    }

    function quickLoadTune(saveName){
        quickLoadTuneOneArr(saveName, paramsArr)
        quickLoadTuneOneArr(saveName, extraParams)
    }

    function displaySavedTunes(){
        var tunes = parseCSV(settingStorage.value("tunes_csv", ""))
        for(var i in tunes){
            downloadedTunesModel.append({"tune": tunes[i]})
        }
    }

    function parseCSV(csv){
        var lines=csv.split("\r\n");
        var result = [];
        var headers=lines[0].split(",");

        for(var i=1;i<lines.length;i++){
            var obj = {};
            var currentline=lines[i].split(",");
            for(var j=0;j<headers.length;j++){
                if(currentline[j]){
                    obj[headers[j]] = currentline[j];
                }
            }
            result.push(obj);
        }

        // return JSON.stringify(result); //JSON
        return result; //JavaScript object
    }

    function applyDownloadedTune(tune){
        var found = false;
        for (const [key, value] of Object.entries(tune)) {
            if(!key.startsWith("_")){
                VescIf.emitStatusMessage(key + ": " + value, true);
                if(key.startsWith("double_")){
                    mCustomConf.updateParamDouble(key.substring(7), value);
                    found = true;
                }else if(key.startsWith("int_")){
                    mCustomConf.updateParamInt(key.substring(4), value);
                    found = true;
                }else if(key.startsWith("bool_")){
                    mCustomConf.updateParamBool(key.substring(5), parseInt(value));
                    found = true;
                }else if(key.startsWith("enum_")){
                    mCustomConf.updateParamEnum(key.substring(5), value);
                    found = true;
                } else {
                    VescIf.emitStatusMessage("Couldn't find key '" + key + "', Value: " + value, False);
                }
            }
        }

        if (found) {
            mCommands.customConfigSet(0, mCustomConf)
        }
    }
}
