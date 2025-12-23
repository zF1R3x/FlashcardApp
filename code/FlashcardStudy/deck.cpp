#include "deck.h"

deck::deck(const QString &name, const QString &tag) : name(name), tag(tag) {}

// Adds a card
void deck::addCard(const flashcard &card){
    cards.append(card);
}

// Return a pointer to the card with the given index
flashcard deck::getCard(int index) const {
    return cards.at(index);
}

// Return deck size
int deck::getSize() const {
    return cards.size();
}

// Return deck name
QString deck::getName() const{
    return name;
}

void deck::setName(const QString &newName) {
    name = newName;
}

bool deck::updateCard(int index, const flashcard& card) {
    if (index < 0 || index >= cards.size()) return false;
    cards[index] = card;
    return true;
}

bool deck::removeCard(int index) {
    if (index < 0 || index >= cards.size()) return false;
    cards.removeAt(index);
    return true;
}

QString deck::getTag() const {
    return tag;
}

void deck::setTag(const QString &newTag) {
    tag = newTag;
}
