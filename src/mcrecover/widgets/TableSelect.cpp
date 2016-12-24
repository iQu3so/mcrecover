/***************************************************************************
 * GameCube Memory Card Recovery Program.                                  *
 * TableSelect.cpp: Directory/Block Table select widget.                   *
 *                                                                         *
 * Copyright (c) 2014-2016 by David Korth.                                 *
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation; either version 2 of the License, or (at your  *
 * option) any later version.                                              *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License along *
 * with this program; if not, write to the Free Software Foundation, Inc., *
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.           *
 ***************************************************************************/

#include "TableSelect.hpp"

#include "card/Card.hpp"
#include "McRecoverQApplication.hpp"

// Qt includes.
#include <QButtonGroup>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

/** TableSelectPrivate **/

class TableSelectPrivate
{
	public:
		explicit TableSelectPrivate(TableSelect *q);

	private:
		TableSelect *const q_ptr;
		Q_DECLARE_PUBLIC(TableSelect)
		Q_DISABLE_COPY(TableSelectPrivate)

	public:
		struct Ui_TableSelect {
			QGridLayout *gridMain;

			// Directory table.
			QLabel *lblDirImage;
			QButtonGroup *btnDirGroup;
			QVector<QPushButton*> btnDir;
			QVector<QLabel*> lblDirStatus;

			// Block table.
			QLabel *lblBlockImage;
			QButtonGroup *btnBlockGroup;
			QVector<QPushButton*> btnBlock;
			QVector<QLabel*> lblBlockStatus;

			void setupUi(QWidget *TableSelect);
			void retranslateUi(QWidget *TableSelect);
		};
		Ui_TableSelect ui;

		Card *card;

		// Icon size.
		static const int iconSz = 16;

	protected:
		/**
		 * Update the DAT/BAT widget count based on the active Card.
		 */
		void updateWidgetCount(void);

		/**
		 * Update the Directory Table display.
		 */
		void updateDirTableDisplay(void);

		/**
		 * Update the Block Table display.
		 */
		void updateBlockTableDisplay(void);

	public:
		/**
		 * Update the widget display.
		 */
		void updateWidgetDisplay(void);
};

TableSelectPrivate::TableSelectPrivate(TableSelect *q)
	: q_ptr(q)
	, card(nullptr)
{ }

/**
 * Initialize the UI.
 * @param TableSelect TableSelect.
 */
void TableSelectPrivate::Ui_TableSelect::setupUi(QWidget *TableSelect)
{
	if (TableSelect->objectName().isEmpty())
		TableSelect->setObjectName(QLatin1String("TableSelect"));

	// Icon size. [TODO: Where to determine icon size?]
	const QSize qIconSz(TableSelectPrivate::iconSz, TableSelectPrivate::iconSz);

	gridMain = new QGridLayout(TableSelect);
	gridMain->setObjectName(QLatin1String("gridMain"));
	gridMain->setContentsMargins(0, 0, 0, 0);
	gridMain->setVerticalSpacing(0);

	// Directory Table selection.
	lblDirImage = new QLabel(TableSelect);
	lblDirImage->setObjectName(QLatin1String("lblDirImage"));
	gridMain->addWidget(lblDirImage, 0, 0, 1, 1);

	QIcon iconDirTable = McRecoverQApplication::StandardIcon(
		QStyle::SP_DirClosedIcon, nullptr, lblDirImage);
	lblDirImage->setPixmap(iconDirTable.pixmap(qIconSz));

	// NOTE: btnDir and lblDirStatus are dynamically
	// generated by updateWidgetCount().
	btnDirGroup = new QButtonGroup(TableSelect);
	btnDirGroup->setObjectName(QLatin1String("btnDirGroup"));
	btnDirGroup->setExclusive(true);

	// Block Table selection.
	lblBlockImage = new QLabel(TableSelect);
	lblBlockImage->setObjectName(QLatin1String("lblBlockImage"));
	gridMain->addWidget(lblBlockImage, 0, 3, 1, 1);

	QIcon iconBlockTable;
#ifdef Q_OS_WIN
	// Win32: Get the icon from Windows Defragmenter.
	iconBlockTable = McRecoverQApplication::Win32Icon(
		McRecoverQApplication::W32ICON_DEFRAG, qIconSz);
	if (iconBlockTable.isNull())
#endif /* Q_OS_WIN */
	{
		iconBlockTable = McRecoverQApplication::IconFromTheme(
			QLatin1String("partitionmanager"));
	}
	lblBlockImage->setPixmap(iconBlockTable.pixmap(qIconSz));

	// NOTE: btnBlock and lblBlockStatus are dynamically
	// generated by updateWidgetCount().
	btnBlockGroup = new QButtonGroup(TableSelect);
	btnBlockGroup->setObjectName(QLatin1String("btnBlockGroup"));
	btnBlockGroup->setExclusive(true);

	retranslateUi(TableSelect);
}

