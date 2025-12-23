#ifndef FLASHCARD_H
#define FLASHCARD_H

#include <QString>

class flashcard
{
private:
    QString question;
    QString answer;

public:
    flashcard(const QString &q = "", const QString &a = "");
    QString getQuestion() const;
    QString getAnswer() const;
    void setQuestion(const QString &q);
    void setAnswer(const QString &a);
};

#endif // FLASHCARD_H
