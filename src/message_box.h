#pragma once
class QString;
namespace msgbox {
void ShowError(const QString& msg);
void ShowInfo(const QString& msg);
void ShowWarn(const QString& msg);
void ShowQuestion(const QString& msg);
} // namespace msgbox