/**
 * Initialize the UI.
 * @param TableSelect TableSelect.
 */
void TableSelectPrivate::Ui_TableSelect::retranslateUi(QWidget *TableSelect)
{
#ifndef QT_NO_TOOLTIP
        lblDirImage->setToolTip(TableSelect::tr("Directory Table"));
	lblBlockImage->setToolTip(TableSelect::tr("Block Table"));
#endif // QT_NO_TOOLTIP
        Q_UNUSED(TableSelect);
}

/**
 * Update the DAT/BAT widget count based on the active Card.
 */
void TableSelectPrivate::updateWidgetCount(void)
{
	if (!card) {
		// No Card.
		return;
	}

	Q_Q(TableSelect);
	QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	const QSize btnSize(23, 23);

	// Check if we need to update Dir widgets.
	const int datCount = card->datCount();
	if (datCount < ui.btnDir.size()) {
		// Too many Dir widgets.
		// Remove unneeded ones.
		for (int i = ui.btnDir.size()-1; i >= datCount; i--) {
			delete ui.btnDir[i];
			delete ui.lblDirStatus[i];
		}
		ui.btnDir.resize(datCount);
		ui.lblDirStatus.resize(datCount);
	} else if (datCount > ui.btnDir.size()) {
		// Not enough Dir widgets.
		// Add new ones.
		ui.btnDir.reserve(datCount);
		ui.lblDirStatus.reserve(datCount);
		for (int i = ui.btnDir.size(); i < datCount; i++) {
			QPushButton *btn = new QPushButton(q);
			btn->setMinimumSize(btnSize);
			btn->setMaximumSize(btnSize);
			btn->setBaseSize(btnSize);
			btn->setText(QString(QChar(L'A' + i)));
			btn->setCheckable(true);
			sizePolicy.setHeightForWidth(btn);
			btn->setSizePolicy(sizePolicy);
			ui.gridMain->addWidget(btn, i, 1, 1, 1);
			ui.btnDir.append(btn);
			ui.btnDirGroup->addButton(btn, i);

			QLabel *lbl = new QLabel(q);
			ui.gridMain->addWidget(lbl, i, 2, 1, 1);
			ui.lblDirStatus.append(lbl);
		}
	}

	// Check if we need to update Block widgets.
	const int batCount = card->batCount();
	if (batCount < ui.btnBlock.size()) {
		// Too many Block widgets.
		// Remove unneeded ones.
		for (int i = ui.btnBlock.size()-1; i >= batCount; i--) {
			delete ui.btnBlock[i];
			delete ui.lblBlockStatus[i];
		}
		ui.btnBlock.resize(batCount);
		ui.lblBlockStatus.resize(batCount);
	} else if (batCount > ui.btnBlock.size()) {
		// Not enough Block widgets.
		// Add new ones.
		ui.btnBlock.reserve(batCount);
		ui.lblBlockStatus.reserve(batCount);
		for (int i = ui.btnBlock.size(); i < batCount; i++) {
			QPushButton *btn = new QPushButton(q);
			btn->setMinimumSize(btnSize);
			btn->setMaximumSize(btnSize);
			btn->setBaseSize(btnSize);
			btn->setText(QString(QChar(L'A' + i)));
			btn->setCheckable(true);
			sizePolicy.setHeightForWidth(btn);
			btn->setSizePolicy(sizePolicy);
			ui.gridMain->addWidget(btn, i, 4, 1, 1);
			ui.btnBlock.append(btn);
			ui.btnBlockGroup->addButton(btn, i);

			QLabel *lbl = new QLabel(q);
			ui.gridMain->addWidget(lbl, i, 5, 1, 1);
			ui.lblBlockStatus.append(lbl);
		}
	}
}

