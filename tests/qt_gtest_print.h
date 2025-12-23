#pragma once
#include <QString>
#include <ostream>

// GoogleTest pretty-printer for QString (so ASSERT_EQ prints readable values).
// Based on the "Print Qt Data Types in Output" guidance from your Unit Testing slides.
namespace QT_BEGIN_NAMESPACE {
inline void PrintTo(const QString& qString, ::std::ostream* os)
{
    *os << qPrintable(qString);
}
} // namespace QT_BEGIN_NAMESPACE
