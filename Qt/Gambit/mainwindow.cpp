/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MIT License

Copyright (c) 2015-2018 Andy S. Lustig

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Gambit");
    qApp->setStyleSheet("QListWidget::item:selected { background-color: gold; color: black}");
    //Menu bar
    fileMenu = menuBar()->addMenu("File");
        toggleDebug = new QAction(this);
        toggleDebug->setText("Enter Debug Mode");
        toggleDebug->setShortcut(QKeySequence(tr("Ctrl+D")));
    fileMenu->addAction(toggleDebug);
        gotoSettings = new QAction(this);
        gotoSettings->setText("Settings...");
        gotoSettings->setShortcut(QKeySequence(tr("Ctrl+,")));
    fileMenu->addAction(gotoSettings);
    goMenu = menuBar()->addMenu("Go To");
        openDir = new QAction(this);
        openDir->setText("Default Save Directory");
    goMenu->addAction(openDir);
        gotoApplocation = new QAction(this);
        gotoApplocation->setText("App install directory");
    goMenu->addAction(gotoApplocation);
    helpMenu = menuBar()->addMenu("Help");
        gotoDocs = new QAction(this);
        gotoDocs->setText("User Guide");
    helpMenu->addAction(gotoDocs);
        about = new QAction(this);
        about->setText("About...");
    helpMenu->addAction(about);

    /*Semantic Versioning
        MAJOR version when you make incompatible API changes,
        MINOR version when you add functionality in a backwards-compatible manner, and
        PATCH version when you make backwards-compatible bug fixes.
    */
    aboutDialog = new QMessageBox();
        aboutDialog->setWindowTitle("About");
        gambitVersion = "0.0.2";
        QString aboutString = "\t"+gambitVersion+"\nUpdated:\t12/13/2018";
        aboutDialog->setText("Version:"+aboutString);
        aboutDialog->setStandardButtons(QMessageBox::Close);

    //Experimental setup
    equipmentBox = new QGroupBox("Session Setup");
        equipmentLayout = new QGridLayout();
            cerebroNum_lbl = new QLabel("Cerebro #");
        equipmentLayout->addWidget(cerebroNum_lbl,0,1);
            rig_lbl = new QPushButton("Rig #");
            rig_lbl->setFlat(true);
        equipmentLayout->addWidget(rig_lbl,0,2,Qt::AlignCenter);
            rat_lbl = new QPushButton("Rat-Implant: (LSet/RSet)");
            rat_lbl->setFlat(true);
        equipmentLayout->addWidget(rat_lbl,0,3,1,2,Qt::AlignCenter);
            cerebroNum_spin = new QSpinBox();
            cerebroNum_spin->setAlignment(Qt::AlignCenter);
            cerebroNum_spin->setRange(1,254);
        equipmentLayout->addWidget(cerebroNum_spin,1,1);
            rigSelect = new QListWidget();
        equipmentLayout->addWidget(rigSelect,1,2,4,1,Qt::AlignTop);
            ratSelect = new QListWidget();
            ratSelect->setMinimumWidth(150);
        equipmentLayout->addWidget(ratSelect,1,3,4,2,Qt::AlignTop|Qt::AlignHCenter);
            connectBS_label = new QLabel("Base Station Serial Port");
        equipmentLayout->addWidget(connectBS_label,0,5,1,3,Qt::AlignCenter);
            refresh_btn = new QPushButton("Rescan");
        equipmentLayout->addWidget(refresh_btn,1,5);
            serialPortList = new QComboBox();
            serialPortList->setMinimumWidth(150);
        equipmentLayout->addWidget(serialPortList,1,6,1,2);
            debugCheckbox = new QCheckBox("Debug Mode");
        equipmentLayout->addWidget(debugCheckbox,2,5,1,3,Qt::AlignCenter);
            connect_btn = new QPushButton("Start Session");
            connect_btn->setCheckable(true);
        equipmentLayout->addWidget(connect_btn,3,5,1,3);
        equipmentLayout->setColumnStretch(0,1);
        equipmentLayout->setColumnStretch(8,1);
    equipmentBox->setLayout(equipmentLayout);

    //Cerebro Status
    int titleRow = 0;
    int infoRow = 1;
    int battRow =2;
    int buttonRow = 3;

    cerStatusBox = new QGroupBox("Cerebro Status");
        cerStatusLayout = new QGridLayout();
            implantTitle_lbl = new QLabel("Version Info");
            implantTitle_lbl->setAlignment(Qt::AlignCenter);
            implantTitle_lbl->setWordWrap(true);
            implantTitle_lbl->setStyleSheet("font-weight:bold;");
        cerStatusLayout->addWidget(implantTitle_lbl,titleRow,0,1,2);
            serialNumber_lbl = new QLabel("Serial #\n");
            serialNumber_lbl->setAlignment(Qt::AlignCenter);
            serialNumber_lbl->setWordWrap(true);
        cerStatusLayout->addWidget(serialNumber_lbl,infoRow,0);
            cerFirmware_lbl = new QLabel("Firmware\n");
            cerFirmware_lbl->setAlignment(Qt::AlignCenter);
            cerFirmware_lbl->setWordWrap(true);
        cerStatusLayout->addWidget(cerFirmware_lbl,infoRow,1);
            waveformTitle_lbl = new QLabel("Waveform Parameters");
            waveformTitle_lbl->setAlignment(Qt::AlignCenter);
            waveformTitle_lbl->setWordWrap(true);
            waveformTitle_lbl->setStyleSheet("font-weight:bold;");
        cerStatusLayout->addWidget(waveformTitle_lbl,titleRow,5,1,3);
            cerDelay_lbl = new QLabel("Delay\n");
            cerDelay_lbl->setAlignment(Qt::AlignCenter);
            cerDelay_lbl->setWordWrap(true);
        cerStatusLayout->addWidget(cerDelay_lbl,infoRow,3);
            PosAmp_lbl = new QLabel("PosAmp\n");
            PosAmp_lbl->setAlignment(Qt::AlignCenter);
            PosAmp_lbl->setWordWrap(true);
        cerStatusLayout->addWidget(PosAmp_lbl,infoRow,4);
            posDur_lbl = new QLabel("PosDur\n");
            posDur_lbl->setAlignment(Qt::AlignCenter);
            posDur_lbl->setWordWrap(true);
        cerStatusLayout->addWidget(posDur_lbl,infoRow,5);
            NegAmp_lbl = new QLabel("NegAmp\n");
            NegAmp_lbl->setAlignment(Qt::AlignCenter);
            NegAmp_lbl->setWordWrap(true);
        cerStatusLayout->addWidget(NegAmp_lbl,infoRow,6);
            negDur_lbl = new QLabel("NegDur\n");
            negDur_lbl->setAlignment(Qt::AlignCenter);
            negDur_lbl->setWordWrap(true);
        cerStatusLayout->addWidget(negDur_lbl,infoRow,7);
            freq_lbl = new QLabel("Freq\n");
            freq_lbl->setAlignment(Qt::AlignCenter);
            freq_lbl->setWordWrap(true);
        cerStatusLayout->addWidget(freq_lbl,infoRow,8);
            trainDur_lbl = new QLabel("Train\n");
            trainDur_lbl->setAlignment(Qt::AlignCenter);
            trainDur_lbl->setWordWrap(true);
        cerStatusLayout->addWidget(trainDur_lbl,infoRow,9);
            batteryIndicator = new QProgressBar();
            batteryIndicator->setOrientation(Qt::Horizontal);
            batteryIndicator->setAlignment(Qt::AlignHCenter);
            batteryIndicator->setMinimumWidth(30);
            batteryIndicator->setValue(0);
            batteryIndicator->setMinimum(0);
            batteryIndicator->setMaximum(100);
            batteryIndicator->setTextVisible(false);
            batteryIndicator->setFormat("%p%");
        cerStatusLayout->addWidget(batteryIndicator,battRow,0,1,9);
            battery_lbl = new QLabel(batteryIndicator->text());
            battery_lbl->setAlignment(Qt::AlignJustify);
        cerStatusLayout->addWidget(battery_lbl,battRow,9,1,2,Qt::AlignCenter);
            getInfo_btn = new QPushButton("Check Wireless Connection and Update Battery Status");
        cerStatusLayout->addWidget(getInfo_btn,buttonRow,0,1,10,Qt::AlignCenter);
    cerStatusBox->setLayout(cerStatusLayout);
    cerStatusBox->setStyleSheet("");
    cerStatusBox->setEnabled(false);

    //Base station monitor
    baseBox = new QGroupBox("Base Station Monitor");
        serialMonitorLayout = new QGridLayout();
            baseChannel_lbl = new QLabel("Channel:");
        serialMonitorLayout->addWidget(baseChannel_lbl,0,0);
            baseFilter_label = new QLabel("Filter Duration:");
        serialMonitorLayout->addWidget(baseFilter_label,1,0);
        clearBase_btn = new QPushButton();
            clearBase_btn->setText("Restart Session");
        serialMonitorLayout->addWidget(clearBase_btn,0,1,2,1,Qt::AlignRight);
            baseMonitor = new QPlainTextEdit();
            baseMonitor->setMinimumHeight(360);
        serialMonitorLayout->addWidget(baseMonitor,2,0,1,2);
            saveMonitor_btn = new QPushButton();
            saveMonitor_btn->setText("Save Session");
            saveMonitor_btn->setMinimumHeight(40);
            saveMonitor_btn->setEnabled(true);
        serialMonitorLayout->addWidget(saveMonitor_btn,3,0,1,2);
    baseBox->setMinimumWidth(300);
    #ifdef __APPLE__
        baseBox->setMinimumWidth(350);
    #endif
    baseBox->setLayout(serialMonitorLayout);
    baseBox->setEnabled(false);


    //Waveform Adjustment
    adjustBox = new QGroupBox("Waveform Parameters");
        adjustmentLayout = new QGridLayout();
            startDelay_checkbox = new QCheckBox("Start Delay");
            startDelay_checkbox->setLayoutDirection(Qt::RightToLeft);
        adjustmentLayout->addWidget(startDelay_checkbox,0,2,Qt::AlignRight);
            startDelay_spn = new QDoubleSpinBox();
            startDelay_spn->setDecimals(3);
            startDelay_spn->setRange(0,65.535);
            startDelay_spn->setSingleStep(.050);
            startDelay_spn->setSuffix(" s");
            startDelay_spn->setAlignment(Qt::AlignCenter);
            startDelay_spn->setEnabled(false);
            startDelay_spn->setStyleSheet("text-decoration: line-through;");
        adjustmentLayout->addWidget(startDelay_spn,0,3);
            postive_lbl = new QLabel("Positive");
            postive_lbl->setStyleSheet("font-weight:bold;");
            postive_lbl->setAlignment(Qt::AlignHCenter);
        adjustmentLayout->addWidget(postive_lbl,1,0,1,3);
            negative_lbl = new QLabel("Negative");
            negative_lbl->setStyleSheet("font-weight:bold;");
            negative_lbl->setAlignment(Qt::AlignHCenter);
        adjustmentLayout->addWidget(negative_lbl,1,3,1,3);
            posAmp_spn = new QDoubleSpinBox();
            posAmp_spn->setDecimals(0);
            posAmp_spn->setRange(1,208);
            posAmp_spn->setSingleStep(10);
            posAmp_spn->setSuffix(" µA amplitude");
            posAmp_spn->setAlignment(Qt::AlignCenter);
        adjustmentLayout->addWidget(posAmp_spn,2,0,1,3);
            posDur_spn = new QDoubleSpinBox();
            posDur_spn->setDecimals(0);
            posDur_spn->setRange(30,65535);
            posDur_spn->setSingleStep(10);
            posDur_spn->setSuffix(" µs duration");
            posDur_spn->setAlignment(Qt::AlignCenter);
        adjustmentLayout->addWidget(posDur_spn,3,0,1,3);
            negAmp_spn = new QDoubleSpinBox();
            negAmp_spn->setDecimals(0);
            negAmp_spn->setRange(1,208);
            negAmp_spn->setSingleStep(10);
            negAmp_spn->setPrefix("-");
            negAmp_spn->setSuffix(" µA amplitude");
            negAmp_spn->setAlignment(Qt::AlignCenter);
        adjustmentLayout->addWidget(negAmp_spn,2,3,1,3);
            negDur_spn = new QDoubleSpinBox();
            negDur_spn->setDecimals(0);
            negDur_spn->setRange(30,65535);
            negDur_spn->setSingleStep(50);
            negDur_spn->setSuffix(" µs duration");
            negDur_spn->setAlignment(Qt::AlignCenter);
            negDur_spn->setVisible(true);
        adjustmentLayout->addWidget(negDur_spn,3,3,1,3);
            pulsefreq_lbl = new QLabel("Pulse Frequency");
            pulsefreq_lbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        adjustmentLayout->addWidget(pulsefreq_lbl,4,2);
            freq_spn = new QDoubleSpinBox();
            freq_spn->setDecimals(0);
            freq_spn->setRange(1,500);
            freq_spn->setSingleStep(10);
            freq_spn->setSuffix(" Hz");
            freq_spn->setAlignment(Qt::AlignCenter);
        adjustmentLayout->addWidget(freq_spn,4,3);
            trainDuration_lbl = new QLabel("Train Duration");
            trainDuration_lbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        adjustmentLayout->addWidget(trainDuration_lbl,5,2);
            trainDuration_spn = new QDoubleSpinBox();
            trainDuration_spn->setDecimals(3);
            trainDuration_spn->setRange(0,9999.999);
            trainDuration_spn->setSingleStep(.250);
            trainDuration_spn->setSuffix(" s");
            trainDuration_spn->setAlignment(Qt::AlignCenter);
        adjustmentLayout->addWidget(trainDuration_spn,5,3);
            sendSettings_btn = new QPushButton("Send New Waveform Parameters");
            sendSettings_btn->setAutoDefault(true);
        adjustmentLayout->addWidget(sendSettings_btn,7,0,1,6);
    adjustBox->setLayout(adjustmentLayout);
    adjustBox->setMinimumWidth(300);
    adjustBox->setEnabled(false);
    adjustBox->setPalette(Qt::gray);


    //Triggering & debugging
    bugBox = new QGroupBox("Debug");
        triggerLayout = new QGridLayout();
            trigger_btn = new QPushButton;
            trigger_btn->setText("Trigger");
            trigger_btn->setAutoDefault(true);
        triggerLayout->addWidget(trigger_btn,1,0);
            testProgress = new QProgressBar();
            testProgress->setValue(0);
            testProgress->setVisible(false);
        triggerLayout->addWidget(testProgress,2,0,1,2);
            abort_btn = new QPushButton("Stop");
        triggerLayout->addWidget(abort_btn,1,1);
            toggleStim_btn = new QPushButton("Toggle Stim");
        triggerLayout->addWidget(toggleStim_btn,2,0);
            stimOff_btn = new QPushButton("Off");
        triggerLayout->addWidget(stimOff_btn,2,1);
            macroText = new QLineEdit();
        triggerLayout->addWidget(macroText,3,0,Qt::AlignRight);
            macro_btn = new QPushButton("Send Text");
        triggerLayout->addWidget(macro_btn,3,1);
            cerMonitor_btn = new QPushButton("Cerebro Monitor");
        triggerLayout->addWidget(cerMonitor_btn,4,0,1,2);
    bugBox->setLayout(triggerLayout);
    bugBox->setEnabled(false);
    bugBox->setPalette(Qt::gray);

    //Cerebro Monitor
    downloaderDialog = new QDialog();
        connectionLayout2 = new QGridLayout();
            connectLU_label = new QLabel("Cerebro Serial Port");
        connectionLayout2->addWidget(connectLU_label,0,0,1,3,Qt::AlignCenter);
            refresh2_btn = new QPushButton("Rescan");
        connectionLayout2->addWidget(refresh2_btn,1,0);
            serialPortList2 = new QComboBox();
            serialPortList2->setMinimumWidth(150);
        connectionLayout2->addWidget(serialPortList2,1,1,1,2);
            connect2_btn = new QPushButton("Connect to Cerebro");
            connect2_btn->setCheckable(true);
        connectionLayout2->addWidget(connect2_btn,2,0,1,3);
            download_title = new QLabel("<h3><u>Cerebro Monitor</u>:");
            download_title->setEnabled(false);
        connectionLayout2->addWidget(download_title,3,0,1,2);
            clearDownload_btn = new QPushButton("Clear Monitor");
            clearDownload_btn->setEnabled(false);
        connectionLayout2->addWidget(clearDownload_btn,3,2,Qt::AlignRight);
            cerebroMonitor = new QPlainTextEdit();
            cerebroMonitor->setMinimumWidth(275);
            cerebroMonitor->setMinimumHeight(400);
            cerebroMonitor->setEnabled(false);
        connectionLayout2->addWidget(cerebroMonitor,4,0,1,3);
            queryCerebro_btn = new QPushButton("Get Cerebro Info");
            queryCerebro_btn->setEnabled(false);
        connectionLayout2->addWidget(queryCerebro_btn,5,0,1,3);
            channelLabel = new QLabel("New Channel");
            channelLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            channelLabel->setEnabled(false);
        connectionLayout2->addWidget(channelLabel,6,0);
            channelSpinBox = new QSpinBox();
            channelSpinBox->setRange(0,255);
            channelSpinBox->setSingleStep(1);
            channelSpinBox->setAlignment(Qt::AlignCenter);
            channelSpinBox->setEnabled(false);
        connectionLayout2->addWidget(channelSpinBox,6,1);
            channelSendButton = new QPushButton("Update Channel");
            channelSendButton->setEnabled(false);
        connectionLayout2->addWidget(channelSendButton,6,2);
        connectionLayout2->setRowStretch(4,1);
    downloaderDialog->setLayout(connectionLayout2);
    downloaderDialog->setWindowTitle("Cerebro Monitor");
    downloaderDialog->setWindowFlags(downloaderDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);//removes "?" from dialog box


    //Session Start Dialog
    sessionStartDialog = new QDialog();
        sessionStartLayout = new QGridLayout();
            retry_btn = new QPushButton("Retry Connection");
        sessionStartLayout->addWidget(retry_btn,0,0);
            newCerebro_btn = new QPushButton("Setup New Cerebro");
            newCerebro_btn->setEnabled(false);
        sessionStartLayout->addWidget(newCerebro_btn,0,1);
            newCerebro_lbl = new QLabel("Serial Number:");
        sessionStartLayout->addWidget(newCerebro_lbl,1,0,Qt::AlignRight);
            newCerebro_spin = new QSpinBox();
            newCerebro_spin->setRange(1,254);
        sessionStartLayout->addWidget(newCerebro_spin,1,1);
            setSerial_btn = new QPushButton("Set Serial Number");
        sessionStartLayout->addWidget(setSerial_btn,2,1);
            sessionStartMonitor = new QPlainTextEdit();
            sessionStartMonitor->setMinimumHeight(400);
        sessionStartLayout->addWidget(sessionStartMonitor,3,0,1,2);
            baseConnected_lbl = new QLabel("Base Station Connected \u2718");
            baseConnected_lbl->setAlignment(Qt::AlignCenter);
        sessionStartLayout->addWidget(baseConnected_lbl,4,0,1,2);
            cerebroConnected_lbl = new QLabel("Cerebro Wireless Connection \u2718");
            cerebroConnected_lbl->setAlignment(Qt::AlignCenter);
        sessionStartLayout->addWidget(cerebroConnected_lbl,5,0,1,2);
            implantSettingsMatch_lbl = new QLabel("Implant Settings Match \u2718");
            implantSettingsMatch_lbl->setAlignment(Qt::AlignCenter);
        sessionStartLayout->addWidget(implantSettingsMatch_lbl,6,0,1,2);
            startSession_btn = new QPushButton("Start Session");
            startSession_btn->setEnabled(true);
        sessionStartLayout->addWidget(startSession_btn,7,0,1,2);
        QFont codefont("Arial Unicode MS");
        baseConnected_lbl->setFont(codefont);
        cerebroConnected_lbl->setFont(codefont);
        implantSettingsMatch_lbl->setFont(codefont);
        baseConnected_lbl->setStyleSheet("color:red");
        cerebroConnected_lbl->setStyleSheet("color:red");
        implantSettingsMatch_lbl->setStyleSheet("color:red");
    sessionStartDialog->setLayout(sessionStartLayout);
    sessionStartDialog->setWindowTitle("Startup Sequence");
    sessionStartDialog->setWindowFlags(sessionStartDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);//removes "?" from dialog box

    //Add Elements to Main Layout
    mainLayout = new QGridLayout();
    mainLayout->addWidget(equipmentBox,0,0,1,3);
    mainLayout->addWidget(cerStatusBox,1,0,1,3);
    mainLayout->addWidget(baseBox,2,1,4,2);
    mainLayout->addWidget(adjustBox,2,0);
    mainLayout->addWidget(bugBox,3,0);
