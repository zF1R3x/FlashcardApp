#ifndef STUDYWINDOW_H
#define STUDYWINDOW_H

#include <QWidget>
#include "deck.h"

namespace Ui {
class studywindow;
}

class studywindow : public QWidget
{
    Q_OBJECT

public:
    explicit studywindow(deck *deck, QWidget *parent = nullptr);
    ~studywindow();

private slots:
    void onCheckAnswerClicked();
    void onNextCardClicked();
    void updateCardDisplay();
    void onReturnClicked();

private:
    Ui::studywindow *ui;
    deck *currentdeck;
    int currentIndex;
};

#endif // STUDYWINDOW_H
