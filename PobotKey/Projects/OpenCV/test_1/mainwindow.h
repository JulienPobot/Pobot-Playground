#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <QtGui>
#include "Qxt/QxtGui/QxtSpanSlider"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "QOpenCVWidget.h"
#include "paramProcess.h"

class MainWindow : public QWidget {
    Q_OBJECT

    private:
        QGridLayout * layout;
        QMenuBar * menu;
        QMenu * fileMenu;
        QMenu * configurationMenu;
        QAction * exitAction;
        QAction ** deviceAction;
        QAction ** timingAction;
        QStatusBar * statusBar;
        QLabel * hue_label;
        QLabel * saturation_label;
        QLabel * brightness_label;
        QxtSpanSlider * hue_slider;
        QxtSpanSlider * saturation_slider;
        QxtSpanSlider * brightness_slider;
        QSpinBox * hue_min_spin;
        QSpinBox * hue_max_spin;
        QSpinBox * saturation_min_spin;
        QSpinBox * saturation_max_spin;
        QSpinBox * brightness_min_spin;
        QSpinBox * brightness_max_spin;
        QOpenCVWidget * videoOrigin;
        QOpenCVWidget * videoProcess;
        CvCapture * camera;
        ParamProcess * hsbParam;
        IplImage * image;
        IplImage * masque;
        IplImage * image_zoom;
        IplImage * masque_zoom;
        IplImage * hsv;
        bool cameraChoosen;
        bool timerChoosen;
        int timerId;
        int indexCamera;
        int indexTimer;

    public slots:
        void chooseIndexCamera();
        void chooseTimerValue();

    private:
        void buildVideo();
        void buildMenu();
        void buildToolBar();
        void buildStatusBar();
        void buildLabel();
        void buildSlider();
        void buildInputText();
        void buildLayout();
        void timerEvent(QTimerEvent*);
        void manageStatusBar();

    public:
        MainWindow();
        ~MainWindow();
};

#endif // MAINWINDOW_H