//    mainLayout->setColumnStretch(2,1);
//    mainLayout->setRowStretch(5,1);

    QWidget *window = new QWidget();
    window->setLayout(mainLayout);
    setCentralWidget(window);


    settingsDlog = new settingsDialog();


#ifdef _WIN32   //--------Windows USB parse settings------------
    usbTag = "COM";
//    usbDescription = "Adafruit Feather 32u4 (800C:00)";
#elif __APPLE__ //---------Mac USB parse settings---------------
    usbTag  = "cu.usbmodem";
//    usbDescription = "Feather 32u4";
#elif __linux__ //---------Linux USB parse settings-------------
    usbTag  = "ttyUSB";
//    usbDescription = "Feather 32u4";
#endif

    //program setup
    serial = new QSerialPort(this);
    serial2 = new QSerialPort(this);
    startingUp = true;
    baseConnected = false;
    cerebroConnected = false;
    inTestloop = false;
    testCount = 0;
    timer = new QTimer(this);
    onTimeString = "";
    offTimeString = "";
    sessionHasBegun = false;
    baseChannel = -1;

    if(QSysInfo::WindowsVersion==48){ //If Windows XP
        onTimeString = " ms\t\t\nOn Time:\t\t";
        offTimeString = " ms\nOff Time:\t\t";
    }
    else{
        onTimeString = " ms\t\t\nOn Time:\t";
        offTimeString = " ms\nOff Time:\t";
    }
    applySettings();


    //menu functions
    connect(openDir,SIGNAL(triggered()),this,SLOT(gotoDir()));
    connect(gotoApplocation,SIGNAL(triggered()),this,SLOT(gotoAppLocation()));
    connect(gotoDocs,SIGNAL(triggered()),this,SLOT(openDocs()));
    connect(about,SIGNAL(triggered(bool)),aboutDialog,SLOT(exec()));
    connect(toggleDebug,SIGNAL(triggered()),this,SLOT(setDebug()));
    connect(debugCheckbox,SIGNAL(clicked(bool)),this,SLOT(setDebug()));
    connect(gotoSettings,SIGNAL(triggered()),settingsDlog,SLOT(openSettings()));

    //Startup Sequence
    connect(startSession_btn,SIGNAL(clicked()),this,SLOT(startSession()));
    connect(sessionStartDialog,SIGNAL(rejected()),this,SLOT(connectBasePort()));
    connect(retry_btn,SIGNAL(clicked(bool)),this,SLOT(checkForBase()));
    connect(newCerebro_btn,SIGNAL(clicked()),this,SLOT(setupCerebro()));
    connect(setSerial_btn,SIGNAL(clicked()),this,SLOT(updateSerial()));

    //Port Connections
    connect(rig_lbl,SIGNAL(clicked()),settingsDlog,SLOT(openSettings()));
    connect(rat_lbl,SIGNAL(clicked()),settingsDlog,SLOT(openSettings()));
    connect(settingsDlog,SIGNAL(dialogClosed()),this,SLOT(applySettings()));
    connect(refresh_btn,SIGNAL(clicked()),this,SLOT(fillBasestationPorts()));
    connect(refresh2_btn,SIGNAL(clicked()),this,SLOT(fillCerebroPorts()));
    connect(connect_btn,SIGNAL(clicked()),this,SLOT(connectBasePort()));
    connect(connect2_btn,SIGNAL(clicked()),this,SLOT(connectCerebroPort()));

    //Monitors
    connect(serial,SIGNAL(readyRead()),this,SLOT(readFromBase()));
    connect(serial2,SIGNAL(readyRead()),this,SLOT(readFromCerebro()));
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(errorMsg()));
    connect(clearBase_btn,SIGNAL(clicked()),this,SLOT(clearMonitor()));
    connect(clearDownload_btn,SIGNAL(clicked()),this,SLOT(clearMonitor2()));
    connect(saveMonitor_btn,SIGNAL(clicked()),this,SLOT(saveFile()));
    connect(queryCerebro_btn,SIGNAL(clicked()),this,SLOT(getCerebroInfoOverSerial()));
    connect(channelSendButton,SIGNAL(clicked()),this,SLOT(updateChannel()));
    connect(channelSpinBox,SIGNAL(editingFinished()),this,SLOT(updateChannel()));


    //Cerebro Parameters
    connect(startDelay_checkbox,SIGNAL(clicked()),this,SLOT(startDelayChecked()));
    connect(sendSettings_btn,SIGNAL(clicked()),this,SLOT(set()));

    //Debug Commands
    connect(macro_btn,SIGNAL(clicked()),this,SLOT(macro()));
    connect(macroText,SIGNAL(returnPressed()),this,SLOT(macro()));
    connect(abort_btn,SIGNAL(clicked()),this,SLOT(abort()));
    connect(getInfo_btn,SIGNAL(clicked(bool)),this,SLOT(getBatteryStatus()));
    connect(trigger_btn,SIGNAL(clicked()),this,SLOT(sendTrigger()));
    connect(toggleStim_btn,SIGNAL(clicked()),this,SLOT(toggleStim()));
    connect(stimOff_btn,SIGNAL(clicked()),this,SLOT(stimOff()));

}


