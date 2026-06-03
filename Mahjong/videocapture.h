#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QPixmap>

class VideoCaptureWidget : public QWidget {
    Q_OBJECT
public:
    explicit VideoCaptureWidget(QWidget *parent = nullptr);

    void setRegion(int x, int y, int w, int h);
    void startCapture();
    void stopCapture();
    bool isCapturing() const;
    QPixmap currentPixmap() const;

    int regionX() const { return m_x; }
    int regionY() const { return m_y; }
    int regionW() const { return m_w; }
    int regionH() const { return m_h; }

    static QPixmap captureScreen(int x, int y, int w, int h);

signals:
    void captured(const QPixmap &pixmap);

private slots:
    void onCaptureTimer();
    void onTestCapture();

private:
    void setupUI();
    void doCapture();

    QTimer *m_timer;
    QLabel *m_previewLabel;
    QPushButton *m_startBtn;
    QPushButton *m_stopBtn;
    QPushButton *m_testBtn;
    QLabel *m_regionLabel;
    QSpinBox *m_xSpin;
    QSpinBox *m_ySpin;
    QSpinBox *m_wSpin;
    QSpinBox *m_hSpin;

    int m_x = 100;
    int m_y = 100;
    int m_w = 400;
    int m_h = 300;
};

#endif // VIDEOCAPTURE_H
