#ifndef DECKWINDOW_H
#define DECKWINDOW_H

#pragma once
#include <QDialog>
#include "deck.h"
#include <QStringListModel>
#include "studywindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DeckWindow; }
QT_END_NAMESPACE

class deck;
class flashcard;
class studywindow;

class DeckWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DeckWindow(deck* d, QWidget* parent = nullptr);
    ~DeckWindow();

private slots:
    // --- CRUD operations ---
    void onAddButtonClicked();
    void onUpdateButtonClicked();
    void onDeleteButtonClicked();
    void onStudyButtonClicked();
    // --- Navigation ---
    void onPrevButtonClicked();
    void onNextButtonClicked();

    // --- List selection handling ---
    void onRowChanged(const QModelIndex& index);

    // --- Window controls ---
    void onCloseButtonClicked();

private:
    // --- Internal helper methods ---
    void rebuildList(int keepIndex = -1);   // repopulate the list view
    void loadFieldsFromCurrent();
    void setCurrentIndex(int i);

private:
    Ui::DeckWindow* ui;
    deck* m_deck = nullptr;
    int m_current = -1;
    studywindow* studywindow;

    QStringListModel* m_model = nullptr;    // <-- model for the QListView
};
#endif // DECKWINDOW_H