//--------------------------------------------------------------------------------------------------------//
MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (baseConnected){
        QMessageBox alert;
            alert.setWindowTitle("Exit");
            alert.setIcon(QMessageBox::Warning);
            alert.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
            alert.setDefaultButton(QMessageBox::Cancel);
            alert.setEscapeButton(QMessageBox::Cancel);
            alert.setText("Are you sure you want to exit without saving?");
        if (alert.exec() == QMessageBox::Cancel){
            event->ignore();
        }
        else{
            event->accept();
        }
    }
    else{
        event->accept();
    }
}

void MainWindow::applySettings()
{
    //get the Qstringlists from memory
    QSettings settings("Bobcat Engineering","Gambit");
    settings.beginGroup("sessionLists");
        rigList = settings.value("rigList").toStringList();
        ratList = settings.value("ratList").toStringList();
    settings.endGroup();

    //populate the list widgets
    rigSelect->clear();
    ratSelect->clear();
    rigSelect->addItems(rigList);
    ratSelect->addItems(ratList);
    int itemHeight = 25;
    int itemWidth = 75;
    rigSelect->setMaximumSize(itemWidth,itemHeight*rigSelect->count());
    ratSelect->setMaximumSize(itemWidth,itemHeight*ratSelect->count());

    //
    settings.beginGroup("Features");
        pythonEnabled = settings.value("pythonEnabled").toBool();
        showHistogram = settings.value("showHistogram").toBool();
        mcubeEnabled = settings.value("mcubeEnabled").toBool();
    settings.endGroup();
//    toolMenu->setEnabled(pythonEnabled);


    //Populate COM port Dropdowns
    settings.beginGroup("usbPorts");
        aliasStringList = settings.value("portList").toStringList();
    settings.endGroup();
    fillBasestationPorts();
    fillCerebroPorts();

    debugOn = false;
    showDebug();
    if (cerebroConnected){
        connectCerebroPort(); //disconnect download port after settings change
    }
}

