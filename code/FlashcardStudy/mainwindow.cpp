#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "deck.h"
#include "deckwindow.h"
#include "flashcardmanager.h"
#include "statstracker.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenuBar>
#include <QAction>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Ensure manager is loaded from disk early
    (void)flashcardManager::instance();

    setupDeckButtons();

    // Create/delete buttons
    connect(ui->createDeckButton, &QPushButton::clicked, this, &MainWindow::onCreateDeckClicked);
    connect(ui->deleteDeckButton, &QPushButton::clicked, this, &MainWindow::onDeleteDeckClicked);

    // Rename button
    connect(ui->renameDeckButton, &QPushButton::clicked, this, &MainWindow::onRenameDeckClicked);

    // Filter button
    connect(ui->filterDeckButton, &QPushButton::clicked, this, &MainWindow::onFilterDeckClicked);

    // Navigation buttons
    connect(ui->nextPageButton, &QPushButton::clicked, this, &MainWindow::onNextPageClicked);
    connect(ui->prevPageButton, &QPushButton::clicked, this, &MainWindow::onPrevPageClicked);

    // Stats
    connect(ui->statsButton, &QPushButton::clicked, this, &MainWindow::onViewStatsClicked);

    for (auto *btn : deckButtons) {
        connect(btn, &QPushButton::clicked, this, &MainWindow::onDeckButtonClicked);
    }

    connect(ui->importDeckButton, &QPushButton::clicked, this, &MainWindow::onImportDeckClicked);
    connect(ui->exportDeckButton, &QPushButton::clicked, this, &MainWindow::onExportDeckClicked);
    connect(ui->exportAllButton, &QPushButton::clicked, this, &MainWindow::onExportAllDecksClicked);


    refreshDeckButtons();
}

MainWindow::~MainWindow()
{
    // Ensure last state is saved (in case any in-place edits happened without explicit save).
    flashcardManager::instance().saveToDisk(nullptr);

    delete ui;
}

void MainWindow::setupDeckButtons()
{
    deckButtons = {
        ui->Deck1, ui->Deck2, ui->Deck3,
        ui->Deck4, ui->Deck5, ui->Deck6,
        ui->Deck7, ui->Deck8, ui->Deck9
    };
}

QStringList MainWindow::getFilteredDeckNames() const
{
    QStringList names = flashcardManager::instance().getDeckNames();
    if (currentTagFilter.trimmed().isEmpty()) return names;

    QStringList filtered;
    for (const QString& name : names) {
        const deck* d = flashcardManager::instance().getDeck(name);
        if (d && d->getTag() == currentTagFilter) filtered.append(name);
    }
    return filtered;
}

int MainWindow::totalPages() const
{
    const int totalDecks = getFilteredDeckNames().size();
    return (totalDecks + decksPerPage - 1) / decksPerPage;
}

void MainWindow::refreshDeckButtons()
{
    QStringList names = getFilteredDeckNames();

    int total = totalPages();
    if (total == 0) total = 1;
    if (currentPage >= total) currentPage = total - 1;
    if (currentPage < 0) currentPage = 0;

    int startIndex = currentPage * decksPerPage;
    int i = 0;

    for (; i < deckButtons.size(); ++i) {
        int deckIndex = startIndex + i;
        if (deckIndex >= names.size()) break;
        deckButtons[i]->setText(names[deckIndex]);
        deckButtons[i]->setEnabled(true);
    }

    for (; i < deckButtons.size(); ++i) {
        deckButtons[i]->setText("Empty");
        deckButtons[i]->setEnabled(false);
    }

    ui->pageLabel->setText(QString("Page %1 of %2").arg(currentPage + 1).arg(total));
}

void MainWindow::onNextPageClicked()
{
    if (currentPage + 1 < totalPages()) {
        currentPage++;
        refreshDeckButtons();
    }
}

void MainWindow::onPrevPageClicked()
{
    if (currentPage > 0) {
        currentPage--;
        refreshDeckButtons();
    }
}

void MainWindow::onDeckButtonClicked()
{
    auto *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    const QString deckName = btn->text();
    deck *d = flashcardManager::instance().getDeck(deckName);
    if (!d) return;

    DeckWindow *w = new DeckWindow(d, this);
    w->show();
}

