/*
 * This file is part of the SmuView project.
 *
 * Copyright (C) 2018 Frank Stettner <frank-stettner@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UI_PROCESSING_DIALOGS_USERINPUTBLOCKDIALOG_HPP
#define UI_PROCESSING_DIALOGS_USERINPUTBLOCKDIALOG_HPP

#include <memory>

#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QWidget>

#include "src/session.hpp"

using std::shared_ptr;

namespace sv {
namespace ui {
namespace processing {
namespace dialogs {

class UserInputBlockDialog : public QDialog
{
	Q_OBJECT

public:
	UserInputBlockDialog(shared_ptr<Session> session, QWidget *parent = nullptr);

	QString message() const;

private:
	shared_ptr<Session> session_;

	QLineEdit *name_edit_;
	QLineEdit *msg_edit_;
	QDialogButtonBox *button_box_;

	void setup_ui();

public Q_SLOTS:
	void accept() override;

};

} // namespace dialogs
} // namespace processing
} // namespace ui
} // namespace sv

#endif // UI_PROCESSING_DIALOGS_USERINPUTBLOCKDIALOG_HPP