void MainWindow::fillBasestationPorts()
{
    refresh_btn->setDown(1);
    serialPortList->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        if (!info.isBusy()){
//            qDebug()<<info.description();
//            qDebug()<<"manufacturer"<<info.manufacturer();
//            qDebug()<<info.portName();
//            qDebug()<<info.productIdentifier();
//            qDebug()<<info.serialNumber();
            if(info.manufacturer().contains("Cypress")){
                if (!aliasStringList.filter(info.portName()).isEmpty()){
                    if (!aliasStringList.filter(info.portName())[0].contains("Downloader",Qt::CaseInsensitive)){
                        list << aliasStringList.filter(info.portName())[0];
                        serialPortList->addItem(list.first(), list);
                    }
                }
                else{
                    list << info.portName();
                    serialPortList->addItem(list.first(), list);
                }
            }
        }
    }
    refresh_btn->setDown(0);
}

void MainWindow::fillCerebroPorts()
{
    refresh2_btn->setDown(1);
    serialPortList2->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
//        qDebug()<<"Available Ports";
        if (!info.isBusy()){
//            qDebug()<<"description"<<info.description();
//            qDebug()<<"manufacturer"<<info.manufacturer();
//            qDebug()<<info.portName();
//            qDebug()<<info.productIdentifier();
//            qDebug()<<info.serialNumber();
            if (!aliasStringList.filter(info.portName()).isEmpty()){
                qDebug()<<aliasStringList.filter(info.portName())[0]<<"added";
                list << aliasStringList.filter(info.portName())[0];
                serialPortList2->addItem(list.first(), list);
            }
            else{
//                qDebug()<<info.portName()<<"added";
                list << info.portName();
                serialPortList2->addItem(list.first(), list);
            }
        }
    }
    refresh2_btn->setDown(0);
}

