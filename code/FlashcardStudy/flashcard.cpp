#include "flashcard.h"

flashcard::flashcard(const QString &q, const QString &a)
    : question(q), answer(a) {}

// Returns question
QString flashcard::getQuestion() const { return question; }

// Returns answer
QString flashcard::getAnswer() const { return answer; }

// Set question to the given text
void flashcard::setQuestion(const QString &q) { question = q; }

// Set answer to the given text
void flashcard::setAnswer(const QString &a) {answer = a;}