void MainWindow::onCreateDeckClicked()
{
    bool ok = false;
    QString name = QInputDialog::getText(this, "Create Deck", "Deck name:", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    name = name.trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Invalid Name", "Deck name cannot be empty.");
        return;
    }

    QString tag = QInputDialog::getText(this, "Create Deck", "Tag (optional):", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    tag = tag.trimmed();

    deck newDeck(name, tag);
    flashcardManager::instance().addDeck(newDeck);
    StatsTracker::instance().trackDeckCreated();
    refreshDeckButtons();
}

void MainWindow::onDeleteDeckClicked()
{
    QStringList names = flashcardManager::instance().getDeckNames();
    if (names.isEmpty()) {
        QMessageBox::information(this, "No Decks", "There are no decks to delete.");
        return;
    }

    bool ok = false;
    QString deckToDelete = QInputDialog::getItem(this, "Delete Deck", "Select a deck to delete:", names, 0, false, &ok);
    if (!ok) return;

    // Allow deletion of any selected deck name (names list won't include empty ones now)
    if (flashcardManager::instance().removeDeck(deckToDelete)) {
        StatsTracker::instance().trackDeckCreated();
        refreshDeckButtons();
        QMessageBox::information(this, "Deleted", "Deck deleted successfully.");
    } else {
        QMessageBox::warning(this, "Delete Failed", "Could not delete deck.");
    }
}

void MainWindow::onRenameDeckClicked()
{
    QStringList names = flashcardManager::instance().getDeckNames();
    if (names.isEmpty()) {
        QMessageBox::information(this, "No Decks", "There are no decks to rename.");
        return;
    }

    bool ok = false;
    QString deckToRename = QInputDialog::getItem(this, "Rename Deck", "Select a deck:", names, 0, false, &ok);
    if (!ok) return;

    QString newName = QInputDialog::getText(this, "Rename Deck", "New deck name:", QLineEdit::Normal, deckToRename, &ok);
    if (!ok) return;

    newName = newName.trimmed();
    if (newName.isEmpty()) {
        QMessageBox::warning(this, "Invalid Name", "Deck name cannot be empty.");
        return;
    }

    if (newName == deckToRename) return;

    // Rename = remove old key + reinsert deck under new key
    deck *d = flashcardManager::instance().getDeck(deckToRename);
    if (!d) return;

    deck copy = *d;
    copy.setName(newName);

    flashcardManager::instance().removeDeck(deckToRename);
    flashcardManager::instance().addDeck(copy);

    refreshDeckButtons();
    QMessageBox::information(this, "Renamed", "Deck renamed successfully.");
}

void MainWindow::onFilterDeckClicked()
{
    bool ok = false;
    QString tag = QInputDialog::getText(this, "Filter Decks", "Enter tag to filter (leave empty to clear):", QLineEdit::Normal, currentTagFilter, &ok);
    if (!ok) return;

    currentTagFilter = tag.trimmed();
    currentPage = 0;
    refreshDeckButtons();
}

void MainWindow::onViewStatsClicked()
{
    StatsTracker& tracker = StatsTracker::instance();

    QString msg = QString("Total Decks: %1\nTotal Cards: %2\nTotal Reviews: %3\nCorrect: %4\nIncorrect: %5")
        .arg(tracker.getTotalDecks())
        .arg(tracker.getTotalCards())
        .arg(tracker.getTotalReviews())
        .arg(tracker.getTotalCorrect())
        .arg(tracker.getTotalIncorrect());

    QMessageBox::information(this, "Statistics", msg);
}

void MainWindow::onImportDeckClicked()
{
    const QString filePath = QFileDialog::getOpenFileName(this, "Import Deck", QString(), "JSON Files (*.json)");
    if (filePath.isEmpty()) return;

    QString importedName;
    QString err;
    if (!flashcardManager::instance().importDeckFromFile(filePath, &importedName, &err)) {
        QMessageBox::warning(this, "Import Failed", err.isEmpty() ? "Could not import deck." : err);
        return;
    }

    refreshDeckButtons();
    QMessageBox::information(this, "Imported", QString("Imported deck: %1").arg(importedName));
}

void MainWindow::onExportDeckClicked()
{
    QStringList names = flashcardManager::instance().getDeckNames();
    if (names.isEmpty()) {
        QMessageBox::information(this, "No Decks", "There are no decks to export.");
        return;
    }

    bool ok = false;
    QString deckName = QInputDialog::getItem(this, "Export Deck", "Select a deck:", names, 0, false, &ok);
    if (!ok) return;

    const QString filePath = QFileDialog::getSaveFileName(this, "Export Deck", deckName + ".json", "JSON Files (*.json)");
    if (filePath.isEmpty()) return;

    QString err;
    if (!flashcardManager::instance().exportDeckToFile(deckName, filePath, &err)) {
        QMessageBox::warning(this, "Export Failed", err.isEmpty() ? "Could not export deck." : err);
        return;
    }

    QMessageBox::information(this, "Exported", "Deck exported successfully.");
}

void MainWindow::onExportAllDecksClicked()
{
    const QString filePath = QFileDialog::getSaveFileName(this, "Export All Decks", "all_decks.json", "JSON Files (*.json)");
    if (filePath.isEmpty()) return;

    QString err;
    if (!flashcardManager::instance().exportAllDecksToFile(filePath, &err)) {
        QMessageBox::warning(this, "Export Failed", err.isEmpty() ? "Could not export decks." : err);
        return;
    }

    QMessageBox::information(this, "Exported", "All decks exported successfully.");
}
