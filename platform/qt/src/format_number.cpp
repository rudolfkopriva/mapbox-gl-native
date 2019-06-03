#include <mbgl/util/platform.hpp>

#include <QtQml/QtQml>

namespace mbgl {
namespace platform {

std::string formatNumber(double number, const std::string& localeId, const std::string& currency,
                         uint8_t minFractionDigits, uint8_t maxFractionDigits) {

    std::string formatted;
    QQmlEngine engine;
    QQmlComponent component(&engine, QUrl(QStringLiteral("qrc:/FormatNumber.qml")));
    QObject *object = component.create();

    QVariant returnedValue;
    QVariant qNumber = number;
    QVariant qLocale = QString::fromStdString(localeId);
    QVariant qCurrency = QString::fromStdString(currency);
    // Qt toLocaleString() API takes only one precision argument
    (void)minFractionDigits;
    QVariant qMaxFractionDigits = maxFractionDigits;
    QMetaObject::invokeMethod(object, "formatNumber", Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, qNumber), Q_ARG(QVariant, qLocale), Q_ARG(QVariant, qCurrency),
                              Q_ARG(QVariant, qMaxFractionDigits));

    formatted = returnedValue.toString().toStdString();
    delete object;

    return formatted;
}

} // namespace platform
} // namespace mbgl
