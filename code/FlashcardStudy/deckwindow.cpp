#include "deckwindow.h"
#include "ui_deckwindow.h"
#include "deck.h"
#include "mainwindow.h"
#include "studywindow.h"
#include "statstracker.h"
#include "flashcardmanager.h"

#include <QMessageBox>

DeckWindow::DeckWindow(class deck* d, QWidget* parent)
    : QDialog(parent),
    ui(new Ui::DeckWindow),
    m_deck(d),
    m_model(new QStringListModel(this)),
    studywindow(nullptr)
{
    ui->setupUi(this);
    ui->labelTitle->setText(m_deck->getName());
    ui->listViewDeck->setModel(m_model);
    ui->listViewDeck->setSelectionMode(QAbstractItemView::SingleSelection);
    // connect signals / populate using m_deck here


    connect(ui->pushButtonNew, &QPushButton::clicked, this, &DeckWindow::onAddButtonClicked);
    connect(ui->pushButtonUpdate, &QPushButton::clicked, this, &DeckWindow::onUpdateButtonClicked);
    connect(ui->pushButtonDelete, &QPushButton::clicked, this, &DeckWindow::onDeleteButtonClicked);
    connect(ui->pushButtonMenu,  &QPushButton::clicked, this, &DeckWindow::close);
    connect(ui->pushButtonStudy,  &QPushButton::clicked, this, &DeckWindow::onStudyButtonClicked);


    connect(ui->pushButtonPrevious, &QPushButton::clicked, this, &DeckWindow::onPrevButtonClicked);
    connect(ui->pushButtonNext, &QPushButton::clicked, this, &DeckWindow::onNextButtonClicked);

    connect(ui->listViewDeck->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &DeckWindow::onRowChanged);

    rebuildList(0);
}

DeckWindow::~DeckWindow() {
    delete ui;
}

void DeckWindow::rebuildList(int keepIndex)
{
    if (!m_deck) return;

    QStringList items;
    const int n = m_deck->getSize();

    for (int i = 0; i < n; ++i) {
        flashcard fc = m_deck->getCard(i);
        QString display = QString("Q: %1\nA: %2")
                              .arg(fc.getQuestion())
                              .arg(fc.getAnswer());
        items << display;
    }

    m_model->setStringList(items);

    if (n == 0) {
        m_current = -1;
        ui->lineEditQuestion->clear();
        ui->textEditAnswer->clear();
        return;
    }

    int idx = keepIndex;
    if (idx < 0 || idx >= n) idx = 0;
    setCurrentIndex(idx);
}

void DeckWindow::setCurrentIndex(int i)
{
    const int n = m_deck ? m_deck->getSize() : 0;
    if (n == 0) { m_current = -1; return; }

    if (i < 0) i = 0;
    if (i >= n) i = n - 1;

    m_current = i;

    // select it in the view
    QModelIndex index = m_model->index(i);
    ui->listViewDeck->setCurrentIndex(index);
    loadFieldsFromCurrent();

    ui->pushButtonPrevious->setEnabled(m_current > 0);
    ui->pushButtonNext->setEnabled(m_current + 1 < n);
}

void DeckWindow::loadFieldsFromCurrent()
{
    if (!m_deck || m_current < 0 || m_current >= m_deck->getSize()) {
        ui->lineEditQuestion->clear();
        ui->textEditAnswer->clear();
        return;
    }

    flashcard fc = m_deck->getCard(m_current);
    ui->lineEditQuestion->setText(fc.getQuestion());
    ui->textEditAnswer->setPlainText(fc.getAnswer());
}

// ---------------------------------------------------------
// CRUD
// ---------------------------------------------------------

void DeckWindow::onAddButtonClicked()
{
    if (!m_deck) return;

    flashcard newCard;
    newCard.setQuestion(ui->lineEditQuestion->text().trimmed());
    newCard.setAnswer(ui->textEditAnswer->toPlainText().trimmed());

    if (newCard.getQuestion().isEmpty()) {
        QMessageBox::warning(this, "Missing Question", "Please enter a question.");
        return;
    }


    m_deck->addCard(newCard);
    flashcardManager::instance().saveToDisk(nullptr);
    StatsTracker::instance().trackCardCreated();
    rebuildList(m_deck->getSize() - 1);

    ui->lineEditQuestion->clear();
    ui->textEditAnswer->clear();
}

void DeckWindow::onUpdateButtonClicked()
{
    if (!m_deck || m_current < 0) return;

    flashcard fc;
    fc.setQuestion(ui->lineEditQuestion->text().trimmed());
    fc.setAnswer(ui->textEditAnswer->toPlainText().trimmed());

    if (fc.getQuestion().isEmpty()) {
        QMessageBox::warning(this, "Missing Question", "Please enter a question.");
        return;
    }

    m_deck->updateCard(m_current, fc);
    rebuildList(m_current);
}

void DeckWindow::onDeleteButtonClicked()
{
    if (!m_deck || m_current < 0) return;

    m_deck->removeCard(m_current);
    int nextIndex = m_current;
    if (nextIndex >= m_deck->getSize()) nextIndex = m_deck->getSize() - 1;
    rebuildList(nextIndex);
}

void DeckWindow::onStudyButtonClicked()
{

    if (!m_deck) return;

    // Create the study window if it doesn’t exist yet
    if (!studywindow) {
        studywindow = new class::studywindow(m_deck, nullptr);
    }
    //studywindow->setAttribute(Qt::WA_DeleteOnClose);
    studywindow->show();
    studywindow->raise();
    studywindow->activateWindow();
}

// ---------------------------------------------------------
// Navigation
// ---------------------------------------------------------

void DeckWindow::onPrevButtonClicked()
{
    if (m_current > 0)
        setCurrentIndex(m_current - 1);
}

void DeckWindow::onNextButtonClicked()
{
    if (m_deck && m_current + 1 < m_deck->getSize())
        setCurrentIndex(m_current + 1);
}

// ---------------------------------------------------------
// Selection handling
// ---------------------------------------------------------

void DeckWindow::onRowChanged(const QModelIndex& index)
{
    int row = index.row();
    if (row != m_current)
        setCurrentIndex(row);
}

// ---------------------------------------------------------
// Close
// ---------------------------------------------------------

void DeckWindow::onCloseButtonClicked()
{
    if (auto* main = parentWidget()) {
        main->show();
        main->raise();
        main->activateWindow();
    }

    close();
}
