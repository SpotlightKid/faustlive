//
//  FLToolBar.cpp
//  
//
//  Created by Sarah Denoux on 13/05/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "FLToolBar.h"
#include "FLSettings.h"

#include "utilities.h"

//--------------------------FLToolBar

FLToolBar::FLToolBar(QSettings* settings, QWidget* parent) : QToolBar(parent){

    fSettings = settings;
    
    fButtonState = fFold;
    
    setAutoFillBackground(true);
    
    fWindowOptions = new QPushButton(">   Parameters");
    fWindowOptions->setFlat(true);
    fWindowOptions->setDefault(false);
    addWidget(fWindowOptions);
    
    connect(fWindowOptions, SIGNAL(clicked()), this, SLOT(buttonStateChanged()));
    

    fWindowOptions->setStyleSheet("*{"
                              "background-color: transparent;"
                              "color: black;"
                              "border-color: transparent;"
                              "margin: 8px;"
                                  "}");
    
    fSaveButton = new QPushButton("Apply Changes");
    fSaveButton->setFlat(true);
    fSaveButton->setEnabled(false);
    connect(fSaveButton, SIGNAL(clicked()), this, SLOT(modifiedOptions()));
    
    fSaveButton->setStyleSheet("*:flat{"
                               "background-color: transparent;"
                               "color: black;"
                               "border: 2px solid black;"
                               "border-radius: 6px;"
                               "}"
                               "*:flat:hover:enabled{"
                               "border: 2px solid #FFC0CB;" 
                               "}" 
                               "*:flat:enabled:pressed{"
                               "background-color: #FFC0CB;"
                               "}"
                               "*:disabled{"
                               "background-color: #606060;"
                               "}"
                               );
    
    init();
}

