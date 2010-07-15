#include "mainwindow.h"



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow() : QWidget() {
    cameraChoosen = false;
    timerChoosen = false;

    hsbParam = new ParamProcess();
    hsbParam->initValues();

    buildLayout();

    // connect slide to spin
    QObject::connect(hue_slider, SIGNAL(lowerValueChanged(int)), hue_min_spin, SLOT(setValue(int)));
    QObject::connect(hue_slider, SIGNAL(upperValueChanged(int)), hue_max_spin, SLOT(setValue(int)));
    QObject::connect(saturation_slider, SIGNAL(lowerValueChanged(int)), saturation_min_spin, SLOT(setValue(int)));
    QObject::connect(saturation_slider, SIGNAL(upperValueChanged(int)), saturation_max_spin, SLOT(setValue(int)));
    QObject::connect(brightness_slider, SIGNAL(lowerValueChanged(int)), brightness_min_spin, SLOT(setValue(int)));
    QObject::connect(brightness_slider, SIGNAL(upperValueChanged(int)), brightness_max_spin, SLOT(setValue(int)));

    // connect spin to slide
    QObject::connect(hue_min_spin, SIGNAL(valueChanged(int)), hue_slider, SLOT(setLowerValue(int)));
    QObject::connect(hue_max_spin, SIGNAL(valueChanged(int)), hue_slider, SLOT(setUpperValue(int)));
    QObject::connect(saturation_min_spin, SIGNAL(valueChanged(int)), saturation_slider, SLOT(setLowerValue(int)));
    QObject::connect(saturation_max_spin, SIGNAL(valueChanged(int)), saturation_slider, SLOT(setUpperValue(int)));
    QObject::connect(brightness_min_spin, SIGNAL(valueChanged(int)), brightness_slider, SLOT(setLowerValue(int)));
    QObject::connect(brightness_max_spin, SIGNAL(valueChanged(int)), brightness_slider, SLOT(setUpperValue(int)));

    // connect menu
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    for (int i=0 ; i<4 ; i++) {
        connect(deviceAction[i], SIGNAL(toggled(bool)), this, SLOT(chooseIndexCamera()));
    }
    for (int i=0 ; i<4 ; i++) {
        connect(timingAction[i], SIGNAL(toggled(bool)), this, SLOT(chooseTimerValue()));
    }
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::chooseIndexCamera() {
    bool error = false;
    for (int i=0 ; i<4 ; i++) {
        if (deviceAction[i]->isChecked()) {
            indexCamera = i;
            try {
                camera = cvCreateCameraCapture(i);
            } catch (...) {
                cameraChoosen = false;
                error = true;
            }

            if (!cameraChoosen && !error) {
                image = cvQueryFrame(camera);
                masque = cvCreateImage(cvSize(image->width, image->height), image->depth, image->nChannels);
                image_zoom = cvCreateImage(cvSize(640, 480), image->depth, image->nChannels);
                masque_zoom = cvCreateImage(cvSize(640, 480), image->depth, image->nChannels);
                hsv = cvCloneImage(image);
                cameraChoosen = true;
            }
            manageStatusBar();
        }
    }
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::chooseTimerValue() {
    for (int i=0 ; i<4 ; i++) {
        if (timingAction[i]->isChecked()) {
            indexTimer = i;
            if (timerChoosen) {
                killTimer(timerId);
            }
            timerId = startTimer(i*50+50);
            timerChoosen = true;
            manageStatusBar();
        }
    }
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow() {
    cvReleaseCapture(&camera);
    cvReleaseImage(&image);
    cvReleaseImage(&masque);
    cvReleaseImage(&image_zoom);
    cvReleaseImage(&masque_zoom);
    cvReleaseImage(&hsv);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::timerEvent(QTimerEvent*) {
    if (cameraChoosen) {
        hsbParam->updateValues(hue_min_spin->value(), hue_max_spin->value(), saturation_min_spin->value(), saturation_max_spin->value(), brightness_min_spin->value(), brightness_max_spin->value());
        image = cvQueryFrame(camera);

        cvCvtColor(image, hsv, CV_BGR2HSV);
        CvScalar scal;
        for(int x=0; x < hsv->width; x++) {
            for(int y=0; y < hsv->height; y++) {
                scal = cvGet2D(hsv, y, x);

                if(scal.val[0] >= hsbParam->getLowerHueValue() && scal.val[0] <= hsbParam->getUpperHueValue() &&
                   scal.val[1] >= hsbParam->getLowerSaturationValue() && scal.val[1] <= hsbParam->getUpperSaturationValue() &&
                   scal.val[2] >= hsbParam->getLowerBrightnessValue() && scal.val[2] <= hsbParam->getUpperBrightnessValue()) {
                    scal.val[0] = 255;
                    scal.val[1] = 255;
                    scal.val[2] = 255;
                 } else {
                    scal.val[0] = 0;
                    scal.val[1] = 0;
                    scal.val[2] = 0;
                }
                cvSet2D(masque, y, x, scal);
            }
        }

        cvResize(image, image_zoom);
        cvResize(masque, masque_zoom);
        videoOrigin->putImage(image_zoom);
        videoProcess->putImage(masque_zoom);
    }
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::buildLayout() {
    buildMenu();
    buildStatusBar();
    buildVideo();
    buildLabel();
    buildSlider();
    buildInputText();

    layout = new QGridLayout();
    layout->addWidget(menu,               0,0,1,7);
    layout->addWidget(statusBar,          8,0,1,7);
    layout->addWidget(videoOrigin,        1,0,1,3);
    layout->addWidget(videoProcess,       1,4,1,3);
    layout->addWidget(hue_min_spin,       2,0,2,1);
    layout->addWidget(hue_label,          2,1,1,1);
    layout->addWidget(hue_slider,         3,1,1,1);
    layout->addWidget(hue_max_spin,       2,2,2,1);
    layout->addWidget(saturation_min_spin,4,0,2,1);
    layout->addWidget(saturation_label,   4,1,1,1);
    layout->addWidget(saturation_slider,  5,1,1,1);
    layout->addWidget(saturation_max_spin,4,2,2,1);
    layout->addWidget(brightness_min_spin,6,0,2,1);
    layout->addWidget(brightness_label,   6,1,1,1);
    layout->addWidget(brightness_slider,  7,1,1,1);
    layout->addWidget(brightness_max_spin,6,2,2,1);

    this->setLayout(layout);

    this->show();
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::buildVideo() {
    videoOrigin = new QOpenCVWidget();
    videoProcess = new QOpenCVWidget();
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::buildMenu() {
    menu = new QMenuBar(this);

    fileMenu = new QMenu("&Fichier", this);
    exitAction = new QAction("&Quitter", this);
    fileMenu->addAction(exitAction);

    configurationMenu = new QMenu("&Configuration", this);
    QActionGroup * deviceGroup = new QActionGroup(this);
    deviceAction = new QAction * [4];
    for (int i=0 ; i<4 ; i++) {
        char string[10];
        sprintf(string,"Device %d",i);
        deviceAction[i] = new QAction(string, this);
        deviceAction[i]->setCheckable(true);
        deviceAction[i]->setActionGroup(deviceGroup);
        configurationMenu->addAction(deviceAction[i]);
    }

    configurationMenu->addSeparator();

    QActionGroup * timingGroup = new QActionGroup(this);
    timingAction = new QAction * [4];
    for (int i=0 ; i<4 ; i++) {
        char string[10];
        sprintf(string,"Timer %d",i*50+50);
        timingAction[i] = new QAction(string, this);
        timingAction[i]->setCheckable(true);
        timingAction[i]->setActionGroup(timingGroup);
        configurationMenu->addAction(timingAction[i]);
    }

    menu->addMenu(fileMenu);
    menu->addMenu(configurationMenu);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::buildToolBar() {

}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::buildStatusBar() {
    statusBar = new QStatusBar();
    manageStatusBar();
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::manageStatusBar() {
    std::string statusBarMessage;
    char string[2];
    if (cameraChoosen) {
        statusBarMessage.append("Camera ");
        sprintf(string,"%d",indexCamera);
        statusBarMessage.append(string);
        statusBarMessage.append(" selected");
    } else {
        statusBarMessage.append("No camera selected");
    }
    statusBarMessage.append(" - ");
    if (timerChoosen) {
        statusBarMessage.append("Timer ");
        sprintf(string,"%d",indexTimer*50+50);
        statusBarMessage.append(string);
        statusBarMessage.append(" selected");
    } else {
        statusBarMessage.append("No timer selected");
    }
    statusBar->showMessage(statusBarMessage.c_str());
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::buildLabel() {
    hue_label = new QLabel("Hue :");
    saturation_label = new QLabel("Saturation :");
    brightness_label = new QLabel("Brightness :");
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::buildInputText() {
    hue_min_spin = new QSpinBox();
    hue_max_spin = new QSpinBox();
    saturation_min_spin = new QSpinBox();
    saturation_max_spin = new QSpinBox();
    brightness_min_spin = new QSpinBox();
    brightness_max_spin = new QSpinBox();

    hue_min_spin->setRange(hsbParam->getLowerHueValue(), hsbParam->getUpperHueValue());
    hue_max_spin->setRange(hsbParam->getLowerHueValue(), hsbParam->getUpperHueValue());
    saturation_min_spin->setRange(hsbParam->getLowerSaturationValue(), hsbParam->getUpperSaturationValue());
    saturation_max_spin->setRange(hsbParam->getLowerSaturationValue(), hsbParam->getUpperSaturationValue());
    brightness_min_spin->setRange(hsbParam->getLowerBrightnessValue(), hsbParam->getUpperBrightnessValue());
    brightness_max_spin->setRange(hsbParam->getLowerBrightnessValue(), hsbParam->getUpperBrightnessValue());

    hue_min_spin->setSingleStep(1);
    hue_max_spin->setSingleStep(1);
    saturation_min_spin->setSingleStep(1);
    saturation_max_spin->setSingleStep(1);
    brightness_min_spin->setSingleStep(1);
    brightness_max_spin->setSingleStep(1);

    hue_min_spin->setValue(hsbParam->getLowerHueValue());
    hue_max_spin->setValue(hsbParam->getUpperHueValue());
    saturation_min_spin->setValue(hsbParam->getLowerSaturationValue());
    saturation_max_spin->setValue(hsbParam->getUpperSaturationValue());
    brightness_min_spin->setValue(hsbParam->getLowerBrightnessValue());
    brightness_max_spin->setValue(hsbParam->getUpperBrightnessValue());
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::buildSlider() {
    hue_slider = new QxtSpanSlider(Qt::Horizontal);
    saturation_slider = new QxtSpanSlider(Qt::Horizontal);
    brightness_slider = new QxtSpanSlider(Qt::Horizontal);

    hue_slider->setRange(hsbParam->getLowerHueValue(), hsbParam->getUpperHueValue());
    saturation_slider->setRange(hsbParam->getLowerSaturationValue(), hsbParam->getUpperSaturationValue());
    brightness_slider->setRange(hsbParam->getLowerBrightnessValue(), hsbParam->getUpperBrightnessValue());

    hue_slider->setLowerValue(hsbParam->getLowerHueValue());
    hue_slider->setUpperValue(hsbParam->getUpperHueValue());
    saturation_slider->setLowerValue(hsbParam->getLowerSaturationValue());
    saturation_slider->setUpperValue(hsbParam->getUpperSaturationValue());
    brightness_slider->setLowerValue(hsbParam->getLowerBrightnessValue());
    brightness_slider->setUpperValue(hsbParam->getUpperBrightnessValue());
}
