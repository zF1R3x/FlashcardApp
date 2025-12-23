#include "flashcardmanager.h"

#include <QFile>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

static QJsonObject flashcardToJson(const flashcard& fc)
{
    QJsonObject o;
    o["question"] = fc.getQuestion();
    o["answer"] = fc.getAnswer();
    return o;
}

static flashcard flashcardFromJson(const QJsonObject& o)
{
    return flashcard(o.value("question").toString(), o.value("answer").toString());
}

static QJsonObject deckToJson(const deck& d)
{
    QJsonObject o;
    o["name"] = d.getName();
    o["tag"]  = d.getTag();

    QJsonArray cards;
    for (int i = 0; i < d.getSize(); ++i) {
        cards.append(flashcardToJson(d.getCard(i)));
    }
    o["cards"] = cards;
    return o;
}

static deck deckFromJson(const QJsonObject& o)
{
    const QString name = o.value("name").toString();
    const QString tag  = o.value("tag").toString();
    deck d(name, tag);

    const QJsonArray cards = o.value("cards").toArray();
    for (const auto& v : cards) {
        if (!v.isObject()) continue;
        d.addCard(flashcardFromJson(v.toObject()));
    }
    return d;
}

flashcardManager::flashcardManager()
{
    // Lazy load: do nothing here. instance() will call load once.
}

flashcardManager& flashcardManager::instance()
{
    static flashcardManager inst;
    if (!inst.m_loaded) {
        inst.loadFromDisk(nullptr);
        inst.m_loaded = true;
    }
    return inst;
}

QString flashcardManager::storageFilePath() const
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return QDir(dir).filePath("decks.json");
}

bool flashcardManager::saveToDisk(QString *errorOut) const
{
    const QString path = storageFilePath();
    QDir().mkpath(QFileInfo(path).absolutePath());

    QJsonArray deckArr;
    for (auto it = decks.constBegin(); it != decks.constEnd(); ++it) {
        deckArr.append(deckToJson(it.value()));
    }

    QJsonObject root;
    root["version"] = 1;
    root["decks"] = deckArr;

    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (errorOut) *errorOut = QString("Could not open %1 for writing.").arg(path);
        return false;
    }
    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

bool flashcardManager::loadFromDisk(QString *errorOut)
{
    const QString path = storageFilePath();
    QFile f(path);
    if (!f.exists()) {
        // First run: nothing to load
        return true;
    }
    if (!f.open(QIODevice::ReadOnly)) {
        if (errorOut) *errorOut = QString("Could not open %1 for reading.").arg(path);
        return false;
    }

    const QByteArray data = f.readAll();
    const QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        if (errorOut) *errorOut = QString("Invalid JSON in %1.").arg(path);
        return false;
    }

    const QJsonObject root = doc.object();
    const QJsonArray deckArr = root.value("decks").toArray();

    // Replace current decks with loaded decks
    decks.clear();

    for (const auto& v : deckArr) {
        if (!v.isObject()) continue;
        deck d = deckFromJson(v.toObject());
        // Ensure key matches the deck name
        decks.insert(d.getName(), d);
    }
    return true;
}

void flashcardManager::addDeck(const deck &d)
{
    // Ensure loaded
    (void)instance();

    decks[d.getName()] = d;
    if (!m_suppressAutosave) saveToDisk(nullptr);
}

deck* flashcardManager::getDeck(const QString &name)
{
    (void)instance();

    auto it = decks.find(name);
    if (it == decks.end()) return nullptr;
    return &it.value();
}

bool flashcardManager::removeDeck(const QString &name)
{
    (void)instance();

    const int removed = decks.remove(name);
    if (removed > 0 && !m_suppressAutosave) saveToDisk(nullptr);
    return removed > 0;
}

QStringList flashcardManager::getDeckNames() const
{
    // Note: const function cannot call instance() safely without const_cast.
    // In practice, MainWindow calls instance() early.
    return decks.keys();
}

static QString uniqueNameForImport(const QMap<QString, deck>& decks, QString base)
{
    if (!decks.contains(base)) return base;
    int i = 2;
    while (decks.contains(QString("%1 (%2)").arg(base).arg(i))) ++i;
    return QString("%1 (%2)").arg(base).arg(i);
}

bool flashcardManager::exportDeckToFile(const QString& deckName, const QString& filePath, QString *errorOut) const
{
    auto it = decks.constFind(deckName);
    if (it == decks.constEnd()) {
        if (errorOut) *errorOut = "Deck not found.";
        return false;
    }

    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (errorOut) *errorOut = "Could not open file for writing.";
        return false;
    }

    QJsonObject root;
    root["version"] = 1;
    root["deck"] = deckToJson(it.value());

    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

bool flashcardManager::exportAllDecksToFile(const QString& filePath, QString *errorOut) const
{
    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (errorOut) *errorOut = "Could not open file for writing.";
        return false;
    }

    QJsonArray deckArr;
    for (auto it = decks.constBegin(); it != decks.constEnd(); ++it) {
        deckArr.append(deckToJson(it.value()));
    }

    QJsonObject root;
    root["version"] = 1;
    root["decks"] = deckArr;

    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

bool flashcardManager::importDeckFromFile(const QString& filePath, QString *importedNameOut, QString *errorOut)
{
    (void)instance();

    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) {
        if (errorOut) *errorOut = "Could not open file for reading.";
        return false;
    }

    const QByteArray data = f.readAll();
    const QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        if (errorOut) *errorOut = "Invalid JSON file.";
        return false;
    }

    const QJsonObject root = doc.object();

    // Accept either {"deck": {...}} or a raw deck object
    QJsonObject deckObj;
    if (root.contains("deck") && root.value("deck").isObject()) {
        deckObj = root.value("deck").toObject();
    } else {
        deckObj = root;
    }

    deck d = deckFromJson(deckObj);
    QString name = d.getName().trimmed();
    if (name.isEmpty()) name = "Imported Deck";

    name = uniqueNameForImport(decks, name);
    d.setName(name);

    // addDeck will autosave
    addDeck(d);

    if (importedNameOut) *importedNameOut = name;
    return true;
}