void FLToolBar::init(){
    
    fContainer = new QToolBox;
    fContainer->setStyleSheet("*{background-color:transparent;}");
    
//------- Compilation Options
    QWidget* compilationOptions = new QWidget();
    QVBoxLayout* compilationLayout = new QVBoxLayout;
    
    //--Faust
    fOptionLine = new QLineEdit(tr(""), compilationOptions);
    fOptionLine->setStyleSheet("*{background-color:white;}");
    compilationLayout->addWidget(new QLabel(tr("FAUST Options")));
    compilationLayout->addWidget(fOptionLine);
    
    connect(fOptionLine, SIGNAL(textEdited(const QString&)), this, SLOT(enableButton(const QString&)));
    //--LLVM
    fOptValLine = new QLineEdit(tr(""), compilationOptions);
    fOptValLine->setStyleSheet("*{background-color:white;}");
    
    fOptValLine->setMaxLength(3);
    fOptValLine->adjustSize();
                              
    compilationLayout->addWidget(new QLabel(tr("LLVM Optimization")));  
    compilationLayout->addWidget(fOptValLine);
    
    connect(fOptValLine, SIGNAL(textEdited(const QString&)), this, SLOT(enableButton(const QString&)));
                              
    compilationOptions->setLayout(compilationLayout);
    fContainer->addItem(compilationOptions, tr("Compilation"));

//------- OSC Control
#ifdef HTTPCTRL
    
    QWidget* oscBox = new QWidget();
    
    fOSCCheckBox = new QCheckBox;
    
    connect(fOSCCheckBox, SIGNAL(stateChanged(int)), this, SLOT(enableButton(int)));
    
    QFormLayout* oscLayout = new QFormLayout;
    
    fPortInOscLine = new QLineEdit(tr(""), oscBox);
    fPortInOscLine->setStyleSheet("*{background-color:white;}");
    fPortInOscLine->setMaxLength(4);
    fPortInOscLine->setMaximumWidth(50);
    connect(fPortInOscLine, SIGNAL(textEdited(const QString&)), this, SLOT(enableButton(const QString&)));
    
    fPortOutOscLine = new QLineEdit(tr(""), oscBox);
    fPortOutOscLine->setStyleSheet("*{background-color:white;}");
    fPortOutOscLine->setMaxLength(4);
    fPortOutOscLine->setMaximumWidth(50);
    connect(fPortOutOscLine, SIGNAL(textEdited(const QString&)), this, SLOT(enableButton(const QString&)));
    
    fDestHostLine = new QLineEdit(tr(""), oscBox);
//    fDestHostLine->setInputMask("000.000.000.000");
    fDestHostLine->setStyleSheet("*{background-color:white;}");
    connect(fDestHostLine, SIGNAL(textEdited(const QString&)), this, SLOT(enableButton(const QString&)));
    
    fPortErrOscLine = new QLineEdit(tr(""), oscBox);
    fPortErrOscLine->setStyleSheet("*{background-color:white;}");
    fPortErrOscLine->setMaxLength(4);
    fPortErrOscLine->setMaximumWidth(50);
    connect(fPortErrOscLine, SIGNAL(textEdited(const QString&)), this, SLOT(enableButton(const QString&)));
    
    oscLayout->addRow("Enable Control", fOSCCheckBox);    
    oscLayout->addRow(new QLabel(tr("In Port")), fPortInOscLine);
    oscLayout->addRow(new QLabel(tr("Out Port")), fPortOutOscLine);
    oscLayout->addRow(new QLabel(tr("Destination Host")), fDestHostLine);
    oscLayout->addRow(new QLabel(tr("Error Port")), fPortErrOscLine);
    
    oscBox->setLayout(oscLayout);
    
    fContainer->addItem(oscBox, "OSC Control");
    
//------- HTTP Control
    
    QWidget* httpBox = new QWidget;

    fHttpCheckBox = new QCheckBox();
    
    connect(fHttpCheckBox, SIGNAL(stateChanged(int)), this, SLOT(enableButton(int)));
    
    QFormLayout* httpLayout = new QFormLayout;
    
    fHttpPort = new QLabel(tr(""), httpBox);
    
    httpLayout->addRow(new QLabel(tr("Enable Control")), fHttpCheckBox);
    httpLayout->addRow(new QLabel(tr("Port")), fHttpPort);
    
    httpBox->setLayout(httpLayout);
    fContainer->addItem(httpBox, "Http Control");
    
#endif
 
#ifdef REMOTE
//-------- Remote Processing
    
    QWidget* remoteBox = new QWidget;
    QFormLayout* remoteLayout = new QFormLayout;
    
    fCVLine = new QLineEdit;
    fCVLine->setMaxLength(4);
    fCVLine->setMaximumWidth(50);
    fCVLine->setStyleSheet("*{background-color:white;}");
    connect(fCVLine, SIGNAL(textEdited(const QString&)), this, SLOT(enableButton(const QString&)));
    
    fMTULine = new QLineEdit;
    fMTULine->setMaxLength(6);
    fMTULine->setMaximumWidth(50);
    fMTULine->setStyleSheet("*{background-color:white;}");
    connect(fMTULine, SIGNAL(textEdited(const QString&)), this, SLOT(enableButton(const QString&)));
    
    fLatLine = new QLineEdit;
    fLatLine->setMaxLength(4);
    fLatLine->setMaximumWidth(50);
    fLatLine->setStyleSheet("*{background-color:white;}");
    connect(fLatLine, SIGNAL(textEdited(const QString&)), this, SLOT(enableButton(const QString&)));
    
    remoteLayout->addRow(new QLabel(tr("Compression")), fCVLine);
    remoteLayout->addRow(new QLabel(tr("MTU")), fMTULine);
    remoteLayout->addRow(new QLabel(tr("Latency")), fLatLine);
    
    remoteBox->setLayout(remoteLayout);
    fContainer->addItem(remoteBox, "Remote Processing");
    
//------- Publish DSP
    
    QWidget* publishBox = new QWidget;
    QFormLayout* publishLayout = new QFormLayout;
    
    fPublishBox = new QCheckBox;
    connect(fPublishBox, SIGNAL(stateChanged(int)), this, SLOT(enableButton(int)));
    
    publishLayout->addRow(new QLabel(tr("Release DSP")), fPublishBox);
    
    publishBox->setLayout(publishLayout);
    fContainer->addItem(publishBox, "Release");
#endif
    syncVisualParams();
}