/**
 * Update the Directory Table display.
 */
void TableSelectPrivate::updateDirTableDisplay(void)
{
	// Active table CSS.
	// Used to indicate which table is active according to the card header.
	// TODO: Use a better indicator.
	const QString cssActiveHdr =
		QLatin1String("QFrame { border: 2px solid rgb(0,255,0); }");
	const QString cssInactiveHdr =
		QLatin1String("QFrame { margin: 2px; }");

	// Check which table is currently active, selected by the user.
	const int activeDatIdx = card->activeDatIdx();
	if (activeDatIdx >= 0 && activeDatIdx < ui.btnDir.count()) {
		ui.btnDir[activeDatIdx]->setChecked(true);
	} else {
		// No active table.
		foreach (QPushButton *btn, ui.btnDir) {
			btn->setChecked(false);
		}
	}

	// Icon size. [TODO: Where to determine icon size?]
	const QSize qIconSz(iconSz, iconSz);

	// Check which tables are active and valid,
	// according to the card header.
	const int activeDatHdrIdx = card->activeDatHdrIdx();
	for (int i = ui.lblDirStatus.size()-1; i >= 0; i--) {
		QLabel *const lbl = ui.lblDirStatus[i];
		if (i == activeDatHdrIdx) {
			lbl->setStyleSheet(cssActiveHdr);
		} else {
			lbl->setStyleSheet(cssInactiveHdr);
		}

		const bool isDatValid = card->isDatValid(i);
		QStyle::StandardPixmap sp = (isDatValid
					? QStyle::SP_DialogApplyButton
					: QStyle::SP_MessageBoxCritical);
		QIcon icon = McRecoverQApplication::StandardIcon(sp, nullptr, lbl);
		lbl->setPixmap(icon.pixmap(qIconSz));

#ifndef QT_NO_TOOLTIP
		// Set the status tooltip.
		QString tip;
		if (isDatValid) {
			if (i == activeDatHdrIdx) {
				//: %1 == "Block Table" or "Directory Table"; %2 == table ID ('A', 'B', etc.)
				tip = TableSelect::tr("%1 %2 is valid, and is the active table on the card.");
			} else {
				//: %1 == "Block Table" or "Directory Table"; %2 == table ID ('A', 'B', etc.)
				tip = TableSelect::tr("%1 %2 is valid.");
			}
		} else {
			//: %1 == "Block Table" or "Directory Table"; %2 == table ID ('A', 'B', etc.)
			tip = TableSelect::tr("%1 %2 is invalid.");
		}

		lbl->setToolTip(tip
			.arg(TableSelect::tr("Directory Table"))
			.arg(QChar(L'A' + i)));
#endif /* QT_NO_TOOLTIP */
	}
}

/**
 * Update the Block Table display.
 */
