#include "statstracker.h"

StatsTracker::StatsTracker(): settings("MyFlashcardApp", "Statistics") {}

StatsTracker& StatsTracker::instance(){
    static StatsTracker instance;
    return instance;
}

void StatsTracker::trackDeckCreated() {
    int current = settings.value("decksCreated", 0).toInt();
    settings.setValue("decksCreated", current + 1);
}

void StatsTracker::trackCardCreated() {
    int current = settings.value("cardsCreated", 0).toInt();
    settings.setValue("cardsCreated", current + 1);
}

void StatsTracker::trackReview() {
    int current = settings.value("cardsReviewed", 0).toInt();
    settings.setValue("cardsReviewed", current + 1);
}

int StatsTracker::getTotalDecks() const {
    return settings.value("decksCreated", 0).toInt();
}

int StatsTracker::getTotalCards() const {
    return settings.value("cardsCreated", 0).toInt();
}

int StatsTracker::getTotalReviews() const {
    return settings.value("cardsReviewed", 0).toInt();
}

void StatsTracker::resetStats() {
    settings.clear();
}
void StatsTracker::trackCorrectAnswer() {
    int current = settings.value("answersCorrect", 0).toInt();
    settings.setValue("answersCorrect", current + 1);
}

void StatsTracker::trackIncorrectAnswer() {
    int current = settings.value("answersIncorrect", 0).toInt();
    settings.setValue("answersIncorrect", current + 1);
}
int StatsTracker::getTotalCorrect() const {
    return settings.value("answersCorrect", 0).toInt();
}

int StatsTracker::getTotalIncorrect() const {
    return settings.value("answersIncorrect", 0).toInt();
}