///*item*/ is useless but QT signal forces the slot parameters
void FLToolBar::buttonStateChanged(){
    
    if(fButtonState == fFold){
        fButtonState = fUnFold;
        expansionAction();
        fWindowOptions->setText("V   Parameters");
    } 
    else{
        fButtonState = fFold;
        collapseAction();
        fWindowOptions->setText(">   Parameters");
    }
}

//TRICK to be able to add/remove objects from the toolbar 
void FLToolBar::expansionAction(){
    
    fSaveButton->show();
    fContainer->show();
    
    fAction2 = addWidget(fSaveButton);
    fAction1 = addWidget(fContainer);
    
    setOrientation(Qt::Vertical);
    
    adjustSize();
    emit sizeGrowth();
}

void FLToolBar::collapseAction(){
    
    removeAction(fAction1);
    removeAction(fAction2);
    
    adjustSize();
    emit sizeReduction();
    
    syncVisualParams();
    
    setOrientation(Qt::Horizontal);
}

FLToolBar::~FLToolBar(){}

//Changes in parameters = enable save changes button
void FLToolBar::enableButton(const QString& /*newText*/){
    fSaveButton->setEnabled(hasStateChanged());
}

void FLToolBar::enableButton(int /*state*/){
    fSaveButton->setEnabled(hasStateChanged());
}

bool FLToolBar::hasStateChanged(){
    return 
        hasCompilationOptionsChanged()||
        wasOscSwitched() ||
        hasOscOptionsChanged() ||
        wasHttpSwitched() ||
        hasRemoteOptionsChanged() ||
        hasReleaseOptionsChanged();
    
}

bool FLToolBar::hasCompilationOptionsChanged(){
    
    FLSettings* generalSettings = FLSettings::getInstance();
    
    QString val = fOptValLine->text();
    
    bool ok;
    int value = val.toInt(&ok);
	if(!ok)
        value = 3;
    
    if(fOptionLine->text() != (fSettings->value("Compilation/FaustOptions", generalSettings->value("Compilation/FaustOptions", "").toString()).toString()) || 
       value != fSettings->value("Compilation/OptValue", generalSettings->value("Compilation/OptValue", 3).toInt()).toInt())
            return true;
    else
        return false;
}

bool FLToolBar::wasOscSwitched(){
#ifdef OSCVAR
    
    if(fSettings->value("Osc/Enabled", FLSettings::getInstance()->value("Network/OscDefaultChecked", false)) != fOSCCheckBox->isChecked())
        return true;
#endif
    return false;
}

bool FLToolBar::hasOscOptionsChanged(){
    
#ifdef OSCVAR

    if(fOSCCheckBox->isChecked()){
        if(fPortInOscLine->text() != fSettings->value("Osc/InPort", "5510").toString())
            return true;
        
        if(fPortOutOscLine->text() != fSettings->value("Osc/OutPort", "5511").toString())
            return true;
        
        if(fDestHostLine->text() != fSettings->value("Osc/DestHost", "localhost").toString())
            return true;
        
        if(fPortErrOscLine->text() != fSettings->value("Osc/ErrPort", "5512").toString())
            return true;
    }
#endif 
    return false;
}

bool FLToolBar::wasHttpSwitched(){

#ifdef HTTPCTRL   
    if(fSettings->value("Http/Enabled", FLSettings::getInstance()->value("Network/HttpDefaultChecked", false)) != fHttpCheckBox->isChecked())
        return true;
 #endif    
    return false;
}

bool FLToolBar::hasRemoteOptionsChanged(){
#ifdef REMOTE
    if(fCVLine->text() != fSettings->value("RemoteProcessing/CV", "64").toString())
       return true;

    if(fMTULine->text() != fSettings->value("RemoteProcessing/MTU", "1500").toString())
       return true;
       
    if(fLatLine->text() != fSettings->value("RemoteProcessing/Latency", "10").toString())
       return true;
#endif
    return false;
}

bool FLToolBar::hasReleaseOptionsChanged(){
    
#ifdef REMOTE   
    if(fSettings->value("Release/Enabled", false) != fPublishBox->isChecked())
        return true;
#endif    
    return false;
}

