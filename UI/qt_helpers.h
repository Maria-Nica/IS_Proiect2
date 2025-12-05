#pragma once

#include <QWidget>
#include <QPushButton>
#include <QString>

inline QPushButton* createStyledButton(QWidget* parent, const QString& text,
	const QString& style = QString(), int minWidth = 0)
{
	QPushButton* btn = new QPushButton(text, parent);
	if (!style.isEmpty())
		btn->setStyleSheet(style);
	if (minWidth > 0)
		btn->setMinimumWidth(minWidth);
	return btn;
}

namespace UiStyles
{
	inline const QString& titleStyle()
	{
		static const QString s = "font-size:18pt; font-weight: bold; margin:10px;";
		return s;
	}
	inline const QString& largeLabelStyle()
	{
		static const QString s = "font-size:18pt; font-weight: bold; margin:20px;";
		return s;
	}
	inline const QString& headerLabelStyle()
	{
		static const QString s = "font-size:24pt; font-weight: bold; color: white;";
		return s;
	}
	inline const QString& sectionLabelStyle()
	{
		static const QString s = "font-size:14pt; font-weight: bold;";
		return s;
	}
	inline const QString& greenButton()
	{
		static const QString s = "font-size:14pt; padding:15px; background-color: #4CAF50; color: white;";
		return s;
	}
	inline const QString& blueButton()
	{
		static const QString s = "font-size:16pt; padding:20px; background-color: #2196F3; color: white;";
		return s;
	}
	inline const QString& orangeButton()
	{
		static const QString s = "font-size:14pt; padding:15px; background-color: #FF9800; color: white;";
		return s;
	}
	inline const QString& rotateButtonStyle()
	{
		static const QString s = "font-size:12pt; padding:10px;";
		return s;
	}
}