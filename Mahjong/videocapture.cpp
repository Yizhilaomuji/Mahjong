#include "videocapture.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStandardPaths>
#include <QDateTime>

// macOS CoreGraphics for screen capture (triggers proper permission prompt)
#include <CoreGraphics/CoreGraphics.h>

VideoCaptureWidget::VideoCaptureWidget(QWidget *parent)
    : QWidget(parent)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(2000);
    connect(m_timer, &QTimer::timeout, this, &VideoCaptureWidget::onCaptureTimer);

    setupUI();
    setVisible(false);
}

void VideoCaptureWidget::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);

    // Region config row
    QHBoxLayout *regionRow = new QHBoxLayout();
    QLabel *regionTitle = new QLabel("截取区域:", this);
    regionTitle->setStyleSheet("font-weight: bold; color: #ccc;");
    regionRow->addWidget(regionTitle);

    m_xSpin = new QSpinBox(this);
    m_xSpin->setRange(0, 3000);
    m_xSpin->setValue(m_x);
    m_xSpin->setPrefix("X:");
    m_xSpin->setMaximumWidth(80);
    regionRow->addWidget(m_xSpin);

    m_ySpin = new QSpinBox(this);
    m_ySpin->setRange(0, 3000);
    m_ySpin->setValue(m_y);
    m_ySpin->setPrefix("Y:");
    m_ySpin->setMaximumWidth(80);
    regionRow->addWidget(m_ySpin);

    m_wSpin = new QSpinBox(this);
    m_wSpin->setRange(100, 3000);
    m_wSpin->setValue(m_w);
    m_wSpin->setPrefix("W:");
    m_wSpin->setMaximumWidth(80);
    regionRow->addWidget(m_wSpin);

    m_hSpin = new QSpinBox(this);
    m_hSpin->setRange(100, 3000);
    m_hSpin->setValue(m_h);
    m_hSpin->setPrefix("H:");
    m_hSpin->setMaximumWidth(80);
    regionRow->addWidget(m_hSpin);

    QPushButton *applyRegionBtn = new QPushButton("应用", this);
    applyRegionBtn->setStyleSheet(
        "QPushButton { background-color: #444; color: #ccc; padding: 2px 8px; "
        "border-radius: 3px; } QPushButton:hover { background-color: #555; }");
    connect(applyRegionBtn, &QPushButton::clicked, this, [this]() {
        setRegion(m_xSpin->value(), m_ySpin->value(),
                  m_wSpin->value(), m_hSpin->value());
    });
    regionRow->addWidget(applyRegionBtn);

    // Permission hint
    QLabel *permHint = new QLabel(
        "需要「屏幕录制」权限: 系统设置 → 隐私与安全性 → 屏幕录制", this);
    permHint->setStyleSheet("color: #888; font-size: 10px;");
    regionRow->addWidget(permHint);
    regionRow->addStretch();
    mainLayout->addLayout(regionRow);

    // Buttons row
    QHBoxLayout *btnRow = new QHBoxLayout();
    m_startBtn = new QPushButton("开始截屏", this);
    m_startBtn->setStyleSheet(
        "QPushButton { background-color: #5769F7; color: white; padding: 4px 12px; "
        "border-radius: 3px; } QPushButton:hover { background-color: #4a5ad4; }");
    connect(m_startBtn, &QPushButton::clicked, this, &VideoCaptureWidget::startCapture);
    btnRow->addWidget(m_startBtn);

    m_stopBtn = new QPushButton("停止截屏", this);
    m_stopBtn->setEnabled(false);
    m_stopBtn->setStyleSheet(
        "QPushButton { background-color: #888; color: white; padding: 4px 12px; "
        "border-radius: 3px; } QPushButton:hover { background-color: #999; }");
    connect(m_stopBtn, &QPushButton::clicked, this, &VideoCaptureWidget::stopCapture);
    btnRow->addWidget(m_stopBtn);

    m_testBtn = new QPushButton("测试截屏", this);
    m_testBtn->setStyleSheet(
        "QPushButton { background-color: #D77757; color: white; padding: 4px 12px; "
        "border-radius: 3px; } QPushButton:hover { background-color: #c06648; }");
    connect(m_testBtn, &QPushButton::clicked, this, &VideoCaptureWidget::onTestCapture);
    btnRow->addWidget(m_testBtn);

    m_regionLabel = new QLabel(this);
    m_regionLabel->setStyleSheet("color: #888; font-size: 11px;");
    btnRow->addWidget(m_regionLabel);
    btnRow->addStretch();
    mainLayout->addLayout(btnRow);

    // Preview
    m_previewLabel = new QLabel(this);
    m_previewLabel->setMinimumSize(200, 150);
    m_previewLabel->setMaximumHeight(250);
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setStyleSheet(
        "QLabel { background-color: #2a2a2a; border: 1px solid #444; "
        "border-radius: 4px; color: #666; font-size: 13px; }");
    m_previewLabel->setText("等待截屏...");
    m_previewLabel->setScaledContents(false);
    mainLayout->addWidget(m_previewLabel);

    update();
}

