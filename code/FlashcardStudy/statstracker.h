#ifndef STATSTRACKER_H
#define STATSTRACKER_H

#include <QSettings>
#include <QString>

class StatsTracker
{
public:
    static StatsTracker& instance();

    void trackDeckCreated();
    void trackCardCreated();
    void trackReview();

    int getTotalDecks() const;
    int getTotalCards() const;
    int getTotalReviews() const;
    void trackCorrectAnswer();
    void trackIncorrectAnswer();
    int getTotalCorrect() const;
    int getTotalIncorrect() const;
    void resetStats();

private:
    StatsTracker();
    QSettings settings;

};

#endif // STATSTRACKER_H