void TableSelectPrivate::updateBlockTableDisplay(void)
{
	// Active table CSS.
	// Used to indicate which table is active according to the card header.
	// TODO: Use a better indicator.
	const QString cssActiveHdr =
		QLatin1String("QFrame { border: 2px solid rgb(0,255,0); }");
	const QString cssInactiveHdr =
		QLatin1String("QFrame { margin: 2px; }");

	// Check which table is currently active, selected by the user.
	const int activeBatIdx = card->activeBatIdx();
	if (activeBatIdx >= 0 && activeBatIdx < ui.btnBlock.count()) {
		ui.btnBlock[activeBatIdx]->setChecked(true);
	} else {
		// No active table.
		foreach (QPushButton *btn, ui.btnBlock) {
			btn->setChecked(false);
		}
	}

	// Icon size. [TODO: Where to determine icon size?]
	const QSize qIconSz(iconSz, iconSz);

	// Check which tables are active and valid,
	// according to the card header.
	const int activeBatHdrIdx = card->activeBatHdrIdx();
	for (int i = ui.lblBlockStatus.size()-1; i >= 0; i--) {
		QLabel *const lbl = ui.lblBlockStatus[i];
		if (i == activeBatHdrIdx) {
			lbl->setStyleSheet(cssActiveHdr);
		} else {
			lbl->setStyleSheet(cssInactiveHdr);
		}

		const bool isBatValid = card->isBatValid(i);
		QStyle::StandardPixmap sp = (isBatValid
					? QStyle::SP_DialogApplyButton
					: QStyle::SP_MessageBoxCritical);
		QIcon icon = McRecoverQApplication::StandardIcon(sp, nullptr, lbl);
		lbl->setPixmap(icon.pixmap(qIconSz));

#ifndef QT_NO_TOOLTIP
		// Set the status tooltip.
		QString tip;
		if (isBatValid) {
			if (i == activeBatHdrIdx) {
				//: %1 == "Block Table" or "Directory Table"; %2 == table ID ('A', 'B', etc.)
				tip = TableSelect::tr("%1 %2 is valid, and is the active table on the card.");
			} else {
				//: %1 == "Block Table" or "Directory Table"; %2 == table ID ('A', 'B', etc.)
				tip = TableSelect::tr("%1 %2 is valid.");
			}
		} else {
			//: %1 == "Block Table" or "Directory Table"; %2 == table ID ('A', 'B', etc.)
			tip = TableSelect::tr("%1 %2 is invalid.");
		}

		lbl->setToolTip(tip
			.arg(TableSelect::tr("Block Table"))
			.arg(QChar(L'A' + i)));
#endif /* QT_NO_TOOLTIP */
	}
}

/**
 * Update the widget display.
 */
void TableSelectPrivate::updateWidgetDisplay(void)
{
	Q_Q(TableSelect);
	if (!card) {
		// Hide the widget display.
		foreach (QObject *obj, q->children()) {
			QWidget *widget = qobject_cast<QWidget*>(obj);
			if (widget) {
				widget->hide();
			}
		}
		return;
	}

	// Update the widget count.
	updateWidgetCount();

	// Update the table displays.
	// TODO: Consolidate the two functions?
	updateDirTableDisplay();
	updateBlockTableDisplay();

	// Show the widgets.
	foreach (QObject *obj, q->children()) {
		QWidget *widget = qobject_cast<QWidget*>(obj);
		if (widget) {
			widget->show();
		}
	}
}

/** TableSelect **/

TableSelect::TableSelect(QWidget *parent)
	: super(parent)
	, d_ptr(new TableSelectPrivate(this))
{
	Q_D(TableSelect);
	d->ui.setupUi(this);

	// Connect button group signals.
	QObject::connect(d->ui.btnDirGroup, SIGNAL(buttonClicked(int)),
			 this, SLOT(setActiveDatIdx(int)));
	QObject::connect(d->ui.btnBlockGroup, SIGNAL(buttonClicked(int)),
			 this, SLOT(setActiveBatIdx(int)));
}

TableSelect::~TableSelect()
{
	Q_D(TableSelect);
	delete d;
}

/**
 * Get the Card being displayed.
 * @return Card.
 */
Card *TableSelect::card(void) const
{
	Q_D(const TableSelect);
	return d->card;
}

/**
 * Set the Card being displayed.
 * @param card Card.
 */