void VideoCaptureWidget::setRegion(int x, int y, int w, int h) {
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
    m_xSpin->setValue(x);
    m_ySpin->setValue(y);
    m_wSpin->setValue(w);
    m_hSpin->setValue(h);
    m_regionLabel->setText(
        QString("区域: (%1,%2) %3x%4").arg(x).arg(y).arg(w).arg(h));
}

void VideoCaptureWidget::startCapture() {
    setRegion(m_xSpin->value(), m_ySpin->value(),
              m_wSpin->value(), m_hSpin->value());
    m_timer->start();
    m_startBtn->setEnabled(false);
    m_stopBtn->setEnabled(true);
    doCapture();
}

void VideoCaptureWidget::stopCapture() {
    m_timer->stop();
    m_startBtn->setEnabled(true);
    m_stopBtn->setEnabled(false);
}

bool VideoCaptureWidget::isCapturing() const {
    return m_timer->isActive();
}

QPixmap VideoCaptureWidget::currentPixmap() const {
    return m_previewLabel->pixmap(Qt::ReturnByValue);
}

void VideoCaptureWidget::onCaptureTimer() {
    doCapture();
}

void VideoCaptureWidget::onTestCapture() {
    setRegion(m_xSpin->value(), m_ySpin->value(),
              m_wSpin->value(), m_hSpin->value());
    doCapture();
}

// Convert CGImageRef to QPixmap
static QPixmap cgImageToPixmap(CGImageRef cgImg) {
    if (!cgImg) return QPixmap();

    size_t w = CGImageGetWidth(cgImg);
    size_t h = CGImageGetHeight(cgImg);
    if (w == 0 || h == 0) return QPixmap();

    QImage qimg(static_cast<int>(w), static_cast<int>(h), QImage::Format_ARGB32);
    qimg.fill(Qt::transparent);

    CGColorSpaceRef cs = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
    if (!cs) return QPixmap();

    CGContextRef ctx = CGBitmapContextCreate(
        qimg.bits(), w, h, 8, qimg.bytesPerLine(),
        cs, kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Host);

    CGColorSpaceRelease(cs);

    if (!ctx) return QPixmap();

    CGContextDrawImage(ctx, CGRectMake(0, 0, w, h), cgImg);
    CGContextRelease(ctx);

    return QPixmap::fromImage(qimg);
}

QPixmap VideoCaptureWidget::captureScreen(int x, int y, int w, int h) {
    CGRect rect = CGRectMake(static_cast<CGFloat>(x),
                             static_cast<CGFloat>(y),
                             static_cast<CGFloat>(w),
                             static_cast<CGFloat>(h));

    CGImageRef screenShot = CGWindowListCreateImage(
        rect,
        kCGWindowListOptionOnScreenOnly,
        kCGNullWindowID,
        kCGWindowImageDefault);

    if (!screenShot) return QPixmap();

    QPixmap pix = cgImageToPixmap(screenShot);
    CGImageRelease(screenShot);
    return pix;
}

void VideoCaptureWidget::doCapture() {
    // Clear preview first so stale image doesn't remain on failure
    m_previewLabel->clear();
    m_previewLabel->setText("截屏中...");

    QPixmap pix = captureScreen(m_x, m_y, m_w, m_h);

    if (!pix.isNull()) {
        QPixmap scaled = pix.scaled(400, 250,
                                     Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation);
        m_previewLabel->setPixmap(scaled);
        emit captured(pix);
    } else {
        m_previewLabel->setText(
            "截屏失败\n请确认已授予「屏幕录制」权限:\n"
            "系统设置 → 隐私与安全性 → 屏幕录制\n"
            "找到 Mahjong 并开启开关");
    }
}
