#include <gtest/gtest.h>
#include "qt_gtest_print.h"

#include "deck.h"
#include "flashcard.h"

class DeckTest : public ::testing::Test {
protected:
    deck d {"Test Deck", "tag1"};
    flashcard fc1 {"Q1", "A1"};
    flashcard fc2 {"Q2", "A2"};
    flashcard fc3 {"Q3", "A3"};
};

TEST_F(DeckTest, StartsEmpty)
{
    EXPECT_EQ(d.getSize(), 0);
    EXPECT_EQ(d.getName(), QString("Test Deck"));
    EXPECT_EQ(d.getTag(),  QString("tag1"));
}

TEST_F(DeckTest, AddCardIncreasesSizeAndPreservesOrder)
{
    d.addCard(fc1);
    d.addCard(fc2);

    ASSERT_EQ(d.getSize(), 2);
    EXPECT_EQ(d.getCard(0).getQuestion(), QString("Q1"));
    EXPECT_EQ(d.getCard(0).getAnswer(),   QString("A1"));
    EXPECT_EQ(d.getCard(1).getQuestion(), QString("Q2"));
    EXPECT_EQ(d.getCard(1).getAnswer(),   QString("A2"));
}

TEST_F(DeckTest, UpdateCardValidIndexReplacesCard)
{
    d.addCard(fc1);
    d.addCard(fc2);

    flashcard replacement {"Q2-new", "A2-new"};
    EXPECT_TRUE(d.updateCard(1, replacement));

    EXPECT_EQ(d.getCard(1).getQuestion(), QString("Q2-new"));
    EXPECT_EQ(d.getCard(1).getAnswer(),   QString("A2-new"));
}

TEST_F(DeckTest, UpdateCardInvalidIndexReturnsFalseAndDoesNotChange)
{
    d.addCard(fc1);

    flashcard replacement {"Qx", "Ax"};
    EXPECT_FALSE(d.updateCard(-1, replacement));
    EXPECT_FALSE(d.updateCard( 1, replacement));

    ASSERT_EQ(d.getSize(), 1);
    EXPECT_EQ(d.getCard(0).getQuestion(), QString("Q1"));
    EXPECT_EQ(d.getCard(0).getAnswer(),   QString("A1"));
}

TEST_F(DeckTest, RemoveCardValidIndexRemovesAndShifts)
{
    d.addCard(fc1);
    d.addCard(fc2);
    d.addCard(fc3);

    EXPECT_TRUE(d.removeCard(1)); // remove middle

    ASSERT_EQ(d.getSize(), 2);
    EXPECT_EQ(d.getCard(0).getQuestion(), QString("Q1"));
    EXPECT_EQ(d.getCard(1).getQuestion(), QString("Q3")); // shifted
}

TEST_F(DeckTest, RemoveCardInvalidIndexReturnsFalseAndDoesNotChange)
{
    d.addCard(fc1);
    d.addCard(fc2);

    EXPECT_FALSE(d.removeCard(-1));
    EXPECT_FALSE(d.removeCard( 2));

    ASSERT_EQ(d.getSize(), 2);
    EXPECT_EQ(d.getCard(0).getQuestion(), QString("Q1"));
    EXPECT_EQ(d.getCard(1).getQuestion(), QString("Q2"));
}

TEST_F(DeckTest, SettersUpdateMetadata)
{
    d.setName("New Name");
    d.setTag("New Tag");

    EXPECT_EQ(d.getName(), QString("New Name"));
    EXPECT_EQ(d.getTag(),  QString("New Tag"));
}
