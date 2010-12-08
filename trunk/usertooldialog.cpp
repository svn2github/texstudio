/***************************************************************************
 *   copyright       : (C) 2003-2007 by Pascal Brachet                     *
 *   http://www.xm1math.net/texmaker/                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "usertooldialog.h"
#include "buildmanager.h"

UserToolDialog::UserToolDialog(QWidget *parent, QString name, BuildManager* bm) : QDialog(parent), buildManager(bm) {
	setWindowTitle(name);
	setModal(true);
	ui.setupUi(this);

	previous_index=0;

	connect(ui.comboBox, SIGNAL(activated(int)), SLOT(change(int)));

	connect(ui.pushButtonWizard, SIGNAL(clicked()), SLOT(openWizard()));

	connect(ui.okButton, SIGNAL(clicked()), SLOT(slotOk()));
}

UserToolDialog::~UserToolDialog() {
}

void UserToolDialog::init() {
	for (int i=0;i<qMax(3,Tool.size());i++)
		ui.comboBox->insertItem(i, tr("Command %1").arg(i+1));
	ui.toolEdit->setText(Tool.value(0,""));
	ui.itemEdit->setText(Name.value(0,""));
	ui.comboBox->setCurrentIndex(0);
}

void UserToolDialog::change(int index) {
	while (Tool.size() <= previous_index) Tool << "";
	while (Name.size() <= previous_index) Name << "";
	Tool[previous_index]=ui.toolEdit->text();
	Name[previous_index]=ui.itemEdit->text();
	ui.toolEdit->setText(Tool.value(index,""));
	ui.itemEdit->setText(Name.value(index,""));
	previous_index=index;
}

void UserToolDialog::slotOk() {
	while (Tool.size() <= previous_index) Tool << "";
	while (Name.size() <= previous_index) Name << "";
	Tool[previous_index]=ui.toolEdit->text();
	Name[previous_index]=ui.itemEdit->text();
	accept();
}
void UserToolDialog::openWizard(){
	if (!buildManager) return;
	ui.toolEdit->setText(buildManager->editCommandList(ui.toolEdit->text()));
}
