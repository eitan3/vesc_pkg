import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

import Vedder.vesc.utility 1.0
import Vedder.vesc.commands 1.0
import Vedder.vesc.configparams 1.0

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
                              ["double", "tunea_transition_speed"], ["double", "tuneb_transition_speed"], ["double", "tunec_transition_speed"], 
                              ["enum", "transitions_order"], ["enum", "tunes_mixing_b"], ["enum", "tunes_mixing_c"],
                              ["double", "asym_min_accel_b"], ["double", "asym_max_accel_b"], ["int", "asym_min_erpm_b"], ["int", "asym_max_erpm_b"], 
                              ["double", "asym_min_accel_c"], ["double", "asym_max_accel_c"], ["int", "asym_min_erpm_c"], ["int", "asym_max_erpm_c"], 
                              ["double", "mahony_kp"], ["double", "tiltback_duty_angle"], ["double", "tiltback_duty_speed"], ["double", "tiltback_duty"],
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
                              ["double", "booster_max_pitch"], ["double", "booster_current_limit"], ["double", "booster_min_pitch_b"], 
                              ["double", "booster_max_pitch_b"], ["double", "booster_current_limit_b"], ["double", "booster_min_pitch_c"], 
                              ["double", "booster_max_pitch_c"], ["double", "booster_current_limit_c"]]
    property var extraParams: [["int", "hertz"], ["int", "loop_time_filter"], ["double", "fault_adc1"], ["double", "fault_adc2"], 
                               ["int", "fault_delay_switch_half"], ["int", "fault_delay_switch_full"], ["int", "fault_adc_half_erpm"], 
                               ["bool", "fault_is_single_switch"], ["double", "fault_pitch"], ["double", "fault_roll"], ["int", "fault_delay_pitch"], 
                               ["int", "fault_delay_roll"], ["double", "tiltback_hv"], ["double", "tiltback_lv"], ["double", "startup_speed"],
                               ["double", "brake_current"], ["double", "temp_tiltback_start_offset"], ["double", "temp_tiltback_speed"],
                               ["double", "temp_tiltback_angle"], ["bool", "enable_reverse_stop"], ["bool", "enable_quickstop"],
                               ["int", "quickstop_erpm"], ["double", "quickstop_angle"], ["int", "startup_click_current"], 
                               ["double", "softstart_speed"], ["double", "mahony_kp_roll"], ["double", "mahony_kp_yaw"], 
                               ["double", "bf_accel_confidence_decay"]]
    
    property var is_playing_dc_buzzer: false
    property var enable_buzzer_dc: false
    property var buzzer_dc_threshold: 90
    property var buzzer_error_text: ""
    property var buzzer_volume: 50
    property var audio_notes: []
    property var audio_devs: []
    
    //ToneSynthBuzzer {
    //    id: mBuzzer
    //}
    
    Settings {
        id: settingStorage
    }
    
    Component.onCompleted: {
        displaySavedTunes()

        // Add wave type options
        dcWaveTypeModel.append({"text": "Sine Wave"})
        dcWaveTypeModel.append({"text": "Saw Wave"})

        // Get available notes and load the last selected note
        /*
        audio_notes = mBuzzer.GetNotesAvailable();
        for(var i in audio_notes){
            buzzerDcNoteModel.append({ "text": audio_notes[i] })
        }
        buzzerDcNoteCB.currentIndex = settingStorage.value("dc_note", 0)

        // Scan audio devices and load the last used device
        mBuzzer.ScanForDevices()
        audio_devs = mBuzzer.GetDevicesNames()
        for(var i in audio_devs){
            audioDevicesDbModel.append({ "text": audio_devs[i] })
        }
        audioDevicesDownBox.currentIndex = settingStorage.value("audio_device", 0)
        mBuzzer.SetDeviceByName(audio_devs[audioDevicesDownBox.currentIndex])

        // Load the last used wave type
        dcWaveTypeCB.currentIndex = settingStorage.value("dc_wave_type", 0)
        if (dcWaveTypeCB.currentIndex == 0)
            mBuzzer.SetWaveType("Sine")
        else if (dcWaveTypeCB.currentIndex == 1)
            mBuzzer.SetWaveType("Saw")

        // Load volume
        buzzerVolumeSlider.value = settingStorage.value("buzzer_volume", 50)
        mBuzzer.SetVolume(buzzerVolumeSlider.value)
        
        // Load enable/disable buzzer
        enable_buzzer_dc = settingStorage.value("enable_buzzer_dc", false)
        if (enable_buzzer_dc == "true")
            enable_buzzer_dc = true
        else if (enable_buzzer_dc == "false")
            enable_buzzer_dc = false
        buzzerEnabledForDC.checked = enable_buzzer_dc

        // Load buzzer threshold
        buzzer_dc_threshold = settingStorage.value("buzzer_dc_threshold", "90")
        buzzerDcThreshold_TF.text = buzzer_dc_threshold

        // Load octave
        buzzerDcOctave_TF.text = settingStorage.value("buzzer_dc_octave", 3)
        mBuzzer.SetOctave(buzzerDcOctave_TF.text)
        */
    }

    Component.onDestruction: {
        
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
            property int buttons: 4
            property int buttonWidth: 120

            Repeater {
                model: ["Info", "Tunes", "Buzzer", "Dev"]
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

                Button {
                    id: downloadTunesButton
                    text: "Refresh Tune Archive"
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: {
                        downloadTunesButton.text = "Downloading Tunes..."
                        downloadedTunesModel.clear()
                        var http = new XMLHttpRequest()
                        var url = "http://eitan3.pythonanywhere.com/get_tunes?version=2"
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

            ColumnLayout { // Buzzer Page
                id: buzzerColumn
                Layout.fillWidth: true

                Text {
                    color: Utility.getAppHexColor("lightText")
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                    text: "Audio Output Device: "
                }
                ComboBox {
                    id: audioDevicesDownBox
                    Layout.fillWidth: true
                    editable: false
                    
                    model: ListModel {
                        id: audioDevicesDbModel
                    }
                    
                    onCurrentIndexChanged: {
                        settingStorage.setValue("audio_device", currentIndex)
                        //mBuzzer.SetDeviceByName(audio_devs[currentIndex])
                    }
                }
                RowLayout{
                    Text {
                        color: Utility.getAppHexColor("lightText")
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.WordWrap
                        text: "General Volume:"
                    }
                    Slider {
                        id: buzzerVolumeSlider
                        from: 0
                        value: 50
                        to: 100
                        Layout.fillWidth: true

                        onValueChanged: {
                            if(buzzerVolumeSlider.value != buzzer_volume){
                                buzzer_volume = buzzerVolumeSlider.value
                                //mBuzzer.SetVolume(buzzer_volume)
                                settingStorage.setValue("buzzer_volume", buzzer_volume)
                            }
                        }
                    }
                }
                
                Item {
                    Layout.fillWidth: true
                    height: 25
                    Rectangle {
                        id: rect
                        anchors.fill: parent
                        color: {color = Utility.getAppHexColor("darkAccent")}
                        radius: 5

                        Text {
                            anchors.centerIn: parent
                            color: {color = Utility.getAppHexColor("lightText")}
                            id: buzzerDcSeparator
                            text: "Duty Cycle Buzzer"
                            font.bold: true
                            font.pointSize: 12
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
                CheckBox {
                    id: buzzerEnabledForDC
                    checked: false
                    text: qsTr("Enabled Buzzer for Duty Cycle")
                    onClicked: {
                        enable_buzzer_dc = buzzerEnabledForDC.checked
                        settingStorage.setValue("enable_buzzer_dc", enable_buzzer_dc)
                    }
                }
                RowLayout{
                    Text {
                        color: Utility.getAppHexColor("lightText")
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.WordWrap
                        text: "DC Threshold:"
                    }
                    TextField {
                        id: buzzerDcThreshold_TF
                        placeholderText: "0-100"
                        text: "90"
                        validator: IntValidator{bottom: 0; top: 100;}
                        onTextChanged: {
                            buzzer_dc_threshold = text
                            settingStorage.setValue("buzzer_dc_threshold", text)
                        }
                    }
                }
                RowLayout{
                    Text {
                        color: Utility.getAppHexColor("lightText")
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.WordWrap
                        text: "Octave:"
                    }
                    TextField {
                        id: buzzerDcOctave_TF
                        placeholderText: "0-6"
                        text: "3"
                        validator: IntValidator{bottom: 0; top: 6;}
                        onTextChanged: {
                            settingStorage.setValue("buzzer_dc_octave", buzzerDcOctave_TF.text)
                            //mBuzzer.SetOctave(buzzerDcOctave_TF.text)
                        }
                    }
                    Text {
                        color: Utility.getAppHexColor("lightText")
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.WordWrap
                        text: "Note:"
                    }
                    ComboBox {
                        id: buzzerDcNoteCB

                        model: ListModel {
                            id: buzzerDcNoteModel
                        }
                        
                        onCurrentIndexChanged: {
                            settingStorage.setValue("dc_note", currentIndex)
                        }
                    }
                }
                RowLayout{
                    Text {
                        color: Utility.getAppHexColor("lightText")
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.WordWrap
                        text: "Wave:"
                    }
                    ComboBox {
                        id: dcWaveTypeCB
                        Layout.fillWidth: true
                        editable: false
                        
                        model: ListModel {
                            id: dcWaveTypeModel
                        }
                        
                        onCurrentIndexChanged: {
                            settingStorage.setValue("dc_wave_type", currentIndex)
                            if (currentIndex == 0)
                            {
                                //mBuzzer.SetWaveType("Sine")
                            }
                            else if (currentIndex == 1)
                            {
                                //mBuzzer.SetWaveType("Saw")
                            }
                        }
                    }
                }

                RowLayout{
                    Button {
                        text: "Play"
                        onClicked: {                
                            //mBuzzer.NoteOn(audio_notes[settingStorage.value("dc_note", 0)]) // Include octave and volume
                        }
                    }
                    Button {
                        text: "Stop"
                        onClicked: {                
                            //mBuzzer.NoteOff()
                        }
                    }
                }
                RowLayout{
                    Button {
                        text: "Get Error"
                        onClicked: {              
                            //buzzer_error_text = mBuzzer.GetError()
                        }
                    }
                    Button {
                        text: "Clear Error"
                        onClicked: {         
                            //mBuzzer.ClearError()         
                            buzzer_error_text = ""
                        }
                    }
                }
                
                Text {
                    color: Utility.getAppHexColor("lightText")
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                    text: buzzer_error_text
                }
            }

            ColumnLayout { // Dev Page
                Layout.fillWidth: true

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
                        text: "Dump"
                        Layout.fillWidth: true
                                
                        onClicked: {
                            // REMOVE ME FOR PUBLIC RELEASE
                            VescIf.emitStatusMessage("This feature is disabled!", true)
                        }
                    }

                    
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
            var duty_cycle = dv.getFloat32(ind); ind += 4;
            var motor_current = dv.getFloat32(ind); ind += 4;
            var filtered_current = dv.getFloat32(ind); ind += 4;
            var erpm = dv.getFloat32(ind); ind += 4;
            var acceleration = dv.getFloat32(ind); ind += 4;
            var braking = dv.getInt16(ind); ind += 2;
            var current_request = dv.getFloat32(ind); ind += 4;
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
                "ERPM               : " + erpm.toFixed(2) + "\n" +
                "Acceleration       : " + acceleration.toFixed(2) + "\n" +
                "Braking            : " + braking + "\n" + 
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
                        // mLogWriter.writeToLogFile(dumpingText)
                        dumpingText = ""
                        dumpingCount = 0
                    }
                }
            }

            // Buzzer
            if (enable_buzzer_dc == true)
            {
                var abs_erpm = erpm;
                if (abs_erpm < 0)
                    abs_erpm *= -1;

                if (duty_cycle >= buzzer_dc_threshold / 100 && is_playing_dc_buzzer == false)
                {
                    is_playing_dc_buzzer = true
                    //mBuzzer.NoteOn(audio_notes[settingStorage.value("dc_note", 0)]) // Include octave and volume
                }
                else if (abs_erpm >= 300 && (adc1 <= 1 && adc2 <= 1) && is_playing_dc_buzzer == false)
                {
                    is_playing_dc_buzzer = true
                    //mBuzzer.NoteOn(audio_notes[settingStorage.value("dc_note", 0)]) // Include octave and volume
                }
                else if (duty_cycle < buzzer_dc_threshold / 100 && is_playing_dc_buzzer == true)
                {
                    is_playing_dc_buzzer = false
                    //mBuzzer.NoteOff()
                }
                else if ((adc1 >= 1 || adc2 >= 1) && is_playing_dc_buzzer == true)
                {
                    is_playing_dc_buzzer = false
                    //mBuzzer.NoteOff()
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

        return result;
    }

    function applyDownloadedTune(tune){
        var found = false;
        for (const [key, value] of Object.entries(tune)) {
            if(!key.startsWith("_")){
                VescIf.emitStatusMessage(key + ": " + value, true)
                if(key.startsWith("double_")){
                    mCustomConf.updateParamDouble(key.substring(7), value)
                    found = true;
                }else if(key.startsWith("int_")){
                    mCustomConf.updateParamInt(key.substring(4), value)
                    found = true;
                }else if(key.startsWith("bool_")){
                    mCustomConf.updateParamBool(key.substring(5), parseInt(value))
                    found = true;
                }else if(key.startsWith("enum_")){
                    mCustomConf.updateParamEnum(key.substring(5), value)
                    found = true;
                } else {
                    VescIf.emitStatusMessage("Couldn't find key '" + key + "', Value: " + value, False)
                }
            }
        }

        if (found) {
            mCommands.customConfigSet(0, mCustomConf)
        }
    }
}