//Reaction to enter one of the QLineEdit
void FLToolBar::modifiedOptions(){
    
#ifdef OSCVAR   
    if(hasCompilationOptionsChanged()){
        
        fSettings->setValue("Compilation/OptValue", fOptValLine->text()); 
        fSettings->setValue("Compilation/FaustOptions", fOptionLine->text()); 

        emit compilationOptionsChanged();
    }

    if(hasOscOptionsChanged()){
        
        fSettings->setValue("Osc/InPort", fPortInOscLine->text());
        fSettings->setValue("Osc/OutPort", fPortOutOscLine->text());
        fSettings->setValue("Osc/DestHost", fDestHostLine->text());
        fSettings->setValue("Osc/ErrPort", fPortErrOscLine->text());
        
        if(wasOscSwitched()){
            
            fSettings->setValue("Osc/Enabled", fOSCCheckBox->isChecked());
            emit switch_osc(fOSCCheckBox->isChecked());
        
        }
//-- Port changes are declared only if osc isn't switched & if osc is on
        else
            emit oscPortChanged();
    }
    else if(wasOscSwitched()){
        fSettings->setValue("Osc/Enabled", fOSCCheckBox->isChecked());
        emit switch_osc(fOSCCheckBox->isChecked());
    }

#endif
    if(wasHttpSwitched()){
        fSettings->setValue("Http/Enabled", fHttpCheckBox->isChecked());
        emit switch_http(fHttpCheckBox->isChecked());
    }
#ifdef REMOTE
    if(hasRemoteOptionsChanged()){
        
        fSettings->setValue("RemoteProcessing/CV", fCVLine->text());  
        fSettings->setValue("RemoteProcessing/MTU", fMTULine->text());   
        fSettings->setValue("RemoteProcessing/Latency", fLatLine->text());  
        
        emit compilationOptionsChanged();
    }
    
    if(hasReleaseOptionsChanged()){
        fSettings->setValue("Release/Enabled", fPublishBox->isChecked()); 
        emit switch_release(fPublishBox->isChecked());
    }
#endif
    fSaveButton->setEnabled(false);
}
                             
void FLToolBar::switchHttp(bool on){
//#ifndef _WIN32 || HTTPDVAR
#ifdef HTTPCTRL
        fHttpCheckBox->setChecked(on);
        modifiedOptions();
#endif
}

void FLToolBar::switchOsc(bool on){
//#ifndef _WIN32 || OSCVAR
#ifdef OSCVAR
        fOSCCheckBox->setChecked(on);
        modifiedOptions();
#endif
}

void FLToolBar::syncVisualParams(){
    
    FLSettings* generalSettings= FLSettings::getInstance();
    
//---- Compilation
    fOptionLine->setText(fSettings->value("Compilation/FaustOptions", generalSettings->value("Compilation/FaustOptions", "").toString()).toString());
    fOptValLine->setText(QString::number(fSettings->value("Compilation/OptValue", generalSettings->value("Compilation/OptValue", 3).toInt()).toInt()));
    
//------ OSC
    bool checked = fSettings->value("Osc/Enabled", generalSettings->value("Network/OscDefaultChecked", false)).toBool();
    
    fOSCCheckBox->setChecked(checked);
    
    fPortInOscLine->setText(fSettings->value("Osc/InPort", "5510").toString());
    fPortOutOscLine->setText(fSettings->value("Osc/OutPort", "5511").toString());
    fDestHostLine->setText(fSettings->value("Osc/DestHost", "localhost").toString());
    fPortErrOscLine->setText(fSettings->value("Osc/ErrPort", "5512").toString());
    
//------ Http    
    checked = fSettings->value("Http/Enabled", generalSettings->value("Network/HttpDefaultChecked", false)).toBool();
    
    fHttpCheckBox->setChecked(checked);
    
    fHttpPort->setText(fSettings->value("Http/Port", "5510").toString());

#ifdef REMOTE
//------ RemoteProcessing
    fCVLine->setText(fSettings->value("RemoteProcessing/CV", "64").toString());  
    fMTULine->setText(fSettings->value("RemoteProcessing/MTU", "1500").toString());   
    fLatLine->setText(fSettings->value("RemoteProcessing/Latency", "10").toString());  
    
//----- Release
    checked = fSettings->value("Release/Enabled", false).toBool();
    
    fPublishBox->setChecked(checked);
#endif
    fSaveButton->setEnabled(false);
}