void MainWindow::connectBasePort()
{
    newCerebro_btn->setEnabled(false);
    setSerial_btn->setVisible(false);
    newCerebro_lbl->setVisible(false);
    newCerebro_spin->setVisible(false);
    baseConnected_lbl->setText("Base Station Connection \u2718");
    baseConnected_lbl->setStyleSheet("color:red");
    cerebroConnected_lbl->setText("Cerebro Wireless Connection \u2718");
    cerebroConnected_lbl->setStyleSheet("color:red");
    implantSettingsMatch_lbl->setText("Implant Settings Match \u2718");
    implantSettingsMatch_lbl->setStyleSheet("color:red");
    if((rigSelect->selectedItems().size()==0) && !baseConnected && !debugOn ){ //didn't select rig
        QMessageBox alert;
        alert.setText("Please select a Rig # to continue");
        alert.setIcon(QMessageBox::Warning);
        alert.setWindowTitle("Missing Rig #");
        alert.exec();
        }
    else if((ratSelect->selectedItems().size()==0) && !baseConnected && !debugOn){ //didn't select rat
        QMessageBox alert;
        alert.setText("Please select a Rat ID to continue");
        alert.setIcon(QMessageBox::Warning);
        alert.setWindowTitle("Missing Rat ID");
        alert.exec();
    }
    else{ //  disable and enable components depending on whether we are connecting or disconnecting to the base station
        gotoSettings->setEnabled(baseConnected);
        toggleDebug->setEnabled(baseConnected);
        rig_lbl->setEnabled(baseConnected);
        rat_lbl->setEnabled(baseConnected);
        rigSelect->setEnabled(baseConnected);
        ratSelect->setEnabled(baseConnected);
        connectBS_label->setEnabled(baseConnected);
        serialPortList->setEnabled(baseConnected);
        refresh_btn->setEnabled(baseConnected);
        debugCheckbox->setEnabled(baseConnected);
        adjustBox->setEnabled(!baseConnected);
        bugBox->setEnabled(!baseConnected && debugOn);
        baseBox->setEnabled(!baseConnected);
        cerStatusBox->setEnabled(!baseConnected);
        if(!baseConnected){ //connect to serial port
            //isolate the COMXX part of the port name
            QString tempPortName = serialPortList->currentText();
            tempPortName.remove(0,tempPortName.indexOf("("+usbTag)+1);
            tempPortName.remove(QChar (')'));
            //open serial connection
            qDebug()<<"Attemtping to connect to base station through port "<<tempPortName;
            serial->setPortName(tempPortName);
            serial->setBaudRate(57600);
            serial->setDataBits(QSerialPort::Data8);
            serial->setParity(QSerialPort::NoParity);
            serial->setStopBits(QSerialPort::OneStop);
            serial->setFlowControl(QSerialPort::NoFlowControl);
            qDebug()<<"opening serial"<<serial->open(QIODevice::ReadWrite);
            qDebug()<<serial->errorString();
            connect_btn->setText("Disconnect");
            if(!debugOn){
                QStringList ratInfo = ratSelect->currentItem()->text().split(QRegExp("[:,\\-()\\s\\/]"),QString::SkipEmptyParts);
                qDebug()<<"rat info "<<ratInfo;
                ratNumber = ratInfo[0];
                rigNumber = rigSelect->currentItem()->text();
                titleLeftPower = ratInfo[2].toInt();
                titleRightPower = ratInfo[3].toInt();
                setWindowTitle("Rig " + rigNumber + " Rat " + ratInfo[0] );}
            else{
                ratNumber = "9999";
                rigNumber = "7.3";
                titleLeftPower = 100;
                titleRightPower = 100;
                setWindowTitle("Gambit (Debug)");
            }
            clearBase_btn->setEnabled(false);
            baseConnected = true;
            errorThrown = false;
            sessionStartMonitor->clear();
            QTimer::singleShot(750, this, SLOT(checkForBase()));
            sessionStartDialog->exec();
        }
        else{ //disconnect from serial port
            cerStatusBox->setStyleSheet("");
            QString time = "\r" + serialPortList->currentText() + " Disconnected - " + QDate::currentDate().toString() + " " + QTime::currentTime().toString()+ "\r---------------------------------------------------------\r";
            baseMonitor->textCursor().insertText(time);
            serial->close();
            setWindowTitle("Gambit");
            debugOn = false;
            showDebug();
            baseConnected = false;
            sessionHasBegun = false;
            clearMonitor();
            cerebroConnected_lbl->setText("Cerebro Wireless Connection \u2714");
            baseConnected_lbl->setText("Base Station Connected \u2718");
            cerebroConnected_lbl->setText("Cerebro Wireless Connection \u2718");
            implantSettingsMatch_lbl->setText("Implant Settings Match \u2718");
            baseConnected_lbl->setStyleSheet("color:green");
            cerebroConnected_lbl->setStyleSheet("color:green");
            implantSettingsMatch_lbl->setStyleSheet("color:green");
        }
    }
    connect_btn->setChecked(baseConnected);
}

