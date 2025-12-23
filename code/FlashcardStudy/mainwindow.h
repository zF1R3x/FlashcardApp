#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDeckButtonClicked();

    void onCreateDeckClicked();
    void onDeleteDeckClicked();
    void onRenameDeckClicked();

    void onFilterDeckClicked();
    void onNextPageClicked();
    void onPrevPageClicked();
    void onViewStatsClicked();

    // Import/Export (JSON)
    void onImportDeckClicked();
    void onExportDeckClicked();
    void onExportAllDecksClicked();

private:
    Ui::MainWindow *ui;
    QVector<QPushButton*> deckButtons;

    int currentPage = 0;
    const int decksPerPage = 9;

    void setupDeckButtons();
    void refreshDeckButtons();
    int totalPages() const;
    QStringList getFilteredDeckNames() const;

    QString currentTagFilter = "";
};

#endif // MAINWINDOW_H
