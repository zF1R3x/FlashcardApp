#include <gtest/gtest.h>

#include <QStandardPaths>
#include <QStringList>

#include "flashcardmanager.h"
#include "deck.h"
#include "flashcard.h"

// IMPORTANT:
// flashcardManager is a singleton and it does disk IO via QStandardPaths.
// This makes tests touch user data unless we enable Qt "test mode".
static void enableQtTestModeOnce()
{
    static bool done = false;
    if (!done) {
        QStandardPaths::setTestModeEnabled(true);
        done = true;
    }
}

// Clear state between tests (singleton keeps state across runs)
static void clearAllDecks(flashcardManager& mgr)
{
    const QStringList names = mgr.getDeckNames();
    for (const QString& n : names) {
        mgr.removeDeck(n);
    }
}

TEST(FlashcardManagerTest, StartsEmptyAfterClear)
{
    enableQtTestModeOnce();

    flashcardManager& mgr = flashcardManager::instance();
    clearAllDecks(mgr);

    EXPECT_TRUE(mgr.getDeckNames().isEmpty());
}

TEST(FlashcardManagerTest, AddDeckThenGetDeck)
{
    enableQtTestModeOnce();

    QStandardPaths::setTestModeEnabled(true);

    flashcardManager& mgr = flashcardManager::instance();
    clearAllDecks(mgr);

    deck d("TestDeck", "tag1");
    d.addCard(flashcard("Q1", "A1"));

    mgr.addDeck(d);

    deck* got = mgr.getDeck("TestDeck");
    ASSERT_NE(got, nullptr);
    EXPECT_EQ(got->getName(), "TestDeck");
    EXPECT_EQ(got->getTag(), "tag1");
    EXPECT_EQ(got->getSize(), 1);
    EXPECT_EQ(got->getCard(0).getQuestion(), "Q1");
    EXPECT_EQ(got->getCard(0).getAnswer(), "A1");
}

TEST(FlashcardManagerTest, RemoveDeckWorksAndMissingDeckReturnsNull)
{
    enableQtTestModeOnce();

    flashcardManager& mgr = flashcardManager::instance();
    clearAllDecks(mgr);

    mgr.addDeck(deck("ToRemove", ""));

    // sanity
    ASSERT_NE(mgr.getDeck("ToRemove"), nullptr);

    EXPECT_TRUE(mgr.removeDeck("ToRemove"));
    EXPECT_EQ(mgr.getDeck("ToRemove"), nullptr);

    // removing again should fail (or at least not crash)
    EXPECT_FALSE(mgr.removeDeck("ToRemove"));
}
