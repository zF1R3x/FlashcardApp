#ifndef FLASHCARDMANAGER_H
#define FLASHCARDMANAGER_H

#include <QMap>
#include <QStringList>
#include "deck.h"

/*
 * flashcardManager (Singleton) + JSON persistence + Import/Export
 *
 * Persistence:
 *  - Automatically loads decks from disk on first use.
 *  - Saves decks to disk when add/remove is called.
 *  - Call saveToDisk() after in-place edits (e.g., adding cards inside a DeckWindow).
 *
 * Import/Export:
 *  - exportDeckToFile(...) writes a single deck as JSON.
 *  - importDeckFromFile(...) reads a deck JSON and adds it (renaming on collision).
 */

class flashcardManager
{
public:
    // Singleton access
    static flashcardManager& instance();

    // Deck CRUD
    void addDeck(const deck &d);
    deck* getDeck(const QString &name);
    bool removeDeck(const QString &name);
    QStringList getDeckNames() const;

    // Persistence
    bool saveToDisk(QString *errorOut = nullptr) const;
    bool loadFromDisk(QString *errorOut = nullptr);

    // Import/Export
    bool exportDeckToFile(const QString& deckName, const QString& filePath, QString *errorOut = nullptr) const;
    bool exportAllDecksToFile(const QString& filePath, QString *errorOut = nullptr) const;
    bool importDeckFromFile(const QString& filePath, QString *importedNameOut = nullptr, QString *errorOut = nullptr);

    // Config
    QString storageFilePath() const;

private:
    flashcardManager(); // private for singleton
    flashcardManager(const flashcardManager&) = delete;
    flashcardManager& operator=(const flashcardManager&) = delete;

    QMap<QString, deck> decks;
    bool m_loaded = false;
    bool m_suppressAutosave = false;
};

#endif // FLASHCARDMANAGER_H