void MainWindow::connectCerebroPort()
{
    QString tempPortName = serialPortList2->currentText();
    if (tempPortName.isEmpty() & !cerebroConnected &!debugOn){
        QMessageBox noSerial;
        noSerial.setText("Make sure a Downloader Cord is connected \nand click ''Rescan''' to see available Serial Ports.");
        noSerial.setIcon(QMessageBox::Warning);
        noSerial.setWindowTitle("No Available Serial Ports");
        noSerial.exec();
    }
    else{
        connectLU_label->setEnabled(cerebroConnected);
        serialPortList2->setEnabled(cerebroConnected);
        refresh2_btn->setEnabled(cerebroConnected);
        cerebroMonitor->setEnabled(!cerebroConnected);
        download_title->setEnabled(!cerebroConnected);
        clearDownload_btn->setEnabled(!cerebroConnected);
        channelLabel->setEnabled(!cerebroConnected);
        channelSendButton->setEnabled(!cerebroConnected);
        channelSpinBox->setEnabled(!cerebroConnected);
        queryCerebro_btn->setEnabled(!cerebroConnected);
        if(!cerebroConnected){//if it's not connected, we'll connect it
            //isolate the COMXX part of the port name
            tempPortName.remove(0,tempPortName.indexOf("("+usbTag)+1);
            tempPortName.remove(QChar (')'));
            //open serial connection
            qDebug()<<"attempting to connect to "<<tempPortName;
            serial2->setPortName(tempPortName);
            serial2->setBaudRate(QSerialPort::Baud115200);
            serial2->setDataBits(QSerialPort::Data8);
            serial2->setParity(QSerialPort::NoParity);
            serial2->setStopBits(QSerialPort::OneStop);
            serial2->setFlowControl(QSerialPort::NoFlowControl);
            serial2->open(QIODevice::ReadWrite);
            qDebug()<<serial2->errorString();
            serial2->setDataTerminalReady(true);
            bool dtrHigh = serial2->isDataTerminalReady();
            qDebug()<<"DTR high?"<<dtrHigh; // DTR needs to be high! https://electronics.stackexchange.com/questions/128839/how-to-get-32u4-arduino-to-ignore-dtr-for-usb-serial
            connect2_btn->setText("Disconnect");
            cerebroConnected = true;
            getCerebroInfoOverSerial();
        }
        else{// if it is connected, we'll disconnect
            serial2->close();
            connect2_btn->setText("Connect To Cerebro");
            cerebroConnected = false;
            fillCerebroPorts();
        }
    }
    connect2_btn->setChecked(cerebroConnected);
}

void MainWindow::sendTime()
{
    qDebug()<<"Sending Time";
    if (serial->isOpen()){
        startTime = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm");
        QString startup =  QString("\n~~~~~~~~~New Session~~~~~~~~~\n"
                                   "Gambit Version,%1"
                                   "\nRig,%2"
                                   "\nRat,%3"
                                   "\nStart Time,%4").arg(gambitVersion,rigNumber,ratNumber,startTime);
        baseMonitor->textCursor().insertText(startup);
        checkForBase();
        connect_btn->setEnabled(true);
        clearBase_btn->setEnabled(true);
    }
}

void MainWindow::readFromBase()
{
    if(sessionHasBegun){
        baseMonitor->moveCursor(QTextCursor::End);
    }
    else{
        sessionStartMonitor->moveCursor(QTextCursor::End);
    }
    while(!serial->atEnd()){
        baseBuffer += serial->readAll();
    }
    int dataStart = baseBuffer.indexOf("*");
    int dataEnd = baseBuffer.indexOf("&");
//    qDebug()<<"buffer,datastart,dataend"<<baseBuffer<<dataStart<<dataEnd;

    if (dataStart!=-1 && dataEnd!=-1){
        QString dataString = baseBuffer.mid(dataStart+1,dataEnd-dataStart-1);
        baseBuffer.remove(dataStart,dataEnd+1-dataStart);
//        qDebug()<<"data removed, new buffer: "<<baseBuffer;
        QStringList dataFromBaseMsg = dataString.split("~");
        qDebug()<<"data = "<<dataFromBaseMsg;
//        qDebug()<<"length "<<dataFromBaseMsg.length();
        cerStatusBox->setStyleSheet("");
        if (dataFromBaseMsg[0] == "Cerebro Info"){
            cerebroConnected_lbl->setText("Cerebro Wireless Connection \u2714");
            cerebroConnected_lbl->setStyleSheet("color:green");
            cerFirmware_lbl->setText("Firmware\n"+dataFromBaseMsg[1]);
            serialNumber_lbl->setText("Serial #\n"+dataFromBaseMsg[2]);
        }
        else if (dataFromBaseMsg[0] == "BaseOn"){
            baseConnected_lbl->setText("Base Station Connection \u2714");
            baseConnected_lbl->setStyleSheet("color:green");
            newCerebro_btn->setEnabled(true);
        }
        else if (dataFromBaseMsg[0] == "Base Channel"){
            baseChannel = dataFromBaseMsg[1].toInt();
            baseChannel_lbl->setText("Channel: "+ QString::number(baseChannel));
        }
        else if (dataFromBaseMsg[0] == "Diode Powers"){

            if (((dataFromBaseMsg[1].toInt() != titleLeftPower) || (dataFromBaseMsg[2].toInt() != titleRightPower))  && !sessionHasBegun){
                QTimer::singleShot(500, this, SLOT(matchPowers()));
            }
            else{
                implantSettingsMatch_lbl->setText("Implant Settings Match \u2714");
                implantSettingsMatch_lbl->setStyleSheet("color:green");
            }
        }
        else if (dataFromBaseMsg[0] == "Waveform"){
            cerDelay_lbl->setText("Delay\n"+dataFromBaseMsg[1]);
            qDebug()<<"first new start delay:"<<dataFromBaseMsg[1].toInt()/1000.0;

            if (dataFromBaseMsg[1].toInt()/1000.0 > 0){
                startDelay_checkbox->setChecked(true);
                startDelay_spn->setValue(dataFromBaseMsg[1].toInt()/1000.0);
            }
            else{
                startDelay_checkbox->setChecked(false);
            }
            PosAmp_lbl->setText("PosAmp\n"+dataFromBaseMsg[2]); posAmp_spn->setValue(dataFromBaseMsg[2].toInt());
            posDur_lbl->setText("PosDur\n"+dataFromBaseMsg[3]); posDur_spn->setValue(dataFromBaseMsg[3].toInt());
            NegAmp_lbl->setText("NegAmp\n"+dataFromBaseMsg[4]); negAmp_spn->setValue(dataFromBaseMsg[4].toInt());
            negDur_lbl->setText("NegDur\n"+dataFromBaseMsg[5]); negDur_spn->setValue(dataFromBaseMsg[5].toInt());
            freq_lbl->setText("Freq\n"+ dataFromBaseMsg[6]);    freq_spn->setValue(dataFromBaseMsg[6].toInt());
            trainDur_lbl->setText("Train\n"+dataFromBaseMsg[7]);trainDuration_spn->setValue(dataFromBaseMsg[7].toInt()/1000.0);

            startDelayChecked();
            updateFilter();
        }
        else if (dataFromBaseMsg[0] == "Battery"){
//            qDebug()<<dataFromBaseMsg[1];
            cerStatusBox->setStyleSheet("");
            batteryIndicator->setValue(dataFromBaseMsg[1].toInt());
            battery_lbl->setText(batteryIndicator->text()+" Battery Life");
        }
        else if (dataFromBaseMsg[0] == 'X'){
            cerStatusBox->setStyleSheet("color:#ed0b0b");
        }
    }
    int newLineIndex = baseBuffer.lastIndexOf("\n");
    if (newLineIndex>-1 ){ //if we come the end of the line reaches the buffer, print the buffer to the monitor
//        qDebug()<<"newline Index"<<newLineIndex<<baseBuffer.mid(0,newLineIndex+1)<<"||"<<baseBuffer.mid(newLineIndex+1,baseBuffer.length()-newLineIndex-1);
        if(sessionHasBegun){
            baseMonitor->insertPlainText(baseBuffer.mid(0,newLineIndex+1));
        }
        else{
            sessionStartMonitor->insertPlainText(baseBuffer.mid(0,newLineIndex+1));
        }
        baseBuffer = baseBuffer.mid(newLineIndex+1,baseBuffer.length()-newLineIndex-1);
    }
    baseMonitor->ensureCursorVisible();
}
void MainWindow::readFromCerebro()
{
    cerebroMonitor->moveCursor(QTextCursor::End);
    cerebroMonitor->insertPlainText(serial2->readAll());
    cerebroMonitor->ensureCursorVisible();
}

