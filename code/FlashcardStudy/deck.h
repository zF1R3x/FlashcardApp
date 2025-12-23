#ifndef DECK_H
#define DECK_H

#include <QVector>
#include "flashcard.h"

class deck
{
private:
    QVector<flashcard> cards;
    QString name;
    QString tag;
public:
    deck(const QString &name = "Untitled Deck", const QString &tag = "");
    void addCard(const flashcard &card);
    flashcard getCard(int index) const;
    int getSize() const;
    QString getName() const;
    void setName(const QString &newName);
    bool updateCard(int index, const flashcard& card);
    bool removeCard(int index);
    QString getTag() const;
    void setTag(const QString &newTag);
};

#endif // DECK_H
