#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QString>
#include <QStringList>

#include "deck.h"

using ::testing::Return;
using ::testing::Truly;

struct IFlashcardManager {
    virtual ~IFlashcardManager() = default;
    virtual void addDeck(const deck& d) = 0;
    virtual deck* getDeck(const QString& name) = 0;
    virtual QStringList getDeckNames() const = 0;
    virtual bool removeDeck(const QString& name) = 0;
};

class MockFlashcardManager : public IFlashcardManager {
public:
    MOCK_METHOD(void, addDeck, (const deck& d), (override));
    MOCK_METHOD(deck*, getDeck, (const QString& name), (override));
    MOCK_METHOD(QStringList, getDeckNames, (), (const, override));
    MOCK_METHOD(bool, removeDeck, (const QString& name), (override));
};

TEST(FlashcardManagerMockTest, CanMockFlashcardManagerBehavior)
{
    MockFlashcardManager mgr;

    deck fake("MockDeck", "");

    EXPECT_CALL(mgr, getDeckNames())
        .WillOnce(Return(QStringList{ "MockDeck" }));

    EXPECT_CALL(mgr, getDeck(Truly([](const QString& s){
                    return s == "MockDeck";
                }))).WillOnce(Return(&fake));

    EXPECT_CALL(mgr, removeDeck(Truly([](const QString& s){
                    return s == "MockDeck";
                }))).WillOnce(Return(true));

    const QStringList names = mgr.getDeckNames();
    ASSERT_EQ(names.size(), 1);
    EXPECT_EQ(names[0], "MockDeck");

    deck* got = mgr.getDeck("MockDeck");
    ASSERT_NE(got, nullptr);
    EXPECT_EQ(got->getName(), "MockDeck");

    EXPECT_TRUE(mgr.removeDeck("MockDeck"));
}