void MainWindow::clearMonitor()
{
    qDebug()<<"clearing monitor";
    baseMonitor->clear();
    baseFilter_label->setText("Filter Duration:");
    baseChannel_lbl->setText("Channel:");
    batteryIndicator->setValue(0);
    battery_lbl->setText(batteryIndicator->text());
    serialNumber_lbl->setText("Serial #\n");
    cerFirmware_lbl->setText("Firmware\n");
    PosAmp_lbl->setText("PosAmp\n");
    NegAmp_lbl->setText("NegAmp\n");
    cerDelay_lbl->setText("Delay\n");
    negDur_lbl->setText("NegDur\n");
    posDur_lbl->setText("PosDur\n");
    trainDur_lbl->setText("Train\n");
    freq_lbl->setText("Freq\n");
    QTimer::singleShot(1000, this, SLOT(sendTime()));
}

void MainWindow::clearMonitor2()
{
    cerebroMonitor->clear();
}

void MainWindow::set()
{
    QString startDelay = "0";
    if(startDelay_checkbox->isChecked()){
        startDelay = QString::number(startDelay_spn->value()*1000);
    }
    QString positiveAmplitude = QString::number(posAmp_spn->value());
    QString positiveDuration = QString::number(posDur_spn->value());
    QString negativeAmplitude = QString::number(negAmp_spn->value());
    QString negativeDuration = QString::number(negDur_spn->value());
    QString pulseFrequency = QString::number(freq_spn->value());
    QString trainDuration = QString::number(trainDuration_spn->value()*1000);

    //send parameters to base station
    QString msg = "W,"+
            startDelay +","+
            positiveAmplitude +","+
            positiveDuration +","+
            negativeAmplitude + ","+
            negativeDuration + ","+
            pulseFrequency + ","+
            trainDuration + "\n";
    serial->write(msg.toLocal8Bit());
    qDebug()<<"message sent"<<msg;

}

void MainWindow::sendTrigger()
{
    QString msg = "T";
    serial->write(msg.toLocal8Bit());
    qDebug()<<msg<<" Sent";
}


void MainWindow::saveFile()
{
    QString msg = "M";
    serial->write(msg.toLocal8Bit());
    qDebug()<<msg<<" Sent";
    bool continueSaving = true;
    //get the default save directory
    QSettings settings("Bobcat Engineering","Gambit");
    settings.beginGroup("Saving");
    QString saveDirectoryPath = settings.value("DefaultDir").toString();
    settings.endGroup();
    //if the default save directory hasn't been set yet, use the desktop as the save directory
    if(saveDirectoryPath.isEmpty()){
        saveDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    }
    QDateTime saveTime = saveTime.currentDateTime();
    //check if the rat# folder exists in the save directory, if not go ahead and make a new rat# folder
    if(!QDir(saveDirectoryPath+ "/" + ratNumber).exists()){
        QMessageBox newFolder;
        QPushButton *connectButton = newFolder.addButton(tr("Yes, Create new folder"), QMessageBox::YesRole);
        newFolder.addButton(QMessageBox::Cancel);
        newFolder.setText("'" + ratNumber + "' folder does not currently exist in the '" + saveDirectoryPath + "' directory. Would you like to create one?");
        newFolder.exec();
        if (newFolder.clickedButton() == connectButton) {
            // connect
            QDir().mkdir(saveDirectoryPath+ "/" + ratNumber);
        }
        else{
            continueSaving = false;
        }
    }
    if(continueSaving){
        if (!ratNumber.isEmpty()){
            //save the base station log
            saveName1 = QFileDialog::getSaveFileName(this,
                tr("Save Base Station Log"), saveDirectoryPath + "/" + ratNumber + "/" + ratNumber+ "_" + startTime +"_baseLog",tr("(*.csv)") );
            qDebug()<<saveName1;
            if (!saveName1.isEmpty()){
                QFile file(saveName1);
                file.open(QIODevice::WriteOnly | QIODevice::Text);
                QTextStream out(&file);
                QString log = baseMonitor->toPlainText();
                out << log;
                file.close();
                //Disconnect the com port if connected.
                if(baseConnected){
                    connectBasePort();
                    fillBasestationPorts();
                }
                rigSelect->clearSelection();
                ratSelect->clearSelection();
                baseMonitor->clear();
                debugOn = false;
                showDebug();
            }
        }
    }
}


void MainWindow::errorMsg()
{
    if (((serial->error()==11)|(serial->error()==7)|(serial->error()==8)|(serial->error()==9)&& errorThrown==false)){                //http://doc.qt.io/qt-5/qserialport.html#SerialPortError-enum
        errorThrown = true;
        baseMonitor->textCursor().insertText("ERROR\r");
        connectBasePort();
        fillBasestationPorts();
        QMessageBox alert;
        alert.setText("Serial Communication Error");
        alert.setIcon(QMessageBox::Warning);
        alert.setWindowTitle("Cerebro Control");
        alert.exec();
    }
}

void MainWindow::updateFilter(){
    baseFilter = startDelay_spn->value()+ trainDuration_spn->value();
    baseFilter_label->setText("Filter Duration: "+ QString::number(baseFilter) + " s");
 }


void MainWindow::abort(){
    QString msg = "A\n";
    serial->write(msg.toLocal8Bit());
    qDebug()<<msg<<"sent";
}
void MainWindow::macro(){
    QString msg = macroText->text()+"\n";
    serial->write(msg.toLocal8Bit());
    qDebug()<<msg<<"sent";
}

void MainWindow::getBatteryStatus(){
    QString msg = "B\n";
    serial->write(msg.toLocal8Bit());
    qDebug()<<msg<<"sent";
    cerStatusBox->setStyleSheet("color:#ed0b0b");                            
}

void MainWindow::checkForBase(){
    newCerebro_btn->setEnabled(false);
    baseConnected_lbl->setText("Base Station Connection \u2718");
    baseConnected_lbl->setStyleSheet("color:red");
    cerebroConnected_lbl->setText("Cerebro Wireless Connection \u2718");
    cerebroConnected_lbl->setStyleSheet("color:red");
    implantSettingsMatch_lbl->setText("Implant Settings Match \u2718");
    implantSettingsMatch_lbl->setStyleSheet("color:red");

    if (sessionHasBegun){
        QString msg = "N\n";
        serial->write(msg.toLocal8Bit());
        qDebug()<<msg<<"sent";
    }
    else{
        QString msg = "K,"+ QString::number(cerebroNum_spin->value());
        serial->write(msg.toLocal8Bit());
        qDebug()<<"message sent"<<msg;
    }

}

void MainWindow::getInfo(){
    QString msg = "I\n";
    serial->write(msg.toLocal8Bit());
    qDebug()<<msg<<"sent";
}

