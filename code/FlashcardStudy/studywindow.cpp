#include "studywindow.h"
#include "ui_studywindow.h"
#include "statstracker.h"
#include <QMessagebox>

studywindow::studywindow(deck *deck, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::studywindow)
    , currentdeck(deck)
    , currentIndex(0)
{
    ui->setupUi(this);

    if(currentdeck)
        setWindowTitle("Quiz: " + currentdeck->getName());
    else
        setWindowTitle("Quiz Mode");

    connect(ui->checkAnswerButton, &QPushButton::clicked, this, &studywindow::onCheckAnswerClicked);
    connect(ui->nextButton, &QPushButton::clicked, this, &studywindow::onNextCardClicked);
    connect(ui->returnButton, &QPushButton::clicked, this, &studywindow::onReturnClicked);

    updateCardDisplay();
}

void studywindow::updateCardDisplay() {
    if (!currentdeck || currentdeck->getSize() == 0) {
        ui->questionLabel->setText("No cards in this deck.");
        ui->answerInput->setEnabled(false);
        ui->checkAnswerButton->setEnabled(false);
        ui->nextButton->setEnabled(false);
        ui->feedbackLabel->clear();
        return;
    }

    flashcard card = currentdeck->getCard(currentIndex);
    ui->questionLabel->setText(card.getQuestion());
    ui->answerInput->clear();
    ui->feedbackLabel->clear();
    ui->answerInput->setEnabled(true);
    ui->checkAnswerButton->setEnabled(true);
}

void studywindow::onCheckAnswerClicked() {
    if (!currentdeck) return;

    flashcard card = currentdeck->getCard(currentIndex);
    QString userAnswer = ui->answerInput->text().trimmed();

    if (userAnswer.compare(card.getAnswer().trimmed(), Qt::CaseInsensitive) == 0) {
        ui->feedbackLabel->setText("✅ Correct!");
        StatsTracker::instance().trackCorrectAnswer();
    } else {
        ui->feedbackLabel->setText("❌ Incorrect. The answer is: " + card.getAnswer());
        StatsTracker::instance().trackIncorrectAnswer();
    }
    StatsTracker::instance().trackReview();
    ui->checkAnswerButton->setEnabled(false);
}

void studywindow::onNextCardClicked() {
    if (!currentdeck) return;

    currentIndex++;
    if (currentIndex >= currentdeck->getSize()) {
        QMessageBox::information(this, "End of Deck", "You’ve finished all questions!");
        currentIndex = 0;
    }
    updateCardDisplay();
}

void studywindow::onReturnClicked(){
    if (auto* main = parentWidget()) {
        main->show();
        main->raise();
        main->activateWindow();
    }

    close();
}

studywindow::~studywindow()
{
    delete ui;
}
