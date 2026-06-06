#ifndef OPPONENTANALYSIS_H
#define OPPONENTANALYSIS_H

#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>

class OpponentAnalysis : public QWidget
{
    Q_OBJECT
public:
    explicit OpponentAnalysis(QWidget *parent = nullptr);

private:
    QLabel *dangerOutput;

private slots:
    void calculateDanger();
};

#endif // OPPONENTANALYSIS_H