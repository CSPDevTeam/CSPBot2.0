#pragma once
class QString;
namespace msgbox {
void ShowError(const QString& msg);
void ShowHint(const QString& msg);
void ShowWarn(const QString& msg);
} // namespace msgbox