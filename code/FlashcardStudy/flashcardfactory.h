#ifndef FLASHCARDFACTORY_H
#define FLASHCARDFACTORY_H

#include "flashcard.h"
#include <QString>
#include <memory>

/*
 * Factory Method pattern (minimal, non-invasive):
 * - FlashcardType selects which concrete "Creator" is responsible for producing a flashcard.
 * - Today your concrete data model is a single `flashcard` class (question/answer),
 *   so all creators currently return a `flashcard`. This still satisfies the pattern
 *   and lets you extend later (e.g., by evolving flashcard into a polymorphic hierarchy).
 */

enum class FlashcardType {
    Text,
    Image,          // placeholder for future extension
    MultipleChoice  // placeholder for future extension
};

class FlashcardCreator {
public:
    virtual ~FlashcardCreator() = default;
    virtual flashcard create(const QString& question, const QString& answer) const = 0;
};

class TextFlashcardCreator final : public FlashcardCreator {
public:
    flashcard create(const QString& question, const QString& answer) const override {
        return flashcard(question, answer);
    }
};

class ImageFlashcardCreator final : public FlashcardCreator {
public:
    // Placeholder: until you add image fields, we store text like a regular flashcard.
    flashcard create(const QString& question, const QString& answer) const override {
        return flashcard(question, answer);
    }
};

class MultipleChoiceFlashcardCreator final : public FlashcardCreator {
public:
    // Placeholder: until you add options/correct choice fields, we store text like a regular flashcard.
    flashcard create(const QString& question, const QString& answer) const override {
        return flashcard(question, answer);
    }
};

class FlashcardFactory {
public:
    // Factory Method entry point
    static flashcard create(FlashcardType type, const QString& question, const QString& answer) {
        switch (type) {
        case FlashcardType::Text: {
            static TextFlashcardCreator creator;
            return creator.create(question, answer);
        }
        case FlashcardType::Image: {
            static ImageFlashcardCreator creator;
            return creator.create(question, answer);
        }
        case FlashcardType::MultipleChoice: {
            static MultipleChoiceFlashcardCreator creator;
            return creator.create(question, answer);
        }
        default: {
            static TextFlashcardCreator creator;
            return creator.create(question, answer);
        }
        }
    }
};

#endif // FLASHCARDFACTORY_H
