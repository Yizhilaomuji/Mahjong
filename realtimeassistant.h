#ifndef REALTIMEASSISTANT_H
#define REALTIMEASSISTANT_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include "mahjonglogic.h"

class RealTimeAssistant : public QWidget
{
    Q_OBJECT
public:
    explicit RealTimeAssistant(QWidget *parent = nullptr);

private slots:
    void simulateDataCapture();

private:
    std::vector<Tile> currentHand;
    QLabel *handLabel;
    QTextEdit *suggestionBox;

    void updateSuggestions();
};

#endif // REALTIMEASSISTANT_H