void TableSelect::setCard(Card *card)
{
	Q_D(TableSelect);

	// Disconnect the Card's signals if a Card is already set.
	if (d->card) {
		disconnect(d->card, SIGNAL(destroyed(QObject*)),
			   this, SLOT(memCard_destroyed_slot(QObject*)));
		disconnect(d->card, SIGNAL(activeDatIdxChanged(int)),
			   this, SLOT(memCard_activeDatIdxChanged_slot(int)));
		disconnect(d->card, SIGNAL(activeBatIdxChanged(int)),
			   this, SLOT(memCard_activeBatIdxChanged_slot(int)));
	}

	d->card = card;

	// Connect the Card's signals.
	if (d->card) {
		connect(d->card, SIGNAL(destroyed(QObject*)),
			this, SLOT(memCard_destroyed_slot(QObject*)));
		connect(d->card, SIGNAL(activeDatIdxChanged(int)),
			this, SLOT(memCard_activeDatIdxChanged_slot(int)));
		connect(d->card, SIGNAL(activeBatIdxChanged(int)),
			this, SLOT(memCard_activeBatIdxChanged_slot(int)));
	}

	// Update the widget display.
	d->updateWidgetDisplay();
}

/** Events. **/

/**
 * Widget state has changed.
 * @param event State change event.
 */
void TableSelect::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange) {
		// Retranslate the UI.
		Q_D(TableSelect);
		d->ui.retranslateUi(this);
		d->updateWidgetDisplay();
	}

	// Pass the event to the base class.
	super::changeEvent(event);
}

/** Properties. **/

/**
 * Get the selected directory table.
 * @return Selected directory table index, or -1 on error.
 */
int TableSelect::activeDatIdx(void) const
{
	Q_D(const TableSelect);
	if (!d->card)
		return -1;
	return d->card->activeDatIdx();
}

/**
 * Get the selected block table.
 * @return Selected block table index, or -1 on error.
 */
int TableSelect::activeBatIdx(void) const
{
	Q_D(const TableSelect);
	if (!d->card)
		return -1;
	return d->card->activeBatIdx();
}

/** Internal slots. **/

/**
 * Card object was destroyed.
 * @param obj QObject that was destroyed.
 */
void TableSelect::memCard_destroyed_slot(QObject *obj)
{
	Q_D(TableSelect);

	if (obj == d->card) {
		// Our Card was destroyed.
		d->card = nullptr;

		// Update the widget display.
		d->updateWidgetDisplay();
	}
}

/**
 * Card's active Directory Table index was changed.
 * @param idx New active Directory Table index.
 */
void TableSelect::memCard_activeDatIdxChanged_slot(int idx)
{
	// Update the Directory Table buttons without emitting signals.
	// TODO: VERIFY!
	Q_D(TableSelect);
	if (idx >= 0 && idx < d->ui.btnDir.size()) {
		d->ui.btnDir[idx]->setChecked(true);
	}
}

/**
 * Card's active Block Table index was changed.
 * @param idx New active Block Table index.
 */
void TableSelect::memCard_activeBatIdxChanged_slot(int idx)
{
	// Update the Directory Table buttons without emitting signals.
	// TODO: VERIFY!
	Q_D(TableSelect);
	if (idx >= 0 && idx < d->ui.btnBlock.size()) {
		d->ui.btnBlock[idx]->setChecked(true);
	}
}

/** Public slots. **/

/**
 * Set the active Directory Table index.
 * NOTE: This function reloads the file list, without lost files.
 * @param idx Active Directory Table index. (0 or 1)
 */
void TableSelect::setActiveDatIdx(int idx)
{
	Q_D(TableSelect);
	if (!d->card || idx < 0 || idx >= 2)
		return;
	d->card->setActiveDatIdx(idx);
}

/**
 * Set the active Block Table index.
 * NOTE: This function reloads the file list, without lost files.
 * @param idx Active Block Table index. (0 or 1)
 */
void TableSelect::setActiveBatIdx(int idx)
{
	Q_D(TableSelect);
	if (!d->card || idx < 0 || idx >= 2)
		return;
	d->card->setActiveBatIdx(idx);
}
