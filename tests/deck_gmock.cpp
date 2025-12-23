#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "qt_gtest_print.h"

#include "deck.h"
#include "flashcard.h"

using ::testing::_;
using ::testing::InSequence;

class ICardSink {
public:
    virtual ~ICardSink() = default;
    virtual void onCard(const QString& question, const QString& answer) = 0;
};

class MockCardSink : public ICardSink {
public:
    MOCK_METHOD(void, onCard, (const QString& question, const QString& answer), (override));
};

static void streamDeckToSink(const deck& d, ICardSink& sink)
{
    for (int i = 0; i < d.getSize(); ++i) {
        flashcard fc = d.getCard(i);
        sink.onCard(fc.getQuestion(), fc.getAnswer());
    }
}

TEST(DeckWithMock, StreamsCardsToSinkInOrder)
{
    deck d("D", "t");
    d.addCard(flashcard("Q1", "A1"));
    d.addCard(flashcard("Q2", "A2"));

    MockCardSink sink;

    InSequence seq;
    EXPECT_CALL(sink, onCard(QString("Q1"), QString("A1")));
    EXPECT_CALL(sink, onCard(QString("Q2"), QString("A2")));

    streamDeckToSink(d, sink);
}

TEST(DeckWithMock, EmptyDeckMakesNoCalls)
{
    deck d("Empty", "");
    MockCardSink sink;

    EXPECT_CALL(sink, onCard(_, _)).Times(0);
    streamDeckToSink(d, sink);
}