void MainWindow::gotoDir(){
    //get the default save directory
    QSettings settings("Bobcat Engineering","Gambit");
    settings.beginGroup("Saving");
    QString saveDirectoryPath = settings.value("DefaultDir").toString();
    settings.endGroup();
    if(saveDirectoryPath.isEmpty()){
        saveDirectoryPath = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(saveDirectoryPath));
}

void MainWindow::gotoAppLocation(){
    QString saveDirectoryPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
    QDesktopServices::openUrl(QUrl::fromLocalFile(saveDirectoryPath));
}



void MainWindow::showDebug(){
    debugCheckbox->setChecked(debugOn);
    if (debugOn){
        rigSelect->setEnabled(false);
        ratSelect->setEnabled(false);
        rigSelect->clearSelection();
        ratSelect->clearSelection();
        connect_btn->setText("Connect to Base Station (Debug Mode)");
        toggleDebug->setText("Exit Debug Mode");
        bugBox->setVisible(true);
    }
    else{
        rigSelect->setEnabled(true);
        ratSelect->setEnabled(true);
        connect_btn->setText("Connect to Base Station");
        toggleDebug->setText("Enter Debug Mode");
        bugBox->setVisible(false);
    }
}


void MainWindow::startDelayChecked(){
    startDelay_spn->setEnabled(startDelay_checkbox->isChecked());
    if (startDelay_checkbox->isChecked()){
        startDelay_spn->setStyleSheet("");
    }
    else{
        startDelay_spn->setStyleSheet("text-decoration: line-through;");
    }

}

void MainWindow::openDocs(){
//    opens online documentation
    QUrl site = QUrl::fromEncoded( "https://karpova-lab.github.io/cerebro/Software/Xavier.html");
    QDesktopServices::openUrl(site);
}

void MainWindow::getGraphs()
{
    //get the default save directory
    QSettings settings("Bobcat Engineering","Gambit");
    settings.beginGroup("Saving");
    QString saveDirectoryPath = settings.value("DefaultDir").toString();
    settings.endGroup();
    QString baseData = QFileDialog::getOpenFileName(this,tr("Select Base Station Log File"),saveDirectoryPath,tr("(*station.csv)"));
    if (baseData!=""){
        //the cerebro data should be in the same folder, so we have the getOpenFileName dialog start in the the same folder
        QString betterPath = baseData.mid(saveDirectoryPath.length(),baseData.indexOf("/",saveDirectoryPath.length()+1)-saveDirectoryPath.length())+"/";
        QString cerData = QFileDialog::getOpenFileName(this,tr("Select Base Station Log File"),betterPath,tr("(*cerebroLog.csv)"));
        if (cerData!=""){
            //Run python script for creating graph
            QProcess *process = new QProcess(this);
            QString plotArg;
            if (debugOn){
                plotArg = "alignment and histogram and outcsv";
                debugOn = false;
                showDebug();
            }
            else{
                plotArg = "alignment and histogram";
            }
            QStringList pythonArgs;
            pythonArgs<<qApp->applicationDirPath()+"/python scripts/parseLogs.py"<<baseData<<cerData<<plotArg; //pass the two log file locations into the python script
            process->start("python",pythonArgs);
            process->waitForFinished(-1);
            QString errorString = process->readAllStandardError();
            QString printedString = process->readAll();
            //Display summary in a popup message
            QMessageBox alert;
            if (!errorString.isEmpty()){
                alert.setWindowTitle("Python Error");
                alert.setText(errorString);
                alert.exec();
            }
            else{
                alert.setWindowTitle("Sesssion Summary");
                alert.setText(printedString);
                alert.exec();
            }
        }
    }
}

void MainWindow::setDebug()
{
    if (!baseConnected){
        debugOn = !debugOn;
        showDebug();
    }
}

void MainWindow::matchPowers(){
    sessionStartMonitor->textCursor().insertText("\nDiode parameter mismatch. Sending new diode values to Cerebro\n");
    QString msg = "D";
    QString leftPowerValue = QString::number(titleLeftPower);
    QString rightPowerValue = QString::number(titleRightPower);
    msg = msg + "," + leftPowerValue + "," + rightPowerValue + "\n";
    serial->write(msg.toLocal8Bit());
    qDebug()<<msg<<" Sent";
}


void MainWindow::startSession(){
    sessionStartDialog->accept();
    baseConnected_lbl->setText("Base Station Connection \u2718");
    baseConnected_lbl->setStyleSheet("color:red");
    cerebroConnected_lbl->setText("Cerebro Wireless Connection \u2718");
    cerebroConnected_lbl->setStyleSheet("color:red");
    implantSettingsMatch_lbl->setText("Implant Settings Match \u2718");
    implantSettingsMatch_lbl->setStyleSheet("color:red");
    sessionHasBegun=true;
    clearMonitor();
}

void MainWindow::updateChannel(){
    QString msg = "K,";
    QString newChannel = QString::number(channelSpinBox->value());
    msg = msg + newChannel + "\n";
    serial2->write(msg.toLocal8Bit());
    qDebug()<<msg<<" Sent";
}

void MainWindow::getCerebroInfoOverSerial(){
    QString msg = "?\n";
    serial2->write(msg.toLocal8Bit());
    qDebug()<<msg<<" Sent";
}

void MainWindow::setupCerebro(){
    QString msg = "K,0";
    serial->write(msg.toLocal8Bit());
    qDebug()<<"message sent"<<msg;
    setSerial_btn->setVisible(true);
    newCerebro_lbl->setVisible(true);
    newCerebro_spin->setVisible(true);
    baseConnected_lbl->setText("Base Station Connection \u2718");
    baseConnected_lbl->setStyleSheet("color:red");
    cerebroConnected_lbl->setText("Cerebro Wireless Connection \u2718");
    cerebroConnected_lbl->setStyleSheet("color:red");
    implantSettingsMatch_lbl->setText("Implant Settings Match \u2718");
    implantSettingsMatch_lbl->setStyleSheet("color:red");
}

void MainWindow::updateSerial(){
    cerebroNum_spin->setValue(newCerebro_spin->value());
    QString msg = "S,";
    QString newSerial = QString::number(newCerebro_spin->value());
    msg = msg + newSerial + "\n";
    serial->write(msg.toLocal8Bit());
    qDebug()<<msg<<" Sent";
    setSerial_btn->setVisible(false);
    newCerebro_lbl->setVisible(false);
    newCerebro_spin->setVisible(false);
    baseConnected_lbl->setText("Base Station Connection \u2718");
    baseConnected_lbl->setStyleSheet("color:red");
    cerebroConnected_lbl->setText("Cerebro Wireless Connection \u2718");
    cerebroConnected_lbl->setStyleSheet("color:red");
    implantSettingsMatch_lbl->setText("Implant Settings Match \u2718");
    implantSettingsMatch_lbl->setStyleSheet("color:red");
}


void MainWindow::toggleStim(){
    QString msg = "+\n";
    serial->write(msg.toLocal8Bit());
    qDebug()<<msg<<" Sent";
}


void MainWindow::stimOff(){
    QString msg = "0\n";
    serial->write(msg.toLocal8Bit());
    qDebug()<<msg<<" Sent";
}
